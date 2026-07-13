// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Full implementation of FindReplaceDlg with Find, Replace, Find in Files.
#include "FindReplaceDlg.h"
#include "ScintillaComponent.h"
#include "Parameters.h"
#include "NppDarkMode.h"
// NppSciCompat.h is included via ScintillaComponent.h above (pragma once allows
// re-inclusion). Do NOT include it directly — that would hit pragma once and
// skip its SC_UPDATE_* undefs, leaving SC_UPDATE_NONE still macro-defined.
#include "Buffer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QTextCursor>
#include <QRegularExpression>
#include <QAbstractItemView>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QMutexLocker>
#include <QStandardPaths>
#include <algorithm>
#include <fstream>

using namespace npp_sci;

FindOption FindReplaceDlg::_options;
FindOption* FindReplaceDlg::_env = &_options;

// =============================================================================
// FindReplaceDlg — constructor and init
// =============================================================================
FindReplaceDlg::FindReplaceDlg(QWidget* parent)
    : QWidget(parent, Qt::Window)
{
    setWindowTitle("Find / Replace");
    setMinimumWidth(480);
    setMinimumHeight(400);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    _tab = new QTabWidget(this);
    mainLayout->addWidget(_tab);
    setupFindTab();
    setupReplaceTab();
    setupFindInFilesTab();
    NppDarkMode::instance().applyToWidget(this);
}

FindReplaceDlg::~FindReplaceDlg()
{
    stopFifSearch();
}

void FindReplaceDlg::init(void* /*hInst*/, QWidget*& /*parent*/, ScintillaComponent** ppEditView)
{
    if (!ppEditView) throw std::runtime_error("FindReplaceDlg::init: ppEditView is null.");
    _ppEditView = ppEditView;
    _currentEditor = ppEditView ? *ppEditView : nullptr;
}

// =============================================================================
// Find Tab
// =============================================================================
void FindReplaceDlg::setupFindTab()
{
    _findTab = new QWidget(this);
    QVBoxLayout* vl = new QVBoxLayout(_findTab);
    vl->setContentsMargins(8, 8, 8, 8);

    QHBoxLayout* searchRow = new QHBoxLayout;
    QLabel* findLbl = new QLabel("Fi&nd:", _findTab);
    _findEdit = new QLineEdit(_findTab);
    _findEdit->setMinimumWidth(250);
    findLbl->setBuddy(_findEdit);
    searchRow->addWidget(findLbl);
    searchRow->addWidget(_findEdit);
    vl->addLayout(searchRow);

    QGridLayout* optGrid = new QGridLayout;
    int row = 0;

    QLabel* modeLbl = new QLabel("Se&arch mode:", _findTab);
    QComboBox* modeCombo = new QComboBox(_findTab);
    modeCombo->setObjectName("modeCombo");
    modeCombo->addItem("Normal", QVariant::fromValue(static_cast<int>(FindNormal)));
    modeCombo->addItem("Extended (\\n, \\r, \\t)", QVariant::fromValue(static_cast<int>(FindExtended)));
    modeCombo->addItem("Regular expression", QVariant::fromValue(static_cast<int>(FindRegex)));
    connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx) { _options._searchType = static_cast<SearchType>(idx); });
    optGrid->addWidget(modeLbl, row, 0, Qt::AlignRight);
    optGrid->addWidget(modeCombo, row, 1);
    ++row;

    _matchCaseChk = new QCheckBox("Match &case", _findTab);
    _matchCaseChk->setObjectName("matchCaseChk");
    optGrid->addWidget(_matchCaseChk, row, 0, 1, 2);
    ++row;

    _wholeWordChk = new QCheckBox("Match &whole word", _findTab);
    _wholeWordChk->setObjectName("wholeWordChk");
    optGrid->addWidget(_wholeWordChk, row, 0, 1, 2);
    ++row;

    _wrapChk = new QCheckBox("Wrap a&round", _findTab);
    _wrapChk->setObjectName("wrapChk");
    _wrapChk->setChecked(true);
    optGrid->addWidget(_wrapChk, row, 0, 1, 2);
    ++row;

    vl->addLayout(optGrid);
    vl->addStretch();

    QHBoxLayout* btnRow = new QHBoxLayout;
    btnRow->addStretch();
    _findNextBtn = new QPushButton("Find &Next", _findTab);
    _findNextBtn->setDefault(true);
    connect(_findNextBtn, &QPushButton::clicked, this, &FindReplaceDlg::onFindNext);
    btnRow->addWidget(_findNextBtn);

    QPushButton* countBtn = new QPushButton("Cou&nt All", _findTab);
    connect(countBtn, &QPushButton::clicked, this, [this]() {
        if (_findEdit->text().isEmpty()) return;
        _options._str2Search = _findEdit->text().toStdWString();
        _options._isMatchCase = _matchCaseChk && _matchCaseChk->isChecked();
        _options._isWholeWord = _wholeWordChk && _wholeWordChk->isChecked();
        int cnt = markAll(_options._str2Search.c_str(), 0);
        updateStatus(QString("Count: %1 matches").arg(cnt), cnt > 0 ? FSFound : FSNotFound);
    });
    btnRow->addWidget(countBtn);

    QPushButton* markAllBtn = new QPushButton("&Mark All", _findTab);
    connect(markAllBtn, &QPushButton::clicked, this, &FindReplaceDlg::onMarkAll);
    btnRow->addWidget(markAllBtn);
    vl->addLayout(btnRow);

    connect(_findEdit, &QLineEdit::returnPressed, this, &FindReplaceDlg::onFindNext);
    _tab->addTab(_findTab, "Find");
}

