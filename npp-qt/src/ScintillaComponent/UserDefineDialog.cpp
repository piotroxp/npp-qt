// UserDefineDialog.cpp — Qt6 implementation for Notepad++ User Defined Languages
#include "ScintillaComponent/UserDefineDialog.h"
#include "ScintillaComponent/UserLangParser.h"
#include "WinControls/ColourPicker/ColourPopup.h"
#include "Window.h"
#include "NppDarkMode.h"

#include <QApplication>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QVector>
#include <QRegularExpression>

// ============================================================================
// KeywordsTab — Tab 1: 8 keyword sets
// ============================================================================
KeywordsTab::KeywordsTab(QWidget* parent) : QWidget(parent)
{
    setupUi();
}

void KeywordsTab::setupUi()
{
    _rows.reserve(8);
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // 8 keyword sets
    for (int i = 0; i < 8; ++i) {
        QGroupBox* group = new QGroupBox(QString("Keywords %1").arg(i + 1), this);
        QGridLayout* grid = new QGridLayout(group);

        QLabel* nameLabel = new QLabel("Name:", group);
        QLineEdit* nameEdit = new QLineEdit(group);
        nameEdit->setPlaceholderText(QString("e.g. Keyword%1").arg(i + 1));

        QLabel* kwLabel = new QLabel("Keywords:", group);
        QTextEdit* kwEdit = new QTextEdit(group);
        kwEdit->setPlaceholderText("space-separated keywords, e.g. if else for while return");
        kwEdit->setMaximumHeight(60);

        QCheckBox* prefixCheck = new QCheckBox("Match at word start only", group);

        grid->addWidget(nameLabel, 0, 0);
        grid->addWidget(nameEdit, 0, 1, 1, 2);
        grid->addWidget(prefixCheck, 1, 1, 1, 2);
        grid->addWidget(kwLabel, 2, 0, Qt::AlignTop);
        grid->addWidget(kwEdit, 2, 1, 1, 2);
        grid->setRowStretch(2, 1);

        KeywordSetRow row;
        row.nameEdit = nameEdit;
        row.keywordsEdit = kwEdit;
        row.prefixCheck = prefixCheck;
        _rows.append(row);

        mainLay->addWidget(group);
    }
    mainLay->addStretch();
}

void KeywordsTab::loadFromParser(const UserLangParser& parser)
{
    QList<UdlKeywordSet> keywordSets = parser.keywordSets();
    for (int i = 0; i < 8; ++i) {
        if (i < keywordSets.size()) {
            _rows[i].nameEdit->setText(keywordSets[i].name);
            _rows[i].keywordsEdit->setText(keywordSets[i].keywords.join(" "));
        } else {
            _rows[i].nameEdit->clear();
            _rows[i].keywordsEdit->clear();
        }
    }
}

void KeywordsTab::saveToParser(UserLangParser& parser) const
{
    QList<UdlKeywordSet> keywordSets;
    for (int i = 0; i < 8; ++i) {
        UdlKeywordSet ks;
        ks.name = _rows[i].nameEdit->text();
        ks.setIndex = i;
        QString text = _rows[i].keywordsEdit->toPlainText();
        ks.keywords = text.split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);
        keywordSets.append(ks);
    }
    Q_UNUSED(parser);  // data collected via serializeToXml()
}

QList<KeywordsTab::KeywordSetData> KeywordsTab::keywordSets() const
{
    QList<KeywordSetData> result;
    for (int i = 0; i < _rows.size(); ++i) {
        KeywordSetData d;
        d.name = _rows[i].nameEdit->text();
        d.keywords = _rows[i].keywordsEdit->toPlainText();
        d.isPrefix = _rows[i].prefixCheck->isChecked();
        result.append(d);
    }
    return result;
}

void KeywordsTab::clearAll()
{
    for (int i = 0; i < _rows.size(); ++i) {
        _rows[i].nameEdit->clear();
        _rows[i].keywordsEdit->clear();
        _rows[i].prefixCheck->setChecked(false);
    }
}

// ============================================================================
// StylesTab — Tab 2: Style list with fg/bg colour pickers
// ============================================================================
StylesTab::StylesTab(QWidget* parent) : QWidget(parent)
{
    setupUi();
}

