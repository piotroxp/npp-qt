// StyleConfigDialog.cpp — npp-qt Qt6 Style Configurator dialog implementation

#include "StyleConfigDialog.h"
#include <Scintilla.h>
#include <SciLexer.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexermatlab.h>
#include <Qsci/qscilexercustom.h>
#include "ScintillaComponent/LexerRust.h"
#include "ScintillaComponent/LexerGo.h"
#include "ScintillaComponent/LexerPowerShell.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QSettings>
#include <QMessageBox>
#include <QLabel>

// =============================================================================
// Sample code for preview
// =============================================================================

static const char* kSampleCpp = R"(
// C++ Sample
#include <iostream>
namespace Example {
    class Widget {
    public:
        Widget() = default;
        virtual ~Widget() {}
        void setValue(int v) { _value = v; }
        int getValue() const { return _value; }
    private:
        int _value = 42;
    };
}
int main() {
    Example::Widget w;
    w.setValue(123);
    std::cout << "Value: " << w.getValue() << std::endl;
    return 0;
}
)";

static const char* kSamplePython = R"(
# Python Sample
import sys
class Animal:
    def __init__(self, name: str, age: int):
        self.name = name
        self.age = age
    def speak(self) -> str:
        return f"{self.name} says hello"
def main():
    dog = Animal("Buddy", 3)
    print(dog.speak())
if __name__ == "__main__":
    main()
)";

static const char* kSampleHtml = R"(
<!DOCTYPE html>
<html>
<head><title>Sample</title></head>
<body>
  <h1>Hello World</h1>
  <p>This is a paragraph with <strong>bold</strong> text.</p>
  <script>console.log("Hello from JS");</script>
</body>
</html>
)";

static const char* kSampleXml = R"(
<?xml version="1.0" encoding="UTF-8"?>
<project name="Example" version="1.0">
    <settings>
        <option name="debug">true</option>
    </settings>
</project>
)";

static const char* kSampleJson = R"(
{
    "name": "npp-qt",
    "version": "1.0.0",
    "features": ["syntax", "tabs", "themes"]
}
)";

static const char* kSampleSql = R"(
-- SQL Sample
SELECT u.id, u.name, COUNT(o.id) AS order_count
FROM users u
LEFT JOIN orders o ON o.user_id = u.id
WHERE u.created_at > '2024-01-01'
GROUP BY u.id, u.name
ORDER BY order_count DESC;
)";

static const char* kSampleBash = R"(
#!/bin/bash
set -euo pipefail
LOG_FILE="/tmp/app.log"
log() { echo "[$(date '+%H:%M:%S')] $*" >> "$LOG_FILE"; }
main() {
    log "Starting..."
    log "Done."
}
main
)";

static const char* kSampleDefault = R"(
# Notepad++ Qt6 Port
# Default sample text

This is a simple text file for preview purposes.
Plain text, numbers 12345, and symbols !@#$%^&*().
)";

// =============================================================================
// StyleConfigDialog
// =============================================================================

StyleConfigDialog::StyleConfigDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Style Configurator"));
    setMinimumSize(900, 600);
    setupUi();
    populateLexers();
}