// =============================================================================
// Replace Tab
// =============================================================================
void FindReplaceDlg::setupReplaceTab()
{
    _replaceTab = new QWidget(this);
    QVBoxLayout* vl = new QVBoxLayout(_replaceTab);
    vl->setContentsMargins(8, 8, 8, 8);

    QHBoxLayout* findRow = new QHBoxLayout;
    QLabel* findLbl = new QLabel("Fi&nd:", _replaceTab);
    _findEdit = new QLineEdit(_replaceTab);
    _findEdit->setMinimumWidth(250);
    findLbl->setBuddy(_findEdit);
    findRow->addWidget(findLbl);
    findRow->addWidget(_findEdit);
    vl->addLayout(findRow);

    QHBoxLayout* replRow = new QHBoxLayout;
    QLabel* replLbl = new QLabel("Re&place:", _replaceTab);
    _replaceEdit = new QLineEdit(_replaceTab);
    replLbl->setBuddy(_replaceEdit);
    replRow->addWidget(replLbl);
    replRow->addWidget(_replaceEdit);
    vl->addLayout(replRow);

    QGridLayout* optGrid = new QGridLayout;
    int row = 0;

    QLabel* modeLbl = new QLabel("Se&arch mode:", _replaceTab);
    QComboBox* modeCombo = new QComboBox(_replaceTab);
    modeCombo->addItem("Normal", QVariant::fromValue(static_cast<int>(FindNormal)));
    modeCombo->addItem("Extended (\\n, \\r, \\t)", QVariant::fromValue(static_cast<int>(FindExtended)));
    modeCombo->addItem("Regular expression", QVariant::fromValue(static_cast<int>(FindRegex)));
    connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx) { _options._searchType = static_cast<SearchType>(idx); });
    optGrid->addWidget(modeLbl, row, 0, Qt::AlignRight);
    optGrid->addWidget(modeCombo, row, 1);
    ++row;

    _matchCaseChk = new QCheckBox("Match &case", _replaceTab);
    optGrid->addWidget(_matchCaseChk, row, 0, 1, 2);
    ++row;

    _wholeWordChk = new QCheckBox("Match &whole word", _replaceTab);
    optGrid->addWidget(_wholeWordChk, row, 0, 1, 2);
    ++row;

    _wrapChk = new QCheckBox("Wrap a&round", _replaceTab);
    _wrapChk->setChecked(true);
    optGrid->addWidget(_wrapChk, row, 0, 1, 2);
    ++row;

    vl->addLayout(optGrid);
    vl->addStretch();

    QHBoxLayout* btnRow = new QHBoxLayout;
    btnRow->addStretch();
    _replaceBtn = new QPushButton("&Replace", _replaceTab);
    connect(_replaceBtn, &QPushButton::clicked, this, &FindReplaceDlg::onReplace);
    btnRow->addWidget(_replaceBtn);

    _replaceAllBtn = new QPushButton("Replace &All", _replaceTab);
    connect(_replaceAllBtn, &QPushButton::clicked, this, &FindReplaceDlg::onReplaceAll);
    btnRow->addWidget(_replaceAllBtn);
    vl->addLayout(btnRow);

    connect(_findEdit, &QLineEdit::returnPressed, this, &FindReplaceDlg::onReplace);
    connect(_replaceEdit, &QLineEdit::returnPressed, this, &FindReplaceDlg::onReplaceAll);

    _tab->addTab(_replaceTab, "Replace");
}

