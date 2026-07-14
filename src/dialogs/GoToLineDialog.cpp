// GoToLineDialog.cpp - Go to line dialog with offset mode
// Copyright (C) 2026 Agent Army
// GPL v3

#include "GoToLineDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QKeyEvent>
#include <QStyle>

GoToLineDialog::GoToLineDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Go to Line   (Ctrl+G)");
    setModal(true);
    setFixedSize(320, 200);

    setupUi();
    applyStyle();

    connect(lineSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GoToLineDialog::validateInput);
    connect(columnSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &GoToLineDialog::validateInput);
    connect(offsetCheckBox, &QCheckBox::toggled, this, &GoToLineDialog::validateInput);
    connect(goBtn, &QPushButton::clicked, this, &GoToLineDialog::onGoClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &GoToLineDialog::onCancelClicked);

    validateInput();
}

GoToLineDialog::~GoToLineDialog() = default;

void GoToLineDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 12);
    mainLayout->setSpacing(10);

    // Line number section
    QLabel* lineLabel = new QLabel("Line number:", this);
    lineLabel->setObjectName("lineLabel");

    lineSpinBox = new QSpinBox(this);
    lineSpinBox->setObjectName("lineSpinBox");
    lineSpinBox->setMinimum(1);
    lineSpinBox->setMaximum(maxLine);
    lineSpinBox->setValue(1);
    lineSpinBox->setAlignment(Qt::AlignCenter);

    // Column number section
    QLabel* columnLabel = new QLabel("Column number:", this);
    columnLabel->setObjectName("columnLabel");

    columnSpinBox = new QSpinBox(this);
    columnSpinBox->setObjectName("columnSpinBox");
    columnSpinBox->setMinimum(1);
    columnSpinBox->setMaximum(maxColumn);
    columnSpinBox->setValue(1);
    columnSpinBox->setAlignment(Qt::AlignCenter);

    // Offset mode checkbox
    offsetCheckBox = new QCheckBox("Go to offset (bytes from start)", this);
    offsetCheckBox->setObjectName("offsetCheckBox");

    // Range label
    rangeLabel = new QLabel(this);
    rangeLabel->setObjectName("rangeLabel");
    rangeLabel->setAlignment(Qt::AlignCenter);

    // Status label
    statusLabel = new QLabel(this);
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);

    // Spacer
    mainLayout->addStretch();

    // Form layout for inputs
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    formLayout->addRow(lineLabel, lineSpinBox);
    formLayout->addRow(columnLabel, columnSpinBox);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(offsetCheckBox);
    mainLayout->addWidget(rangeLabel);
    mainLayout->addWidget(statusLabel);
    mainLayout->addStretch();

    // Button layout
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->setSpacing(8);

    goBtn = new QPushButton("Go", this);
    goBtn->setObjectName("goBtn");
    goBtn->setDefault(true);

    cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setObjectName("cancelBtn");
    cancelBtn->setAutoDefault(false);

    btnLayout->addWidget(goBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(btnLayout);
}

void GoToLineDialog::applyStyle() {
    setStyleSheet(R"(
        QDialog {
            background-color: #2b2b2b;
            border-radius: 6px;
        }
        QLabel {
            color: #cccccc;
            font-size: 12px;
        }
        #lineLabel, #columnLabel {
            font-weight: bold;
        }
        QSpinBox {
            background-color: #404040;
            color: #e0e0e0;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 4px 8px;
            min-width: 80px;
            font-size: 13px;
        }
        QSpinBox:focus {
            border: 1px solid #007acc;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #505050;
            border-radius: 2px;
            width: 16px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background-color: #606060;
        }
        QSpinBox::up-button:pressed, QSpinBox::down-button:pressed {
            background-color: #707070;
        }
        QCheckBox {
            color: #cccccc;
            font-size: 12px;
            spacing: 6px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border-radius: 3px;
            border: 1px solid #555555;
            background-color: #404040;
        }
        QCheckBox::indicator:checked {
            background-color: #007acc;
            border-color: #007acc;
        }
        QCheckBox::indicator:hover {
            border-color: #777777;
        }
        #rangeLabel {
            color: #888888;
            font-size: 11px;
        }
        #statusLabel {
            color: #00aa00;
            font-size: 12px;
            font-weight: bold;
            padding: 4px;
            border-radius: 4px;
        }
        #statusLabel.highlight {
            color: #ffaa00;
        }
        QPushButton {
            background-color: #404040;
            color: #e0e0e0;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 6px 16px;
            font-size: 12px;
            min-width: 60px;
        }
        QPushButton:hover {
            background-color: #505050;
            border-color: #666666;
        }
        QPushButton:pressed {
            background-color: #353535;
        }
        #goBtn {
            background-color: #007acc;
            border-color: #007acc;
            font-weight: bold;
        }
        #goBtn:hover {
            background-color: #0098dd;
            border-color: #0098dd;
        }
        #goBtn:pressed {
            background-color: #006699;
        }
        #goBtn:disabled {
            background-color: #333333;
            color: #666666;
            border-color: #444444;
        }
        #cancelBtn {
            background-color: #353535;
        }
        #cancelBtn:hover {
            background-color: #454545;
        }
    )");
}

void GoToLineDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    lineSpinBox->setFocus();
    lineSpinBox->selectAll();
    validateInput();
}

void GoToLineDialog::setMaxLineNumber(int maxLine) {
    this->maxLine = maxLine;
    if (lineSpinBox) {
        lineSpinBox->setMaximum(maxLine);
        updateStatusLabel();
    }
}

void GoToLineDialog::setMaxColumnNumber(int maxCol) {
    this->maxColumn = maxCol;
    if (columnSpinBox) {
        columnSpinBox->setMaximum(maxCol);
    }
}

void GoToLineDialog::validateInput() {
    if (!lineSpinBox || !goBtn || !statusLabel)
        return;

    int line = lineSpinBox->value();
    int column = columnSpinBox->value();
    bool isOffset = offsetCheckBox->isChecked();

    bool valid = true;
    if (isOffset) {
        valid = line >= 0 && line <= maxLine;
    } else {
        valid = line >= 1 && line <= maxLine;
    }

    goBtn->setEnabled(valid);
    updateStatusLabel();
}

void GoToLineDialog::updateStatusLabel() {
    if (!lineSpinBox || !statusLabel || !rangeLabel)
        return;

    int line = lineSpinBox->value();
    bool isOffset = offsetCheckBox->isChecked();

    if (isOffset) {
        rangeLabel->setText(QString("Offset range: 0 - %1 bytes").arg(maxLine));
        if (maxLine > 0) {
            int percent = (line * 100) / maxLine;
            statusLabel->setText(QString("Offset: %1 of %2 (%3%)")
                .arg(line).arg(maxLine).arg(percent));
        } else {
            statusLabel->setText(QString("Offset: %1 of %2").arg(line).arg(maxLine));
        }
    } else {
        rangeLabel->setText(QString("Line range: 1 - %1").arg(maxLine));
        if (maxLine > 0) {
            int percent = (line * 100) / maxLine;
            statusLabel->setText(QString("Line %1 of %2 (%3%)")
                .arg(line).arg(maxLine).arg(percent));
        } else {
            statusLabel->setText(QString("Line %1 of %2").arg(line).arg(maxLine));
        }
    }

    // Highlight when line is >90% of file
    if (!isOffset && maxLine > 0) {
        double ratio = static_cast<double>(line) / maxLine;
        if (ratio > 0.9) {
            statusLabel->setProperty("highlight", true);
            statusLabel->setStyleSheet("color: #ffaa00; font-weight: bold;");
        } else {
            statusLabel->setProperty("highlight", false);
            statusLabel->setStyleSheet("color: #00aa00; font-weight: bold;");
        }
        statusLabel->style()->unpolish(statusLabel);
        statusLabel->style()->polish(statusLabel);
    }
}

void GoToLineDialog::onGoClicked() {
    validateInput();
    if (goBtn->isEnabled()) {
        accept();
    }
}

void GoToLineDialog::onCancelClicked() {
    reject();
}

void GoToLineDialog::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (goBtn->isEnabled()) {
            onGoClicked();
        }
        event->accept();
    } else if (event->key() == Qt::Key_Escape) {
        onCancelClicked();
        event->accept();
    } else {
        QDialog::keyPressEvent(event);
    }
}
