// GoToLineDialog.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "GoToLineDialog.h"
#include "../editor/ScintillaEditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>

GoToLineDialog::GoToLineDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Go to Line");
    setModal(true);
    setMinimumWidth(300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout();
    _lineSpinBox = new QSpinBox(this);
    _lineSpinBox->setMinimum(1);
    _lineSpinBox->setMaximum(999999);
    _totalLabel = new QLabel("of 1", this);
    formLayout->addRow("Line number:", _lineSpinBox);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* gotoBtn = new QPushButton("Go to", this);
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    btnLayout->addStretch();
    btnLayout->addWidget(gotoBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);

    connect(gotoBtn, &QPushButton::clicked, this, &GoToLineDialog::onGoto);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(_lineSpinBox, &QSpinBox::editingFinished, this, &GoToLineDialog::onGoto);
}

GoToLineDialog::~GoToLineDialog() = default;

void GoToLineDialog::setEditor(ScintillaEditor* editor) {
    _editor = editor;
    if (_editor) {
        int count = _editor->lineCount();
        _lineSpinBox->setMaximum(count);
        _totalLabel->setText(QString("of %1").arg(count));
    }
}

int GoToLineDialog::lineCount() const {
    return _editor ? _editor->lineCount() : 0;
}

void GoToLineDialog::onGoto() {
    if (_editor) {
        int line = _lineSpinBox->value() - 1;
        _editor->gotoLine(line, 0);
    }
    accept();
}
