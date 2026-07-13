// npp-qt: UserDefineDialog — Qt6 implementation for Notepad++ User Defined Languages
// Semantic Lift: PowerEditor/src/ScintillaComponent/UserDefineDialog.h/.cpp
// Target:       npp-qt/src/ScintillaComponent/UserDefineDialog.h/.cpp
//
// UserDefineDialog is the main configuration dialog for User Defined Languages (UDL).
// It allows users to create/edit syntax highlighting rules for custom languages.
// The dialog is organized into 4 tabs via QTabWidget:
//
//   Tab 1 — Keywords & Lists: 8 keyword sets (name + keywords text)
//   Tab 2 — Styles:           Foreground/background colour + font styling
//   Tab 3 — Operators & Delimiters: single-char operators and delimiter pairs
//   Tab 4 — Comments:         Line comment, stream comment open/close
//
// The dialog reads/writes UDL XML files in the userDefineLangs/ directory
// and uses UserLangParser for XML parsing and LexerUDL for the Scintilla lexer.
#pragma once

#include "Window.h"
#include "UserLangParser.h"
#include "WinControls/ColourPicker.h"

#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QDialog>
#include <QFontComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QString>
#include <QTabWidget>
#include <QTextEdit>
#include <QVector>

// Forward declarations
class LexerUDL;

// ============================================================================
// UdlStyleItem — a style entry displayed in Tab 2
// ============================================================================
struct UdlStyleItem {
    int styleId = -1;       // SCE_USER_STYLE_* constant
    QString name;           // display name, e.g. "KEYWORDS1"
    QColor fgColor;
    QColor bgColor;
    QString fontName;
    int fontSize = 0;
    bool bold = false;
    bool italic = false;
    bool underline = false;

    int toFontStyle() const {
        int fs = 0;
        if (bold)      fs |= 1;
        if (italic)    fs |= 2;
        if (underline) fs |= 4;
        return fs;
    }
};

// ============================================================================
// KeywordsTab — Tab 1: 8 keyword sets
// ============================================================================
class KeywordsTab : public QWidget {
    Q_OBJECT
public:
    explicit KeywordsTab(QWidget* parent = nullptr);
    ~KeywordsTab() override = default;

    // Load keyword sets from a parsed UDL
    void loadFromParser(const UserLangParser& parser);

    // Extract keyword sets back into a parser
    void saveToParser(UserLangParser& parser) const;

    // Populate the 8 keyword set names from the parser
    void setKeywordSetNames(const QStringList& names);

    // Clear all keyword set editors
    void clearAll();

    // Accessor: get all keyword set data
    struct KeywordSetData {
        QString name;
        QString keywords;
        bool isPrefix;
    };
    QList<KeywordSetData> keywordSets() const;

signals:
    void changed();

private:
    void setupUi();

    // 8 keyword set editors: each has a name field and a keywords text field
    struct KeywordSetRow {
        QLineEdit* nameEdit = nullptr;
        QTextEdit* keywordsEdit = nullptr;
        QCheckBox* prefixCheck = nullptr;
    };
    QVector<KeywordSetRow> _rows;  // size 8
};

// ============================================================================
// StylesTab — Tab 2: Style list with fg/bg colour pickers
// ============================================================================
class StylesTab : public QWidget {
    Q_OBJECT
public:
    explicit StylesTab(QWidget* parent = nullptr);
    ~StylesTab() override = default;

    // Load styles from a parsed UDL
    void loadFromParser(const UserLangParser& parser);

    // Extract styles back into a parser
    void saveToParser(UserLangParser& parser) const;

    // Get the list of style items
    QList<UdlStyleItem> styleItems() const { return _styles; }

signals:
    void changed();

private slots:
    void onStyleSelectionChanged();
    void onFgColorClicked();
    void onBgColorClicked();
    void onFontChanged(const QString& family);
    void onFontSizeChanged(int size);
    void onBoldToggled(bool checked);
    void onItalicToggled(bool checked);
    void onUnderlineToggled(bool checked);

private:
    void setupUi();
    void updateStyleControls();

