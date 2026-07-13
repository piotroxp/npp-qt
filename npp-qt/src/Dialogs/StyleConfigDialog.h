// StyleConfigDialog.h — npp-qt Qt6 Style Configurator dialog
// Replaces stub in NppCommands::settingsStyleConfig()

#pragma once

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QColorDialog>
#include <QFontDialog>
#include <QDialogButtonBox>
#include <QSettings>
#include <QColor>
#include <QFont>
#include <map>
#include <QVector>

// Scintilla style constants — use npp_sci::STYLE_* from NppSciCompat.h

// =============================================================================
// StyleItem — data for one style token row
// =============================================================================

struct StyleItem {
    int styleIndex = 0;         // Scintilla style constant (e.g., STYLE_KEYWORD)
    QString name;               // Display name (e.g., "Keyword", "Comment")
    QColor fgColor;
    QColor bgColor;
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool enabled = false;       // Whether this style is customized
};

// =============================================================================
// LexerStyle — all styles for one lexer/language
// =============================================================================

struct LexerStyle {
    QString lexerName;
    QString displayName;
    int lexerId;                // Scintilla lexer ID (SCLEX_CPP, SCLEX_PYTHON, etc.)
    QVector<StyleItem> styles;
};

// =============================================================================
// StyleConfigDialog — lexer style configuration dialog
// =============================================================================

class StyleConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StyleConfigDialog(QWidget* parent = nullptr);
    ~StyleConfigDialog() override = default;

private slots:
    void onLexerChanged(int index);
    void onStyleItemChanged(QTreeWidgetItem* item, int column);
    void onFgColorClicked();
    void onBgColorClicked();
    void onBoldClicked();
    void onItalicClicked();
    void onUnderlineClicked();
    void onFontNameClicked();
    void onFontSizeChanged(int size);
    void onApply();
    void onOk();
    void onCancel();

private:
    void setupUi();
    void populateLexers();
    void populateStyleTree(const LexerStyle& lexer);
    void applyStyleToPreview();
    void updateStyleFromTree(QTreeWidgetItem* item);
    QColor styleFgColor(int styleIdx) const;
    QColor styleBgColor(int styleIdx) const;
    void saveStyles();

    // Lexer definitions
    QVector<LexerStyle> _lexers;

    // Controls
    QComboBox* _lexerCombo = nullptr;
    QTreeWidget* _styleTree = nullptr;
    QPushButton* _btnFgColor = nullptr;
    QPushButton* _btnBgColor = nullptr;
    QPushButton* _btnFontName = nullptr;
    QSpinBox* _sbFontSize = nullptr;
    QCheckBox* _cbBold = nullptr;
    QCheckBox* _cbItalic = nullptr;
    QCheckBox* _cbUnderline = nullptr;
    QLabel* _lblFgColor = nullptr;
    QLabel* _lblBgColor = nullptr;
    QLabel* _lblFontName = nullptr;
    QLabel* _previewLabel = nullptr;

    // Preview widget — uses QsciScintilla for live syntax highlighting preview
    class QsciScintilla* _previewWidget = nullptr;

    // Current state
    int _currentLexerIdx = -1;
    StyleItem _currentItem;

    QDialogButtonBox* _buttonBox = nullptr;
};