void StylesTab::setupUi()
{
    QHBoxLayout* mainLay = new QHBoxLayout(this);

    // Left: style list
    _styleList = new QListWidget(this);
    _styleList->setAlternatingRowColors(true);
    mainLay->addWidget(_styleList, 1);

    // Right: style properties
    QWidget* props = new QWidget(this);
    QVBoxLayout* propLay = new QVBoxLayout(props);
    propLay->addWidget(new QLabel("<b>Style Properties</b>", props));

    _styleNameLabel = new QLabel("Select a style from the list", props);

    // Foreground colour
    QHBoxLayout* fgLay = new QHBoxLayout();
    fgLay->addWidget(new QLabel("Foreground:"));
    _fgColorPicker = new ColourPicker(props);
    fgLay->addWidget(_fgColorPicker);
    fgLay->addStretch();

    // Background colour
    QHBoxLayout* bgLay = new QHBoxLayout();
    bgLay->addWidget(new QLabel("Background:"));
    _bgColorPicker = new ColourPicker(props);
    bgLay->addWidget(_bgColorPicker);
    bgLay->addStretch();

    // Font
    QHBoxLayout* fontLay = new QHBoxLayout();
    fontLay->addWidget(new QLabel("Font:"));
    _fontCombo = new QFontComboBox(props);
    fontLay->addWidget(_fontCombo, 1);
    fontLay->addWidget(new QLabel("Size:"));
    _fontSizeSpin = new QSpinBox(props);
    _fontSizeSpin->setRange(4, 72);
    _fontSizeSpin->setValue(10);
    fontLay->addWidget(_fontSizeSpin);

    // Style flags
    QHBoxLayout* styleLay = new QHBoxLayout();
    _boldCheck = new QCheckBox("Bold", props);
    _italicCheck = new QCheckBox("Italic", props);
    _underlineCheck = new QCheckBox("Underline", props);
    styleLay->addWidget(_boldCheck);
    styleLay->addWidget(_italicCheck);
    styleLay->addWidget(_underlineCheck);
    styleLay->addStretch();

    propLay->addWidget(_styleNameLabel);
    propLay->addLayout(fgLay);
    propLay->addLayout(bgLay);
    propLay->addLayout(fontLay);
    propLay->addLayout(styleLay);
    propLay->addStretch();

    mainLay->addWidget(props, 2);

    // Connections
    connect(_styleList, &QListWidget::currentRowChanged,
            this, &StylesTab::onStyleSelectionChanged);
    connect(_fgColorPicker, &ColourPicker::colorPicked,
            this, &StylesTab::onFgColorClicked);
    connect(_bgColorPicker, &ColourPicker::colorPicked,
            this, &StylesTab::onBgColorClicked);
    connect(_fontCombo, &QFontComboBox::currentTextChanged,
            this, &StylesTab::onFontChanged);
    connect(_fontSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &StylesTab::onFontSizeChanged);
    connect(_boldCheck, &QCheckBox::toggled, this, &StylesTab::onBoldToggled);
    connect(_italicCheck, &QCheckBox::toggled, this, &StylesTab::onItalicToggled);
    connect(_underlineCheck, &QCheckBox::toggled, this, &StylesTab::onUnderlineToggled);
}

void StylesTab::loadFromParser(const UserLangParser& parser)
{
    QList<UdlStyle> srcStyles = parser.styles();
    _styleList->clear();
    _styles.clear();
    for (const UdlStyle& s : srcStyles) {
        UdlStyleItem item;
        item.styleId = s.styleId;
        item.name = s.name;
        item.fgColor = s.fgColor;
        item.bgColor = s.bgColor;
        item.fontName = s.fontName;
        item.fontSize = s.fontSize;
        item.bold = s.bold;
        item.italic = s.italic;
        item.underline = s.underline;
        _styles.append(item);
        _styleList->addItem(s.name);
    }
}

void StylesTab::saveToParser(UserLangParser& parser) const
{
    // Styles are applied directly via the ScintillaComponent after save
    Q_UNUSED(parser);
}

void StylesTab::onStyleSelectionChanged()
{
    int idx = _styleList->currentRow();
    if (idx < 0 || idx >= _styles.size())
        return;
    _selectedStyleIndex = idx;
    const UdlStyleItem& s = _styles[idx];
    _styleNameLabel->setText(QString("<b>%1</b>").arg(s.name));
    _fgColorPicker->setColor(s.fgColor.isValid() ? s.fgColor : Qt::black);
    _bgColorPicker->setColor(s.bgColor.isValid() ? s.bgColor : Qt::white);
    // Font would need the actual font family — default for now
    _boldCheck->setChecked(s.bold);
    _italicCheck->setChecked(s.italic);
    _underlineCheck->setChecked(s.underline);
    updateStyleControls();
}

void StylesTab::onFgColorClicked()
{
    if (_selectedStyleIndex < 0) return;
    _styles[_selectedStyleIndex].fgColor = _fgColorPicker->color();
    emit changed();
}

void StylesTab::onBgColorClicked()
{
    if (_selectedStyleIndex < 0) return;
    _styles[_selectedStyleIndex].bgColor = _bgColorPicker->color();
    emit changed();
}

void StylesTab::onFontChanged(const QString& family)
{
    if (_selectedStyleIndex < 0) return;
    _styles[_selectedStyleIndex].fontName = family;
    emit changed();
}

void StylesTab::onFontSizeChanged(int size)
{
    if (_selectedStyleIndex < 0) return;
    _styles[_selectedStyleIndex].fontSize = size;
    emit changed();
}

void StylesTab::onBoldToggled(bool checked)
{
    if (_selectedStyleIndex < 0) return;
    _styles[_selectedStyleIndex].bold = checked;
    emit changed();
}

void StylesTab::onItalicToggled(bool checked)
{
    if (_selectedStyleIndex < 0) return;
    _styles[_selectedStyleIndex].italic = checked;
    emit changed();
}

void StylesTab::onUnderlineToggled(bool checked)
{
    if (_selectedStyleIndex < 0) return;
    _styles[_selectedStyleIndex].underline = checked;
    emit changed();
}

void StylesTab::updateStyleControls()
{
    // Called when style selection changes to enable/disable controls
    bool hasSelection = _selectedStyleIndex >= 0;
    _fgColorPicker->setEnabled(hasSelection);
    _bgColorPicker->setEnabled(hasSelection);
    _fontCombo->setEnabled(hasSelection);
    _fontSizeSpin->setEnabled(hasSelection);
    _boldCheck->setEnabled(hasSelection);
    _italicCheck->setEnabled(hasSelection);
    _underlineCheck->setEnabled(hasSelection);
}