    QListWidget* _styleList = nullptr;
    QList<UdlStyleItem> _styles;

    // Style editing controls (right side)
    ColourPicker* _fgColorPicker = nullptr;
    ColourPicker* _bgColorPicker = nullptr;
    QFontComboBox* _fontCombo = nullptr;
    QSpinBox* _fontSizeSpin = nullptr;
    QCheckBox* _boldCheck = nullptr;
    QCheckBox* _italicCheck = nullptr;
    QCheckBox* _underlineCheck = nullptr;

    QLabel* _styleNameLabel = nullptr;

    int _selectedStyleIndex = -1;
};

// ============================================================================
// OperatorsTab — Tab 3: Operators and Delimiters
// ============================================================================
class OperatorsTab : public QWidget {
    Q_OBJECT
public:
    explicit OperatorsTab(QWidget* parent = nullptr);
    ~OperatorsTab() override = default;

    void loadFromParser(const UserLangParser& parser);
    void saveToParser(UserLangParser& parser) const;

    // Delimiters (up to 8 pairs) — defined first so DelimiterPair is complete for QVector
    struct DelimiterPair {
        QLineEdit* openEdit = nullptr;
        QLineEdit* escapeEdit = nullptr;
        QLineEdit* closeEdit = nullptr;
    };

    // Accessors for serialization
    QString op1() const { return _op1Edit->text(); }
    QString op2() const { return _op2Edit->text(); }
    QVector<DelimiterPair> delimiters() const { return _delimiters; }

signals:
    void changed();

private:
    void setupUi();

    // Operators (up to 2 sets)
    QLineEdit* _op1Edit = nullptr;
    QLineEdit* _op2Edit = nullptr;
    QVector<DelimiterPair> _delimiters;  // size 8
};

// ============================================================================
// CommentsTab — Tab 4: Comments and Number formatting
// ============================================================================
class CommentsTab : public QWidget {
    Q_OBJECT
public:
    explicit CommentsTab(QWidget* parent = nullptr);
    ~CommentsTab() override = default;

    void loadFromParser(const UserLangParser& parser);
    void saveToParser(UserLangParser& parser) const;

    // Accessors for serialization
    QString lineComment() const { return _lineCommentEdit->text(); }
    QString commentOpen() const { return _commentOpenEdit->text(); }
    QString commentClose() const { return _commentCloseEdit->text(); }
    QString numPrefix1() const { return _numPrefix1Edit->text(); }
    QString numPrefix2() const { return _numPrefix2Edit->text(); }
    QString numSuffix1() const { return _numSuffix1Edit->text(); }
    QString numSuffix2() const { return _numSuffix2Edit->text(); }
    QString numExtras1() const { return _numExtras1Edit->text(); }
    QString numExtras2() const { return _numExtras2Edit->text(); }
    QString numRange() const { return _numRangeEdit->text(); }
    int decimalSeparator() const { return _decimalSepCombo->currentIndex(); }

signals:
    void changed();

private:
    void setupUi();

    // Stream comment
    QLineEdit* _commentOpenEdit = nullptr;
    QLineEdit* _commentCloseEdit = nullptr;

    // Line comment
    QLineEdit* _lineCommentEdit = nullptr;

    // Decimal separator
    QComboBox* _decimalSepCombo = nullptr;

    // Number prefixes/suffixes
    QLineEdit* _numPrefix1Edit = nullptr;
    QLineEdit* _numPrefix2Edit = nullptr;
    QLineEdit* _numSuffix1Edit = nullptr;
    QLineEdit* _numSuffix2Edit = nullptr;
    QLineEdit* _numExtras1Edit = nullptr;
    QLineEdit* _numExtras2Edit = nullptr;
    QLineEdit* _numRangeEdit = nullptr;
};

// ============================================================================
// UserDefineDialog — Main UDL configuration dialog
// ============================================================================
class UserDefineDialog : public QDialog
{
    Q_OBJECT

public:
    // Singleton accessor
    static UserDefineDialog* getInstance();