// =============================================================================
// Find in Files Tab (FIND-1, FIND-2)
// =============================================================================
void FindReplaceDlg::setupFindInFilesTab()
{
    _findInFilesTab = new QWidget(this);
    QVBoxLayout* vl = new QVBoxLayout(_findInFilesTab);
    vl->setContentsMargins(8, 8, 8, 8);

    QHBoxLayout* findRow = new QHBoxLayout;
    findRow->addWidget(new QLabel("Fi&nd:", _findInFilesTab));
    _findEdit = new QLineEdit(_findInFilesTab);
    _findEdit->setObjectName("fifFindEdit");
    findRow->addWidget(_findEdit);
    vl->addLayout(findRow);

    QHBoxLayout* dirRow = new QHBoxLayout;
    dirRow->addWidget(new QLabel("In &directory:", _findInFilesTab));
    _fifDirEdit = new QLineEdit(_findInFilesTab);
    _fifDirEdit->setPlaceholderText("/path/to/directory");
    _fifBrowseBtn = new QToolButton(_findInFilesTab);
    _fifBrowseBtn->setText("...");
    _fifBrowseBtn->setToolTip("Browse...");
    connect(_fifBrowseBtn, &QPushButton::clicked, this, &FindReplaceDlg::onFifBrowse);
    dirRow->addWidget(_fifDirEdit);
    dirRow->addWidget(_fifBrowseBtn);
    vl->addLayout(dirRow);

    QHBoxLayout* filterRow = new QHBoxLayout;
    filterRow->addWidget(new QLabel("Fi&lters:", _findInFilesTab));
    _filterCombo = new QComboBox(_findInFilesTab);
    _filterCombo->setEditable(true);
    _filterCombo->addItems(QStringList() << "*.*" << "*.txt" << "*.cpp" << "*.h"
                            << "*.py" << "*.js" << "*.json" << "*.xml" << "*.html"
                            << "*.css" << "*.md" << "*.ini" << "*.yaml" << "*.yml");
    _filterCombo->setCurrentText("*.*");
    filterRow->addWidget(_filterCombo);
    vl->addLayout(filterRow);

    QGridLayout* optGrid = new QGridLayout;
    int row = 0;
    _matchCaseChk = new QCheckBox("Match &case", _findInFilesTab);
    optGrid->addWidget(_matchCaseChk, row, 0);
    ++row;

    _wholeWordChk = new QCheckBox("Match &whole word", _findInFilesTab);
    optGrid->addWidget(_wholeWordChk, row, 0);
    ++row;

    _regexChk = new QCheckBox("Regular e&xpression", _findInFilesTab);
    optGrid->addWidget(_regexChk, row, 0);
    ++row;

    _isRecursiveChk = new QCheckBox("&Recursive", _findInFilesTab);
    _isRecursiveChk->setChecked(true);
    optGrid->addWidget(_isRecursiveChk, row, 0);
    ++row;

    _isInHiddenChk = new QCheckBox("In hidden &folders", _findInFilesTab);
    optGrid->addWidget(_isInHiddenChk, row, 0);
    ++row;

    vl->addLayout(optGrid);

    QHBoxLayout* replRow = new QHBoxLayout;
    replRow->addWidget(new QLabel("Re&place with:", _findInFilesTab));
    _fifReplaceEdit = new QLineEdit(_findInFilesTab);
    replRow->addWidget(_fifReplaceEdit);
    vl->addLayout(replRow);

    _fifProgressBar = new QProgressBar(_findInFilesTab);
    _fifProgressBar->setRange(0, 100);
    _fifProgressBar->setValue(0);
    _fifProgressBar->setTextVisible(true);
    _fifProgressBar->setFormat("%p% — %v matches");
    vl->addWidget(_fifProgressBar);

    _fifResultsEdit = new QTextBrowser(_findInFilesTab);
    _fifResultsEdit->setReadOnly(true);
    _fifResultsEdit->setMaximumHeight(60);
    _fifResultsEdit->setFont(QFont("monospace"));
    _fifResultsEdit->setOpenExternalLinks(false);
    _fifResultsEdit->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    _fifResultsEdit->setText(QStringLiteral(
        "<span style='color:#888'>Results appear in the Find Result panel below. "
        "Use <b>Search → Find in Files</b> or dock the Find Result panel.</span>"));
    vl->addWidget(_fifResultsEdit);

    // FindResultPanel — the real results panel (dockable)
    _findResultPanel = new FindResultPanel(_findInFilesTab);
    _findResultPanel->setMinimumHeight(200);
    vl->addWidget(_findResultPanel, 1);  // stretch = 1 to fill remaining space

    // Wire panel result double-click → emit fifResultFileOpenRequested
    connect(_findResultPanel, &FindResultPanel::resultSelected,
            this, [this](const QString& filePath, int lineNumber) {
                emit fifResultFileOpenRequested(filePath, lineNumber);
            });

    QHBoxLayout* btnRow = new QHBoxLayout;
    btnRow->addStretch();
    _fifFindBtn = new QPushButton("Find All", _findInFilesTab);
    _fifFindBtn->setDefault(true);
    connect(_fifFindBtn, &QPushButton::clicked, this, [this]() { startFifSearch(false); });
    btnRow->addWidget(_fifFindBtn);

    _fifReplaceBtn = new QPushButton("Replace in Files", _findInFilesTab);
    connect(_fifReplaceBtn, &QPushButton::clicked, this, [this]() { startFifSearch(true); });
    btnRow->addWidget(_fifReplaceBtn);

    _fifCancelBtn = new QPushButton("Cancel", _findInFilesTab);
    _fifCancelBtn->setEnabled(false);
    connect(_fifCancelBtn, &QPushButton::clicked, this, &FindReplaceDlg::onFifCancel);
    btnRow->addWidget(_fifCancelBtn);

    vl->addLayout(btnRow);
    _tab->addTab(_findInFilesTab, "Find in Files");
}

// =============================================================================
// Dialog management
// =============================================================================
void FindReplaceDlg::doDialog(DIALOG_TYPE whichType, bool isRTL)
{
    _currentStatus = whichType;
    _isRTL = isRTL;
    if (whichType < _tab->count()) _tab->setCurrentIndex(whichType);
    show();
    raise();
    activateWindow();
    _findEdit->setFocus();
    _findEdit->selectAll();
}

void FindReplaceDlg::create(DIALOG_TYPE whichType)
{
    doDialog(whichType, false);
}

// =============================================================================
// Search flags builder (FIND-3: regex wiring)
// =============================================================================
unsigned int FindReplaceDlg::buildSearchFlags(const FindOption* opt) const
{
    unsigned int flags = 0;
    if (!opt) return flags;
    if (opt->_isMatchCase) flags |= npp_sci::SCFIND_MATCHCASE;
    if (opt->_isWholeWord) flags |= npp_sci::SCFIND_WHOLEWORD;
    if (opt->_searchType == FindRegex) flags |= npp_sci::SCFIND_REGEXP;
    return flags;
}

// =============================================================================
// onFindNext — FIND-3: wires regex, whole word, match case
// =============================================================================
void FindReplaceDlg::onFindNext()
{
    QString findText = _findEdit->text();
    if (findText.isEmpty()) return;

    _options._str2Search = findText.toStdWString();
    _options._isMatchCase = _matchCaseChk && _matchCaseChk->isChecked();
    _options._isWholeWord = _wholeWordChk && _wholeWordChk->isChecked();
    _options._isWrapAround = _wrapChk && _wrapChk->isChecked();

    if (auto* combo = findChild<QComboBox*>(); combo && combo->objectName() == "modeCombo") {
        _options._searchType = static_cast<SearchType>(combo->currentData().toInt());
    }

    FindStatus status;
    bool found = processFindNext(_options._str2Search.c_str(), &_options, &status);
    updateStatus(found ? "Found" : "Not found", status);
}