void StyleConfigDialog::setupUi()
{
    auto* mainLayout = new QHBoxLayout(this);

    // --- Left panel: language selector + style tree ---
    auto* leftPanel = new QWidget(this);
    auto* leftLayout = new QVBoxLayout(leftPanel);

    // Language selector
    auto* langLabel = new QLabel(QStringLiteral("Language:"), this);
    _lexerCombo = new QComboBox(this);
    connect(_lexerCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &StyleConfigDialog::onLexerChanged);

    auto* langRow = new QHBoxLayout;
    langRow->addWidget(langLabel);
    langRow->addWidget(_lexerCombo, 1);
    leftLayout->addLayout(langRow);

    // Style tree
    _styleTree = new QTreeWidget(this);
    _styleTree->setHeaderLabels({QStringLiteral("Style"),
                                  QStringLiteral("Foreground"),
                                  QStringLiteral("Background"),
                                  QStringLiteral("Font")});
    _styleTree->header()->setStretchLastSection(false);
    _styleTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    _styleTree->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    _styleTree->header()->setSectionResizeMode(2, QHeaderView::Fixed);
    _styleTree->header()->setSectionResizeMode(3, QHeaderView::Stretch);
    _styleTree->setColumnWidth(1, 70);
    _styleTree->setColumnWidth(2, 70);
    connect(_styleTree, &QTreeWidget::itemClicked,
            this, &StyleConfigDialog::onStyleItemChanged);
    leftLayout->addWidget(_styleTree);

    // Style controls
    auto* ctrlGroup = new QGroupBox(QStringLiteral("Style Settings"), this);
    auto* ctrlLayout = new QFormLayout(ctrlGroup);

    _btnFgColor = new QPushButton(QStringLiteral("Choose..."), this);
    _lblFgColor = new QLabel(this);
    _lblFgColor->setFixedSize(40, 20);
    _lblFgColor->setAutoFillBackground(true);
    connect(_btnFgColor, &QPushButton::clicked, this, &StyleConfigDialog::onFgColorClicked);

    auto* fgRow = new QHBoxLayout;
    fgRow->addWidget(_lblFgColor);
    fgRow->addWidget(_btnFgColor);
    ctrlLayout->addRow(QStringLiteral("Foreground:"), fgRow);

    _btnBgColor = new QPushButton(QStringLiteral("Choose..."), this);
    _lblBgColor = new QLabel(this);
    _lblBgColor->setFixedSize(40, 20);
    _lblBgColor->setAutoFillBackground(true);
    connect(_btnBgColor, &QPushButton::clicked, this, &StyleConfigDialog::onBgColorClicked);

    auto* bgRow = new QHBoxLayout;
    bgRow->addWidget(_lblBgColor);
    bgRow->addWidget(_btnBgColor);
    ctrlLayout->addRow(QStringLiteral("Background:"), bgRow);

    _cbBold = new QCheckBox(QStringLiteral("Bold"), this);
    _cbItalic = new QCheckBox(QStringLiteral("Italic"), this);
    _cbUnderline = new QCheckBox(QStringLiteral("Underline"), this);
    connect(_cbBold, &QCheckBox::toggled, this, &StyleConfigDialog::onBoldClicked);
    connect(_cbItalic, &QCheckBox::toggled, this, &StyleConfigDialog::onItalicClicked);
    connect(_cbUnderline, &QCheckBox::toggled, this, &StyleConfigDialog::onUnderlineClicked);

    auto* fontStyleRow = new QHBoxLayout;
    fontStyleRow->addWidget(_cbBold);
    fontStyleRow->addWidget(_cbItalic);
    fontStyleRow->addWidget(_cbUnderline);
    fontStyleRow->addStretch();
    ctrlLayout->addRow(QStringLiteral("Font Style:"), fontStyleRow);

    _btnFontName = new QPushButton(QStringLiteral("Choose Font..."), this);
    _lblFontName = new QLabel(QStringLiteral(""), this);
    connect(_btnFontName, &QPushButton::clicked, this, &StyleConfigDialog::onFontNameClicked);

    auto* fnRow = new QHBoxLayout;
    fnRow->addWidget(_lblFontName);
    fnRow->addWidget(_btnFontName);
    ctrlLayout->addRow(QStringLiteral("Font:"), fnRow);

    _sbFontSize = new QSpinBox(this);
    _sbFontSize->setRange(4, 72);
    connect(_sbFontSize, qOverload<int>(&QSpinBox::valueChanged),
            this, &StyleConfigDialog::onFontSizeChanged);
    ctrlLayout->addRow(QStringLiteral("Font Size:"), _sbFontSize);

    leftLayout->addWidget(ctrlGroup);

    // --- Right panel: preview ---
    auto* rightPanel = new QWidget(this);
    auto* rightLayout = new QVBoxLayout(rightPanel);

    auto* previewLabel = new QLabel(QStringLiteral("Preview"), this);
    rightLayout->addWidget(previewLabel);

    // Use QsciScintilla for preview
    _previewWidget = new QsciScintilla(rightPanel);
    _previewWidget->setMinimumWidth(300);
    _previewWidget->setReadOnly(true);
    _previewWidget->setMarginsFont(QFont(QStringLiteral("monospace"), 10));
    _previewWidget->setMarginLineNumbers(0, true);
    _previewWidget->setMarginWidth(0, 40);
    _previewWidget->setFont(QFont(QStringLiteral("monospace"), 10));
    rightLayout->addWidget(_previewWidget, 1);

    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel, 1);

    // Buttons
    _buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        this);
    connect(_buttonBox, &QDialogButtonBox::accepted, this, &StyleConfigDialog::onOk);
    connect(_buttonBox, &QDialogButtonBox::rejected, this, &StyleConfigDialog::onCancel);
    connect(_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &StyleConfigDialog::onApply);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addLayout(mainLayout, 1);
    outerLayout->addWidget(_buttonBox);
}

