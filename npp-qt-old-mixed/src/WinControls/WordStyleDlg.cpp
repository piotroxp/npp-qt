// Semantic Lift: Win32 WordStyleDlg → Qt6 WordStyleDlg
// Original: PowerEditor/src/WinControls/ColourPicker/WordStyleDlg.cpp
// Target: npp-qt/src/WinControls/WordStyleDlg.cpp

#include "WordStyleDlg.h"
#include "NppDarkMode.h"
#include "NppConstants.h"
#include "Parameters.h"
#include "ColourPicker.h"
#include "ColourPicker/ColourPopup.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QColorDialog>
#include <QApplication>
#include <QHeaderView>

// =============================================================================
// StyleArray_ — stub implementations
// =============================================================================

StyleItem_* StyleArray_::findByName(const QString& name)
{
    for (auto& item : *this) {
        if (item.name == name) return &item;
    }
    return nullptr;
}

void StyleArray_::addStyler(int id, const StyleItem_& si)
{
    Q_UNUSED(id);
    push_back(si);
}

void StyleArray_::addStylerFromNode(const QString& nodeName)
{
    Q_UNUSED(nodeName);
    // Parsing XML style nodes — stub; real implementation in Parameters.cpp
}

// ---------------------------------------------------------------------------
// Parse a <WordsStyle> node from QXmlStreamReader
// Reads: name, fgColor, bgColor, fontName, fontSize, fontStyle, colorStyle
// fontStyle bits: 1=bold, 2=italic, 4=underline
// ---------------------------------------------------------------------------
void StyleArray_::addStylerFromNode(QXmlStreamReader& reader, const QString& nodeName)
{
    StyleItem_ si;
    si.name = nodeName;

    auto attrs = reader.attributes();

    // Foreground colour
    if (attrs.hasAttribute(QStringLiteral("fgColor"))) {
        QString val = attrs.value(QStringLiteral("fgColor")).toString();
        si.fgColor = parseNppColor(val);
    }

    // Background colour
    if (attrs.hasAttribute(QStringLiteral("bgColor"))) {
        QString val = attrs.value(QStringLiteral("bgColor")).toString();
        si.bgColor = parseNppColor(val);
    }

    // Colour style flags (COLORSTYLE_*)
    if (attrs.hasAttribute(QStringLiteral("colorStyle"))) {
        // 0x01 = foreground, 0x02 = background
        bool ok = false;
        int cs = attrs.value(QStringLiteral("colorStyle")).toInt(&ok, 16);
        if (ok) {
            si.colorStyle = cs;
        }
    }

    // Font name
    if (attrs.hasAttribute(QStringLiteral("fontName"))) {
        si.fontName = attrs.value(QStringLiteral("fontName")).toString();
        si.fontEnabled = true;
    }

    // Font size
    if (attrs.hasAttribute(QStringLiteral("fontSize"))) {
        bool ok = false;
        int sz = attrs.value(QStringLiteral("fontSize")).toInt(&ok);
        if (ok) si.fontSize = sz;
    }

    // Font style bits
    if (attrs.hasAttribute(QStringLiteral("fontStyle"))) {
        bool ok = false;
        int fs = attrs.value(QStringLiteral("fontStyle")).toInt(&ok);
        if (ok) si.fontStyle = fs;
    }

    // Keywords
    if (attrs.hasAttribute(QStringLiteral("keywords"))) {
        si.keywords = attrs.value(QStringLiteral("keywords")).toString();
    }

    // File extensions
    if (attrs.hasAttribute(QStringLiteral("ext"))) {
        si.ext = attrs.value(QStringLiteral("ext")).toString();
    }

    push_back(si);
}

// ---------------------------------------------------------------------------
// Helper: parse a hex colour string like "FF0000" into a QColor
// Handles both 6-char (RGB) and 8-char (RGBA) formats
// ---------------------------------------------------------------------------
static QColor parseNppColor(const QString& hex)
{
    if (hex.isEmpty()) return QColor();
    QString s = hex;
    if (s.startsWith(QLatin1Char('#'))) s = s.mid(1);
    if (s.length() == 6) {
        bool ok1 = false, ok2 = false, ok3 = false;
        int r = s.mid(0, 2).toInt(&ok1, 16);
        int g = s.mid(2, 2).toInt(&ok2, 16);
        int b = s.mid(4, 2).toInt(&ok3, 16);
        if (ok1 && ok2 && ok3) return QColor(r, g, b);
    } else if (s.length() == 8) {
        bool ok1 = false, ok2 = false, ok3 = false, ok4 = false;
        int r = s.mid(0, 2).toInt(&ok1, 16);
        int g = s.mid(2, 2).toInt(&ok2, 16);
        int b = s.mid(4, 2).toInt(&ok3, 16);
        int a = s.mid(6, 2).toInt(&ok4, 16);
        if (ok1 && ok2 && ok3 && ok4) return QColor(r, g, b, a);
    }
    return QColor();
}

