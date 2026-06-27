// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Stub implementation of FindReplaceDlg (full port pending)
// Full Win32→Qt6 port requires:
//   - Regex search engine (Boost or QRegularExpression)
//   - Directory traversal with QDirIterator
//   - Finder results panel with QTreeWidget
//   - Incremental search with QTimer
#include "FindReplaceDlg.h"
#include "ScintillaComponent.h"
#include "Parameters.h"
#include "NppDarkMode.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>

FindOption FindReplaceDlg::_options;
FindOption* FindReplaceDlg::_env = &_options;  // Win32 compat: _env defaulted to _options

FindReplaceDlg::FindReplaceDlg() : QWidget() {}

FindReplaceDlg::~FindReplaceDlg() = default;

void FindReplaceDlg::init(QWidget* parent, ScintillaComponent** ppEditView)
{
    if (!ppEditView) throw std::runtime_error("FindReplaceDlg::init: ppEditView is null.");
    _ppEditView = ppEditView;
    setWindowTitle("Find / Replace");
    setMinimumWidth(400);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    _tab = new QTabWidget(this);
    mainLayout->addWidget(_tab);
    setupFindTab();
    setupReplaceTab();
    NppDarkMode::instance().applyToWidget(this);
}

void FindReplaceDlg::setupFindTab()
{
    _findTab = new QWidget(this);
    QFormLayout* fl = new QFormLayout(_findTab);
    _findEdit = new QLineEdit(_findTab);
    QCheckBox* wholeWordChk = new QCheckBox("Whole word", _findTab);
    QCheckBox* matchCaseChk = new QCheckBox("Match case", _findTab);
    QCheckBox* wrapChk = new QCheckBox("Wrap around", _findTab);
    wrapChk->setChecked(true);
    QHBoxLayout* btnRow = new QHBoxLayout;
    _findNextBtn = new QPushButton("Find Next", _findTab);
    QPushButton* countBtn = new QPushButton("Count All", _findTab);
    QPushButton* findAllBtn = new QPushButton("Find All in All Opened Docs", _findTab);
    connect(_findNextBtn, &QPushButton::clicked, this, &FindReplaceDlg::onFindNext);
    btnRow->addWidget(_findNextBtn);
    btnRow->addWidget(countBtn);
    btnRow->addWidget(findAllBtn);
    btnRow->addStretch();
    fl->addRow("Find:", _findEdit);
    fl->addRow(wholeWordChk);
    fl->addRow(matchCaseChk);
    fl->addRow(wrapChk);
    fl->addRow(btnRow);
    _tab->addTab(_findTab, "Find");
}

void FindReplaceDlg::setupReplaceTab()
{
    _replaceTab = new QWidget(this);
    QFormLayout* fl = new QFormLayout(_replaceTab);
    _replaceEdit = new QLineEdit(_replaceTab);
    QHBoxLayout* btnRow = new QHBoxLayout;
    _replaceBtn = new QPushButton("Replace", _replaceTab);
    _replaceAllBtn = new QPushButton("Replace All", _replaceTab);
    connect(_replaceBtn, &QPushButton::clicked, this, &FindReplaceDlg::onReplace);
    btnRow->addWidget(_replaceBtn);
    btnRow->addWidget(_replaceAllBtn);
    btnRow->addStretch();
    fl->addRow("Find:", _findEdit);
    fl->addRow("Replace:", _replaceEdit);
    fl->addRow(btnRow);
    _tab->addTab(_replaceTab, "Replace");
}

void FindReplaceDlg::doDialog(DIALOG_TYPE whichType, bool isRTL)
{
    _currentStatus = whichType;
    _isRTL = isRTL;
    if (whichType < _tab->count()) _tab->setCurrentIndex(whichType);
    show();
    raise();
    _findEdit->setFocus();
}

void FindReplaceDlg::create(DIALOG_TYPE whichType)
{
    doDialog(whichType, false);
}

void FindReplaceDlg::onFindNext()
{
    QString findText = _findEdit->text();
    if (findText.isEmpty()) return;
    FindOption opt = _options;
    opt._str2Search = findText.toStdWString();
    FindStatus status;
    bool found = processFindNext(opt._str2Search.c_str(), &opt, &status);
    updateStatus(found ? "Found" : "Not found", status);
}

void FindReplaceDlg::onReplace()
{
    if (!_ppEditView || !*_ppEditView) return;
    QString findText = _findEdit->text();
    QString replaceText = _replaceEdit->text();
    if (findText.isEmpty()) return;
    _options._str2Search = findText.toStdWString();
    _options._str4Replace = replaceText.toStdWString();
    bool replaced = processReplace(_options._str2Search.c_str(), _options._str4Replace.c_str(), &_options);
    updateStatus(replaced ? "Replaced" : "Nothing to replace", replaced ? FSFound : FSNotFound);
}

void FindReplaceDlg::onReplaceAll()
{
    replaceAllInOpenedDocs();
}

void FindReplaceDlg::onFindAll()
{
    if (_findEdit->text().isEmpty()) return;
    _options._str2Search = _findEdit->text().toStdWString();
    int count = processAll(ProcessFindAll, &_options, true);
    updateStatus(QString("Found %1 matches").arg(count), count > 0 ? FSFound : FSNotFound);
    emit searchCompleted(count);
}

void FindReplaceDlg::onMarkAll()
{
    if (_findEdit->text().isEmpty()) return;
    _options._str2Search = _findEdit->text().toStdWString();
    int count = markAll(_options._str2Search.c_str(), 0);
    updateStatus(QString("Marked %1 matches").arg(count), count > 0 ? FSFound : FSNotFound);
}