// =============================================================================
// Lexer definitions
// =============================================================================

void StyleConfigDialog::populateLexers()
{
    // Clear existing
    _lexers.clear();
    _lexerCombo->clear();

    // Helper to add a lexer with its standard styles
    auto addLexer = [this](const QString& name, int lexerId,
                           const QVector<StyleItem>& styles,
                           const char* sample) {
        LexerStyle ls;
        ls.displayName = name;
        ls.lexerName = name.toLower();
        ls.lexerId = lexerId;
        ls.styles = styles;
        _lexers.append(ls);
        _lexerCombo->addItem(name);
    };

    // C / C++
    addLexer(QStringLiteral("C++"), 3, QVector<StyleItem>{
        {9,  QStringLiteral("Default"),        QColor(0, 0, 0),     QColor(255, 255, 255)},
        {7,  QStringLiteral("Comment"),        QColor(0, 100, 0),   QColor(255, 255, 255)},
        {10, QStringLiteral("Comment Line"),   QColor(0, 100, 0),   QColor(255, 255, 255)},
        {17, QStringLiteral("Preprocessor"),   QColor(100, 0, 100), QColor(255, 240, 255)},
        {1,  QStringLiteral("Keyword"),        QColor(0, 0, 128),   QColor(255, 255, 255)},
        {13, QStringLiteral("String"),          QColor(139, 0, 0),   QColor(255, 240, 240)},
        {12, QStringLiteral("Character"),       QColor(139, 0, 0),   QColor(255, 240, 240)},
        {2,  QStringLiteral("Number"),          QColor(0, 0, 0),     QColor(255, 255, 255)},
        {6,  QStringLiteral("Operator"),        QColor(0, 0, 0),     QColor(255, 255, 255)},
        {4,  QStringLiteral("Identifier"),      QColor(0, 0, 0),     QColor(255, 255, 255)},
        {15, QStringLiteral("Class Name"),      QColor(70, 130, 180), QColor(255, 255, 255)},
        {16, QStringLiteral("Function Name"),   QColor(70, 130, 180), QColor(255, 255, 255)},
    }, kSampleCpp);

    // Python
    addLexer(QStringLiteral("Python"), 1, QVector<StyleItem>{
        {9,  QStringLiteral("Default"),        QColor(0, 0, 0),     QColor(255, 255, 255)},
        {31, QStringLiteral("Comment"),        QColor(98, 98, 98),  QColor(255, 255, 255)},
        {0,  QStringLiteral("Keyword"),        QColor(0, 0, 200),   QColor(255, 255, 255)},
        {3,  QStringLiteral("String"),          QColor(0, 139, 0),   QColor(255, 255, 255)},
        {2,  QStringLiteral("Number"),          QColor(139, 0, 139), QColor(255, 255, 255)},
        {10, QStringLiteral("Decorator"),      QColor(100, 0, 100), QColor(255, 255, 255)},
        {40, QStringLiteral("Function Name"),  QColor(0, 100, 0),   QColor(255, 255, 255)},
    }, kSamplePython);

    // HTML
    addLexer(QStringLiteral("HTML"), 4, QVector<StyleItem>{
        {9,  QStringLiteral("Default"),        QColor(0, 0, 0),     QColor(255, 255, 255)},
        {9,  QStringLiteral("HTML Tag"),       QColor(0, 0, 128),   QColor(255, 255, 255)},
        {10, QStringLiteral("HTML Value"),     QColor(139, 0, 139), QColor(255, 255, 255)},
        {11, QStringLiteral("HTML Number"),     QColor(0, 139, 0),   QColor(255, 255, 255)},
        {7,  QStringLiteral("HTML Comment"),    QColor(128, 128, 128), QColor(255, 255, 255)},
        {5,  QStringLiteral("HTML Attribute"),  QColor(128, 0, 128), QColor(255, 255, 255)},
        {3,  QStringLiteral("HTML String"),     QColor(0, 139, 0),   QColor(255, 255, 255)},
    }, kSampleHtml);

    // XML
    addLexer(QStringLiteral("XML"), 5, QVector<StyleItem>{
        {9,  QStringLiteral("Default"),         QColor(0, 0, 0),     QColor(255, 255, 255)},
        {1,  QStringLiteral("XML Tag"),         QColor(0, 0, 128),   QColor(255, 255, 255)},
        {2,  QStringLiteral("XML Attribute"),  QColor(128, 0, 128), QColor(255, 255, 255)},
        {3,  QStringLiteral("XML Value"),      QColor(139, 0, 139), QColor(255, 255, 255)},
        {7,  QStringLiteral("XML Comment"),    QColor(128, 128, 128), QColor(255, 255, 255)},
        {4,  QStringLiteral("XML CDATA"),       QColor(0, 100, 0),   QColor(240, 240, 240)},
    }, kSampleXml);

    // JSON
    addLexer(QStringLiteral("JSON"), 18, QVector<StyleItem>{
        {9,  QStringLiteral("Default"),         QColor(0, 0, 0),     QColor(255, 255, 255)},
        {0,  QStringLiteral("Keyword"),         QColor(0, 0, 180),   QColor(255, 255, 255)},
        {3,  QStringLiteral("String"),          QColor(139, 0, 0),   QColor(255, 255, 255)},
        {2,  QStringLiteral("Number"),          QColor(0, 139, 139), QColor(255, 255, 255)},
        {4,  QStringLiteral("Property"),        QColor(128, 0, 128), QColor(255, 255, 255)},
    }, kSampleJson);

    // SQL
    addLexer(QStringLiteral("SQL"), 6, QVector<StyleItem>{
        {9,  QStringLiteral("Default"),         QColor(0, 0, 0),     QColor(255, 255, 255)},
        {1,  QStringLiteral("Keyword"),         QColor(0, 0, 128),   QColor(255, 255, 255)},
        {2,  QStringLiteral("Number"),          QColor(0, 139, 139), QColor(255, 255, 255)},
        {3,  QStringLiteral("String"),          QColor(139, 0, 0),   QColor(255, 255, 255)},
        {13, QStringLiteral("Comment"),         QColor(0, 100, 0),   QColor(255, 255, 255)},
        {5,  QStringLiteral("Identifier"),     QColor(0, 0, 0),     QColor(255, 255, 255)},
    }, kSampleSql);

    // Bash
    addLexer(QStringLiteral("Bash"), 10, QVector<StyleItem>{
        {9,  QStringLiteral("Default"),         QColor(0, 0, 0),     QColor(255, 255, 255)},
        {1,  QStringLiteral("Keyword"),         QColor(0, 0, 200),   QColor(255, 255, 255)},
        {3,  QStringLiteral("String"),          QColor(139, 0, 0),   QColor(255, 255, 255)},
        {2,  QStringLiteral("Number"),          QColor(0, 139, 139), QColor(255, 255, 255)},
        {12, QStringLiteral("Comment"),         QColor(98, 98, 98),  QColor(255, 255, 255)},
        {4,  QStringLiteral("Variable"),       QColor(0, 100, 0),   QColor(255, 255, 255)},
    }, kSampleBash);

    // Rust
    addLexer(QStringLiteral("Rust"), SCLEX_RUST, QVector<StyleItem>{
        {9,  QStringLiteral("Default"),         QColor(0, 0, 0),       QColor(255, 255, 255)},
        {1,  QStringLiteral("Keyword"),         QColor(0, 0, 139),     QColor(255, 255, 255)},
        {2,  QStringLiteral("Number"),          QColor(0, 139, 139),   QColor(255, 255, 255)},
        {7,  QStringLiteral("Comment"),         QColor(98, 98, 98),    QColor(255, 255, 255)},
        {10, QStringLiteral("Comment Line"),   QColor(98, 98, 98),    QColor(255, 255, 255)},
        {13, QStringLiteral("String"),          QColor(139, 0, 0),     QColor(255, 255, 255)},
        {15, QStringLiteral("Class Name"),      QColor(70, 130, 180),  QColor(255, 255, 255)},
        {16, QStringLiteral("Function Name"),   QColor(70, 130, 180),  QColor(255, 255, 255)},
        {20, QStringLiteral("Attribute"),        QColor(100, 0, 100),  QColor(255, 255, 255)},
        {21, QStringLiteral("Lifetime"),         QColor(180, 50, 0),    QColor(255, 255, 255)},
        {22, QStringLiteral("Macro"),           QColor(0, 100, 100),   QColor(255, 255, 255)},
    }, "# fn main() {\n    println!(\"Hello, world!\");\n}\n");

    // Go
    addLexer(QStringLiteral("Go"), 200, QVector<StyleItem>{
        {9,  QStringLiteral("Default"),         QColor(0, 0, 0),       QColor(255, 255, 255)},
        {1,  QStringLiteral("Keyword"),         QColor(0, 0, 139),     QColor(255, 255, 255)},
        {2,  QStringLiteral("Number"),          QColor(0, 139, 139),   QColor(255, 255, 255)},
        {7,  QStringLiteral("Comment"),         QColor(98, 98, 98),    QColor(255, 255, 255)},
        {13, QStringLiteral("String"),          QColor(139, 0, 0),     QColor(255, 255, 255)},
        {20, QStringLiteral("Label"),           QColor(0, 0, 139),     QColor(255, 255, 255)},
        {21, QStringLiteral("Package"),         QColor(0, 100, 0),     QColor(255, 255, 255)},
        {22, QStringLiteral("Import"),           QColor(0, 100, 100),   QColor(255, 255, 255)},
    }, "package main\n\nfunc main() {\n    println(\"Hello, world!\")\n}\n");

    // PowerShell
    addLexer(QStringLiteral("PowerShell"), SCLEX_POWERSHELL, QVector<StyleItem>{
        {LexerPowerShell::Default,    QStringLiteral("Default"),      QColor(0, 0, 0),     QColor(255, 255, 255)},
        {LexerPowerShell::Comment,    QStringLiteral("Comment"),     QColor(98, 98, 98),  QColor(255, 255, 255)},
        {LexerPowerShell::String,     QStringLiteral("String"),      QColor(139, 0, 0),   QColor(255, 255, 255)},
        {LexerPowerShell::Keyword,    QStringLiteral("Keyword"),     QColor(0, 0, 139),   QColor(255, 255, 255)},
        {LexerPowerShell::Cmdlet,     QStringLiteral("Cmdlet"),      QColor(100, 0, 100), QColor(255, 255, 255)},
        {LexerPowerShell::Variable,   QStringLiteral("Variable"),   QColor(0, 100, 0),   QColor(255, 255, 255)},
        {LexerPowerShell::Operator,   QStringLiteral("Operator"),   QColor(139, 0, 139), QColor(255, 255, 255)},
        {LexerPowerShell::Number,     QStringLiteral("Number"),      QColor(0, 139, 139), QColor(255, 255, 255)},
    }, "# PowerShell script\nGet-Process | Where-Object { $_.CPU -gt 10 }\nWrite-Host \"Done\"\n");

    // Default (plain text)
    addLexer(QStringLiteral("Normal Text"), 0, QVector<StyleItem>{
        {0, QStringLiteral("Default"), QColor(0, 0, 0), QColor(255, 255, 255)},
    }, kSampleDefault);

    // Select first lexer and populate tree
    if (_lexerCombo->count() > 0) {
        _lexerCombo->setCurrentIndex(0);
        onLexerChanged(0);
    }
}