// ============================================================================
// OperatorsTab — Tab 3: Operators and Delimiters
// ============================================================================
OperatorsTab::OperatorsTab(QWidget* parent) : QWidget(parent)
{
    setupUi();
}

void OperatorsTab::setupUi()
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // Operators group
    QGroupBox* opGroup = new QGroupBox("Operators", this);
    QGridLayout* opGrid = new QGridLayout(opGroup);
    opGrid->addWidget(new QLabel("Set 1:"), 0, 0);
    _op1Edit = new QLineEdit(opGroup);
    _op1Edit->setPlaceholderText("e.g. +-*/%=<>!&|^~");
    opGrid->addWidget(_op1Edit, 0, 1);
    opGrid->addWidget(new QLabel("Set 2:"), 1, 0);
    _op2Edit = new QLineEdit(opGroup);
    _op2Edit->setPlaceholderText("additional operators");
    opGrid->addWidget(_op2Edit, 1, 1);
    mainLay->addWidget(opGroup);

    // Delimiters group
    QGroupBox* delimGroup = new QGroupBox("Delimiters", this);
    QGridLayout* delimGrid = new QGridLayout(delimGroup);
    delimGrid->addWidget(new QLabel("#"), 0, 0);
    delimGrid->addWidget(new QLabel("Open"), 0, 1);
    delimGrid->addWidget(new QLabel("Escape"), 0, 2);
    delimGrid->addWidget(new QLabel("Close"), 0, 3);

    _delimiters.reserve(8);
    for (int i = 0; i < 8; ++i) {
        delimGrid->addWidget(new QLabel(QString::number(i + 1)), i + 1, 0);
        DelimiterPair pair;
        pair.openEdit = new QLineEdit(delimGroup);
        pair.escapeEdit = new QLineEdit(delimGroup);
        pair.closeEdit = new QLineEdit(delimGroup);
        delimGrid->addWidget(pair.openEdit, i + 1, 1);
        delimGrid->addWidget(pair.escapeEdit, i + 1, 2);
        delimGrid->addWidget(pair.closeEdit, i + 1, 3);
        _delimiters.append(pair);
    }
    mainLay->addWidget(delimGroup);
    mainLay->addStretch();
}

void OperatorsTab::loadFromParser(const UserLangParser& parser)
{
    // Load Operators1 and Operators2
    QList<UdlKeywordSet> kss = parser.keywordSets();
    for (const UdlKeywordSet& ks : kss) {
        if (ks.name == QLatin1String("Operators1")) {
            _op1Edit->setText(ks.keywords.join(""));
        } else if (ks.name == QLatin1String("Operators2")) {
            _op2Edit->setText(ks.keywords.join(""));
        }
    }

    // Load Delimiters
    QList<UdlDelimiter> delims = parser.delimiters();
    for (int i = 0; i < 8 && i < delims.size(); ++i) {
        const UdlDelimiter& d = delims[i];
        if (d.open >= 0)
            _delimiters[i].openEdit->setText(QString(QChar(d.open)));
        if (d.escape >= 0)
            _delimiters[i].escapeEdit->setText(QString(QChar(d.escape)));
        if (d.close >= 0)
            _delimiters[i].closeEdit->setText(QString(QChar(d.close)));
    }
}

void OperatorsTab::saveToParser(UserLangParser& parser) const
{
    Q_UNUSED(parser);
}

// ============================================================================
// CommentsTab — Tab 4: Comments and Number formatting
// ============================================================================
CommentsTab::CommentsTab(QWidget* parent) : QWidget(parent)
{
    setupUi();
}

