// FunctionListPanel.cpp - Enhanced function list/outline panel implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FunctionListPanel.h"
#include "FunctionListXmlParser.h"
#include "../editor/ScintillaEditor.h"
#include "../common/Types.h"
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QLabel>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <algorithm>

// ============================================================================
// Language patterns
// ============================================================================

// C/C++: function definitions, classes, structs, namespaces
static const QRegularExpression cppFuncPattern(
    R"(^\s*((?:virtual|static|inline|explicit|const|constexpr|friend|unsigned|signed|short|long|mutable|volatile|override|final)\s+)*"
    R"((?:[\w:]+(?:\s*[*&]+)?\s+)?"       // optional return type
    R"([\w:]+)\s*\([^)]*\)\s*(?:const)?\s*[;{]?\s*$)",
    QRegularExpression::MultilineOption);

// C++ class/struct/enum declarations
static const QRegularExpression cppClassPattern(
    R"(^\s*((?:template|class|struct|enum|union|namespace|using)\s+)(\w+))",
    QRegularExpression::MultilineOption);

// C++ access specifiers
static const QRegularExpression cppAccessPattern(
    R"(^\s*(public|private|protected)\s*:)",
    QRegularExpression::MultilineOption);

// Python: def, class, async def, @decorator
static const QRegularExpression pyFuncPattern(
    R"(^\s*(@\w+.*\n\s*)?(?:(async)\s+)?(def|class)\s+(\w+))",
    QRegularExpression::MultilineOption);

// JavaScript/TypeScript: function, class, const/let/var assignments, arrow functions
static const QRegularExpression jsFuncPattern(
    R"(^\s*(?:(export|async)?\s*)?(?:(function|class)\s+(\w+)|"
    R"((?:(const|let|var)\s+)?(\w+)\s*=\s*(?:async\s+)?(?:function\s*)?(?:\([^)]*\)\s*|"
    R"(\w+\s*=>))))",
    QRegularExpression::MultilineOption);

// Rust: fn, struct, impl, pub, mod, trait, enum, macro_rules!
static const QRegularExpression rustFuncPattern(
    R"(^\s*(pub\s+)?(fn|struct|impl|enum|trait|mod|macro_rules!)\s+(\w+))",
    QRegularExpression::MultilineOption);

// Go: func, type, struct, interface, package
static const QRegularExpression goFuncPattern(
    R"(^\s*(func|type|struct|interface)\s*(\{?\s*)?(\w+)?)",
    QRegularExpression::MultilineOption);

// Ruby: def, class, module, attr_accessor/reader/writer
static const QRegularExpression rubyFuncPattern(
    R"(^\s*(def|class|module|attr_(?:accessor|reader|writer))\s+(\w+))",
    QRegularExpression::MultilineOption);

// Generic function pattern (fallback for unknown languages)
static const QRegularExpression genericFuncPattern(
    R"(^\s*([\w:]+(?:\s*[*&]+)?)\s+([\w_][\w\d_]*)\s*\([^)]*\)\s*\{?\s*$)",
    QRegularExpression::MultilineOption);

// ============================================================================
// Constructor / Destructor
// ============================================================================

FunctionListPanel::FunctionListPanel(QWidget* parent)
    : QDockWidget("Function List", parent)
{
    setObjectName("FunctionListPanel");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget* content = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    // Search box
    _searchBox = new QLineEdit(content);
    _searchBox->setPlaceholderText(tr("Search functions..."));
    _searchBox->setClearButtonEnabled(true);
    layout->addWidget(_searchBox);

    // Top filter row
    QHBoxLayout* filterRow = new QHBoxLayout();
    filterRow->setSpacing(4);

    // Type filter combo box
    _typeFilter = new QComboBox(content);
    _typeFilter->addItem(tr("All Types"), -1);
    _typeFilter->addItem(tr("Functions"), 0);
    _typeFilter->addItem(tr("Classes"), 1);
    _typeFilter->addItem(tr("Methods"), 2);
    _typeFilter->addItem(tr("Structs/Interfaces"), 3);
    filterRow->addWidget(_typeFilter, 1);

    // Access modifier filter (for languages that support it)
    QComboBox* accessFilter = new QComboBox(content);
    accessFilter->setObjectName("accessFilter");
    accessFilter->addItem(tr("All Access"), -1);
    accessFilter->addItem(tr("Public"), 0);
    accessFilter->addItem(tr("Private"), 1);
    accessFilter->addItem(tr("Protected"), 2);
    filterRow->addWidget(accessFilter, 1);

    layout->addLayout(filterRow);

    // Sort combo
    QHBoxLayout* sortRow = new QHBoxLayout();
    sortRow->setSpacing(4);
    QLabel* sortLabel = new QLabel(tr("Sort:"), content);
    sortLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    _sortCombo = new QComboBox(content);
    _sortCombo->addItem(tr("Name (A-Z)"), 0);
    _sortCombo->addItem(tr("Name (Z-A)"), 1);
    _sortCombo->addItem(tr("Line (asc)"), 2);
    _sortCombo->addItem(tr("Line (desc)"), 3);
    sortRow->addWidget(sortLabel);
    sortRow->addWidget(_sortCombo, 1);
    layout->addLayout(sortRow);

    // Tree widget for function list
    _tree = new QTreeWidget(content);
    _tree->setHeaderLabels(QStringList() << tr("Name") << tr("Line") << tr("Type") << tr("Access"));
    _tree->header()->setStretchLastSection(false);
    _tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    _tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    _tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    _tree->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    _tree->setAlternatingRowColors(true);
    _tree->setSortingEnabled(false);  // We handle sorting ourselves
    layout->addWidget(_tree);

    setWidget(content);

    // Connections
    connect(_tree, &QTreeWidget::itemDoubleClicked, this, &FunctionListPanel::onItemDoubleClicked);
    connect(_searchBox, &QLineEdit::textChanged, this, &FunctionListPanel::onSearchTextChanged);
    connect(_typeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FunctionListPanel::onTypeFilterChanged);
    connect(_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FunctionListPanel::onSortChanged);
    connect(accessFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FunctionListPanel::onTypeFilterChanged);

    // Debounce timer for refresh
    _debounceTimer = new QTimer(this);
    _debounceTimer->setSingleShot(true);
    _debounceTimer->setInterval(300);
    connect(_debounceTimer, &QTimer::timeout, this, &FunctionListPanel::refresh);
}