// =============================================================================
// Style tree population
// =============================================================================

void StyleConfigDialog::populateStyleTree(const LexerStyle& lexer)
{
    _styleTree->clear();

    for (const StyleItem& si : lexer.styles) {
        auto* item = new QTreeWidgetItem(_styleTree);
        item->setText(0, si.name);
        item->setData(0, Qt::UserRole, QVariant::fromValue(si));

        // Color swatches as text (color hex)
        item->setText(1, si.fgColor.isValid() ? si.fgColor.name().toUpper() : QStringLiteral("Auto"));
        item->setText(2, si.bgColor.isValid() ? si.bgColor.name().toUpper() : QStringLiteral("Auto"));

        // Font style summary
        QString fontStyle;
        if (si.bold) fontStyle += QStringLiteral("B ");
        if (si.italic) fontStyle += QStringLiteral("I ");
        if (si.underline) fontStyle += QStringLiteral("U ");
        if (fontStyle.isEmpty()) fontStyle = QStringLiteral("-");
        item->setText(3, fontStyle.trimmed());

        _styleTree->addTopLevelItem(item);
    }
}

// =============================================================================
// Style item interaction
// =============================================================================

void StyleConfigDialog::onLexerChanged(int index)
{
    if (index < 0 || index >= _lexers.size())
        return;

    _currentLexerIdx = index;
    const LexerStyle& lexer = _lexers.at(index);
    populateStyleTree(lexer);

    // Set lexer on preview
    auto* sci = qobject_cast<QsciScintilla*>(_previewWidget);
    if (!sci)
        return;

    QsciLexer* newLexer = nullptr;
    switch (lexer.lexerId) {
        case 3: newLexer = new QsciLexerCPP(sci); break;   // C++
        case 1: newLexer = new QsciLexerPython(sci); break; // Python
        case 4: newLexer = new QsciLexerHTML(sci); break;    // HTML
        case 5: newLexer = new QsciLexerXML(sci); break;     // XML
        case 18: newLexer = new QsciLexerJSON(sci); break;   // JSON
        case 6: newLexer = new QsciLexerSQL(sci); break;     // SQL
        case 10: newLexer = new QsciLexerBash(sci); break;   // Bash
        case SCLEX_RUST: newLexer = new LexerRust(sci); break;   // Rust
        case 200: newLexer = new LexerGo(sci); break;          // Go (custom ID)
        case SCLEX_POWERSHELL: newLexer = new LexerPowerShell(sci); break; // PowerShell
        default: break;
    }

    sci->setLexer(newLexer);

    // Set sample code
    const char* sample = kSampleDefault;
    if (lexer.lexerId == 3) sample = kSampleCpp;
    else if (lexer.lexerId == 1) sample = kSamplePython;
    else if (lexer.lexerId == 4) sample = kSampleHtml;
    else if (lexer.lexerId == 5) sample = kSampleXml;
    else if (lexer.lexerId == 18) sample = kSampleJson;
    else if (lexer.lexerId == 6) sample = kSampleSql;
    else if (lexer.lexerId == 10) sample = kSampleBash;
    else if (lexer.lexerId == SCLEX_RUST) sample = "# fn main() {\n    println!(\"Hello, world!\");\n}\n";
    else if (lexer.lexerId == 200) sample = "package main\n\nfunc main() {\n    println(\"Hello, world!\")\n}\n";
    else if (lexer.lexerId == SCLEX_POWERSHELL) sample = "# PowerShell script\nGet-Process | Where-Object { $_.CPU -gt 10 }\nWrite-Host \"Done\"\n";

    sci->setText(QString::fromLatin1(sample));
    applyStyleToPreview();
}

