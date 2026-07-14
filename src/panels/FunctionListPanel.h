// FunctionListPanel.h - Enhanced function list/outline panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTimer>
#include <QToolBar>
#include <QAction>
#include <QHash>
#include <QSet>
#include <QToolTip>
#include <QMenu>

class ScintillaEditor;

class FunctionListPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit FunctionListPanel(QWidget* parent = nullptr);
    ~FunctionListPanel() override;

    void setEditor(ScintillaEditor* editor);
    void refresh();
    void refreshDebounced();

signals:
    void functionSelected(int line);

public slots:
    void onSortModeChanged(int index);
    void setSortByLine(bool byLine);

private slots:
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onSearchTextChanged(const QString& text);
    void onTypeFilterChanged(int index);
    void onExpandAll();
    void onCollapseAll();
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);
    void onContextMenu(const QPoint& pos);
    void copyFunctionName();
    void copyFunctionSignature();
    void goToDefinition();
    void onResort();
    void onEditorTextChanged();
    void onLanguageChanged();

private:
    void parseFunctions();
    void filterFunctions(const QString& text);
    void addFunctionItem(const QString& name, int line, const QString& type,
                         const QString& access = QString(), const QString& lang = QString(),
                         const QString& signature = QString(), const QString& parent = QString());
    void applyFilters();
    void rebuildTree();
    QString extractSignature(const QString& line, int lineNum, const QString& lang) const;
    void updateCounts();
    void setItemIcon(QTreeWidgetItem* item, const QString& type, const QString& lang);
    void expandCategory(const QString& category, bool expand);
    void ensureFollowsCursor();

    // Language-specific parsing helpers
    void parseCpp(const QStringList& lines);
    void parsePython(const QStringList& lines);
    void parseJavaScript(const QStringList& lines);
    void parseRust(const QStringList& lines);
    void parseGo(const QStringList& lines);
    void parseRuby(const QStringList& lines);
    void parsePHP(const QStringList& lines);
    void parseLua(const QStringList& lines);
    void parsePerl(const QStringList& lines);
    void parseSQL(const QStringList& lines);
    void parseCSS(const QStringList& lines);
    void parseHTML(const QStringList& lines);
    void parseShell(const QStringList& lines);
    void parseMakefile(const QStringList& lines);
    void parseJava(const QStringList& lines);
    void parseGeneric(const QStringList& lines);

    // Item data
    struct FunctionItem {
        QString name;
        int line;
        QString type;       // "function", "class", "method", "struct", "enum", "macro", "interface", "trait", "module", "signal", "slot", "property"
        QString access;     // "public", "private", "protected", "package", "internal"
        QString language;   // "cpp", "python", "js", "rust", "go", "ruby", "php", "lua", "perl", "sql", "css", "html", "shell", "make", "java", "generic"
        QString signature;   // full signature line
        QString parent;     // parent container name (class/namespace/module)
    };

    QList<FunctionItem> _allFunctions;
    QList<FunctionItem> _filteredFunctions;

    // Tree categories
    QTreeWidgetItem* _categoryFunctions = nullptr;
    QTreeWidgetItem* _categoryClasses = nullptr;
    QTreeWidgetItem* _categoryStructs = nullptr;
    QTreeWidgetItem* _categoryOther = nullptr;
    int _fnCount = 0, _clsCount = 0, _structCount = 0, _otherCount = 0;

    // State
    ScintillaEditor* _editor = nullptr;
    QTreeWidget* _tree = nullptr;
    QLineEdit* _searchBox = nullptr;
    QComboBox* _typeFilter = nullptr;
    QComboBox* _sortCombo = nullptr;
    QTimer* _debounceTimer = nullptr;
    int _lastSelectedLine = -1;
    QString _lastLanguage;
    bool _sortByLine = false;
    bool _lastFollowsCursor = false;

    // Icons (loaded lazily)
    QIcon _iconFunction, _iconClass, _iconStruct, _iconEnum, _iconMacro,
          _iconInterface, _iconTrait, _iconModule, _iconSignal, _iconProperty,
          _iconMethod, _iconNamespace;

    void loadIcons();
};
