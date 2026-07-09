// FunctionListPanel.cpp - Function list/outline panel implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FunctionListPanel.h"
#include "../editor/ScintillaEditor.h"
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <algorithm>

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

    // Type filter combo box
    _typeFilter = new QComboBox(content);
    _typeFilter->addItem(tr("All"), -1);
    _typeFilter->addItem(tr("Functions"), 0);
    _typeFilter->addItem(tr("Classes"), 1);
    _typeFilter->addItem(tr("Methods"), 2);
    layout->addWidget(_typeFilter);

    // Tree widget for function list
    _tree = new QTreeWidget(content);
    _tree->setHeaderLabels(QStringList() << tr("Name") << tr("Line") << tr("Type"));
    _tree->header()->setStretchLastSection(false);
    _tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    _tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    _tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    _tree->setAlternatingRowColors(true);
    _tree->setSortingEnabled(true);
    layout->addWidget(_tree);

    setWidget(content);

    connect(_tree, &QTreeWidget::itemDoubleClicked, this, &FunctionListPanel::onItemDoubleClicked);
    connect(_searchBox, &QLineEdit::textChanged, this, &FunctionListPanel::onSearchTextChanged);
    connect(_typeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &FunctionListPanel::onTypeFilterChanged);
}

FunctionListPanel::~FunctionListPanel() = default;

void FunctionListPanel::setEditor(ScintillaEditor* editor) {
    // Disconnect old editor signals if any
    if (_editor) {
        disconnect(_editor, &ScintillaEditor::textChanged, this, &FunctionListPanel::refresh);
        disconnect(_editor, &ScintillaEditor::cursorPositionChanged, this, nullptr);
    }

    _editor = editor;

    if (_editor) {
        // Connect to text changes for auto-refresh
        connect(_editor, &ScintillaEditor::textChanged, this, &FunctionListPanel::refresh);
    }

    refresh();
}

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

    // Clear and reparse
    _tree->clear();
    _allFunctions.clear();

    if (!_editor) return;

    parseFunctions();
    applyFilters();

    // Restore selection if possible
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

void FunctionListPanel::parseFunctions() {
    if (!_editor) return;

    const QString text = _editor->toPlainText();
    if (text.isEmpty()) return;

    const QStringList lines = text.split('\n');

    // Regex patterns for different languages
    // C/C++ function pattern: handles virtual, static, inline, explicit, const, constexpr modifiers
    QRegularExpression cppFunc(R"(^\s*((?:virtual|static|inline|explicit|const|constexpr|friend|unsigned|signed|short|long)\s*)*"
                               R"*(?:[\w:]+(?:\s*[*&]+)?\s+)([\w:]+)\s*\([^)]*\)\s*(?:const)?\s*\{?\s*$)");
    cppFunc.setPatternOptions(QRegularExpression::MultilineOption);

    // Python function pattern: def, class, async def
    QRegularExpression pyFunc(R"(^\s*(?:(async)\s+)?(def|class)\s+(\w+))");
    pyFunc.setPatternOptions(QRegularExpression::MultilineOption);

    // JavaScript function pattern: function, class, const/let/var assignments
    QRegularExpression jsFunc(R"(^\s*(?:(function|class)|(?:(const|let|var)\s+\w+\s*=\s*(?:async\s+)?|async\s+(?:function\s+)?))(\w+))");
    jsFunc.setPatternOptions(QRegularExpression::MultilineOption);

    // General function pattern - catches basic function declarations
    QRegularExpression generalFunc(R"(^\s*([\w:]+(?:\s*[*&]+)?)\s+([\w_][\w\d_]*)\s*\([^)]*\)\s*\{?\s*$)");
    generalFunc.setPatternOptions(QRegularExpression::MultilineOption);

    // Lambda/arrow function patterns
    QRegularExpression arrowFunc(R"(^\s*(\w+)\s*=\s*(?:async\s+)?\([^)]*\)\s*=>)");
    arrowFunc.setPatternOptions(QRegularExpression::MultilineOption);

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];

        // Try C/C++ pattern
        QRegularExpressionMatch match = cppFunc.match(line);
        if (match.hasMatch()) {
            QString name = match.captured(2);
            // Check if this is a class declaration
            if (line.contains("class ") && !line.contains(":")) {
                addFunctionItem(name, i + 1, "class");
            } else if (match.captured(1).contains("virtual")) {
                addFunctionItem(name, i + 1, "method");
            } else {
                addFunctionItem(name, i + 1, "function");
            }
            continue;
        }

        // Try Python pattern
        match = pyFunc.match(line);
        if (match.hasMatch()) {
            QString type = match.captured(2);  // def or class
            QString name = match.captured(3);
            if (type == "class") {
                addFunctionItem(name, i + 1, "class");
            } else {
                addFunctionItem(name, i + 1, "function");
            }
            continue;
        }

        // Try JavaScript pattern
        match = jsFunc.match(line);
        if (match.hasMatch()) {
            QString name = match.captured(3);
            if (match.captured(1) == "class") {
                addFunctionItem(name, i + 1, "class");
            } else if (match.captured(2).isEmpty()) {
                // Regular function
                addFunctionItem(name, i + 1, "function");
            } else {
                // const/let/var assignment (could be method or arrow function)
                if (line.contains("=>")) {
                    addFunctionItem(name, i + 1, "method");
                } else {
                    addFunctionItem(name, i + 1, "function");
                }
            }
            continue;
        }

        // Try arrow function pattern
        match = arrowFunc.match(line);
        if (match.hasMatch()) {
            addFunctionItem(match.captured(1), i + 1, "function");
            continue;
        }

        // Try general function pattern (fallback)
        match = generalFunc.match(line);
        if (match.hasMatch()) {
            QString returnType = match.captured(1).trimmed();
            QString name = match.captured(2).trimmed();

            // Skip if it looks like a variable declaration or preprocessor
            if (returnType.isEmpty() || returnType == "return" ||
                name.isEmpty() || returnType.contains("#") ||
                name.contains("(") || returnType.contains("=")) {
                continue;
            }

            // Skip common keywords that aren't function names
            if (returnType == "if" || returnType == "else" || returnType == "for" ||
                returnType == "while" || returnType == "switch" || returnType == "case" ||
                returnType == "return" || returnType == "goto" || returnType == "typedef" ||
                returnType == "include" || returnType == "define") {
                continue;
            }

            addFunctionItem(name, i + 1, "function");
        }
    }

    // Sort alphabetically by name
    std::sort(_allFunctions.begin(), _allFunctions.end(),
              [](const FunctionItem& a, const FunctionItem& b) {
                  return a.name.toLower() < b.name.toLower();
              });
}