void FindReplaceDlg::onClose() { hide(); }

bool FindReplaceDlg::processFindNext(const wchar_t* txt2find, const FindOption* options, FindStatus* oFindStatus)
{
    if (!_ppEditView || !*_ppEditView || !txt2find) return false;
    ScintillaComponent* sci = *_ppEditView;
    // Build search flags
    int flags = 0;
    if (options) {
        if (options->_isWholeWord) flags |= 2;   // SCFIND_WHOLEWORD
        if (options->_isMatchCase) flags |= 1;   // SCFIND_MATCHCASE
        if (options->_searchType == FindRegex) flags |= 4; // SCFIND_REGEXP
    }
    sci->send(SCI_SETSEARCHFLAGS, flags);
    // Set search target to visible range
    size_t startPos = sci->send(SCI_GETCURRENTPOS);
    size_t endPos = sci->send(SCI_GETLENGTH);
    sci->send(SCI_SETTARGETSTART, startPos);
    sci->send(SCI_SETTARGETEND, endPos);
    QByteArray utf8 = QString::fromWCharArray(txt2find).toUtf8();
    intptr_t found = sci->send(SCI_SEARCHINTARGET, utf8.size(), reinterpret_cast<sptr_t>(utf8.constData()));
    if (found == -1) {
        // Wrap around
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
    if (!_ppEditView || !*_ppEditView) return false;
    ScintillaComponent* sci = *_ppEditView;
    bool found = processFindNext(txt2find, options, nullptr);
    if (found) {
        QByteArray utf8 = QString::fromWCharArray(txt2replace).toUtf8();
        sci->send(SCI_REPLACESEL, 0, reinterpret_cast<sptr_t>(utf8.constData()));
    }
    return found;
}

int FindReplaceDlg::markAll(const wchar_t* txt2find, int styleID)
{
    Q_UNUSED(styleID);
    if (!_ppEditView || !*_ppEditView || !txt2find) return 0;
    ScintillaComponent* sci = *_ppEditView;
    sci->send(SCI_SETTARGETSTART, 0);
    sci->send(SCI_SETTARGETEND, sci->send(SCI_GETLENGTH));
    QByteArray utf8 = QString::fromWCharArray(txt2find).toUtf8();
    int count = 0;
    intptr_t found = sci->send(SCI_SEARCHINTARGET, utf8.size(), reinterpret_cast<sptr_t>(utf8.constData()));
    while (found >= 0) {
        sci->send(SCI_SETINDICATORCURRENT, 31);
        size_t targetStart = sci->send(SCI_GETTARGETSTART);
        size_t targetEnd = sci->send(SCI_GETTARGETEND);
        sci->send(SCI_INDICATORFILLRANGE, targetStart, targetEnd - targetStart);
        ++count;
        size_t nextStart = targetEnd;
        sci->send(SCI_SETTARGETSTART, nextStart);
        sci->send(SCI_SETTARGETEND, sci->send(SCI_GETLENGTH));
        found = sci->send(SCI_SEARCHINTARGET, utf8.size(), reinterpret_cast<sptr_t>(utf8.constData()));
    }
    return count;
}

int FindReplaceDlg::processAll(ProcessOperation op, const FindOption* opt, bool isEntire)
{
    Q_UNUSED(op);
    if (!opt || opt->_str2Search.empty()) return 0;
    if (op == ProcessFindAll) return markAll(opt->_str2Search.c_str(), 0);
    return 0;
}

int FindReplaceDlg::processRange(ProcessOperation op, const std::wstring& txt,
                                const FindOption* opt, int colourStyleID, int /*extraArg*/,
                                ScintillaComponent* view2Process)
{
    Q_UNUSED(op); Q_UNUSED(txt); Q_UNUSED(opt); Q_UNUSED(colourStyleID); Q_UNUSED(view2Process);
    return 0;
}

int FindReplaceDlg::processRange(ProcessOperation op, const FindReplaceInfo& fri,
                                const FindOption* opt, int colourStyleID, ScintillaComponent* view2Process)
{
    Q_UNUSED(fri); Q_UNUSED(colourStyleID); Q_UNUSED(view2Process);
    if (!opt || !fri._txt2find) return 0;
    return processAll(op, opt, true);
}

void FindReplaceDlg::replaceAllInOpenedDocs()
{
    // Stub: iterate all buffers and replace
    updateStatus("Replace all: not yet implemented", FSMessage);
}

void FindReplaceDlg::setSearchText(const wchar_t* txt2find)
{
    if (_findEdit && txt2find)
        _findEdit->setText(QString::fromWCharArray(txt2find));
}

void FindReplaceDlg::setFindInFilesDirFilter(const wchar_t* dir, const wchar_t* filters)
{
    Q_UNUSED(dir); Q_UNUSED(filters);
}

void FindReplaceDlg::enableFindInFilesControls(bool, bool) {}

void FindReplaceDlg::saveFindHistory()
{
    // Persist find/replace history to NppParameters settings
    NppParameters::getInstance().writeFindHistory();
}

void FindReplaceDlg::beginNewFilesSearch() {}
void FindReplaceDlg::finishFilesSearch(int, int, bool) {}
void FindReplaceDlg::focusOnFinder() { show(); raise(); }
void FindReplaceDlg::clearAllFinderResults() {}

const std::wstring& FindReplaceDlg::getText2search() const
{
    static std::wstring empty;
    return _findEdit ? _findEdit->text().toStdWString() : empty;
}

void FindReplaceDlg::updateStatus(const QString& message, FindStatus status)
{
    emit updateStatus(message, status);
}