FunctionListPanel::~FunctionListPanel() {
    delete _xmlParser;
}

QString FunctionListPanel::langKeyFromType(LangType t) {
    switch (t) {
        case LangType::L_C:          return "c";
        case LangType::L_CPP:        return "cpp";
        case LangType::L_JAVA:       return "java";
        case LangType::L_CS:         return "cs";
        case LangType::L_PYTHON:     return "python";
        case LangType::L_JS:         return "javascript";
        case LangType::L_HTML:       return "html";
        case LangType::L_XML:        return "xml";
        case LangType::L_PHP:        return "php";
        case LangType::L_RUBY:       return "ruby";
        case LangType::L_PERL:       return "perl";
        case LangType::L_LUA:        return "lua";
        case LangType::L_CSS:        return "css";
        case LangType::L_YAML:       return "yaml";
        case LangType::L_JSON:       return "javascript";  // reuse JS parser
        case LangType::L_MAKEFILE:   return "makefile";
        case LangType::L_CMAKE:      return "cmake";
        case LangType::L_MARKDOWN:   return "markdown";
        case LangType::L_BATCH:      return "batch";
        case LangType::L_POWERSHELL: return "powershell";
        case LangType::L_SQL:        return "sql";
        case LangType::L_PASCAL:      return "pascal";
        case LangType::L_FORTRAN:     return "fortran";
        case LangType::L_LISP:        return "lisp";
        case LangType::L_RUST:        return "rust";
        case LangType::L_GO:          return "go";
        case LangType::L_SWIFT:       return "swift";
        case LangType::L_KOTLIN:      return "kotlin";
        case LangType::L_OBJC:  return "c";  // ObjC shares C parser
        case LangType::L_VERILOG:     return "verilog";
        case LangType::L_VHDL:        return "vhdl";
        case LangType::L_ASM:         return "asm";
        case LangType::L_MATLAB:      return "matlab";
        case LangType::L_R:           return "r";
        case LangType::L_JULIA:       return "julia";
        case LangType::L_NFO:         return "nfo";
        case LangType::L_TCL:         return "tcl";
        case LangType::L_NSIS:        return "nsis";
        case LangType::L_NSIS2:       return "nsis";
        case LangType::L_ERLANG:      return "erlang";
        case LangType::L_BASH:        return "bash";
        case LangType::L_TEX:         return "tex";
        case LangType::L_DIFF:        return "diff";
        case LangType::L_SCALA:       return "scala";
        case LangType::L_TS:          return "typescript";
        case LangType::L_HASKELL:     return "haskell";
        case LangType::L_ADA:         return "ada";
        default:                      return "generic";
    }
}

void FunctionListPanel::initXmlParser() {
    if (_xmlParser)
        return;

    // Look for the functionList XML directory.
    // Precedence: (1) $APP_DATA/functionList/  (2) $REPO_ROOT/src/functionList/
    //             (3) upstream notepad-plus-plus-translation/
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QStringList searchPaths = {
        appData + "/functionList",
        "/home/node/.openclaw/workspace/notepad-plus-plus-translation/PowerEditor/installer/functionList",
    };

    QString foundPath;
    for (const QString& p : searchPaths) {
        if (QFileInfo::exists(p)) {
            foundPath = p;
            break;
        }
    }

    if (foundPath.isEmpty()) {
        qWarning() << "FunctionListPanel: no functionList XML directory found; "
                      "searched:"
                   << searchPaths;
        foundPath = searchPaths.first(); // try anyway
    }

    _xmlParser = new FunctionListXmlParser(foundPath);
    qDebug() << "FunctionListPanel: XML parser initialised with dir:" << foundPath;
}

void FunctionListPanel::ensureXmlParserLoaded(LangType lang) {
    initXmlParser();
    const QString key = langKeyFromType(lang);
    if (!_xmlParserLoaded.contains(key)) {
        bool loaded = _xmlParser->loadParser(key);
        _xmlParserLoaded[key] = loaded;
        if (loaded) {
            qDebug() << "FunctionListPanel: loaded XML parser for" << key;
        }
    }
}

// ============================================================================
// Editor binding
// ============================================================================

void FunctionListPanel::setEditor(ScintillaEditor* editor) {
    if (_editor) {
        disconnect(_editor, &ScintillaEditor::textChanged, this, &FunctionListPanel::refresh);
        disconnect(_editor, &ScintillaEditor::cursorPositionChanged, this, nullptr);
    }

    _editor = editor;

    if (_editor) {
        connect(_editor, &ScintillaEditor::textChanged, this, &FunctionListPanel::refresh);
    }

    refresh();
}

// ============================================================================
// Refresh
// ============================================================================

