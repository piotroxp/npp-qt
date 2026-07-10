// CommandPaletteDialog.h - Quick command palette (Ctrl+Shift+P)
// Copyright (C) 2026 Agent Army | GPL v3

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QString>

class CommandPaletteDialog : public QDialog {
    Q_OBJECT
public:
    explicit CommandPaletteDialog(QWidget* parent = nullptr);
    ~CommandPaletteDialog() override;

    // Execute this command when accepted
    QString selectedCommand() const { return _selectedCommand; }

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onTextChanged(const QString& text);
    void onItemActivated(QListWidgetItem* item);

private:
    void populateAll();
    void filterCommands(const QString& text);
    QString executeCommand(const QString& cmd);

    QLineEdit* _searchBox = nullptr;
    QListWidget* _commandList = nullptr;
    QString _selectedCommand;
    QVector<QPair<QString, QString>> _allCommands;  // name, command-id
};
