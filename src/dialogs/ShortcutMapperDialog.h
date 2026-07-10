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
    Q_OBJECT
public:
    explicit ShortcutMapperDialog(QWidget* parent = nullptr);
    ~ShortcutMapperDialog() override;

private slots:
    void onCategoryChanged(const QString& category);
    void onItemDoubleClicked(int row, int col);
    void onClearShortcut();
    void onResetAll();
    void onApplyAndClose();

private:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void populateShortcuts(const QString& category);
    QString grabKeyComboSync(QWidget* parent);

    QListWidget* _categoryList = nullptr;
    QTableWidget* _shortcutTable = nullptr;
    QDialogButtonBox* _buttonBox = nullptr;
};