void FunctionListPanel::refresh() {
    // Save current selection for restoration
    QTreeWidgetItem* current = _tree->currentItem();
    if (current) {
        bool ok;
        _lastSelectedLine = current->text(1).toInt(&ok) - 1;
        if (!ok) _lastSelectedLine = -1;
    } else {
        _lastSelectedLine = -1;
    }

    _tree->clear();
    _allFunctions.clear();

    if (!_editor) return;

    parseFunctions();
    applyFilters();

    // Restore selection
    if (_lastSelectedLine >= 0) {
        for (int i = 0; i < _tree->topLevelItemCount(); ++i) {
            QTreeWidgetItem* item = _tree->topLevelItem(i);
            if (item) {
                bool ok;
                int itemLine = item->text(1).toInt(&ok) - 1;
                if (ok && itemLine == _lastSelectedLine) {
                    _tree->setCurrentItem(item);
                    _tree->scrollToItem(item);
                    break;
                }
            }
        }
    }
}

// ============================================================================
// Parsing
// ============================================================================

void FunctionListPanel::parseFunctions() {
    if (!_editor) return;

    const QString text = _editor->toPlainText();
    if (text.isEmpty()) return;

    const QStringList lines = text.split('\n');
    LangType lang = _editor->language();

    // Dispatch to language-specific parser
    // Priority: try XML parser first (uses upstream N++ rules), fall back to
    // hard-coded regex.
    switch (lang) {
        // ── XML-backed languages (preferred) ──────────────────────────────
        case LangType::L_CPP:
        case LangType::L_C:
        case LangType::L_JAVA:
        case LangType::L_OBJC:
        case LangType::L_PHP:
        case LangType::L_LUA:
        case LangType::L_CSS:
        case LangType::L_HTML:
        case LangType::L_XML:
        case LangType::L_MAKEFILE:
        case LangType::L_RUST:
        case LangType::L_GO:
        case LangType::L_SWIFT:
        case LangType::L_KOTLIN:
        case LangType::L_PYTHON:
        case LangType::L_PERL:
        case LangType::L_RUBY:
        case LangType::L_JS:
        case LangType::L_JSON:
        case LangType::L_CS:
        case LangType::L_TS:
        case LangType::L_TCL:
        case LangType::L_NFO:
        case LangType::L_NSIS:
        case LangType::L_NSIS2:
        case LangType::L_POWERSHELL:
        case LangType::L_MATLAB:
        case LangType::L_R:
        case LangType::L_JULIA:
        case LangType::L_YAML:
        case LangType::L_FORTRAN:
        case LangType::L_PASCAL:
        case LangType::L_LISP:
        case LangType::L_VERILOG:
        case LangType::L_VHDL:
        case LangType::L_ERLANG:
        case LangType::L_BASH:
        case LangType::L_SCALA:
        case LangType::L_HASKELL:
        case LangType::L_ADA:
        case LangType::L_ASM:
            parseWithXmlParser(lines, lang);
            break;

        // ── Hard-coded regex languages (no upstream XML yet) ──────────────
        case LangType::L_MARKDOWN:
        case LangType::L_BATCH:
        case LangType::L_INI:
            parseCpp(lines);  // C-style heuristics
            break;

        default:
            parseGeneric(lines);
            break;
    }
}

void FunctionListPanel::parseCpp(const QStringList& lines) {
    // Note: Rust, Go, and other languages would need their own parsers.
    // Here we implement a generic C-family parser.
    Q_UNUSED(lines);
    // Basic C++ function pattern
    // For brevity, this delegates to the existing regex approach

    const QString text = _editor->toPlainText();
    const QStringList lines2 = text.split('\n');

    // C++ function definitions
    QRegularExpression cppFunc(
        R"(^\s*((?:virtual|static|inline|explicit|const|constexpr|friend|unsigned|signed|short|long|mutable|volatile|override|final| noexcept)\s+)*"
        R"((?:[\w:]+(?:\s*[*&]+)?\s+)?)"
        R"([\w:]+)\s*\([^)]*\)\s*(?:const)?\s*[;{]?\s*$)",
        QRegularExpression::MultilineOption);

    // Class/struct/enum/namespace declarations
    QRegularExpression cppDecl(
        R"(^\s*(?:template\s*<[^>]*>\s*)?(class|struct|enum|union|namespace)\s+(\w+))",
        QRegularExpression::MultilineOption);

    // Access specifiers
    QString currentAccess = "public";
    QRegularExpression accessRe(R"(^\s*(public|private|protected)\s*:)", QRegularExpression::MultilineOption);

    for (int i = 0; i < lines2.size(); ++i) {
        const QString& line = lines2[i];

        // Check for access specifier
        QRegularExpressionMatch am = accessRe.match(line);
        if (am.hasMatch()) {
            currentAccess = am.captured(1);
            continue;
        }

        // Try function definition
        QRegularExpressionMatch fm = cppFunc.match(line);
        if (fm.hasMatch()) {
            QString name = fm.captured(3);
            // Determine if it's a method or function based on const/void
            QString type = "function";
            if (line.contains("virtual") || line.contains("override")) {
                type = "method";
            } else if (line.contains("static")) {
                type = "function";
            } else if (fm.captured(2).contains("void")) {
                type = "function";
            }
            addFunctionItem(name, i + 1, type, currentAccess, "cpp");
            continue;
        }

        // Try class/struct declaration
        QRegularExpressionMatch dm = cppDecl.match(line);
        if (dm.hasMatch()) {
            QString declType = dm.captured(1);
            QString name = dm.captured(2);
            QString type = (declType == "class" || declType == "struct") ? "class" : declType;
            addFunctionItem(name, i + 1, type, currentAccess, "cpp");
        }
    }
}