void CommentsTab::setupUi()
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // Stream comment
    QGroupBox* streamGroup = new QGroupBox("Stream Comment", this);
    QFormLayout* streamLay = new QFormLayout(streamGroup);
    _commentOpenEdit = new QLineEdit(streamGroup);
    _commentOpenEdit->setPlaceholderText("e.g. /*");
    _commentCloseEdit = new QLineEdit(streamGroup);
    _commentCloseEdit->setPlaceholderText("e.g. */");
    streamLay->addRow("Open:", _commentOpenEdit);
    streamLay->addRow("Close:", _commentCloseEdit);
    mainLay->addWidget(streamGroup);

    // Line comment
    QGroupBox* lineGroup = new QGroupBox("Line Comment", this);
    QFormLayout* lineLay = new QFormLayout(lineGroup);
    _lineCommentEdit = new QLineEdit(lineGroup);
    _lineCommentEdit->setPlaceholderText("e.g. // or #");
    lineLay->addRow("Prefix:", _lineCommentEdit);
    mainLay->addWidget(lineGroup);

    // Number formatting
    QGroupBox* numGroup = new QGroupBox("Number Formatting", this);
    QFormLayout* numLay = new QFormLayout(numGroup);

    // Prefix row
    QHBoxLayout* prefixLay = new QHBoxLayout();
    _numPrefix1Edit = new QLineEdit(numGroup);
    _numPrefix1Edit->setPlaceholderText("0x");
    _numPrefix2Edit = new QLineEdit(numGroup);
    _numPrefix2Edit->setPlaceholderText("0b");
    prefixLay->addWidget(new QLabel("Prefix 1:"));
    prefixLay->addWidget(_numPrefix1Edit);
    prefixLay->addWidget(new QLabel("Prefix 2:"));
    prefixLay->addWidget(_numPrefix2Edit);

    // Suffix row
    QHBoxLayout* suffixLay = new QHBoxLayout();
    _numSuffix1Edit = new QLineEdit(numGroup);
    _numSuffix1Edit->setPlaceholderText("L, U");
    _numSuffix2Edit = new QLineEdit(numGroup);
    _numSuffix2Edit->setPlaceholderText("ull");
    suffixLay->addWidget(new QLabel("Suffix 1:"));
    suffixLay->addWidget(_numSuffix1Edit);
    suffixLay->addWidget(new QLabel("Suffix 2:"));
    suffixLay->addWidget(_numSuffix2Edit);

    // Extras row
    QHBoxLayout* extrasLay = new QHBoxLayout();
    _numExtras1Edit = new QLineEdit(numGroup);
    _numExtras1Edit->setPlaceholderText("e.g. '");
    _numExtras2Edit = new QLineEdit(numGroup);
    extrasLay->addWidget(new QLabel("Extras 1:"));
    extrasLay->addWidget(_numExtras1Edit);
    extrasLay->addWidget(new QLabel("Extras 2:"));
    extrasLay->addWidget(_numExtras2Edit);

    // Range + decimal separator row
    QHBoxLayout* rangeLay = new QHBoxLayout();
    _numRangeEdit = new QLineEdit(numGroup);
    _numRangeEdit->setPlaceholderText("01");
    rangeLay->addWidget(new QLabel("Range:"));
    rangeLay->addWidget(_numRangeEdit);

    rangeLay->addWidget(new QLabel("Decimal:"));
    _decimalSepCombo = new QComboBox(numGroup);
    _decimalSepCombo->addItems({QStringLiteral("Dot (.)"),
                                 QStringLiteral("Comma (,)"),
                                 QStringLiteral("Both")});
    rangeLay->addWidget(_decimalSepCombo);
    rangeLay->addStretch();

    numLay->addRow(prefixLay);
    numLay->addRow(suffixLay);
    numLay->addRow(extrasLay);
    numLay->addRow(rangeLay);
    mainLay->addWidget(numGroup);
    mainLay->addStretch();
}

void CommentsTab::loadFromParser(const UserLangParser& parser)
{
    // Load stream and line comments from the "Comments" keyword list
    // Format: "(0<line> (1<open> (2<close>"
    QList<UdlKeywordSet> kss = parser.keywordSets();
    for (const UdlKeywordSet& ks : kss) {
        if (ks.name == QLatin1String("Comments")) {
            // The keywords list may contain "(0...", "(1...", "(2..." tokens
            const QString text = ks.keywords.join(" ");
            // Parse tokens like "(0//" "(1/*" "(2*/"
            QRegularExpression tokenRe(R"(\(\d)([^\s\(]*)");
            QRegularExpressionMatchIterator it = tokenRe.globalMatch(text);
            while (it.hasNext()) {
                QRegularExpressionMatch m = it.next();
                int type = m.captured(1).toInt();
                QString val = m.captured(2);
                if (type == 0) {
                    _lineCommentEdit->setText(val);
                } else if (type == 1) {
                    _commentOpenEdit->setText(val);
                } else if (type == 2) {
                    _commentCloseEdit->setText(val);
                }
            }
            break;
        }
    }

    // Load number format
    UqlNumberFormat nf = parser.numberFormat();
    _numPrefix1Edit->setText(nf.prefix1);
    _numPrefix2Edit->setText(nf.prefix2);
    _numSuffix1Edit->setText(nf.suffix1);
    _numSuffix2Edit->setText(nf.suffix2);
    _numExtras1Edit->setText(nf.extras1);
    _numExtras2Edit->setText(nf.extras2);
    _numRangeEdit->setText(nf.range);

    // Decimal separator
    int sep = parser.decimalSeparator();
    if (sep == 0) {
        _decimalSepCombo->setCurrentIndex(0); // dot
    } else if (sep == 1) {
        _decimalSepCombo->setCurrentIndex(1); // comma
    } else {
        _decimalSepCombo->setCurrentIndex(2); // both
    }
}

void CommentsTab::saveToParser(UserLangParser& parser) const
{
    Q_UNUSED(parser);
}

// ============================================================================
// UserDefineDialog — Main dialog
// ============================================================================
UserDefineDialog* UserDefineDialog::_instance = nullptr;

// static
UserDefineDialog* UserDefineDialog::getInstance()
{
    if (!_instance)
        _instance = new UserDefineDialog();
    return _instance;
}

UserDefineDialog::UserDefineDialog(QWidget* parent)
    : QDialog(parent)
    , _isDirty(false)
{
    setWindowTitle("User Defined Language");
    setMinimumSize(900, 600);
    setupUi();
    reloadLangList();
}