void StyleConfigDialog::onStyleItemChanged(QTreeWidgetItem* item, int)
{
    if (!item)
        return;

    _currentItem = item->data(0, Qt::UserRole).value<StyleItem>();

    // Update color swatches
    if (_currentItem.fgColor.isValid()) {
        _lblFgColor->setStyleSheet(QStringLiteral("background-color: %1").arg(_currentItem.fgColor.name()));
    } else {
        _lblFgColor->setStyleSheet(QStringLiteral("background-color: white"));
    }

    if (_currentItem.bgColor.isValid()) {
        _lblBgColor->setStyleSheet(QStringLiteral("background-color: %1").arg(_currentItem.bgColor.name()));
    } else {
        _lblBgColor->setStyleSheet(QStringLiteral("background-color: white"));
    }

    _cbBold->setChecked(_currentItem.bold);
    _cbItalic->setChecked(_currentItem.italic);
    _cbUnderline->setChecked(_currentItem.underline);
    _sbFontSize->setValue(10);
}

void StyleConfigDialog::onFgColorClicked()
{
    QColorDialog dlg(this);
    dlg.setCurrentColor(_currentItem.fgColor.isValid() ? _currentItem.fgColor : Qt::black);
    if (dlg.exec() == QDialog::Accepted) {
        _currentItem.fgColor = dlg.currentColor();
        _lblFgColor->setStyleSheet(QStringLiteral("background-color: %1").arg(_currentItem.fgColor.name()));
        applyStyleToPreview();
    }
}

