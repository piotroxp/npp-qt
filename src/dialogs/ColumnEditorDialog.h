// ColumnEditorDialog.h — Column/rectangular selection editor for Notepad--Qt
// Supports inserting sequential numbers or repeated characters into a
// rectangular (column-mode) selection in Scintilla.
// Copyright (C) 2026 Agent Army / GPL v3

#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QGroupBox>

class ScintillaEditor;

class ColumnEditorDialog : public QDialog {
    Q_OBJECT

public:
    enum class ContentType {
        Numeric,
        Character
    };

    explicit ColumnEditorDialog(QWidget* parent = nullptr);
    ~ColumnEditorDialog() override;

    void setEditor(ScintillaEditor* editor);

public Q_SLOTS:
    void onApply();
    void onOk();

private:
    void setupUi();

    /// Insert the configured content into the current rectangular selection
    void applyContent();

    ScintillaEditor* _editor = nullptr;

    // UI fields
    QCheckBox* _columnModeCheck = nullptr;
    QRadioButton* _numericRadio = nullptr;
    QRadioButton* _characterRadio = nullptr;

    // Numeric options
    QGroupBox* _numericGroup = nullptr;
    QSpinBox* _initialValueSpin = nullptr;
    QSpinBox* _stepSpin = nullptr;
    QSpinBox* _paddingSpin = nullptr;

    // Character options
    QGroupBox* _characterGroup = nullptr;
    QLineEdit* _characterEdit = nullptr;
    QLabel* _hexLabel = nullptr;
    QSpinBox* _repeatSpin = nullptr;

    QPushButton* _okButton = nullptr;
    QPushButton* _cancelButton = nullptr;
};
