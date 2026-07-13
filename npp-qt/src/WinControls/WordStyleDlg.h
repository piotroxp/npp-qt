// Semantic Lift: Win32 WordStyleDlg → Qt6 WordStyleDlg
// Original: PowerEditor/src/WinControls/ColourPicker/WordStyleDlg.h
// Target: npp-qt/src/WinControls/ColourPicker.h (expand existing)
//
// WordStyleDlg is the "Style Configurator" dialog — a large dialog for
// configuring lexer styles. Ported to Qt6 as a complex dialog.

#ifndef WORD_STYLE_DLG_H
#define WORD_STYLE_DLG_H

#include "StaticDialog.h"
#include "ColourPicker.h"
#include <QXmlStreamReader>
#include <QDialog>
#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QFontComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QVector>
#include <QStringList>
#include <memory>

// Custom messages (mirrors Win32 WORDSTYLE_USER)
static constexpr int WORDSTYLE_USER = 5400;
// WM_UPDATESCINTILLAS and WM_UPDATEMAINMENUBITMAPS defined in NppConstants.h

// Font style types (mirrors Win32 fontStyleType)
enum fontStyleType { BOLD_STATUS, ITALIC_STATUS, UNDERLINE_STATUS };

// Forward declarations
class ColourPicker;

// =============================================================================
// Style configuration data structures
// Minimal stub types — replace with Parameters.h counterparts when Parameters
// port is complete. These mirror the Win32 style system.
// =============================================================================

// Individual style property set (one per lexer style or global style)
// NOTE: Named with _ suffix to avoid conflict with Parameters.h's Style/StyleArray
struct StyleItem_ {
    QString name;
    QColor fgColor;
    QColor bgColor;
    QString fontName;
    int fontSize = 8;
    int fontStyle = 0;  // bit 0=bold, 1=italic, 2=underline
    int colorStyle = 0; // COLORSTYLE_* flags (0x01=fg, 0x02=bg)
    bool fontEnabled = false;
    QString keywords;
    QString ext;
    // Global-override flags (whether this style inherits from global settings)
    bool globalFg = false, globalBg = false;
    bool globalFont = false, globalFontSize = false;
    bool globalBold = false, globalItalic = false, globalUnderline = false;
    bool globalKeyword = false, globalExt = false;
};

// Collection of styles for one lexer language
// NOTE: Named with _ suffix to avoid conflict with Parameters.h's StyleArray
struct StyleArray_ : public QVector<StyleItem_> {
    StyleItem_* findByName(const QString& name);
    void addStyler(int id, const StyleItem_& si);
    void addStylerFromNode(const QString& nodeName);

    // Parse a <WordsStyle> node from QXmlStreamReader
    // Reads: name, fgColor, bgColor, fontName, fontSize, fontStyle, colorStyle
    void addStylerFromNode(QXmlStreamReader& reader, const QString& nodeName);
};

// Global override flags (NppParameters::_globalOverride)
// NOTE: Named with _ suffix to avoid conflict with Parameters.h's GlobalOverride
struct GlobalOverride_ {
    bool _enable = false;
    QColor _color;
    bool _colorEnabled = false;
    bool _fontEnabled = false;
    int _fontSize = 8;
    bool _fontSizeEnabled = false;
    bool _bold = false;
    bool _italic = false;
    bool _underline = false;
    bool _keyword = false;
    bool _ext = false;
};

// Array of lexer stylers (one StyleArray_ per lexer)
// NOTE: Named with _ suffix to avoid conflict with Parameters.h's LexerStyler
struct LexerStyler_ {
    QString lexerName;
    QString lexerExt;   // user-defined file extensions for this lexer
    StyleArray_ styles;
};

struct LexerStylerArray_ : public QVector<LexerStyler_> {};

// =============================================================================
// WordStyleDlg — Style configuration dialog
// Allows configuring lexer styles: font, size, color, keywords, etc.
// Mirrors Win32 WordStyleDlg class.
// =============================================================================

class WordStyleDlg : public StaticDialog
{
    Q_OBJECT

public:
    WordStyleDlg();
    ~WordStyleDlg() override;