void UserDefineDialog::setupUi()
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // Top bar: language selector + extension
    QHBoxLayout* topLay = new QHBoxLayout();
    topLay->addWidget(new QLabel("Language:"));
    _langCombo = new QComboBox(this);
    _langCombo->setEditable(true);
    _langCombo->setMinimumWidth(200);
    topLay->addWidget(_langCombo, 1);

    QPushButton* newBtn = new QPushButton("+ New", this);
    topLay->addWidget(newBtn);

    QPushButton* delBtn = new QPushButton("Delete", this);
    topLay->addWidget(delBtn);
    delBtn->setObjectName("deleteButton");

    topLay->addWidget(new QLabel("Extension(s):"));
    _extEdit = new QLineEdit(this);
    _extEdit->setPlaceholderText("e.g. mylang or mylang1;mylang2");
    _extEdit->setMaximumWidth(200);
    topLay->addWidget(_extEdit);

    _caseIgnoreCheck = new QCheckBox("Case insensitive", this);
    topLay->addWidget(_caseIgnoreCheck);
    topLay->addStretch();
    mainLay->addLayout(topLay);

    // Tabs
    _tabWidget = new QTabWidget(this);
    _keywordsTab = new KeywordsTab(_tabWidget);
    _stylesTab = new StylesTab(_tabWidget);
    _operatorsTab = new OperatorsTab(_tabWidget);
    _commentsTab = new CommentsTab(_tabWidget);

    _tabWidget->addTab(_keywordsTab, "Keywords");
    _tabWidget->addTab(_stylesTab, "Styles");
    _tabWidget->addTab(_operatorsTab, "Operators & Delimiters");
    _tabWidget->addTab(_commentsTab, "Comments & Numbers");
    mainLay->addWidget(_tabWidget, 1);

    // Bottom buttons
    QHBoxLayout* btnLay = new QHBoxLayout();
    _defaultBtn = new QPushButton("Default", this);
    _importBtn = new QPushButton("Import", this);
    _exportBtn = new QPushButton("Export", this);
    _removeBtn = new QPushButton("Remove", this);
    _saveBtn = new QPushButton("Save & Close", this);
    _saveBtn->setDefault(true);
    _dockBtn = new QPushButton("As Dockable", this);

    btnLay->addWidget(_defaultBtn);
    btnLay->addWidget(_importBtn);
    btnLay->addWidget(_exportBtn);
    btnLay->addWidget(_removeBtn);
    btnLay->addStretch();
    btnLay->addWidget(_dockBtn);
    btnLay->addWidget(_saveBtn);

    _applyBtn = new QPushButton("Apply", this);
    btnLay->addWidget(_applyBtn);

    _okBtn = new QPushButton("OK", this);
    _okBtn->setDefault(false);
    _cancelBtn = new QPushButton("Cancel", this);
    btnLay->addWidget(_okBtn);
    btnLay->addWidget(_cancelBtn);
    mainLay->addLayout(btnLay);

    // Connections
    connect(newBtn, &QPushButton::clicked, this, &UserDefineDialog::onNewClicked);
    connect(delBtn, &QPushButton::clicked, this, &UserDefineDialog::onRemoveClicked);
    connect(_langCombo, QOverload<int>::of(&QComboBox::activated),
            this, &UserDefineDialog::onLanguageChanged);
    connect(_saveBtn, &QPushButton::clicked, this, &UserDefineDialog::onSaveClicked);
    connect(_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(_okBtn, &QPushButton::clicked, this, &UserDefineDialog::onOkClicked);
    connect(_applyBtn, &QPushButton::clicked, this, &UserDefineDialog::onApplyClicked);
    connect(_defaultBtn, &QPushButton::clicked, this, &UserDefineDialog::onDefaultClicked);
    connect(_importBtn, &QPushButton::clicked, this, &UserDefineDialog::onImportClicked);
    connect(_exportBtn, &QPushButton::clicked, this, &UserDefineDialog::onExportClicked);
    connect(_dockBtn, &QPushButton::clicked, this, &UserDefineDialog::onDockToggleClicked);
    connect(_tabWidget, &QTabWidget::currentChanged,
            this, &UserDefineDialog::onTabChanged);
    connect(_keywordsTab, &KeywordsTab::changed, [this]() { _isDirty = true; });
    connect(_stylesTab, &StylesTab::changed, [this]() { _isDirty = true; });
    connect(_operatorsTab, &OperatorsTab::changed, [this]() { _isDirty = true; });
    connect(_commentsTab, &CommentsTab::changed, [this]() { _isDirty = true; });
    connect(_extEdit, &QLineEdit::textChanged,
            this, &UserDefineDialog::onExtChanged);
    connect(_caseIgnoreCheck, &QCheckBox::toggled, [this]() { _isDirty = true; });

    // Dark mode
    NppDarkMode::instance().applyToWidget(this);
}

void UserDefineDialog::onNewClicked()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, "New Language",
        "Enter language name:", QLineEdit::Normal, QString(), &ok);
    if (!ok || name.isEmpty())
        return;

    // Check for duplicates
    if (_langCombo->findText(name) >= 0) {
        QMessageBox::warning(this, "Error", "A language with this name already exists.");
        return;
    }

    newLanguage(name);
    _langCombo->addItem(name);
    _langCombo->setCurrentText(name);
}

void UserDefineDialog::newLanguage(const QString& name)
{
    _currentLangName = name;
    _currentLangExt.clear();
    _isDirty = true;
    // Reset tabs to empty
    _keywordsTab->clearAll();
    _caseIgnoreCheck->setChecked(false);
    _extEdit->clear();
}