void StyleConfigDialog::onBgColorClicked()
{
    QColorDialog dlg(this);
    dlg.setCurrentColor(_currentItem.bgColor.isValid() ? _currentItem.bgColor : Qt::white);
    if (dlg.exec() == QDialog::Accepted) {
        _currentItem.bgColor = dlg.currentColor();
        _lblBgColor->setStyleSheet(QStringLiteral("background-color: %1").arg(_currentItem.bgColor.name()));
        applyStyleToPreview();
    }
}

void StyleConfigDialog::onBoldClicked()
{
    _currentItem.bold = _cbBold->isChecked();
    applyStyleToPreview();
}

void StyleConfigDialog::onItalicClicked()
{
    _currentItem.italic = _cbItalic->isChecked();
    applyStyleToPreview();
}

void StyleConfigDialog::onUnderlineClicked()
{
    _currentItem.underline = _cbUnderline->isChecked();
    applyStyleToPreview();
}

void StyleConfigDialog::onFontNameClicked()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, QFont(QStringLiteral("monospace"), 10), this);
    if (ok) {
        _currentItem.enabled = true;
        _lblFontName->setText(font.family());
        applyStyleToPreview();
    }
}

void StyleConfigDialog::onFontSizeChanged(int)
{
    applyStyleToPreview();
}