void FunctionListPanel::parsePython(const QStringList& lines) {
    // Python: def, class, async def, decorators
    QRegularExpression funcRe(
        R"(^\s*(@\S+.*\n\s*)?(?:(async)\s+)?(def|class)\s+(\w+))",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch m = funcRe.match(line);
        if (m.hasMatch()) {
            bool isAsync = !m.captured(2).isEmpty();
            QString typeKw = m.captured(3);
            QString name = m.captured(4);

            QString type;
            if (typeKw == "class") {
                type = "class";
            } else {
                type = isAsync ? "async function" : "function";
            }
            addFunctionItem(name, i + 1, type, "public", "python");
        }
    }
}

void FunctionListPanel::parseJavaScript(const QStringList& lines) {
    // JavaScript/TypeScript: function, class, arrow functions, const/let/var assignments
    QRegularExpression funcRe(
        R"(^\s*(?:(export|default|async)\s+)?((?:(function)\s+(\w+)|"
        R"((class)\s+(\w+)(?:\s+extends\s+\w+)?)|"
        R"((?:(const|let|var)\s+)?(\w+)\s*=\s*(?:async\s+)?(?:function\s*)?"
        R"((?:\([^)]*\)|[\w]+)\s*(?:=>|{|\s*function)))))",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch m = funcRe.match(line);
        if (m.hasMatch()) {
            // function name
            if (!m.captured(3).isEmpty()) {
                addFunctionItem(m.captured(3), i + 1, "function", "public", "js");
            }
            // class name
            else if (!m.captured(5).isEmpty()) {
                addFunctionItem(m.captured(5), i + 1, "class", "public", "js");
            }
            // const/let/var assignment
            else if (!m.captured(8).isEmpty()) {
                QString name = m.captured(8);
                QString type = line.contains("=>") ? "arrow function" : "function";
                addFunctionItem(name, i + 1, type, "public", "js");
            }
            // constructor
            else if (!m.captured(10).isEmpty()) {
                addFunctionItem("constructor", i + 1, "method", "public", "js");
            }
        }
    }

    // Also catch TypeScript interfaces and types
    QRegularExpression tsRe(
        R"(^\s*(?:export\s+)?(?:(interface|type|enum|namespace)\s+(\w+)))",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        QRegularExpressionMatch m = tsRe.match(lines[i]);
        if (m.hasMatch()) {
            QString type = m.captured(1);
            if (type == "interface" || type == "type" || type == "enum") {
                addFunctionItem(m.captured(2), i + 1, type, "public", "js");
            }
        }
    }
}

void FunctionListPanel::parseRust(const QStringList& lines) {
    // Rust: fn, struct, impl, pub, mod, trait, enum, macro_rules!
    QRegularExpression funcRe(
        R"(^\s*(pub\s+)?(fn|struct|impl|enum|trait|mod|macro_rules!|use|const|static)\s+(\w+))",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch m = funcRe.match(line);
        if (m.hasMatch()) {
            bool isPub = !m.captured(1).isEmpty();
            QString kw = m.captured(2);
            QString name = m.captured(3);
            QString access = isPub ? "public" : "private";

            QString type;
            if (kw == "fn") type = "function";
            else if (kw == "struct") type = "struct";
            else if (kw == "impl") type = "impl";
            else if (kw == "enum") type = "enum";
            else if (kw == "trait") type = "trait";
            else if (kw == "mod") type = "module";
            else if (kw == "macro_rules!") type = "macro";
            else if (kw == "use") continue;  // skip imports
            else type = kw;

            addFunctionItem(name, i + 1, type, access, "rust");
        }
    }
}

void FunctionListPanel::parseGo(const QStringList& lines) {
    // Go: func, type, struct, interface, package
    QRegularExpression funcRe(
        R"(^\s*(func|type|struct|interface)\s*(\{?\s*)?(\w+)?)",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch m = funcRe.match(line);
        if (m.hasMatch()) {
            QString kw = m.captured(1);
            // For func declarations: func Name(...) or func (Receiver) Name(...)
            // For types: type Name ...
            // For structs/interfaces: struct { ... } or interface { ... }
            QString name = m.captured(3);
            if (name.isEmpty()) {
                // Anonymous struct/interface — try to get name from previous line
                if (i > 0 && lines[i-1].contains("type ")) {
                    QRegularExpression nameRe(R"(type\s+(\w+))");
                    QRegularExpressionMatch nm = nameRe.match(lines[i-1]);
                    if (nm.hasMatch()) name = nm.captured(1);
                }
                if (name.isEmpty()) continue;
            }

            QString type;
            if (kw == "func") type = "function";
            else if (kw == "struct") type = "struct";
            else if (kw == "interface") type = "interface";
            else if (kw == "type") type = "type alias";

            addFunctionItem(name, i + 1, type, "public", "go");
        }
    }
}

void FunctionListPanel::parseRuby(const QStringList& lines) {
    // Ruby: def, class, module, attr_accessor/reader/writer
    QRegularExpression funcRe(
        R"(^\s*(def|class|module|attr_(?:accessor|reader|writer|class_reader|class_writer))\s+(\w+))",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch m = funcRe.match(line);
        if (m.hasMatch()) {
            QString kw = m.captured(1);
            QString name = m.captured(2);

            QString type;
            if (kw == "def") type = "method";
            else if (kw == "class") type = "class";
            else if (kw == "module") type = "module";
            else if (kw.startsWith("attr_")) type = "attribute";

            addFunctionItem(name, i + 1, type, "public", "ruby");
        }
    }
}

