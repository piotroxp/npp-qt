// FunctionListPanel.h - Function list/outline panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QComboBox>

class ScintillaEditor;

class FunctionListPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit FunctionListPanel(QWidget* parent = nullptr);
    ~FunctionListPanel() override;

    void setEditor(ScintillaEditor* editor);
    void refresh();

private slots:
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onSearchTextChanged(const QString& text);
    void onTypeFilterChanged(int index);

private:
    void parseFunctions();
    void filterFunctions(const QString& text);
    void addFunctionItem(const QString& name, int line, const QString& type);
    void applyFilters();

    ScintillaEditor* _editor = nullptr;
    QTreeWidget* _tree = nullptr;
    QLineEdit* _searchBox = nullptr;
    QComboBox* _typeFilter = nullptr;

    // Internal storage for all parsed functions
    struct FunctionItem {
        QString name;
        int line;
        QString type;
    };
    QList<FunctionItem> _allFunctions;

    // For selection restoration
    int _lastSelectedLine = -1;
};
