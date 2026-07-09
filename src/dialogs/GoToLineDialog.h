// GoToLineDialog.h - Go to line dialog
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>

class ScintillaEditor;

class GoToLineDialog : public QDialog {
    Q_OBJECT
public:
    explicit GoToLineDialog(QWidget* parent = nullptr);
    ~GoToLineDialog() override;

    void setEditor(ScintillaEditor* editor);
    int lineCount() const;

public slots:
    void onGoto();

private:
    ScintillaEditor* _editor = nullptr;
    QSpinBox* _lineSpinBox = nullptr;
    QLabel* _totalLabel = nullptr;
};