void FunctionListPanel::parseGeneric(const QStringList& lines) {
    // Generic C-style function parser for unknown languages
    QRegularExpression genericRe(
        R"(^\s*([\w:]+(?:\s*[*&]+)?)\s+([\w_][\w\d_]*)\s*\([^)]*\)\s*\{?\s*$)",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch m = genericRe.match(line);
        if (m.hasMatch()) {
            QString retType = m.captured(1).trimmed();
            QString name = m.captured(2).trimmed();

            // Skip noise
            if (retType.isEmpty() || retType == "return" || name.isEmpty() ||
                retType.contains("#") || name.contains("(") || retType.contains("=") ||
                retType == "if" || retType == "else" || retType == "for" ||
                retType == "while" || retType == "switch" || retType == "case" ||
                retType == "return" || retType == "goto" || retType == "typedef" ||
                retType == "include" || retType == "define" || retType == "pragma") {
                continue;
            }

            addFunctionItem(name, i + 1, "function", "public", "generic");
        }
    }
}

void FunctionListPanel::addFunctionItem(const QString& name, int line, const QString& type,
                                        const QString& access, const QString& language) {
    FunctionItem item;
    item.name = name;
    item.line = line;
    item.type = type;
    item.access = access;
    item.language = language;
    _allFunctions.append(item);
}

// ============================================================================
// Filtering
// ============================================================================

void FunctionListPanel::filterFunctions(const QString& text) {
    Q_UNUSED(text);
    applyFilters();
}

void FunctionListPanel::onSearchTextChanged(const QString& text) {
    filterFunctions(text);
}

void FunctionListPanel::onTypeFilterChanged(int) {
    applyFilters();
}

void FunctionListPanel::onSortChanged(int) {
    applyFilters();
}

void FunctionListPanel::applyFilters() {
    _tree->clear();

    QString searchText = _searchBox ? _searchBox->text().toLower() : QString();
    int typeFilter = _typeFilter ? _typeFilter->currentData().toInt() : -1;

    // Get access filter from the panel
    QComboBox* accessFilter = findChild<QComboBox*>("accessFilter");
    int accessIdx = accessFilter ? accessFilter->currentData().toInt() : -1;

    // Sort mode
    int sortMode = _sortCombo ? _sortCombo->currentData().toInt() : 0;

    // Filter the list
    QList<FunctionItem> filtered;
    for (const FunctionItem& item : std::as_const(_allFunctions)) {
        // Type filter
        if (typeFilter == 1 && item.type != "class") continue;
        if (typeFilter == 0 && item.type != "function" && item.type != "method" &&
            item.type != "arrow function" && item.type != "async function") continue;
        if (typeFilter == 2 && item.type != "method" && item.type != "arrow function" &&
            item.type != "async function") continue;
        if (typeFilter == 3 && item.type != "struct" && item.type != "interface" &&
            item.type != "trait" && item.type != "impl" && item.type != "enum") continue;

        // Access filter (only for C-family languages)
        if (accessIdx == 0 && item.access != "public") continue;
        if (accessIdx == 1 && item.access != "private") continue;
        if (accessIdx == 2 && item.access != "protected") continue;

        // Search filter
        if (!searchText.isEmpty() && !item.name.toLower().contains(searchText)) continue;

        filtered.append(item);
    }

    // Sort
    switch (sortMode) {
        case 0:  // Name A-Z
            std::sort(filtered.begin(), filtered.end(),
                      [](const FunctionItem& a, const FunctionItem& b) {
                          return a.name.toLower() < b.name.toLower();
                      });
            break;
        case 1:  // Name Z-A
            std::sort(filtered.begin(), filtered.end(),
                      [](const FunctionItem& a, const FunctionItem& b) {
                          return a.name.toLower() > b.name.toLower();
                      });
            break;
        case 2:  // Line asc
            std::sort(filtered.begin(), filtered.end(),
                      [](const FunctionItem& a, const FunctionItem& b) {
                          return a.line < b.line;
                      });
            break;
        case 3:  // Line desc
            std::sort(filtered.begin(), filtered.end(),
                      [](const FunctionItem& a, const FunctionItem& b) {
                          return a.line > b.line;
                      });
            break;
    }

    // Group items by type
    QList<FunctionItem> classes;
    QList<FunctionItem> structs;
    QList<FunctionItem> funcs;

    for (const FunctionItem& item : std::as_const(filtered)) {
        if (item.type == "class") {
            classes.append(item);
        } else if (item.type == "struct" || item.type == "interface" ||
                   item.type == "trait" || item.type == "impl" ||
                   item.type == "enum" || item.type == "type alias" ||
                   item.type == "module") {
            structs.append(item);
        } else {
            funcs.append(item);
        }
    }

    auto addGroup = [this](const QString& title, const QList<FunctionItem>& items) {
        if (items.isEmpty()) return;
        QTreeWidgetItem* header = new QTreeWidgetItem(_tree);
        header->setText(0, title);
        header->setFirstColumnSpanned(true);
        header->setFlags(header->flags() & ~Qt::ItemIsSelectable);

        for (const FunctionItem& item : items) {
            QTreeWidgetItem* treeItem = new QTreeWidgetItem(header);
            treeItem->setText(0, item.name);
            treeItem->setText(1, QString::number(item.line));
            treeItem->setText(2, item.type);
            treeItem->setText(3, item.access);
            treeItem->setData(0, Qt::UserRole, item.line);
        }
    };

    addGroup(tr("Classes"), classes);
    addGroup(tr("Structs & Types"), structs);
    addGroup(tr("Functions & Methods"), funcs);

    _tree->expandAll();
}

// ============================================================================
// Navigation
// ============================================================================