void FindReplaceDlg::onReplace()
{
    if (!_currentEditor) return;
    QString findText = _findEdit->text();
    QString replaceText = _replaceEdit->text();
    if (findText.isEmpty()) return;

    _options._str2Search = findText.toStdWString();
    _options._str4Replace = replaceText.toStdWString();
    _options._isMatchCase = _matchCaseChk && _matchCaseChk->isChecked();
    _options._isWholeWord = _wholeWordChk && _wholeWordChk->isChecked();

    bool replaced = processReplace(_options._str2Search.c_str(), _options._str4Replace.c_str(), &_options);
    updateStatus(replaced ? "Replaced" : "Nothing to replace", replaced ? FSFound : FSNotFound);
}

void FindReplaceDlg::onReplaceAll()
{
    QString findText = _findEdit->text();
    QString replaceText = _replaceEdit->text();
    if (findText.isEmpty()) return;

    _options._str2Search = findText.toStdWString();
    _options._str4Replace = replaceText.toStdWString();
    _options._isMatchCase = _matchCaseChk && _matchCaseChk->isChecked();
    _options._isWholeWord = _wholeWordChk && _wholeWordChk->isChecked();

    replaceAllInOpenedDocs();
    updateStatus(tr("Replace all completed"), FSFound);
}

void FindReplaceDlg::onFindAll()
{
    if (_findEdit->text().isEmpty()) return;
    _options._str2Search = _findEdit->text().toStdWString();
    _options._isMatchCase = _matchCaseChk && _matchCaseChk->isChecked();
    _options._isWholeWord = _wholeWordChk && _wholeWordChk->isChecked();
    if (auto* combo = findChild<QComboBox*>(); combo && combo->objectName() == "modeCombo") {
        _options._searchType = static_cast<SearchType>(combo->currentData().toInt());
    }
    int count = markAll(_options._str2Search.c_str(), 0);
    updateStatus(QString("Found %1 matches").arg(count), count > 0 ? FSFound : FSNotFound);
    emit searchCompleted(count);
}

void FindReplaceDlg::onMarkAll()
{
    if (_findEdit->text().isEmpty()) return;
    _options._str2Search = _findEdit->text().toStdWString();
    _options._isMatchCase = _matchCaseChk && _matchCaseChk->isChecked();
    _options._isWholeWord = _wholeWordChk && _wholeWordChk->isChecked();
    if (auto* combo = findChild<QComboBox*>(); combo && combo->objectName() == "modeCombo") {
        _options._searchType = static_cast<SearchType>(combo->currentData().toInt());
    }
    int count = markAll(_options._str2Search.c_str(), 0);
    updateStatus(QString("Marked %1 matches").arg(count), count > 0 ? FSFound : FSNotFound);
}

void FindReplaceDlg::onClose() { hide(); }

// =============================================================================
// processFindNext — core find engine using SCI_SEARCHINTARGET
// Supports: Match case, Whole word, Regex, Wrap around, Extended (\n\r\t)
// =============================================================================
bool FindReplaceDlg::processFindNext(const wchar_t* txt2find, const FindOption* options, FindStatus* oFindStatus)
{
    ScintillaComponent* sci = _currentEditor;
    if (!sci || !txt2find) return false;

    const FindOption* opt = options ? options : &_options;
    unsigned int flags = buildSearchFlags(opt);
    sci->send(SCI_SETSEARCHFLAGS, flags);

    QString searchText = QString::fromWCharArray(txt2find);
    if (opt->_searchType == FindExtended) {
        searchText = searchText.replace("\\n", "\n").replace("\\r", "\r").replace("\\t", "\t");
    }
    QByteArray utf8 = searchText.toUtf8();

    size_t startPos = sci->send(SCI_GETCURRENTPOS);
    size_t docLen = sci->send(SCI_GETLENGTH);

    sci->send(SCI_SETTARGETSTART, startPos);
    sci->send(SCI_SETTARGETEND, docLen);

    intptr_t found = sci->send(SCI_SEARCHINTARGET, utf8.size(), reinterpret_cast<sptr_t>(utf8.constData()));

    if (found == -1 && opt->_isWrapAround) {
        sci->send(SCI_SETTARGETSTART, 0);
        sci->send(SCI_SETTARGETEND, startPos);
        found = sci->send(SCI_SEARCHINTARGET, utf8.size(), reinterpret_cast<sptr_t>(utf8.constData()));
    }

    if (found >= 0) {
        sci->send(SCI_SETSEL, found, found + utf8.size());
        if (oFindStatus) *oFindStatus = FSFound;
        return true;
    }

    if (oFindStatus) *oFindStatus = FSNotFound;
    return false;
}

bool FindReplaceDlg::processReplace(const wchar_t* txt2find, const wchar_t* txt2replace, const FindOption* options)
{
    ScintillaComponent* sci = _currentEditor;
    if (!sci) return false;

    bool found = processFindNext(txt2find, options, nullptr);
    if (found) {
        QString replText = QString::fromWCharArray(txt2replace);
        const FindOption* opt = options ? options : &_options;
        if (opt && opt->_searchType == FindExtended) {
            replText = replText.replace("\\n", "\n").replace("\\r", "\r").replace("\\t", "\t");
        }
        QByteArray utf8 = replText.toUtf8();
        sci->send(SCI_REPLACESEL, 0, reinterpret_cast<sptr_t>(utf8.constData()));
    }
    return found;
}