void UserDefineDialog::onLanguageChanged(int index)
{
    if (index < 0)
        return;
    QString name = _langCombo->itemText(index);
    if (_isDirty) {
        QMessageBox::StandardButton btn = QMessageBox::question(this,
            "Unsaved Changes",
            QString("Save changes to \"%1\"?").arg(_currentLangName),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (btn == QMessageBox::Save) {
            onSaveClicked();
        } else if (btn == QMessageBox::Cancel) {
            // Revert selection
            _langCombo->setCurrentText(_currentLangName);
            return;
        }
    }
    setLanguage(name);
}

void UserDefineDialog::setLanguage(const QString& name)
{
    if (!loadUdlFile(name))
        return;
    _currentLangName = name;
    populateTabsFromParser();
    _isDirty = false;
}

QString UserDefineDialog::getLanguageName() const
{
    return _currentLangName;
}

bool UserDefineDialog::loadUdlFile(const QString& name)
{
    QString dirPath = udlDirectory();
    QDir dir(dirPath);
    if (!dir.exists())
        dir.mkpath(dirPath);

    // Find the file — format: "name..udl.xml"
    QString fileName = name.simplified().replace(' ', '_') + "..udl.xml";
    QString filePath = dir.filePath(fileName);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QByteArray data = file.readAll();
    file.close();

    return parseXml(QString::fromUtf8(data));
}

bool UserDefineDialog::parseXml(const QString& xmlData)
{
    return _parser.loadFromData(xmlData.toUtf8());
}

void UserDefineDialog::populateTabsFromParser()
{
    _keywordsTab->loadFromParser(_parser);
    _stylesTab->loadFromParser(_parser);
    _operatorsTab->loadFromParser(_parser);
    _commentsTab->loadFromParser(_parser);
}

void UserDefineDialog::collectTabsToParser()
{
    // Data flows directly from tabs → serializeToXml()
    // No parser mutation needed (parser is read-only)
    Q_UNUSED(_parser);
}

static QString colorToHex(const QColor& c)
{
    if (!c.isValid())
        return QStringLiteral("000000");
    return QStringLiteral("%1%2%3")
        .arg(c.red(),   2, 16, QLatin1Char('0'))
        .arg(c.green(), 2, 16, QLatin1Char('0'))
        .arg(c.blue(),  2, 16, QLatin1Char('0'))
        .toUpper();
}

QString UserDefineDialog::serializeToXml() const
{
    // Build UDL XML directly from tab data
    QString xml;
    QTextStream out(&xml);

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<NotepadPlus>\n";
    out << "  <UserLang name=\"" << _currentLangName << "\""
        << " ext=\"" << _currentLangExt << "\""
        << " udlVersion=\"2.1\">\n";

    // Settings
    out << "    <Settings>\n";
    out << "      <Global caseIgnored=\""
        << (_caseIgnoreCheck->isChecked() ? "yes" : "no")
        << "\" allowFoldOfComments=\"no\""
        << " foldCompact=\"no\""
        << " forcePureLC=\"0\""
        << " decimalSeparator=\"0\" />\n";
    out << "    </Settings>\n";

    // KeywordLists
    out << "    <KeywordLists>\n";

    // Comments
    {
        QString lineC = _commentsTab->lineComment();
        QString openC = _commentsTab->commentOpen();
        QString closeC = _commentsTab->commentClose();
        QString commentStr;
        if (!lineC.isEmpty())
            commentStr += "(0" + lineC + " ";
        if (!openC.isEmpty())
            commentStr += "(1" + openC + " ";
        if (!closeC.isEmpty())
            commentStr += "(2" + closeC;
        out << "      <Keywords name=\"Comments\">"
            << commentStr.toHtmlEscaped() << "</Keywords>\n";
    }

    // Number format entries
    out << "      <Keywords name=\"Numbers, prefix1\">"
        << _commentsTab->numPrefix1().toHtmlEscaped() << "</Keywords>\n";
    out << "      <Keywords name=\"Numbers, prefix2\">"
        << _commentsTab->numPrefix2().toHtmlEscaped() << "</Keywords>\n";
    out << "      <Keywords name=\"Numbers, extras1\">"
        << _commentsTab->numExtras1().toHtmlEscaped() << "</Keywords>\n";
    out << "      <Keywords name=\"Numbers, extras2\">"
        << _commentsTab->numExtras2().toHtmlEscaped() << "</Keywords>\n";
    out << "      <Keywords name=\"Numbers, suffix1\">"
        << _commentsTab->numSuffix1().toHtmlEscaped() << "</Keywords>\n";
    out << "      <Keywords name=\"Numbers, suffix2\">"
        << _commentsTab->numSuffix2().toHtmlEscaped() << "</Keywords>\n";
    out << "      <Keywords name=\"Numbers, range\">"
        << _commentsTab->numRange().toHtmlEscaped() << "</Keywords>\n";

    // Operators
    out << "      <Keywords name=\"Operators1\">"
        << _operatorsTab->op1().toHtmlEscaped() << "</Keywords>\n";
    out << "      <Keywords name=\"Operators2\">"
        << _operatorsTab->op2().toHtmlEscaped() << "</Keywords>\n";

    // Folders (empty in basic version)
    QStringList folderTypes = {QStringLiteral("open"), QStringLiteral("middle"), QStringLiteral("close")};
    for (int i = 1; i <= 3; ++i) {
        for (const QString& t : folderTypes) {
            out << "      <Keywords name=\"Folders in code1, " << t << "\"></Keywords>\n";
        }
    }
    for (int i = 1; i <= 3; ++i) {
        for (const QString& t : folderTypes) {
            out << "      <Keywords name=\"Folders in code2, " << t << "\"></Keywords>\n";
        }
    }
    for (int i = 1; i <= 3; ++i) {
        for (const QString& t : folderTypes) {
            out << "      <Keywords name=\"Folders in comment, " << t << "\"></Keywords>\n";
        }
    }

    // Keywords 1-8
    QList<KeywordsTab::KeywordSetData> kwSets = _keywordsTab->keywordSets();
    for (int i = 0; i < kwSets.size(); ++i) {
        const auto& d = kwSets[i];
        QString name = d.name;
        if (name.isEmpty())
            name = QStringLiteral("Keywords%1").arg(i + 1);
        out << "      <Keywords name=\"" << name << "\">"
            << d.keywords.toHtmlEscaped() << "</Keywords>\n";
    }

    // Delimiters: "OO CC EE OO CC EE ..."
    {
        QString delimStr;
        QVector<OperatorsTab::DelimiterPair> delims = _operatorsTab->delimiters();
        for (int i = 0; i < 8 && i < delims.size(); ++i) {
            const auto& p = delims[i];
            QString open  = p.openEdit->text();
            QString esc   = p.escapeEdit->text();
            QString close = p.closeEdit->text();
            if (!open.isEmpty() || !close.isEmpty()) {
                delimStr += QStringLiteral("%1").arg(i * 3, 2, 10, QLatin1Char('0'))
                          + open + " "
                          + QStringLiteral("%1").arg(i * 3 + 1, 2, 10, QLatin1Char('0'))
                          + esc + " "
                          + QStringLiteral("%1").arg(i * 3 + 2, 2, 10, QLatin1Char('0'))
                          + close + " ";
            }
        }
        out << "      <Keywords name=\"Delimiters\">" << delimStr << "</Keywords>\n";
    }

    out << "    </KeywordLists>\n";

    // Styles
    out << "    <Styles>\n";
    QList<UdlStyleItem> styles = _stylesTab->styleItems();
    for (const UdlStyleItem& st : styles) {
        out << "      <WordsStyle name=\"" << st.name << "\""
            << " fgColor=\"" << colorToHex(st.fgColor) << "\""
            << " bgColor=\"" << colorToHex(st.bgColor) << "\""
            << " colorStyle=\"0\""
            << " fontName=\"" << st.fontName << "\""
            << " fontSize=\"" << st.fontSize << "\""
            << " fontStyle=\"" << st.toFontStyle() << "\""
            << " nesting=\"0\" />\n";
    }
    out << "    </Styles>\n";
    out << "  </UserLang>\n";
    out << "</NotepadPlus>\n";

    return xml;
}

void UserDefineDialog::onSaveClicked()
{
    // Collect data from tabs
    // _parser is already populated from tabs

    // Save the XML
    if (!saveUdlFile(_currentLangName)) {
        QMessageBox::warning(this, "Error",
            QString("Failed to save language \"%1\"").arg(_currentLangName));
        return;
    }

    _isDirty = false;
    emit languageSaved(_currentLangName);
    accept();
}

bool UserDefineDialog::saveUdlFile(const QString& name)
{
    collectTabsToParser();

    QString dirPath = udlDirectory();
    QDir dir(dirPath);
    if (!dir.exists())
        dir.mkpath(dirPath);

    QString fileName = name.simplified().replace(' ', '_') + "..udl.xml";
    QString filePath = dir.filePath(fileName);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    QByteArray data = serializeToXml().toUtf8();
    file.write(data);
    file.close();
    return true;
}

void UserDefineDialog::reloadLangList()
{
    QString prev = _langCombo->currentText();
    _langCombo->clear();

    QString dirPath = udlDirectory();
    QDir dir(dirPath);
    if (dir.exists()) {
        QStringList filters;
        filters << "*..udl.xml";
        QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
        for (const QFileInfo& fi : files) {
            QString name = fi.baseName();  // strips ._userDefinedLang
            _langCombo->addItem(name);
        }
    }

    if (!prev.isEmpty() && _langCombo->findText(prev) >= 0)
        _langCombo->setCurrentText(prev);
}

QString UserDefineDialog::udlDirectory() const
{
    // Standard location: configDir/userDefineLangs/
    QString configDir = QCoreApplication::applicationDirPath() + "/userDefineLangs";
    return configDir;
}

void UserDefineDialog::onRemoveClicked()
{
    int idx = _langCombo->currentIndex();
    if (idx < 0)
        return;
    QString name = _langCombo->itemText(idx);

    QMessageBox::StandardButton btn = QMessageBox::question(this,
        "Delete Language",
        QString("Delete the language \"%1\"? This cannot be undone.").arg(name),
        QMessageBox::Yes | QMessageBox::No);
    if (btn != QMessageBox::Yes)
        return;

    // Find and delete the file
    QString dirPath = udlDirectory();
    QString fileName = name.simplified().replace(' ', '_') + "..udl.xml";
    QString filePath = QDir(dirPath).filePath(fileName);
    QFile::remove(filePath);

    _langCombo->removeItem(idx);
    if (_langCombo->count() > 0)
        _langCombo->setCurrentIndex(0);
}

void UserDefineDialog::onDefaultClicked()
{
    QMessageBox::StandardButton btn = QMessageBox::question(this,
        "Reset to Defaults",
        "Reset all settings to default values?",
        QMessageBox::Yes | QMessageBox::No);
    if (btn == QMessageBox::Yes) {
        _parser = UserLangParser();  // reset to empty parser
        _currentLangExt.clear();
        populateTabsFromParser();
        _isDirty = true;
    }
}

void UserDefineDialog::onImportClicked()
{
    QString path = QFileDialog::getOpenFileName(this,
        "Import UDL", QString(), "UDL Files (*..udl.xml);;All Files (*)");
    if (path.isEmpty())
        return;

    if (!importFromFile(path)) {
        QMessageBox::warning(this, "Import Error", "Failed to import the UDL file.");
    }
}

bool UserDefineDialog::importFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QByteArray data = file.readAll();
    file.close();

    // Parse to get the language name
    UserLangParser tempParser;
    if (!tempParser.loadFromData(data))
        return false;

    QString name = tempParser.languageName();
    if (name.isEmpty())
        name = QFileInfo(path).baseName();

    // Avoid duplicates
    int suffix = 1;
    QString baseName = name;
    while (_langCombo->findText(name) >= 0) {
        name = QString("%1 (%2)").arg(baseName).arg(suffix++);
    }

    // Save to our UDL directory
    newLanguage(name);
    if (!parseXml(QString::fromUtf8(data)))
        return false;

    if (!saveUdlFile(name)) {
        return false;
    }

    _langCombo->addItem(name);
    _langCombo->setCurrentText(name);
    return true;
}