void FunctionListPanel::onItemDoubleClicked(QTreeWidgetItem* item, int) {
    if (!item || !_editor) return;

    int line = item->data(0, Qt::UserRole).toInt();
    if (line <= 0) return;

    // Navigate to line (convert from 1-indexed stored to 0-indexed)
    _editor->gotoLine(line - 1, 0);
    _editor->setFocus();

    emit functionSelected(line);
}

// ============================================================================
// XML-based parsing  (upstream Notepad++ functionList XML parsers)
// ============================================================================

void FunctionListPanel::parseWithXmlParser(const QStringList& lines, LangType lang) {
    ensureXmlParserLoaded(lang);
    const QString key = langKeyFromType(lang);

    const auto* def = _xmlParser ? _xmlParser->parser(key) : nullptr;

    if (def && def->isValid) {
        // Use upstream XML rules
        // Parse class ranges first (nested members belong to classes)
        for (const auto& classRule : def->classRules) {
            parseClassRange(lines, classRule, _allFunctions);
        }
        // Then top-level functions
        parseTopLevelFunctions(lines, def->functionRules, _allFunctions);
    } else {
        // No XML parser available for this language — fall back to built-in regex
        // based on the language family.
        if (lang == LangType::L_CPP || lang == LangType::L_C ||
            lang == LangType::L_JAVA || lang == LangType::L_OBJC ||
            lang == LangType::L_OBJC || lang == LangType::L_PHP ||
            lang == LangType::L_LUA || lang == LangType::L_CSS) {
            parseCpp(lines);
        } else if (lang == LangType::L_JS || lang == LangType::L_JSON ||
                   lang == LangType::L_CS) {
            parseJavaScript(lines);
        } else if (lang == LangType::L_PYTHON || lang == LangType::L_PERL) {
            parsePython(lines);
        } else if (lang == LangType::L_RUST) {
            parseRust(lines);
        } else if (lang == LangType::L_GO) {
            parseGo(lines);
        } else if (lang == LangType::L_RUBY) {
            parseRuby(lines);
        } else if (lang == LangType::L_SWIFT) {
            parseSwift(lines);
        } else if (lang == LangType::L_KOTLIN) {
            parseKotlin(lines);
        } else if (lang == LangType::L_TS) {
            parseTypeScript(lines);
        } else if (lang == LangType::L_CS) {
            parseCsharp(lines);
        } else if (lang == LangType::L_SQL) {
            parseSQL(lines);
        } else if (lang == LangType::L_HTML || lang == LangType::L_XML) {
            parseHTML(lines);
        } else if (lang == LangType::L_PERL) {
            parsePerl(lines);
        } else if (lang == LangType::L_MAKEFILE) {
            parseMakefile(lines);
        } else if (lang == LangType::L_LUA) {
            parseLua(lines);
        } else if (lang == LangType::L_PHP) {
            parsePHP(lines);
        } else {
            parseGeneric(lines);
        }
    }
}

void FunctionListPanel::parseClassRange(const QStringList& lines,
                                        const FunctionListXmlParser::FunctionListRule& classRule,
                                        QList<FunctionItem>& out) {
    if (!classRule.mainExpr.isValid())
        return;

    const QString text = lines.join('\n');

    // Find all class-range matches
    QRegularExpressionMatchIterator it =
        classRule.mainExpr.globalMatch(text);

    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        QString className;
        if (classRule.classNameExpr.isValid()) {
            QRegularExpressionMatch nm = classRule.classNameExpr.match(m.captured(0));
            if (nm.hasMatch())
                className = nm.captured(0);
        }
        if (className.isEmpty())
            className = "class";

        int matchStart = m.capturedStart();

        // Find the body range: look for openSymbole → closeSymbole
        int bodyStart = text.indexOf(classRule.openSymbole, matchStart);
        if (bodyStart < 0) continue;
        int bodyEnd = text.indexOf(classRule.closeSymbole, bodyStart + 1);
        if (bodyEnd < 0) continue;

        // Count lines to get absolute line numbers
        int classLine = text.left(matchStart).count('\n') + 1;
        int bodyEndLine = text.left(bodyEnd).count('\n') + 1;

        // Extract the class body and find methods within
        QString bodyText = text.mid(bodyStart + 1, bodyEnd - bodyStart - 1);
        QStringList bodyLines = bodyText.split('\n');

        // Look for methods using the function rules inside the class body
        // (classRule may have nested function sub-rules we check by re-parsing
        //  the body text with the existing function rules)
        QList<FunctionListRule> emptyRules; // use hard-coded parsers instead
        Q_UNUSED(emptyRules);

        for (int li = 0; li < bodyLines.size(); ++li) {
            const QString& line = bodyLines[li];
            // Convert body-relative line → absolute
            int absLine = text.left(bodyStart + 1).count('\n') + 1 + li;

            // Dispatch to hard-coded parsers for method detection
            // (method parsing uses line content only)
            QRegularExpression funcRe(
                R"(^\s*(?:(public|private|protected)\s+)?"
                R"((?:virtual|static|inline|explicit|const|constexpr|friend|override|final)\s+)*"
                R"((?:[\w:]+(?:\s*[*&]+)?\s+)?"
                R"([\w_][\w\d_]*)\s*\([^)]*\)\s*(?:const)?\s*[;{]?\s*$)",
                QRegularExpression::MultilineOption);

            QRegularExpressionMatch fm = funcRe.match(line);
            if (fm.hasMatch()) {
                FunctionItem item;
                item.name = fm.captured(3);
                item.line = absLine;
                item.type = "method";
                item.access = fm.captured(1).isEmpty() ? "private" : fm.captured(1);
                item.parent = className;
                item.language = "cpp";
                out.append(item);
            }
        }

        // Also record the class/struct itself
        FunctionItem classItem;
        classItem.name = className;
        classItem.line = classLine;
        classItem.type = "class";
        classItem.access = "public";
        classItem.language = "cpp";
        out.append(classItem);
    }
}