int FindReplaceDlg::markAll(const wchar_t* txt2find, int /*styleID*/)
{
    ScintillaComponent* sci = _currentEditor;
    if (!sci || !txt2find) return 0;

    unsigned int flags = buildSearchFlags(&_options);
    sci->send(SCI_SETSEARCHFLAGS, flags);
    sci->send(SCI_SETTARGETSTART, 0);
    sci->send(SCI_SETTARGETEND, sci->send(SCI_GETLENGTH));

    QString searchText = QString::fromWCharArray(txt2find);
    if (_options._searchType == FindExtended) {
        searchText = searchText.replace("\\n", "\n").replace("\\r", "\r").replace("\\t", "\t");
    }
    QByteArray utf8 = searchText.toUtf8();

    int count = 0;
    sci->send(SCI_SETINDICATORCURRENT, 31);
    intptr_t found = sci->send(SCI_SEARCHINTARGET, utf8.size(), reinterpret_cast<sptr_t>(utf8.constData()));

    while (found >= 0) {
        size_t targetStart = sci->send(SCI_GETTARGETSTART);
        size_t targetEnd = sci->send(SCI_GETTARGETEND);
        sci->send(SCI_INDICATORFILLRANGE, targetStart, targetEnd - targetStart);
        ++count;
        sci->send(SCI_SETTARGETSTART, targetEnd);
        sci->send(SCI_SETTARGETEND, sci->send(SCI_GETLENGTH));
        found = sci->send(SCI_SEARCHINTARGET, utf8.size(), reinterpret_cast<sptr_t>(utf8.constData()));
    }
    return count;
}

int FindReplaceDlg::processAll(ProcessOperation op, const FindOption* opt, bool /*isEntire*/)
{
    if (!opt || opt->_str2Search.empty()) return 0;
    if (op == ProcessFindAll) return markAll(opt->_str2Search.c_str(), 0);
    if (op == ProcessReplaceAll) { replaceAllInOpenedDocs(); return 0; }
    return 0;
}

int FindReplaceDlg::processAll(ProcessOperation op, const FindOption* opt, bool isEntire, FindersInfo* /*fi*/)
{
    return processAll(op, opt, isEntire);
}

int FindReplaceDlg::processAll(ProcessOperation op, const FindersInfo::FindOption* opt, bool isEntire, FindersInfo* /*fi*/)
{
    if (!opt) return 0;
    FindOption c;
    c._isRecursive = opt->_isRecursive;
    c._isInHiddenDir = opt->_isInHiddenDir;
    c._dotMatchesNewline = opt->_dotDotMatch;
    c._isMatchCase = opt->_isMatchCase;
    c._isWholeWord = opt->_wholeWord;
    c._str2Search = opt->_str2Search;
    c._directory = opt->_directory;
    c._filters = L"*.*";
    return processAll(op, &c, isEntire);
}

int FindReplaceDlg::processRange(ProcessOperation op, const std::wstring& /*txt*/,
                                const FindOption* opt, int colourStyleID, int /*extraArg*/,
                                ScintillaComponent* view2Process)
{
    Q_UNUSED(colourStyleID);
    if (view2Process && opt) {
        _currentEditor = view2Process;
        if (op == ProcessFindAll || op == ProcessMarkAll)
            return markAll(opt->_str2Search.c_str(), 0);
    }
    return 0;
}

int FindReplaceDlg::processRange(ProcessOperation op, const FindReplaceInfo& fri,
                                const FindOption* opt, int colourStyleID,
                                ScintillaComponent* view2Process)
{
    if (!opt || !fri._txt2find) return 0;
    _options._str2Search = fri._txt2find;
    if (fri._txt2replace) _options._str4Replace = fri._txt2replace;
    if (view2Process) _currentEditor = view2Process;
    return processRange(op, fri._txt2find, opt, colourStyleID, 0, view2Process);
}

// =============================================================================
// replaceAllInOpenedDocs — FIND-5
// =============================================================================
int FindReplaceDlg::replaceAllInOpenedDocs()
{
    if (!_currentEditor) return 0;
    const FindOption* opt = &_options;
    unsigned int flags = buildSearchFlags(opt);
    _currentEditor->send(SCI_SETSEARCHFLAGS, flags);

    QString findText = QString::fromWCharArray(opt->_str2Search.c_str());
    if (opt->_searchType == FindExtended)
        findText = findText.replace("\\n", "\n").replace("\\r", "\r").replace("\\t", "\t");
    QByteArray findUtf8 = findText.toUtf8();

    QString replText = QString::fromWCharArray(opt->_str4Replace.c_str());
    if (opt->_searchType == FindExtended)
        replText = replText.replace("\\n", "\n").replace("\\r", "\r").replace("\\t", "\t");
    QByteArray replUtf8 = replText.toUtf8();

    _currentEditor->send(SCI_BEGINUNDOACTION);
    int count = 0;
    _currentEditor->send(SCI_SETTARGETSTART, 0);
    _currentEditor->send(SCI_SETTARGETEND, _currentEditor->send(SCI_GETLENGTH));
    intptr_t found = _currentEditor->send(SCI_SEARCHINTARGET, findUtf8.size(), reinterpret_cast<sptr_t>(findUtf8.constData()));

    while (found >= 0) {
        size_t targetStart = _currentEditor->send(SCI_GETTARGETSTART);
        _currentEditor->send(SCI_REPLACETARGET, replUtf8.size(), reinterpret_cast<sptr_t>(replUtf8.constData()));
        ++count;
        _currentEditor->send(SCI_SETTARGETSTART, targetStart + replUtf8.size());
        _currentEditor->send(SCI_SETTARGETEND, _currentEditor->send(SCI_GETLENGTH));
        found = _currentEditor->send(SCI_SEARCHINTARGET, findUtf8.size(), reinterpret_cast<sptr_t>(findUtf8.constData()));
    }

    _currentEditor->send(SCI_ENDUNDOACTION);
    return count;
}