    // create mirrors Win32 create()
    void create(int dialogID, bool isRTL = false, bool msgDestParent = true,
                unsigned short fontSize = 8);

    void doDialog(bool isRTL = false);
    void destroy() override;

    // Prepare for cancel (restore values)
    void prepare2Cancel();

    // Force redraw
    void redraw(bool forceUpdate = false);

    // Restore original values
    void restoreGlobalOverrideValues() const;

    // Add last theme entry
    void addLastThemeEntry() const;

    // Select theme by name
    bool selectThemeByName(const QString& themeName);

    // Sync foreground color control
    void syncWithSelFgSingleColorCtrl();

    // Navigate to a specific section: "Language:Style"
    bool goToSection(const QString& sectionNames);

signals:
    // Emitted when style changes need to be applied
    void styleApplied();

public slots:
    void applyCurrentSelectedThemeAndUpdateUI();
    void onSaveClose();
    void onCancel();
    void onApply();

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private slots:
    // --- Style controls ---
    void updateColour(int which);          // which=0: foreground, 1: background
    void updateFontStyleStatus(fontStyleType whichStyle);
    void updateExtension();
    void updateFontName();
    void updateFontSize();
    void updateUserKeywords();
    void switchToTheme();
    void loadLangListFromNppParam();
    void enableFontStyle(bool isEnable) const;
    void showGlobalOverrideCtrls(bool show);
    void updateGlobalOverrideCtrls();

    // --- Navigation ---
    void setStyleListFromLexer(int index);
    void setVisualFromStyleList();
    void onLangChanged(int index);
    void onStyleChanged(int row);

private:
    // --- Controls ---
    ColourPicker* _pFgColour = nullptr;
    ColourPicker* _pBgColour = nullptr;

    QComboBox* _langCombo = nullptr;       // Language selector
    QListWidget* _styleList = nullptr;    // Style list
    QComboBox* _themeCombo = nullptr;      // Theme selector

    QCheckBox* _boldCheck = nullptr;
    QCheckBox* _italicCheck = nullptr;
    QCheckBox* _underlineCheck = nullptr;

    QFontComboBox* _fontNameCombo = nullptr;
    QComboBox* _fontSizeCombo = nullptr;

    QLabel* _fgColourLabel = nullptr;
    QLabel* _bgColourLabel = nullptr;
    QLabel* _fontNameLabel = nullptr;
    QLabel* _fontSizeLabel = nullptr;
    QLabel* _styleInfoLabel = nullptr;

    QPushButton* _globalOverrideLink = nullptr;
    QWidget* _globalOverrideTip = nullptr;

    QLineEdit* _extEdit = nullptr;         // File extension filter
    QTextEdit* _keywordsEdit = nullptr;    // User-defined keywords

    // --- State ---
    int _currentLexerIndex = 0;
    int _currentThemeIndex = 0;

    LexerStylerArray_ _lsArray;     // Per-language stylers
    StyleArray_ _globalStyles;      // Global styles
    QString _themeName;            // Current theme

    // Backup for cancel
    LexerStylerArray_ _styles2restored;
    StyleArray_ _gstyles2restored;
    bool _restoreInvalid = false;

    bool _isDirty = false;
    bool _isThemeDirty = false;
    bool _isShownGOCtrls = false;
    bool _isThemeChanged = false;

    // Current style under editing
    int _currentStyleIndex = -1;

    // --- Helper methods ---
    void apply(int applicationInfo);
    int getApplicationInfo() const;
    bool isDocumentMapStyle() const;
    int whichTabColourIndex() const;
    int whichIndividualTabColourId();
    int whichFindDlgStatusMsgColourIndex();
    int notifyDataModified();

    std::pair<intptr_t, intptr_t> goToPreferencesSettings();

    // --- Internal helpers ---
    void buildUI();                                // Construct the Qt UI
    StyleArray_* currentStyleArray();              // Active style array (global or per-lexer)
    StyleArray_ makeDefaultGlobalStyles() const;  // Built-in default global styles
    QStringList getDefaultLexerStyles(int langIndex) const; // Placeholder styles for lexers
};

#endif // WORD_STYLE_DLG_H
