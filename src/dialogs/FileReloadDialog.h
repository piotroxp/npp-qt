// FileReloadDialog.h - Prompt when file is modified externally
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QVBoxLayout>

// ============================================================================
// FileReloadDialog — Prompt when monitored file changes externally
// ============================================================================
class FileReloadDialog : public QDialog {
    Q_OBJECT

public:
    enum class Action {
        Reload,       // Reload from disk
        KeepDisk,     // Keep disk version, ignore change
        DoNothing     // Do nothing (silently ignore)
    };

    explicit FileReloadDialog(const QString& filePath,
                               QWidget* parent = nullptr);
    ~FileReloadDialog() override;

    Action selectedAction() const { return _action; }
    bool dontAskForAll() const { return _chkDontAskAll->isChecked(); }

    // Static convenience
    static Action prompt(const QString& filePath, QWidget* parent);

private:
    Action _action = Action::DoNothing;
    QRadioButton* _radioReload = nullptr;
    QRadioButton* _radioKeep = nullptr;
    QRadioButton* _radioNothing = nullptr;
    QCheckBox* _chkDontAskAll = nullptr;
};