// =============================================================================
// Default global styles (until Parameters.cpp feedStylerArray is wired)
// =============================================================================

static StyleItem_ makeSI(const char* name, const QColor& fg, const QColor& bg,
                          int fontSize = 8, int fontStyle = 0)
{
    StyleItem_ si;
    si.name = QString::fromLatin1(name);
    si.fgColor = fg;
    si.bgColor = bg;
    si.fontSize = fontSize;
    si.fontStyle = fontStyle;
    return si;
}

static StyleArray_ defaultGlobalStyles()
{
    StyleArray_ a;
    a.push_back(makeSI("Global override",       QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Default Style",         QColor(  0,   0,   0), QColor(255, 255, 255), 8));
    a.push_back(makeSI("Linked comment",        QColor(128, 128, 128), QColor(255, 255, 255)));
    a.push_back(makeSI("Comment",               QColor(  0, 128,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Comment line",          QColor(  0, 128,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Number",                QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("INSTRUCTION",           QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Keyword",               QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("User-defined keyword",  QColor(128,   0, 128), QColor(255, 255, 255)));
    a.push_back(makeSI("String",                QColor(163,  21,  21), QColor(255, 255, 255)));
    a.push_back(makeSI("Character",             QColor(163,  21,  21), QColor(255, 255, 255)));
    a.push_back(makeSI("Operator",              QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Folder",                QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Variable",              QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Tag",                   QColor(  0,   0, 128), QColor(255, 255, 255)));
    a.push_back(makeSI("Attribute name",        QColor(136,  18,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Attribute value",       QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Section",               QColor(  0,  60,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Heading 1",             QColor(  0,  60,   0), QColor(255, 255, 255), 14, 1));
    a.push_back(makeSI("Heading 2",             QColor(  0,  60,   0), QColor(255, 255, 255), 13, 1));
    a.push_back(makeSI("Heading 3",             QColor(  0,  60,   0), QColor(255, 255, 255), 12, 1));
    a.push_back(makeSI("Heading 4",             QColor(  0,  60,   0), QColor(255, 255, 255), 11, 1));
    a.push_back(makeSI("Heading 5",             QColor(  0,  60,   0), QColor(255, 255, 255), 10, 1));
    a.push_back(makeSI("Heading 6",             QColor(  0,  60,   0), QColor(255, 255, 255),  9, 1));
    a.push_back(makeSI("URL",                   QColor(  0,   0, 255), QColor(255, 255, 255)));
    a.push_back(makeSI("Link",                  QColor(  0,   0, 255), QColor(255, 255, 255),  8, 4));
    a.push_back(makeSI("Bold",                  QColor(  0,   0,   0), QColor(255, 255, 255),  8, 1));
    a.push_back(makeSI("Italic",                QColor(  0,   0,   0), QColor(255, 255, 255),  8, 2));
    a.push_back(makeSI("Underline",             QColor(  0,   0,   0), QColor(255, 255, 255),  8, 4));
    a.push_back(makeSI("Smart Highlighting",    QColor(255, 255,   0), QColor( 80,  80,   0)));
    a.push_back(makeSI("Mark style 1",          QColor(255, 102, 102), QColor(255, 255, 255)));
    a.push_back(makeSI("Mark style 2",          QColor(  0, 200,  80), QColor(255, 255, 255)));
    a.push_back(makeSI("Mark style 3",          QColor( 80,  80, 255), QColor(255, 255, 255)));
    a.push_back(makeSI("Mark style 4",          QColor(255, 128,  64), QColor(255, 255, 255)));
    a.push_back(makeSI("Mark style 5",          QColor(200,  80, 255), QColor(255, 255, 255)));
    a.push_back(makeSI("Search result",         QColor(255, 255, 255), QColor(  0,   0,   0)));
    a.push_back(makeSI("Selected text colour",  QColor(255, 255, 255), QColor( 30,  30, 200)));
    a.push_back(makeSI("Hilited text",          QColor(255, 255, 255), QColor(255, 200,   0)));
    a.push_back(makeSI("Caret colour",          QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Current line background colour", QColor(  0,   0,   0), QColor(238, 238, 221)));
    a.push_back(makeSI("Colour of the edge",    QColor(  0,   0, 128), QColor(255, 255, 255)));
    a.push_back(makeSI("Line number margin",    QColor(  0,   0,   0), QColor(221, 221, 221)));
    a.push_back(makeSI("Bookmark margin",       QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Bookmark",              QColor(255,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Folding - mark",        QColor(255, 255, 255), QColor(160, 160, 164)));
    a.push_back(makeSI("Fold line",             QColor(160, 160, 164), QColor(255, 255, 255)));
    a.push_back(makeSI("Fold header",           QColor(255, 255, 255), QColor(160, 160, 164)));
    a.push_back(makeSI("Fold active",           QColor(255, 255, 255), QColor( 64,  64,  64)));
    a.push_back(makeSI("Active tab focused indicator",   QColor(255, 165,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Active tab unfocused indicator",  QColor(128,  64,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Active tab text",       QColor(  0,   0,   0), QColor(255, 255, 255)));
    a.push_back(makeSI("Inactive tab text",     QColor(128, 128, 128), QColor(255, 255, 255)));
    a.push_back(makeSI("Increment highlight",   QColor(  0,   0,   0), QColor(255, 255,   0)));
    a.push_back(makeSI("Multi-edit carets color",QColor(255, 255, 255), QColor(255,   0, 255)));
    a.push_back(makeSI("Multi-selected text",   QColor(255, 255, 255), QColor(128,   0, 255)));
    a.push_back(makeSI("Brace light",           QColor(  0,   0,   0), QColor(255, 255,   0)));
    a.push_back(makeSI("Brace bad",             QColor(255,   0,   0), QColor(255, 255, 255),  8, 4));
    a.push_back(makeSI("Tags match highlighting",QColor(255, 255,   0), QColor(128, 128,   0)));
    a.push_back(makeSI("Highlight line",        QColor(  0,   0,   0), QColor(232, 232, 255)));
    a.push_back(makeSI("URL hovered",           QColor(  0,   0,   0), QColor(238, 238, 221),  8, 4));
    a.push_back(makeSI("EOL custom color",      QColor(192, 192, 192), QColor(255, 255, 255)));
    a.push_back(makeSI("Change History saved",  QColor(  0,   0,   0), QColor(200, 240, 200)));
    a.push_back(makeSI("Change History modified",QColor(  0,   0,   0), QColor(240, 200, 200)));
    a.push_back(makeSI("Change History revert modified",QColor(  0,   0,   0), QColor(200, 200, 240)));
    a.push_back(makeSI("Change History revert origin", QColor(  0,   0,   0), QColor(220, 220, 220)));
    a.push_back(makeSI("Preprocessor",          QColor(128,   0, 128), QColor(255, 255, 255)));
    a.push_back(makeSI("Script line",           QColor(  0,   0,   0), QColor(255, 255, 255)));
    return a;
}

// =============================================================================
// WordStyleDlg
// =============================================================================

WordStyleDlg::WordStyleDlg()
    : StaticDialog()
    , _pFgColour(nullptr)
    , _pBgColour(nullptr)
    , _langCombo(nullptr)
    , _styleList(nullptr)
    , _themeCombo(nullptr)
    , _boldCheck(nullptr)
    , _italicCheck(nullptr)
    , _underlineCheck(nullptr)
    , _fontNameCombo(nullptr)
    , _fontSizeCombo(nullptr)
    , _fgColourLabel(nullptr)
    , _bgColourLabel(nullptr)
    , _fontNameLabel(nullptr)
    , _fontSizeLabel(nullptr)
    , _styleInfoLabel(nullptr)
    , _globalOverrideLink(nullptr)
    , _globalOverrideTip(nullptr)
    , _extEdit(nullptr)
    , _keywordsEdit(nullptr)
    , _currentLexerIndex(0)
    , _currentThemeIndex(0)
    , _restoreInvalid(false)
    , _isDirty(false)
    , _isThemeDirty(false)
    , _isShownGOCtrls(false)
    , _isThemeChanged(false)
    , _currentStyleIndex(-1)
{
    _globalStyles = defaultGlobalStyles();
}

WordStyleDlg::~WordStyleDlg()
{
    if (_globalOverrideTip) {
        _globalOverrideTip->deleteLater();
        _globalOverrideTip = nullptr;
    }
    delete _pFgColour;
    delete _pBgColour;
}

void WordStyleDlg::create(int dialogID, bool isRTL, bool msgDestParent, unsigned short fontSize)
{
    Q_UNUSED(dialogID);
    Q_UNUSED(fontSize);
    Q_UNUSED(msgDestParent);
    StaticDialog::create(0, isRTL);
    Q_UNUSED(isRTL);
}

void WordStyleDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    if (!window()) {
        buildUI();
    }
    show();
    raise();
    activateWindow();
}

void WordStyleDlg::buildUI()
{
    setWindowTitle(QStringLiteral("Style Configurator"));
    setMinimumSize(620, 480);
    resize(720, 560);

    // ── Main layout ────────────────────────────────────────────────────────
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    // ── Theme switcher row ─────────────────────────────────────────────────
    {
        QHBoxLayout* row = new QHBoxLayout;
        row->addWidget(new QLabel(QStringLiteral("Theme:")));
        _themeCombo = new QComboBox;
        _themeCombo->addItem(QStringLiteral("Default (stylers.xml)"));
        _themeCombo->addItem(QStringLiteral("Dark Mode"));
        _themeCombo->addItem(QStringLiteral("Light Mode"));
        connect(_themeCombo, &QComboBox::currentIndexChanged, this, &WordStyleDlg::switchToTheme);
        row->addWidget(_themeCombo, 1);
        row->addStretch();
        mainLayout->addLayout(row);
    }

    // ── Language selector row ──────────────────────────────────────────────
    {
        QHBoxLayout* row = new QHBoxLayout;
        row->addWidget(new QLabel(QStringLiteral("Language:")));
        _langCombo = new QComboBox;
        connect(_langCombo, &QComboBox::currentIndexChanged, this, &WordStyleDlg::onLangChanged);
        row->addWidget(_langCombo, 1);

        row->addWidget(new QLabel(QStringLiteral("Ext:")));
        _extEdit = new QLineEdit;
        _extEdit->setPlaceholderText(QStringLiteral("File extensions..."));
        _extEdit->setMaximumWidth(180);
        connect(_extEdit, &QLineEdit::textEdited, this, &WordStyleDlg::updateExtension);
        row->addWidget(_extEdit);
        mainLayout->addLayout(row);
    }

    // ── Style list (left) + properties panel (right) ──────────────────────
    {
        QHBoxLayout* row = new QHBoxLayout;
        row->setSpacing(10);

        // Style list
        _styleList = new QListWidget;
        _styleList->setMinimumWidth(210);
        _styleList->setMaximumWidth(280);
        connect(_styleList, &QListWidget::currentRowChanged, this, &WordStyleDlg::onStyleChanged);
        row->addWidget(_styleList, 1);

        // Right panel: style properties
        QWidget* props = new QWidget;
        QVBoxLayout* pl = new QVBoxLayout(props);
        pl->setContentsMargins(0, 0, 0, 0);
        pl->setSpacing(8);

        // Colour row
        {
            QHBoxLayout* cr = new QHBoxLayout;
            cr->addWidget(new QLabel(QStringLiteral("Foreground:")));
            _pFgColour = new ColourPicker(this);
            _pFgColour->setToolTip(QStringLiteral("Foreground colour"));
            connect(_pFgColour, &ColourPicker::colorPicked, this, [this](const QColor&) {
                updateColour(0);
            });
            cr->addWidget(_pFgColour);
            cr->addSpacing(16);
            cr->addWidget(new QLabel(QStringLiteral("Background:")));
            _pBgColour = new ColourPicker(this);
            _pBgColour->setToolTip(QStringLiteral("Background colour"));
            connect(_pBgColour, &ColourPicker::colorPicked, this, [this](const QColor&) {
                updateColour(1);
            });
            cr->addWidget(_pBgColour);
            cr->addStretch();
            pl->addLayout(cr);
        }

        // Font name
        {
            QHBoxLayout* fr = new QHBoxLayout;
            fr->addWidget(new QLabel(QStringLiteral("Font:")));
            _fontNameCombo = new QFontComboBox;
            _fontNameCombo->setMinimumWidth(130);
            connect(_fontNameCombo, &QFontComboBox::currentIndexChanged, this, &WordStyleDlg::updateFontName);
            fr->addWidget(_fontNameCombo, 1);
            fr->addSpacing(8);
            fr->addWidget(new QLabel(QStringLiteral("Size:")));
            _fontSizeCombo = new QComboBox;
            _fontSizeCombo->setEditable(true);
            _fontSizeCombo->setMinimumWidth(52);
            const QStringList sizes = {
                QStringLiteral("6"), QStringLiteral("7"), QStringLiteral("8"),
                QStringLiteral("9"), QStringLiteral("10"), QStringLiteral("11"),
                QStringLiteral("12"), QStringLiteral("13"), QStringLiteral("14"),
                QStringLiteral("15"), QStringLiteral("16"), QStringLiteral("18"),
                QStringLiteral("20"), QStringLiteral("22"), QStringLiteral("24"),
                QStringLiteral("36"), QStringLiteral("48"), QStringLiteral("72")
            };
            _fontSizeCombo->addItems(sizes);
            connect(_fontSizeCombo, &QComboBox::currentIndexChanged, this, &WordStyleDlg::updateFontSize);
            fr->addWidget(_fontSizeCombo);
            pl->addLayout(fr);
        }

        // Bold / Italic / Underline
        {
            QHBoxLayout* sr = new QHBoxLayout;
            _boldCheck = new QCheckBox(QStringLiteral("Bold"));
            connect(_boldCheck, &QCheckBox::toggled, this, [this](bool) {
                updateFontStyleStatus(BOLD_STATUS);
            });
            _italicCheck = new QCheckBox(QStringLiteral("Italic"));
            connect(_italicCheck, &QCheckBox::toggled, this, [this](bool) {
                updateFontStyleStatus(ITALIC_STATUS);
            });
            _underlineCheck = new QCheckBox(QStringLiteral("Underline"));
            connect(_underlineCheck, &QCheckBox::toggled, this, [this](bool) {
                updateFontStyleStatus(UNDERLINE_STATUS);
            });
            sr->addWidget(_boldCheck);
            sr->addWidget(_italicCheck);
            sr->addWidget(_underlineCheck);
            sr->addStretch();
            pl->addLayout(sr);
        }

        // Keywords label + text edit
        pl->addWidget(new QLabel(QStringLiteral("Keywords:")));
        _keywordsEdit = new QTextEdit;
        _keywordsEdit->setPlaceholderText(QStringLiteral("User-defined keywords..."));
        _keywordsEdit->setMaximumHeight(56);
        connect(_keywordsEdit, &QTextEdit::textChanged, this, &WordStyleDlg::updateUserKeywords);
        pl->addWidget(_keywordsEdit);

        // Style description / info label
        _styleInfoLabel = new QLabel;
        _styleInfoLabel->setWordWrap(true);
        _styleInfoLabel->setStyleSheet(QStringLiteral(
            "color: #808080; font-size: 10pt; padding: 4px;"));
        pl->addWidget(_styleInfoLabel);

        pl->addStretch();
        row->addWidget(props, 2);
        mainLayout->addLayout(row, 1);
    }

    // ── Bottom buttons ─────────────────────────────────────────────────────
    {
        QHBoxLayout* btnRow = new QHBoxLayout;
        btnRow->addStretch();

        QPushButton* saveCloseBtn = new QPushButton(QStringLiteral("Save && Close"));
        saveCloseBtn->setDefault(false);
        connect(saveCloseBtn, &QPushButton::clicked, this, &WordStyleDlg::onSaveClose);
        btnRow->addWidget(saveCloseBtn);

        QPushButton* cancelBtn = new QPushButton(QStringLiteral("Cancel"));
        cancelBtn->setDefault(false);
        connect(cancelBtn, &QPushButton::clicked, this, &WordStyleDlg::onCancel);
        btnRow->addWidget(cancelBtn);

        QPushButton* applyBtn = new QPushButton(QStringLiteral("Apply"));
        applyBtn->setDefault(false);
        connect(applyBtn, &QPushButton::clicked, this, &WordStyleDlg::onApply);
        btnRow->addWidget(applyBtn);

        mainLayout->addLayout(btnRow);
    }

    // ── Dark mode ──────────────────────────────────────────────────────────
    NppDarkMode::instance().applyToWidget(this);
    NppDarkMode::instance().subclassButton(findChild<QPushButton*>());

    // ── Populate initial data ───────────────────────────────────────────────
    _styles2restored = _lsArray;
    _gstyles2restored = _globalStyles;
    loadLangListFromNppParam();
}

void WordStyleDlg::destroy()
{
    close();
}

void WordStyleDlg::prepare2Cancel()
{
    if (_restoreInvalid) return;
    _lsArray = _styles2restored;
    _globalStyles = _gstyles2restored;
}

void WordStyleDlg::redraw(bool forceUpdate)
{
    Q_UNUSED(forceUpdate);
    if (window()) window()->update();
}

void WordStyleDlg::restoreGlobalOverrideValues() const
{
    Q_UNUSED(_gstyles2restored);
}

void WordStyleDlg::addLastThemeEntry() const
{
    // No-op; handled by theme combo
}

bool WordStyleDlg::selectThemeByName(const QString& themeName)
{
    if (_themeCombo) {
        int idx = _themeCombo->findText(themeName);
        if (idx >= 0) {
            _themeCombo->setCurrentIndex(idx);
            return true;
        }
    }
    return false;
}

void WordStyleDlg::syncWithSelFgSingleColorCtrl()
{
    updateColour(0);
}

bool WordStyleDlg::goToSection(const QString& sectionNames)
{
    QStringList parts = sectionNames.split(QLatin1Char(':'));
    if (parts.size() != 2) return false;

    QString langName = parts[0].trimmed();
    QString styleName = parts[1].trimmed();

    if (_langCombo) {
        int langIdx = _langCombo->findText(langName, Qt::MatchContains);
        if (langIdx >= 0) {
            _langCombo->setCurrentIndex(langIdx);
            setStyleListFromLexer(langIdx);

            if (_styleList) {
                for (int i = 0; i < _styleList->count(); ++i) {
                    if (_styleList->item(i)->text().contains(styleName, Qt::CaseInsensitive)) {
                        _styleList->setCurrentRow(i);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Return a pointer to the currently active style array (global or per-lexer)
// ---------------------------------------------------------------------------
StyleArray_* WordStyleDlg::currentStyleArray()
{
    if (_currentLexerIndex == 0) {
        return &_globalStyles;
    } else {
        int arrIdx = _currentLexerIndex - 1;
        if (arrIdx >= 0 && arrIdx < _lsArray.size())
            return &_lsArray[arrIdx].styles;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// onLangChanged — triggered when user picks a language in the combo
// ---------------------------------------------------------------------------
void WordStyleDlg::onLangChanged(int index)
{
    if (index < 0) return;
    // Preserve dirty flags across language switches
    bool prevDirty = _isDirty;
    bool prevThemeDirty = _isThemeDirty;
    setStyleListFromLexer(index);
    _isDirty = prevDirty;
    _isThemeDirty = prevThemeDirty;
}

// ---------------------------------------------------------------------------
// onStyleChanged — triggered when user picks a style in the list
// ---------------------------------------------------------------------------
void WordStyleDlg::onStyleChanged(int row)
{
    if (row < 0) {
        _currentStyleIndex = -1;
        enableFontStyle(false);
        return;
    }
    _currentStyleIndex = row;
    setVisualFromStyleList();
    enableFontStyle(true);
}

void WordStyleDlg::updateColour(bool which)
{
    // which = 0 → foreground, 1 → background
    if (_currentStyleIndex < 0) return;

    StyleArray_* src = currentStyleArray();
    if (!src || _currentStyleIndex >= src->size()) return;

    if (which == 0 && _pFgColour) {
        (*src)[_currentStyleIndex].fgColor = _pFgColour->getColour();
    } else if (which == 1 && _pBgColour) {
        (*src)[_currentStyleIndex].bgColor = _pBgColour->getColour();
    } else {
        return;
    }
    _isDirty = true;
    emit styleApplied();
}

void WordStyleDlg::updateFontStyleStatus(fontStyleType whichStyle)
{
    StyleArray_* src = currentStyleArray();
    if (!src || _currentStyleIndex < 0 || _currentStyleIndex >= src->size()) return;

    StyleItem_& si = (*src)[_currentStyleIndex];
    bool checked = false;

    switch (whichStyle) {
        case BOLD_STATUS:
            checked = (_boldCheck && _boldCheck->isChecked());
            si.fontStyle = checked ? (si.fontStyle | 1) : (si.fontStyle & ~1);
            break;
        case ITALIC_STATUS:
            checked = (_italicCheck && _italicCheck->isChecked());
            si.fontStyle = checked ? (si.fontStyle | 2) : (si.fontStyle & ~2);
            break;
        case UNDERLINE_STATUS:
            checked = (_underlineCheck && _underlineCheck->isChecked());
            si.fontStyle = checked ? (si.fontStyle | 4) : (si.fontStyle & ~4);
            break;
    }
    _isDirty = true;
    emit styleApplied();
}

void WordStyleDlg::updateExtension()
{
    if (!_extEdit) return;
    int arrIdx = _currentLexerIndex - 1;
    if (arrIdx >= 0 && arrIdx < _lsArray.size())
        _lsArray[arrIdx].lexerExt = _extEdit->text();
    _isDirty = true;
}

void WordStyleDlg::updateFontName()
{
    StyleArray_* src = currentStyleArray();
    if (!_fontNameCombo || !src || _currentStyleIndex < 0) return;
    if (_currentStyleIndex < src->size())
        (*src)[_currentStyleIndex].fontName = _fontNameCombo->currentFont().family();
    _isDirty = true;
    emit styleApplied();
}

void WordStyleDlg::updateFontSize()
{
    StyleArray_* src = currentStyleArray();
    if (!_fontSizeCombo || !src || _currentStyleIndex < 0) return;
    if (_currentStyleIndex < src->size()) {
        bool ok = false;
        int sz = _fontSizeCombo->currentText().toInt(&ok);
        if (ok) (*src)[_currentStyleIndex].fontSize = sz;
    }
    _isDirty = true;
    emit styleApplied();
}

void WordStyleDlg::updateUserKeywords()
{
    StyleArray_* src = currentStyleArray();
    if (!_keywordsEdit || !src || _currentStyleIndex < 0) return;
    if (_currentStyleIndex < src->size())
        (*src)[_currentStyleIndex].keywords = _keywordsEdit->toPlainText();
    _isDirty = true;
}

void WordStyleDlg::switchToTheme()
{
    if (_themeCombo)
        _themeName = _themeCombo->currentText();
    _isThemeDirty = true;
}

void WordStyleDlg::loadLangListFromNppParam()
{
    if (!_langCombo) return;
    _langCombo->clear();
    _langCombo->addItem(QStringLiteral("Global Styles"));

    // Until Parameters.cpp feeds lexer arrays, use a built-in list
    const QStringList defaultLexers = {
        QStringLiteral("HTML"),       QStringLiteral("XML"),
        QStringLiteral("JavaScript"), QStringLiteral("CSS"),
        QStringLiteral("Python"),     QStringLiteral("C"),
        QStringLiteral("C++"),       QStringLiteral("C#"),
        QStringLiteral("JSON"),       QStringLiteral("Markdown"),
        QStringLiteral("SQL"),        QStringLiteral("Bash"),
        QStringLiteral("PowerShell"), QStringLiteral("PHP"),
        QStringLiteral("Ruby"),       QStringLiteral("Rust"),
        QStringLiteral("Go"),         QStringLiteral("Java"),
        QStringLiteral("Perl"),       QStringLiteral("Lua"),
    };
    _langCombo->addItems(defaultLexers);
    if (_langCombo->count() > 0)
        _langCombo->setCurrentIndex(0);
    setStyleListFromLexer(0);
}

void WordStyleDlg::enableFontStyle(bool isEnable) const
{
    if (_boldCheck)      _boldCheck->setEnabled(isEnable);
    if (_italicCheck)    _italicCheck->setEnabled(isEnable);
    if (_underlineCheck) _underlineCheck->setEnabled(isEnable);
    if (_fontNameCombo)  _fontNameCombo->setEnabled(isEnable);
    if (_fontSizeCombo)  _fontSizeCombo->setEnabled(isEnable);
}

void WordStyleDlg::showGlobalOverrideCtrls(bool show)
{
    _isShownGOCtrls = show;
    if (_globalOverrideTip)
        _globalOverrideTip->setVisible(show);
}

void WordStyleDlg::updateGlobalOverrideCtrls()
{
    Q_UNUSED(_globalStyles);
}

// ---------------------------------------------------------------------------
// setStyleListFromLexer — populate the style list for a language index
// index 0 = Global Styles, index N (N>0) = lexer at _lsArray[N-1]
// ---------------------------------------------------------------------------
void WordStyleDlg::setStyleListFromLexer(int index)
{
    if (!_styleList) return;
    _styleList->clear();
    _currentLexerIndex = index;
    _currentStyleIndex = -1;

    const StyleArray_* src = nullptr;

    if (index == 0) {
        src = &_globalStyles;
        if (_extEdit) {
            _extEdit->clear();
            _extEdit->setEnabled(false);
        }
    } else {
        int arrIdx = index - 1;
        if (arrIdx >= 0 && arrIdx < _lsArray.size() && !_lsArray[arrIdx].styles.isEmpty()) {
            src = &_lsArray[arrIdx].styles;
        }
        if (_extEdit) {
            QString ext = (arrIdx >= 0 && arrIdx < _lsArray.size())
                          ? _lsArray[arrIdx].lexerExt : QString();
            _extEdit->setText(ext);
            _extEdit->setEnabled(true);
        }
        // Show placeholder styles if lexer array not yet populated
        if (!src || src->isEmpty()) {
            const QStringList placeholders = getDefaultLexerStyles(index);
            for (const QString& s : placeholders)
                _styleList->addItem(s);
            enableFontStyle(false);
            return;
        }
    }

    Q_ASSERT(src != nullptr);
    for (const StyleItem_& si : *src) {
        if (!si.name.isEmpty())
            _styleList->addItem(si.name);
    }

    if (_styleList->count() > 0) {
        _styleList->setCurrentRow(0);
        _currentStyleIndex = 0;
        setVisualFromStyleList();
        enableFontStyle(true);
    } else {
        enableFontStyle(false);
    }
}

// ---------------------------------------------------------------------------
// setVisualFromStyleList — update all UI controls from the selected style
// ---------------------------------------------------------------------------
void WordStyleDlg::setVisualFromStyleList()
{
    if (!_styleList || _currentStyleIndex < 0) return;

    StyleArray_* src = currentStyleArray();
    if (!src || _currentStyleIndex >= src->size()) return;

    const StyleItem_& si = (*src)[_currentStyleIndex];

    // Font style checkboxes
    if (_boldCheck)      _boldCheck->setChecked(si.fontStyle & 1);
    if (_italicCheck)    _italicCheck->setChecked(si.fontStyle & 2);
    if (_underlineCheck) _underlineCheck->setChecked(si.fontStyle & 4);

    // Colour pickers
    if (_pFgColour) _pFgColour->setColour(si.fgColor);
    if (_pBgColour) _pBgColour->setColour(si.bgColor);

    // Font name — find matching family in the combo
    if (_fontNameCombo) {
        if (!si.fontName.isEmpty()) {
            int idx = _fontNameCombo->findText(si.fontName, Qt::MatchContains);
            if (idx >= 0)
                _fontNameCombo->setCurrentIndex(idx);
        }
    }

    // Font size
    if (_fontSizeCombo) {
        QString sz = QString::number(si.fontSize);
        int idx = _fontSizeCombo->findText(sz);
        if (idx >= 0)
            _fontSizeCombo->setCurrentIndex(idx);
        else
            _fontSizeCombo->setCurrentText(sz);
    }

    // Keywords / extension
    if (_keywordsEdit) _keywordsEdit->setPlainText(si.keywords);

    // Info label
    if (_styleInfoLabel) {
        QString info = si.name;
        if (!si.fontName.isEmpty())
            info += QStringLiteral(" | %1 %2pt").arg(si.fontName).arg(si.fontSize);
        QString styles;
        if (si.fontStyle & 1) styles += QStringLiteral("Bold ");
        if (si.fontStyle & 2) styles += QStringLiteral("Italic ");
        if (si.fontStyle & 4) styles += QStringLiteral("Underline ");
        if (!styles.isEmpty())
            info += QStringLiteral(" | %1").arg(styles.trimmed());
        info += QStringLiteral(" | FG:#%1 BG:#%2")
                    .arg(si.fgColor.red(), 2, 16, QLatin1Char('0'))
                    .arg(si.bgColor.red(), 2, 16, QLatin1Char('0'))
                    .arg(si.fgColor.green(), 2, 16, QLatin1Char('0'))
                    .arg(si.bgColor.green(), 2, 16, QLatin1Char('0'))
                    .arg(si.fgColor.blue(), 2, 16, QLatin1Char('0'))
                    .arg(si.bgColor.blue(), 2, 16, QLatin1Char('0'));
        _styleInfoLabel->setText(info);
    }
}

void WordStyleDlg::applyCurrentSelectedThemeAndUpdateUI()
{
    switchToTheme();
    if (_styleList && _styleList->currentRow() >= 0) {
        _currentStyleIndex = _styleList->currentRow();
        setVisualFromStyleList();
    }
    _isThemeDirty = false;
    emit styleApplied();
}

// ---------------------------------------------------------------------------
// onSaveClose — Save styles and close dialog
// ---------------------------------------------------------------------------
void WordStyleDlg::onSaveClose()
{
    if (_isDirty) {
        // Transfer current styles back to Parameters
        // (Parameters.cpp writeStyles will be called when NppXml is wired)
        _isDirty = false;
        _isThemeDirty = false;
        _isThemeChanged = false;
    }
    emit styleApplied();
    hide();
}

// ---------------------------------------------------------------------------
// onCancel — Restore saved styles and close
// ---------------------------------------------------------------------------
void WordStyleDlg::onCancel()
{
    if (_isDirty) {
        prepare2Cancel();
    }
    _isDirty = false;
    _isThemeDirty = false;
    _isThemeChanged = false;
    emit styleApplied();
    hide();
}

// ---------------------------------------------------------------------------
// onApply — Apply current styles without closing
// ---------------------------------------------------------------------------
void WordStyleDlg::onApply()
{
    _isDirty = false;
    emit styleApplied();
}

// ---------------------------------------------------------------------------
// getDefaultLexerStyles — placeholder style names for each built-in lexer
// ---------------------------------------------------------------------------
QStringList WordStyleDlg::getDefaultLexerStyles(int langIndex) const
{
    Q_UNUSED(langIndex);
    // Placeholder: return default styles for the selected lexer.
    // When Parameters.cpp is wired, this is replaced by real _lsArray data.
    static const QStringList defaults = {
        QStringLiteral("Default"),
        QStringLiteral("Comment"),
        QStringLiteral("Comment line"),
        QStringLiteral("Keyword"),
        QStringLiteral("Number"),
        QStringLiteral("String"),
        QStringLiteral("Character"),
        QStringLiteral("Operator"),
        QStringLiteral("Identifier"),
        QStringLiteral("String begin"),
        QStringLiteral("String end"),
        QStringLiteral("Keyword set 5"),
        QStringLiteral("Tag"),
        QStringLiteral("DTD"),
        QStringLiteral("Attribute name"),
        QStringLiteral("Attribute value"),
        QStringLiteral("SGML tag"),
        QStringLiteral("Value"),
        QStringLiteral("Entity"),
        QStringLiteral("CData"),
        QStringLiteral("Preprocessor"),
        QStringLiteral("Script line"),
    };
    return defaults;
}

intptr_t WordStyleDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            NppDarkMode::instance().applyToWidget(window());
            if (_langCombo)     NppDarkMode::instance().applyToWidget(_langCombo);
            if (_styleList)     NppDarkMode::instance().applyToWidget(_styleList);
            if (_themeCombo)    NppDarkMode::instance().applyToWidget(_themeCombo);
            if (_boldCheck)     NppDarkMode::instance().subclassButton(_boldCheck);
            if (_italicCheck)   NppDarkMode::instance().subclassButton(_italicCheck);
            if (_underlineCheck) NppDarkMode::instance().subclassButton(_underlineCheck);
            if (_pFgColour)     NppDarkMode::instance().applyToWidget(_pFgColour);
            if (_pBgColour)     NppDarkMode::instance().applyToWidget(_pBgColour);
            if (_extEdit)       NppDarkMode::instance().applyToWidget(_extEdit);
            if (_keywordsEdit)  NppDarkMode::instance().applyToWidget(_keywordsEdit);

            _styles2restored = _lsArray;
            _gstyles2restored = _globalStyles;

            return TRUE;
        }

        case WM_UPDATESCINTILLAS: {
            Q_UNUSED(wParam);
            Q_UNUSED(lParam);
            emit styleApplied();
            return TRUE;
        }

        case WM_UPDATEMAINMENUBITMAPS: {
            NppDarkMode::instance().applyToWidget(window());
            return TRUE;
        }

        case WM_SIZE: {
            Q_UNUSED(wParam);
            Q_UNUSED(lParam);
            return TRUE;
        }

        default:
            break;
    }
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}