void FunctionListPanel::addFunctionItem(const QString& name, int line, const QString& type) {
    FunctionItem item;
    item.name = name;
    item.line = line;
    item.type = type;
    _allFunctions.append(item);
}

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

void FunctionListPanel::applyFilters() {
    _tree->clear();

    QString searchText = _searchBox ? _searchBox->text().toLower() : QString();
    int typeFilter = _typeFilter ? _typeFilter->currentData().toInt() : -1;

    // Separate classes and functions for grouping
    QList<FunctionItem> classes;
    QList<FunctionItem> functions;

    for (const FunctionItem& item : _allFunctions) {
        // Apply type filter
        if (typeFilter == 1 && item.type != "class") continue;
        if (typeFilter == 0 && item.type != "function") continue;
        if (typeFilter == 2 && item.type != "method") continue;

        // Apply search filter
        if (!searchText.isEmpty() && !item.name.toLower().contains(searchText)) continue;

        if (item.type == "class") {
            classes.append(item);
        } else {
            functions.append(item);
        }
    }

    // Add classes group
    if (!classes.isEmpty()) {
        QTreeWidgetItem* classHeader = new QTreeWidgetItem(_tree);
        classHeader->setText(0, tr("Classes"));
        classHeader->setFirstColumnSpanned(true);
        classHeader->setFlags(classHeader->flags() & ~Qt::ItemIsSelectable);

        for (const FunctionItem& item : classes) {
            QTreeWidgetItem* treeItem = new QTreeWidgetItem(classHeader);
            treeItem->setText(0, item.name);
            treeItem->setText(1, QString::number(item.line));
            treeItem->setText(2, item.type);
            treeItem->setData(0, Qt::UserRole, item.line);
        }
    }

    // Add functions/methods group
    if (!functions.isEmpty()) {
        QTreeWidgetItem* funcHeader = new QTreeWidgetItem(_tree);
        funcHeader->setText(0, tr("Functions & Methods"));
        funcHeader->setFirstColumnSpanned(true);
        funcHeader->setFlags(funcHeader->flags() & ~Qt::ItemIsSelectable);

        for (const FunctionItem& item : functions) {
            QTreeWidgetItem* treeItem = new QTreeWidgetItem(funcHeader);
            treeItem->setText(0, item.name);
            treeItem->setText(1, QString::number(item.line));
            treeItem->setText(2, item.type);
            treeItem->setData(0, Qt::UserRole, item.line);
        }
    }

    // Expand all groups by default
    _tree->expandAll();
}

void FunctionListPanel::onItemDoubleClicked(QTreeWidgetItem* item, int) {
    if (!item || !_editor) return;

    // Skip group headers (they have line 0 or invalid data)
    int line = item->data(0, Qt::UserRole).toInt();
    if (line <= 0) return;

    // Navigate to the line (0-indexed)
    _editor->gotoLine(line - 1, 0);

    // Set focus back to editor
    _editor->setFocus();
}
