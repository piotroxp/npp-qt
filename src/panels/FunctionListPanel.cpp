// FunctionListPanel.cpp - Enhanced function list/outline panel implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FunctionListPanel.h"
#include "../editor/ScintillaEditor.h"
#include "../common/Types.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QLabel>
#include <QDebug>
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
}

FunctionListPanel::~FunctionListPanel() = default;

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
    switch (lang) {
        case LangType::L_PYTHON:
        case LangType::L_PERL:
            parsePython(lines);
            break;
        case LangType::L_JS:
        case LangType::L_JSON:
        case LangType::L_CS:
            parseJavaScript(lines);
            break;
        case LangType::L_RUBY:
            parseRuby(lines);
            break;
        default:
            // For C/C++/Java/HTML/XML/etc, try C++ parser first
            if (lang == LangType::L_CPP || lang == LangType::L_C ||
                lang == LangType::L_JAVA || lang == LangType::L_OBJC ||
                lang == LangType::L_HTML || lang == LangType::L_XML ||
                lang == LangType::L_PHP || lang == LangType::L_LUA ||
                lang == LangType::L_MARKDOWN || lang == LangType::L_CSS ||
                lang == LangType::L_YAML || lang == LangType::L_MAKEFILE ||
                lang == LangType::L_INI || lang == LangType::L_BATCH) {
                parseCpp(lines);
            } else {
                parseGeneric(lines);
            }
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
