// ShortcutMapperDialog.h - Keyboard shortcut configuration dialog
// Copyright (C) 2026 Agent Army | GPL v3

#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QDialogButtonBox>

class ShortcutMapperDialog : public QDialog {
public:
    explicit ShortcutMapperDialog(QWidget* parent = nullptr);
    ~ShortcutMapperDialog() override;

private slots:
    void onCategoryChanged(int index);
    void onItemDoubleClicked(int row, int col);
    void onClearShortcut();
    void onResetAll();

private:
    void populateShortcuts(const QString& category);
    QString grabKeyCombo(QWidget* parent);

    QListWidget* _categoryList = nullptr;
    QTableWidget* _shortcutTable = nullptr;
    QDialogButtonBox* _buttonBox = nullptr;
};