void StyleConfigDialog::applyStyleToPreview()
{
    auto* sci = qobject_cast<QsciScintilla*>(_previewWidget);
    if (!sci)
        return;

    QsciLexer* lexer = sci->lexer();
    if (!lexer)
        return;

    int styleIdx = _currentItem.styleIndex;
    QColor fg = _currentItem.fgColor.isValid() ? _currentItem.fgColor : Qt::black;
    QColor bg = _currentItem.bgColor.isValid() ? _currentItem.bgColor : Qt::white;

    // Apply via QsciLexer API
    lexer->setColor(fg, styleIdx);
    lexer->setPaper(bg, styleIdx);

    QFont f = lexer->font(styleIdx);
    if (_currentItem.bold) f.setBold(true);
    if (_currentItem.italic) f.setItalic(true);
    if (_currentItem.underline) f.setUnderline(true);
    lexer->setFont(f, styleIdx);

    // Refresh preview
    sci->update();
}

void StyleConfigDialog::onApply()
{
    saveStyles();
}

void StyleConfigDialog::onOk()
{
    saveStyles();
    accept();
}

void StyleConfigDialog::onCancel()
{
    reject();
}

void StyleConfigDialog::saveStyles()
{
    QSettings s(QStringLiteral("NotepadPlusPlus"), QStringLiteral("npp-qt-styles"));
    s.beginGroup(QStringLiteral("Styles"));

    for (const LexerStyle& lexer : _lexers) {
        s.beginGroup(lexer.lexerName);
        for (const StyleItem& si : lexer.styles) {
            s.beginGroup(QStringLiteral("S%1").arg(si.styleIndex));
            s.setValue(QStringLiteral("fg"), si.fgColor);
            s.setValue(QStringLiteral("bg"), si.bgColor);
            int styleFlags = 0;
            if (si.bold) styleFlags |= 1;
            if (si.italic) styleFlags |= 2;
            if (si.underline) styleFlags |= 4;
            s.setValue(QStringLiteral("flags"), styleFlags);
            s.endGroup();
        }
        s.endGroup();
    }
    s.endGroup();
    s.sync();
}