// =============================================================================
// Status bar update
// =============================================================================
void FindReplaceDlg::updateStatus(const QString& message, FindStatus status)
{
    Q_UNUSED(status);
    _lastStatusMsg = message;
    QString base = (_currentStatus == REPLACE_DLG) ? "Replace" : "Find / Replace";
    setWindowTitle(base + " — " + message);
}

// =============================================================================
// Search text setters
// =============================================================================
void FindReplaceDlg::setSearchText(const wchar_t* txt2find)
{
    if (_findEdit && txt2find) _findEdit->setText(QString::fromWCharArray(txt2find));
}

void FindReplaceDlg::setFindInFilesDirFilter(const wchar_t* dir, const wchar_t* filters)
{
    if (_fifDirEdit && dir) _fifDirEdit->setText(QString::fromWCharArray(dir));
    if (_filterCombo && filters) _filterCombo->setCurrentText(QString::fromWCharArray(filters));
}

void FindReplaceDlg::enableFindInFilesControls(bool enableFind, bool enableReplace)
{
    if (_fifFindBtn) _fifFindBtn->setEnabled(enableFind);
    if (_fifReplaceBtn) _fifReplaceBtn->setEnabled(enableReplace);
}

void FindReplaceDlg::saveFindHistory()
{
    NppParameters::getInstance().writeFindHistory();
}

void FindReplaceDlg::beginNewFilesSearch()
{
    _filesSearchedCount = 0;
    _filesFoundCount = 0;
    if (_fifResultsEdit) {
        _fifResultsEdit->clear();
        _fifResultsEdit->append(QStringLiteral("<b>Searching...</b>"));
    }
    if (_fifProgressBar) {
        _fifProgressBar->setValue(0);
        _fifProgressBar->setFormat("%p% — 0 matches");
    }
    // Delegate to FindResultPanel
    if (_findResultPanel) {
        QString dir = _fifDirEdit ? _fifDirEdit->text() : QString();
        QString filter = _filterCombo ? _filterCombo->currentText() : QStringLiteral("*.*");
        QString searchText = _findEdit ? _findEdit->text() : QString();
        _findResultPanel->beginSearch(dir, filter, searchText);
    }
}

void FindReplaceDlg::finishFilesSearch(int count, int searchedCount, bool /*unused*/)
{
    _filesFoundCount = count;
    _filesSearchedCount = searchedCount;
    if (_fifResultsEdit)
        _fifResultsEdit->append(QStringLiteral("<b>Search complete: %1 matches in %2 files</b>").arg(count).arg(searchedCount));
    if (_fifProgressBar)
        _fifProgressBar->setFormat(QString("Done — %1 matches").arg(count));
    if (_findResultPanel)
        _findResultPanel->endSearch(count, searchedCount);
    emit fifFinished(count, searchedCount);
}

void FindReplaceDlg::focusOnFinder() { show(); raise(); activateWindow(); }
void FindReplaceDlg::clearAllFinderResults()
{
    if (_fifResultsEdit) _fifResultsEdit->clear();
    if (_findResultPanel) _findResultPanel->clearResults();
    _foundInfos.clear();
}

void FindReplaceDlg::setStatusbarMessageWithRegExprErr(ScintillaComponent* /*view*/)
{
    updateStatus("Invalid regular expression", FSWarning);
}

const std::wstring& FindReplaceDlg::getText2search() const
{
    static std::wstring empty;
    return _findEdit ? _findEdit->text().toStdWString() : empty;
}

QString FindReplaceDlg::getDir2Search() const
{
    return _fifDirEdit ? _fifDirEdit->text() : QString();
}

// =============================================================================
// Find in Files — Browse button
// =============================================================================
void FindReplaceDlg::onFifBrowse()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Choose Directory"),
        (_fifDirEdit && !_fifDirEdit->text().isEmpty()) ? _fifDirEdit->text()
                                                        : QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
    );
    if (!dir.isEmpty() && _fifDirEdit) _fifDirEdit->setText(dir);
}