void FunctionListPanel::parseTopLevelFunctions(
        const QStringList& lines,
        const QList<FunctionListXmlParser::FunctionListRule>& rules,
        QList<FunctionItem>& out) {
    if (rules.isEmpty())
        return;

    const QString text = lines.join('\n');

    for (const auto& rule : rules) {
        if (!rule.mainExpr.isValid())
            continue;

        QRegularExpressionMatchIterator it = rule.mainExpr.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();

            QString name;
            if (rule.funcNameExpr.isValid()) {
                QRegularExpressionMatch nm = rule.funcNameExpr.match(m.captured(0));
                if (nm.hasMatch()) name = nm.captured(0);
            }
            if (name.isEmpty()) {
                // Try to grab the last captured group
                name = m.lastCapturedIndex() > 0 ? m.captured(m.lastCapturedIndex()) : QString();
            }

            if (name.isEmpty() || name.contains("(") || name.contains("=") ||
                name == "if" || name == "for" || name == "while" || name == "switch" ||
                name == "case" || name == "else" || name == "return" || name == "include" ||
                name == "define" || name == "typedef" || name == "goto") {
                continue;
            }

            // Extract line number from match offset
            int lineNum = text.left(m.capturedStart()).count('\n') + 1;

            FunctionItem item;
            item.name = name;
            item.line = lineNum;
            item.type = "function";
            item.access = "public";

            if (rule.funcSigExpr.isValid()) {
                QRegularExpressionMatch sm = rule.funcSigExpr.match(m.captured(0));
                if (sm.hasMatch())
                    item.signature = sm.captured(0);
            }
            if (item.signature.isEmpty())
                item.signature = m.captured(0).simplified();

            out.append(item);
        }
    }
}

// ============================================================================
// New language parsers (Swift, Kotlin, TypeScript, C#)
// ============================================================================

void FunctionListPanel::parseSwift(const QStringList& lines) {
    // Swift: func, class, struct, enum, protocol, extension, var, let, init, deinit, subscript
    QRegularExpression funcRe(
        R"(^\s*(?:(fileprivate|internal|public|open|private|static|final|override)\s+)?"
        R"((?:func|class|struct|enum|protocol|extension|var|let|init|deinit|subscript)\s+)"
        R"(\w+))",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch m = funcRe.match(line);
        if (m.hasMatch()) {
            QString kw = m.captured(2);
            QString name = m.captured(3);
            QString access = m.captured(1);
            if (access.isEmpty()) access = "internal";

            QString type;
            if (kw == "func" || kw == "init" || kw == "deinit" || kw == "subscript")
                type = "function";
            else if (kw == "class") type = "class";
            else if (kw == "struct") type = "struct";
            else if (kw == "enum") type = "enum";
            else if (kw == "protocol") type = "interface";
            else if (kw == "extension") type = "module";
            else if (kw == "var" || kw == "let") type = "property";
            else type = kw;

            addFunctionItem(name, i + 1, type, access, "swift");
        }
    }
}

void FunctionListPanel::parseKotlin(const QStringList& lines) {
    // Kotlin: fun, class, object, interface, enum class, sealed class, data class, companion, val, var
    QRegularExpression funcRe(
        R"(^\s*(?:(private|protected|public|internal|inline|crossinline|reified|operator|infix|override)\s+)*"
        R"((?:fun|class|object|interface|enum class|sealed class|data class|companion|val|var)\s+)"
        R"(\w+))",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch m = funcRe.match(line);
        if (m.hasMatch()) {
            QString kw = m.captured(2);
            QString name = m.captured(3);

            QString type;
            if (kw == "fun") type = "function";
            else if (kw.startsWith("class") || kw == "object") type = "class";
            else if (kw == "interface") type = "interface";
            else if (kw == "enum class") type = "enum";
            else if (kw == "sealed class") type = "class";
            else if (kw == "data class") type = "class";
            else if (kw == "companion") type = "module";
            else if (kw == "val" || kw == "var") type = "property";
            else type = kw;

            addFunctionItem(name, i + 1, type, "public", "kotlin");
        }
    }
}

void FunctionListPanel::parseTypeScript(const QStringList& lines) {
    // TypeScript: function, class, interface, enum, namespace, module, type, const, let, async, export
    QRegularExpression funcRe(
        R"(^\s*(?:(export|declare|declare\s+)|(?:(async)\s+)?(?:(function|class|interface|enum|namespace|module|type)\s+)"
        R"(\w+))",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch m = funcRe.match(line);
        if (m.hasMatch()) {
            QString kw = m.captured(2).isEmpty() ? m.captured(3) : m.captured(3);
            QString name = m.captured(4);

            QString type;
            if (kw == "function") type = "function";
            else if (kw == "class") type = "class";
            else if (kw == "interface") type = "interface";
            else if (kw == "enum") type = "enum";
            else if (kw == "namespace" || kw == "module") type = "module";
            else if (kw == "type") type = "type alias";
            else type = kw;

            addFunctionItem(name, i + 1, type, "public", "typescript");
        }
    }
}