    explicit UserDefineDialog(QWidget* parent = nullptr);
    ~UserDefineDialog() override;

    // Open the dialog (mirrors Win32 doDialog)
    void doDialog(bool isRTL = false);

    // Destroy/release the dialog
    void destroy();

    // Load a UDL by name (from userDefineLangs/ directory)
    void setLanguage(const QString& name);
    QString getLanguageName() const;

    // Load UDL list from the userDefineLangs directory
    void reloadLangList();

    // Set whether the dialog is docked (affects button labels)
    void setDockStatus(bool isDocked);
    bool isDocked() const { return _isDocked; }

    // Access the current parser
    UserLangParser* parser() { return &_parser; }

    // Create a new empty UDL
    void newLanguage(const QString& name);

    // Delete the current UDL
    void deleteCurrentLanguage();

    // Rename the current UDL
    void renameLanguage(const QString& newName);

    // Export current UDL to a file path
    bool exportToFile(const QString& path);

    // Import a UDL from a file path
    bool importFromFile(const QString& path);

    // Get the file path for the current language
    QString currentLangFilePath() const;

signals:
    // Emitted when UDL is saved — connect to lexer reload
    void languageSaved(const QString& langName);

private slots:
    void onLanguageChanged(int index);
    void onDefaultClicked();
    void onImportClicked();
    void onExportClicked();
    void onRemoveClicked();
    void onSaveClicked();
    void onDockToggleClicked();
    void onApplyClicked();
    void onOkClicked();
    void onTabChanged(int index);
    void onExtChanged(const QString& text);
    void onNewClicked();

private:
    // Build UI (called from constructor)
    void setupUi();
    // Load UDL XML file into _parser from the given name
    bool loadUdlFile(const QString& name);

    // Save _parser to the UDL XML file
    bool saveUdlFile(const QString& name);

    // Get the UDL directory path
    QString udlDirectory() const;

    // Convert parser data → each tab
    void populateTabsFromParser();

    // Collect tab data → parser
    void collectTabsToParser();

    // Build the UDL XML string from _parser
    QString serializeToXml() const;

    // Parse the UDL XML string and populate _parser
    bool parseXml(const QString& xmlData);

    // Build the style items list from parser
    QList<UdlStyleItem> buildStyleItems() const;

    static UserDefineDialog* _instance;

    QTabWidget* _tabWidget = nullptr;
    KeywordsTab* _keywordsTab = nullptr;
    StylesTab* _stylesTab = nullptr;
    OperatorsTab* _operatorsTab = nullptr;
    CommentsTab* _commentsTab = nullptr;

    QComboBox* _langCombo = nullptr;
    QLineEdit* _extEdit = nullptr;
    QPushButton* _defaultBtn = nullptr;
    QPushButton* _importBtn = nullptr;
    QPushButton* _exportBtn = nullptr;
    QPushButton* _removeBtn = nullptr;
    QPushButton* _saveBtn = nullptr;
    QPushButton* _applyBtn = nullptr;
    QPushButton* _dockBtn = nullptr;
    QPushButton* _okBtn = nullptr;
    QPushButton* _cancelBtn = nullptr;

    // Current language being edited
    QString _currentLangName;
    QString _currentLangExt;

    // Case sensitivity
    QCheckBox* _caseIgnoreCheck = nullptr;

    // Parsed UDL data
    UserLangParser _parser;

    // Whether dialog is docked (in panel) or floating
    bool _isDocked = false;

    // Dirty flag
    bool _isDirty = false;

    // Folders tab data
    QString _foldOpen;
    QString _foldMiddle;
    QString _foldClose;
    bool _foldCompact = false;

    // Prefix flags for keyword sets
    QVector<bool> _keywordPrefixes;  // size 8

    // Styles for Tab 2
    QList<UdlStyleItem> _styleItems;

    // Original language name (for rename)
    QString _originalLangName;
};