void UserDefineDialog::onExportClicked()
{
    int idx = _langCombo->currentIndex();
    if (idx < 0) {
        QMessageBox::information(this, "Export", "Please select a language to export.");
        return;
    }

    QString name = _langCombo->itemText(idx);
    QString defaultFile = name.simplified().replace(' ', '_') + "..udl.xml";

    QString path = QFileDialog::getSaveFileName(this,
        "Export UDL", defaultFile,
        "UDL Files (*..udl.xml);;All Files (*)");
    if (path.isEmpty())
        return;

    if (!exportToFile(path)) {
        QMessageBox::warning(this, "Export Error", "Failed to export the UDL file.");
    }
}

bool UserDefineDialog::exportToFile(const QString& path)
{
    collectTabsToParser();
    QByteArray data = serializeToXml().toUtf8();
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    file.write(data);
    file.close();
    return true;
}

void UserDefineDialog::onDockToggleClicked()
{
    _isDocked = !_isDocked;
    _dockBtn->setText(_isDocked ? "As Floating" : "As Dockable");
}

void UserDefineDialog::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    reloadLangList();
    if (!isVisible())
        exec();
}

void UserDefineDialog::destroy()
{
    close();
    deleteLater();
}

QList<UdlStyleItem> UserDefineDialog::buildStyleItems() const
{
    return _stylesTab->styleItems();
}