// =============================================================================
// Find in Files — Start search (non-blocking via QThread)
// =============================================================================
void FindReplaceDlg::startFifSearch(bool doReplace)
{
    QString dir = _fifDirEdit ? _fifDirEdit->text() : QString();
    if (dir.isEmpty()) {
        QMessageBox::warning(this, tr("Find in Files"), tr("Please enter a directory path."));
        return;
    }
    QString searchText = _findEdit ? _findEdit->text() : QString();
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, tr("Find in Files"), tr("Please enter a search pattern."));
        return;
    }

    stopFifSearch();

    _options._str2Search = searchText.toStdWString();
    _options._str4Replace = (_fifReplaceEdit ? _fifReplaceEdit->text() : QString()).toStdWString();
    _options._isMatchCase = _matchCaseChk && _matchCaseChk->isChecked();
    _options._isWholeWord = _wholeWordChk && _wholeWordChk->isChecked();
    _options._isRecursive = _isRecursiveChk && _isRecursiveChk->isChecked();
    _options._isInHiddenDir = _isInHiddenChk && _isInHiddenChk->isChecked();
    _options._searchType = (_regexChk && _regexChk->isChecked()) ? FindRegex : FindNormal;
    _options._filters = (_filterCombo ? _filterCombo->currentText() : "*.*").toStdWString();
    _options._directory = dir.toStdWString();

    beginNewFilesSearch();

    _fifWorker = new FindInFilesWorker(this);
    _fifWorker->configure(
        dir,
        _filterCombo ? _filterCombo->currentText() : "*.*",
        searchText,
        _fifReplaceEdit ? _fifReplaceEdit->text() : QString(),
        _options._isMatchCase,
        _options._isWholeWord,
        _options._searchType == FindRegex,
        _options._isRecursive,
        _options._isInHiddenDir,
        _options._dotMatchesNewline,
        doReplace
    );

    _fifThread = new QThread(this);
    _fifWorker->moveToThread(_fifThread);

    connect(_fifThread, &QThread::started, _fifWorker, &FindInFilesWorker::run);
    connect(_fifWorker, &FindInFilesWorker::progress, this, &FindReplaceDlg::onWorkerProgress);
    connect(_fifWorker, &FindInFilesWorker::resultReady, this, &FindReplaceDlg::onFifResultAppend);
    connect(_fifWorker, &FindInFilesWorker::finished, this, &FindReplaceDlg::onFifFinished);
    connect(_fifWorker, &FindInFilesWorker::finished, _fifThread, &QThread::quit);
    connect(_fifWorker, &FindInFilesWorker::finished, _fifWorker, &QObject::deleteLater);
    connect(_fifThread, &QThread::finished, _fifThread, &QObject::deleteLater);

    if (_fifCancelBtn) _fifCancelBtn->setEnabled(true);
    if (_fifFindBtn) _fifFindBtn->setEnabled(false);
    if (_fifReplaceBtn) _fifReplaceBtn->setEnabled(false);

    _fifThread->start();
}

void FindReplaceDlg::stopFifSearch()
{
    if (_fifWorker) {
        _fifWorker->abort();
        _fifWorker = nullptr;
    }
    if (_fifThread && _fifThread->isRunning()) {
        _fifThread->quit();
        _fifThread->wait(3000);
    }
    if (_fifCancelBtn) _fifCancelBtn->setEnabled(false);
    if (_fifFindBtn) _fifFindBtn->setEnabled(true);
    if (_fifReplaceBtn) _fifReplaceBtn->setEnabled(true);
}

void FindReplaceDlg::onFifCancel()
{
    stopFifSearch();
    if (_fifResultsEdit)
        _fifResultsEdit->append(QStringLiteral("<b>Cancelled.</b>"));
    if (_fifProgressBar)
        _fifProgressBar->setFormat(QStringLiteral("Cancelled"));
}

void FindReplaceDlg::onWorkerProgress(int current, int total, const QString& fileName)
{
    if (_fifProgressBar) {
        _fifProgressBar->setMaximum(total);
        _fifProgressBar->setValue(current);
        _fifProgressBar->setFormat(QString("%p% — %2").arg(fileName));
    }
}

void FindReplaceDlg::onFifResultAppend(int line, int64_t start, int64_t end,
                                        const QString& fileName, const QString& lineContent)
{
    // Always update the compact text-browser log
    if (_fifResultsEdit) {
        QString safeLine = lineContent.toHtmlEscaped();
        QString escapedPath = fileName.toHtmlEscaped();
        _fifResultsEdit->append(
            QStringLiteral("<span style=\"color:#888\">%1:</span>"
                          "<a href=\"file://%2#%3\" style=\"color:#4a9eff\">%2:%3:</a>"
                          "<span style=\"color:#ddd\">%4</span>")
                .arg(line)
                .arg(escapedPath)
                .arg(line)
                .arg(safeLine)
        );
    }

    // Also push to FindResultPanel (the main results tree)
    if (_findResultPanel) {
        _findResultPanel->addResult(fileName, line, static_cast<int>(start),
                                     static_cast<int>(end), lineContent);
    }
}

void FindReplaceDlg::onFifFinished(int totalMatches, int totalFiles)
{
    if (_fifCancelBtn) _fifCancelBtn->setEnabled(false);
    if (_fifFindBtn) _fifFindBtn->setEnabled(true);
    if (_fifReplaceBtn) _fifReplaceBtn->setEnabled(true);
    finishFilesSearch(totalMatches, totalFiles, true);
}

void FindReplaceDlg::onFifResultClicked(const QUrl& link)
{
    QString path = link.path();
    bool ok = false;
    int lineNum = link.fragment().toInt(&ok);
    if (path.isEmpty()) return;
    QFileInfo fi(path);
    if (fi.exists()) {
        // Trigger file open in Notepad_plus — emit signal or call buffer open
        // For now, emit a signal that Notepad_plus can handle
        emit fifResultFileOpenRequested(fi.absoluteFilePath(), ok ? lineNum : 0);
    }
}

void FindReplaceDlg::display(bool doShow)
{
    if (doShow) {
        QWidget::show();
        raise();
        activateWindow();
    } else {
        hide();
    }
}

void FindReplaceDlg::showFifTab()
{
    show();
    raise();
    activateWindow();
    if (_tab) _tab->setCurrentIndex(1);  // 0=Find, 1=Replace, 2=FindInFiles
}