void FunctionListPanel::parseCsharp(const QStringList& lines) {
    // C#: class, struct, enum, interface, record, delegate, namespace, and methods
    QRegularExpression classRe(
        R"(^\s*(?:(?:public|private|protected|internal|static|readonly|partial|abstract|sealed)\s+)*"
        R"((?:class|struct|enum|interface|record|delegate|namespace)\s+)"
        R"(\w+))",
        QRegularExpression::MultilineOption);

    QRegularExpression methodRe(
        R"(^\s*(?:(?:(public|private|protected|internal|static|readonly|virtual|override|abstract|sealed|partial|async)\s+)+)"
        R"((?:void|int|string|bool|double|float|var|Task|[\w<>[\]?]+)\s+)"
        R"(\w+)\s*\([^)]*\)\s*(?:const)?\s*[;{]?\s*$)",
        QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        QRegularExpressionMatch cm = classRe.match(line);
        if (cm.hasMatch()) {
            QString kw = cm.captured(2);
            QString name = cm.captured(3);
            QString type;
            if (kw == "class" || kw == "record") type = "class";
            else if (kw == "struct") type = "struct";
            else if (kw == "enum") type = "enum";
            else if (kw == "interface") type = "interface";
            else if (kw == "delegate") type = "function";
            else if (kw == "namespace") type = "module";
            else type = kw;
            addFunctionItem(name, i + 1, type, "public", "csharp");
            continue;
        }

        QRegularExpressionMatch fm = methodRe.match(line);
        if (fm.hasMatch()) {
            QString name = fm.captured(3);
            if (name.isEmpty() || name == "get" || name == "set" || name == "add" || name == "remove")
                continue;
            QString modifiers = fm.captured(1);
            QString access = "private";
            if (modifiers.contains("public"))
                access = "public";
            else if (modifiers.contains("protected"))
                access = "protected";
            addFunctionItem(name, i + 1, "method", access, "csharp");
        }
    }
}

void FunctionListPanel::parseHTML(const QStringList& lines) {
    Q_UNUSED(lines);
}

void FunctionListPanel::parseSQL(const QStringList& lines) {
    Q_UNUSED(lines);
}

void FunctionListPanel::parseMakefile(const QStringList& lines) {
    Q_UNUSED(lines);
}

// === Stubs ===
void FunctionListPanel::onEditorTextChanged() {
    refreshDebounced();
}

void FunctionListPanel::onLanguageChanged() {
    // Re-init XML parser for the new language and refresh the list
    if (_editor) {
        _lastLanguage.clear();
    }
    initXmlParser();
    refreshDebounced();
}

void FunctionListPanel::onSortModeChanged(int index) {
    // index: 0=name A-Z, 1=name Z-A, 2=line asc, 3=line desc
    _sortByLine = (index == 2 || index == 3);
    onResort();
}

void FunctionListPanel::setSortByLine(bool byLine) {
    _sortByLine = byLine;
    if (_sortCombo) {
        _sortCombo->setCurrentIndex(byLine ? 2 : 0);
    }
    onResort();
}

void FunctionListPanel::onExpandAll() {
    if (_tree) {
        _tree->expandAll();
    }
    updateCounts();
}

void FunctionListPanel::onCollapseAll() {
    if (_tree) {
        _tree->collapseAll();
    }
    updateCounts();
}

void FunctionListPanel::onItemClicked(QTreeWidgetItem* item, int) {
    if (!item) return;
    bool ok = false;
    int line = item->data(0, Qt::UserRole).toInt(&ok);
    if (ok && line >= 0) {
        goToDefinition();
    }
}

void FunctionListPanel::onItemExpanded(QTreeWidgetItem*) {
    updateCounts();
}

void FunctionListPanel::onItemCollapsed(QTreeWidgetItem*) {
    updateCounts();
}

void FunctionListPanel::onContextMenu(const QPoint& pos) {
    if (!_tree) return;
    QMenu menu(_tree);
    menu.addAction(tr("Copy Function Name"), this, &FunctionListPanel::copyFunctionName);
    menu.addAction(tr("Copy Signature"), this, &FunctionListPanel::copyFunctionSignature);
    menu.addAction(tr("Go to Definition"), this, &FunctionListPanel::goToDefinition);
    menu.exec(_tree->viewport()->mapToGlobal(pos));
}

void FunctionListPanel::copyFunctionName() {
    QTreeWidgetItem* item = _tree ? _tree->currentItem() : nullptr;
    if (!item) return;
    QString name = item->text(0);
    if (!name.isEmpty()) {
        QApplication::clipboard()->setText(name);
    }
}

void FunctionListPanel::copyFunctionSignature() {
    QTreeWidgetItem* item = _tree ? _tree->currentItem() : nullptr;
    if (!item) return;
    QString sig = item->data(0, Qt::UserRole + 1).toString();
    if (!sig.isEmpty()) {
        QApplication::clipboard()->setText(sig);
    }
}

void FunctionListPanel::goToDefinition() {
    QTreeWidgetItem* item = _tree ? _tree->currentItem() : nullptr;
    if (!item || !_editor) return;
    bool ok = false;
    int line = item->data(0, Qt::UserRole).toInt(&ok);
    if (!ok || line < 0) return;
    // Scintilla lines are 0-indexed internally
    _editor->gotoLine(line - 1, 0);
    emit functionSelected(line);
}

void FunctionListPanel::onResort() {
    applyFilters();
}

void FunctionListPanel::updateCounts() {
    _fnCount = _clsCount = _structCount = _otherCount = 0;
    for (const FunctionItem& item : std::as_const(_filteredFunctions)) {
        if (item.type == "class" || item.type == "interface" || item.type == "trait")
            ++_clsCount;
        else if (item.type == "struct")
            ++_structCount;
        else if (item.type == "function" || item.type == "method" ||
                 item.type == "arrow function" || item.type == "async function")
            ++_fnCount;
        else
            ++_otherCount;
    }
}

void FunctionListPanel::refreshDebounced() {
    if (_debounceTimer) {
        _debounceTimer->start();
    } else {
        refresh();
    }
}