void UserDefineDialog::deleteCurrentLanguage()
{
    if (_currentLangName.isEmpty())
        return;
    QString filePath = currentLangFilePath();
    if (!filePath.isEmpty()) {
        QFile::remove(filePath);
    }
    // Remove from the combo box
    int idx = _langCombo->findText(_currentLangName);
    if (idx >= 0)
        _langCombo->removeItem(idx);
    _currentLangName.clear();
    _isDirty = false;
    // Select another or clear
    if (_langCombo->count() > 0) {
        _langCombo->setCurrentIndex(0);
        setLanguage(_langCombo->itemText(0));
    }
}

QString UserDefineDialog::currentLangFilePath() const
{
    if (_currentLangName.isEmpty())
        return QString();
    QString dirPath = udlDirectory();
    QString fileName = _currentLangName.simplified().replace(' ', '_') + "..udl.xml";
    return QDir(dirPath).filePath(fileName);
}

void UserDefineDialog::renameLanguage(const QString& newName)
{
    if (_currentLangName.isEmpty() || newName.isEmpty())
        return;
    QString oldPath = currentLangFilePath();
    QString dirPath = udlDirectory();
    QString newFileName = newName.simplified().replace(' ', '_') + "..udl.xml";
    QString newPath = QDir(dirPath).filePath(newFileName);

    // Rename the file
    QFile file(oldPath);
    if (file.exists()) {
        file.rename(newPath);
    }

    _originalLangName = _currentLangName;
    _currentLangName = newName;

    // Update combo box
    int idx = _langCombo->findText(_originalLangName);
    if (idx >= 0)
        _langCombo->setItemText(idx, newName);
    _langCombo->setCurrentText(newName);
}

void UserDefineDialog::onApplyClicked()
{
    // Apply changes without closing
    collectTabsToParser();
    if (!saveUdlFile(_currentLangName)) {
        QMessageBox::warning(this, "Error",
            QString("Failed to save language \"%1\"").arg(_currentLangName));
        return;
    }
    _isDirty = false;
    emit languageSaved(_currentLangName);
}

void UserDefineDialog::onOkClicked()
{
    onApplyClicked();
    accept();
}

void UserDefineDialog::onTabChanged(int index)
{
    Q_UNUSED(index);
    // Could trigger style preview update or validation here
}

void UserDefineDialog::onExtChanged(const QString& text)
{
    _currentLangExt = text;
    _isDirty = true;
}

void UserDefineDialog::setDockStatus(bool isDocked)
{
    _isDocked = isDocked;
    _dockBtn->setText(_isDocked ? QStringLiteral("As Floating") : QStringLiteral("As Dockable"));
}
