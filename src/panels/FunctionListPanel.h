// FunctionListPanel.h - Function list/outline panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QList>

class ScintillaEditor;

class FunctionListPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit FunctionListPanel(QWidget* parent = nullptr);
    ~FunctionListPanel() override;

    void setEditor(ScintillaEditor* editor);
    void refresh();

signals:
    void functionSelected(int line);

private slots:
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onSearchTextChanged(const QString& text);
    void onTypeFilterChanged(int index);
    void onSortChanged(int index);

private:
    void parseFunctions();
    void filterFunctions(const QString& text);
    void addFunctionItem(const QString& name, int line, const QString& type,
                         const QString& access = QString(), const QString& lang = QString());
    void applyFilters();

    // Language-specific parsing helpers
    void parseCpp(const QStringList& lines);
    void parsePython(const QStringList& lines);
    void parseJavaScript(const QStringList& lines);
    void parseRust(const QStringList& lines);
    void parseGo(const QStringList& lines);
    void parseRuby(const QStringList& lines);
    void parseGeneric(const QStringList& lines);

    ScintillaEditor* _editor = nullptr;
    QTreeWidget* _tree = nullptr;
    QLineEdit* _searchBox = nullptr;
    QComboBox* _typeFilter = nullptr;
    QComboBox* _sortCombo = nullptr;

    // Internal storage for all parsed functions
    struct FunctionItem {
        QString name;
        int line;
        QString type;     // "function", "class", "method", "struct", "interface", "trait", "module"
        QString access;    // "public", "private", "protected", "package"
        QString language;  // "cpp", "python", "js", "rust", "go", "ruby"
    };
    QList<FunctionItem> _allFunctions;

    // For selection restoration
    int _lastSelectedLine = -1;
};