void FindReplaceDlg::setDir2Search(const QString& dir)
{
    if (_fifDirEdit) _fifDirEdit->setText(dir);
}

void FindReplaceDlg::setStatusbarMessage(const QString& msg, int /*delayMs*/)
{
    updateStatus(msg, FSMessage);
}

QString FindReplaceDlg::getAndValidatePatterns(bool* isRecursive, bool* isInHiddenDir) const
{
    if (isRecursive) *isRecursive = (_isRecursiveChk && _isRecursiveChk->isChecked());
    if (isInHiddenDir) *isInHiddenDir = (_isInHiddenChk && _isInHiddenChk->isChecked());
    return _filterCombo ? _filterCombo->currentText() : QStringLiteral("*.*");
}

void FindReplaceDlg::putFindResult(const std::vector<struct FoundInfo>& results)
{
    if (!_fifResultsEdit) return;
    for (const auto& fi : results) {
        QString path = QString::fromWCharArray(fi._fullPath.c_str());
        _fifResultsEdit->append(
            QStringLiteral("%1:%2: %3")
                .arg(fi._lineNumber)
                .arg(fi._start)
                .arg(path)
        );
    }
}

// =============================================================================
// Stub methods called by Notepad_plus.cpp
// =============================================================================
void* FindReplaceDlg::getMainFinder() { return nullptr; }
std::vector<void*> FindReplaceDlg::getFindersOfFinder() { return {}; }

// =============================================================================
// FindInFilesWorker — background search thread
// =============================================================================
FindInFilesWorker::FindInFilesWorker(QObject* parent)
    : QObject(parent), _abort(false), _totalMatches(0), _totalFiles(0)
{
}

void FindInFilesWorker::configure(
    const QString& directory,
    const QString& filters,
    const QString& searchText,
    const QString& replaceText,
    bool matchCase,
    bool wholeWord,
    bool isRegex,
    bool isRecursive,
    bool isInHidden,
    bool dotMatchesNewline,
    bool doReplace)
{
    _directory = directory;
    _filters = filters;
    _searchText = searchText;
    _replaceText = replaceText;
    _matchCase = matchCase;
    _wholeWord = wholeWord;
    _isRegex = isRegex;
    _isRecursive = isRecursive;
    _isInHidden = isInHidden;
    _dotMatchesNewline = dotMatchesNewline;
    _doReplace = doReplace;
}

void FindInFilesWorker::abort() { _abort = true; }

void FindInFilesWorker::run()
{
    _abort = false;
    _totalMatches = 0;
    _totalFiles = 0;

    Qt::CaseSensitivity cs = _matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QRegularExpression::PatternOptions po = QRegularExpression::NoPatternOption;
    if (_isRegex) po |= QRegularExpression::MultilineOption;

    QRegularExpression re;
    if (_isRegex) {
        re = QRegularExpression(_searchText, po);
        if (!re.isValid()) {
            emit finished(0, 0);
            return;
        }
    }

    QStringList filterList = _filters.isEmpty() ? QStringList{"*.*"} : _filters.split(' ');
    QDir::Filters dirFilters = QDir::Files | QDir::NoSymLinks;
    if (_isInHidden) dirFilters |= QDir::Hidden;

    int fileCount = 0;
    QStringList allFiles;
    QDirIterator::IteratorFlags iterFlags = QDirIterator::NoIteratorFlags;
    if (_isRecursive) iterFlags |= QDirIterator::Subdirectories;
    QDirIterator it(_directory, dirFilters, iterFlags);
    while (it.hasNext()) {
        it.next();
        QFileInfo fi = it.fileInfo();
        // Skip directories if not recursive
        if (!_isRecursive && fi.isDir()) continue;
        bool match = false;
        for (const QString& f : filterList) {
            QRegularExpression re(QRegularExpression::wildcardToRegularExpression(f),
                                  QRegularExpression::CaseInsensitiveOption);
            if (re.isValid() && re.match(fi.fileName()).hasMatch()) { match = true; break; }
        }
        if (match) allFiles << fi.absoluteFilePath();
    }

    int total = allFiles.size();
    int idx = 0;
    for (const QString& filePath : allFiles) {
        if (_abort) break;
        ++idx;
        emit progress(idx, total, QFileInfo(filePath).fileName());

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

        QTextStream in(&file);
        int lineNum = 0;
        bool foundInFile = false;
        QStringList lines;
        while (!in.atEnd()) {
            if (_abort) break;
            QString line = in.readLine();
            ++lineNum;
            bool matched = false;
            if (_isRegex) {
                QRegularExpressionMatch m = re.match(line);
                matched = m.hasMatch();
            } else {
                matched = line.contains(_searchText, cs);
            }
            if (matched) {
                ++_totalMatches;
                if (!foundInFile) { ++_totalFiles; foundInFile = true; }
                emit resultReady(lineNum, 0, 0, filePath, line);
            }
        }
        file.close();

        if (_doReplace && foundInFile && !_replaceText.isEmpty()) {
            QString content = [&]() {
                QFile f(filePath);
                if (!f.open(QIODevice::ReadWrite | QIODevice::Text)) return QString();
                QString c = f.readAll();
                if (_isRegex) {
                    c.replace(re, _replaceText);
                } else {
                    c.replace(_searchText, _replaceText, cs);
                }
                f.resize(0);
                f.write(c.toUtf8());
                f.close();
                return c;
            }();
            (void)content;
        }
    }

    emit finished(_totalMatches, _totalFiles);
}
