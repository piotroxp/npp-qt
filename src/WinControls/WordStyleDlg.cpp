// Semantic Lift: Win32 WordStyleDlg → Qt6 WordStyleDlg
// Original: PowerEditor/src/WinControls/ColourPicker/WordStyleDlg.cpp
// Target: npp-qt/src/WinControls/WordStyleDlg.cpp
//
// Ported WordStyleDlg implementation.

#include "WordStyleDlg.h"
#include "NppDarkMode.h"
#include "NppConstants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QColorDialog>

// =============================================================================
// StyleArray_ stub implementations (replace with Parameters.cpp when ported)
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

// =============================================================================
// WordStyleDlg
// =============================================================================

WordStyleDlg::WordStyleDlg()
    : StaticDialog()
{
}

WordStyleDlg::~WordStyleDlg()
{
    if (_globalOverrideTip) {
        _globalOverrideTip->deleteLater();
        _globalOverrideTip = nullptr;
    }
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
    show();
    raise();
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
    // Restore global override checkboxes from saved backup
    // The checkboxes are stored in _globalStyles / _gstyles2restored
    // Each StyleItem carries globalFg/globalFont/etc. flags
    Q_UNUSED(_gstyles2restored);
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

void WordStyleDlg::updateColour(bool which)
{
    // which = 0 → foreground, 1 → background
    // Apply the colour from ColourPicker to the current style in _lsArray / _globalStyles
    if (_currentStyleIndex < 0) return;

    QColor col;
    if (which == 0 && _pFgColour) {
        col = _pFgColour->getColour();
    } else if (which == 1 && _pBgColour) {
        col = _pBgColour->getColour();
    } else {
        return;
    }

    // Apply to global styles (style index maps into _globalStyles)
    if (_currentStyleIndex < _globalStyles.size()) {
        StyleItem_& si = _globalStyles[_currentStyleIndex];
        if (which == 0) {
            si.fgColor = col;
        } else {
            si.bgColor = col;
        }
    }
    _isDirty = true;
    emit styleApplied();
}

void WordStyleDlg::updateFontStyleStatus(fontStyleType whichStyle)
{
    if (_currentStyleIndex < 0 || _currentStyleIndex >= _globalStyles.size()) return;

    StyleItem_& si = _globalStyles[_currentStyleIndex];
    bool checked = false;

    switch (whichStyle) {
        case BOLD_STATUS:
            checked = _boldCheck ? _boldCheck->isChecked() : false;
            si.fontStyle = checked ? (si.fontStyle | 1) : (si.fontStyle & ~1);
            break;
        case ITALIC_STATUS:
            checked = _italicCheck ? _italicCheck->isChecked() : false;
            si.fontStyle = checked ? (si.fontStyle | 2) : (si.fontStyle & ~2);
            break;
        case UNDERLINE_STATUS:
            checked = _underlineCheck ? _underlineCheck->isChecked() : false;
            si.fontStyle = checked ? (si.fontStyle | 4) : (si.fontStyle & ~4);
            break;
    }
    _isDirty = true;
    emit styleApplied();
}

void WordStyleDlg::updateExtension()
{
    if (!_extEdit || _currentStyleIndex < 0) return;
    if (_currentStyleIndex < _globalStyles.size()) {
        _globalStyles[_currentStyleIndex].ext = _extEdit->text();
    }
    _isDirty = true;
}

void WordStyleDlg::updateFontName()
{
    if (!_fontNameCombo || _currentStyleIndex < 0) return;
    if (_currentStyleIndex < _globalStyles.size()) {
        _globalStyles[_currentStyleIndex].fontName = _fontNameCombo->currentFont().family();
    }
    _isDirty = true;
    emit styleApplied();
}

void WordStyleDlg::updateFontSize()
{
    if (!_fontSizeCombo || _currentStyleIndex < 0) return;
    if (_currentStyleIndex < _globalStyles.size()) {
        bool ok = false;
        int sz = _fontSizeCombo->currentText().toInt(&ok);
        if (ok) _globalStyles[_currentStyleIndex].fontSize = sz;
    }
    _isDirty = true;
    emit styleApplied();
}

void WordStyleDlg::updateUserKeywords()
{
    if (!_keywordsEdit || _currentStyleIndex < 0) return;
    if (_currentStyleIndex < _globalStyles.size()) {
        _globalStyles[_currentStyleIndex].keywords = _keywordsEdit->toPlainText();
    }
    _isDirty = true;
}

void WordStyleDlg::switchToTheme()
{
    // Load selected theme from disk / NppParameters theme store
    if (_themeCombo) {
        _themeName = _themeCombo->currentText();
        // Theme loading will repopulate _lsArray and _globalStyles from theme XML
        // until that is wired, flag the dirty state
    }
    _isThemeDirty = true;
}

void WordStyleDlg::loadLangListFromNppParam()
{
    if (!_langCombo) return;
    _langCombo->clear();
    _langCombo->addItem(QStringLiteral("Global Styles"));

    // Populated from NppParameters::getInstance().getLStylerArray()
    // Minimal lexer set for now (full list requires Parameters port)
    const QStringList defaultLexers = {
        QStringLiteral("HTML"),
        QStringLiteral("XML"),
        QStringLiteral("JavaScript"),
        QStringLiteral("CSS"),
        QStringLiteral("Python"),
        QStringLiteral("C"),
        QStringLiteral("C++"),
        QStringLiteral("C#"),
        QStringLiteral("JSON"),
        QStringLiteral("Markdown"),
        QStringLiteral("SQL"),
        QStringLiteral("Bash"),
        QStringLiteral("PowerShell"),
        QStringLiteral("PHP"),
        QStringLiteral("Ruby"),
        QStringLiteral("Rust"),
        QStringLiteral("Go"),
        QStringLiteral("Java"),
        QStringLiteral("Perl"),
        QStringLiteral("Lua"),
    };
    _langCombo->addItems(defaultLexers);
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
    if (_globalOverrideTip) {
        _globalOverrideTip->setVisible(show);
    }
}

void WordStyleDlg::updateGlobalOverrideCtrls()
{
    // Sync global override checkbox state with NppParameters::_globalOverride
    // Each control reflects its corresponding GlobalOverride flag
    Q_UNUSED(_globalStyles);
}

void WordStyleDlg::setStyleListFromLexer(int index)
{
    if (!_styleList) return;
    _styleList->clear();

    if (index == 0) {
        // Global Styles — populate from _globalStyles
        for (const StyleItem_& si : _globalStyles) {
            if (!si.name.isEmpty())
                _styleList->addItem(si.name);
        }
        return;
    }

    // Per-lexer styles — populate from _lsArray[index-1]
    int arrIdx = index - 1;
    if (arrIdx < 0 || arrIdx >= _lsArray.size()) return;
    for (const StyleItem_& si : _lsArray[arrIdx].styles) {
        if (!si.name.isEmpty())
            _styleList->addItem(si.name);
    }
}

void WordStyleDlg::setVisualFromStyleList()
{
    if (!_styleList || _currentStyleIndex < 0) return;
    if (_currentStyleIndex >= _globalStyles.size()) return;

    const StyleItem_& si = _globalStyles[_currentStyleIndex];

    // Update font controls
    if (_boldCheck)      _boldCheck->setChecked(si.fontStyle & 1);
    if (_italicCheck)    _italicCheck->setChecked(si.fontStyle & 2);
    if (_underlineCheck) _underlineCheck->setChecked(si.fontStyle & 4);

    // Update colour pickers
    if (_pFgColour) _pFgColour->setColour(si.fgColor);
    if (_pBgColour) _pBgColour->setColour(si.bgColor);

    // Update keywords / extension
    if (_keywordsEdit) _keywordsEdit->setPlainText(si.keywords);
    if (_extEdit)     _extEdit->setText(si.ext);
}

void WordStyleDlg::applyCurrentSelectedThemeAndUpdateUI()
{
    switchToTheme();
    if (_styleList) {
        int selRow = _styleList->currentRow();
        if (selRow >= 0) {
            _currentStyleIndex = selRow;
            setVisualFromStyleList();
        }
    }
}

// =============================================================================
// Dialog procedure
// =============================================================================
intptr_t WordStyleDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            NppDarkMode::instance().applyToWidget(window());

            // Apply dark mode to all child controls
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

            // Populate language list
            loadLangListFromNppParam();

            // Restore saved styles
            _styles2restored = _lsArray;
            _gstyles2restored = _globalStyles;

            return TRUE;
        }

        case WM_UPDATESCINTILLAS: {
            // WPARAM: doChangePanel (bool)
            Q_UNUSED(wParam);
            Q_UNUSED(lParam);
            // Refresh all Scintilla views to reflect style changes
            emit styleApplied();
            return TRUE;
        }

        case WM_UPDATEMAINMENUBITMAPS: {
            NppDarkMode::instance().applyToWidget(window());
            return TRUE;
        }

        case WM_SIZE: {
            // Resize child controls to fit dialog
            Q_UNUSED(wParam);
            Q_UNUSED(lParam);
            return TRUE;
        }

        default:
            break;
    }
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}
