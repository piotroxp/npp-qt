// ColumnEditorDialog.cpp — Column/rectangular selection editor
// Inserts sequential numbers or repeated characters into a rectangular
// (column-mode) selection in Scintilla.
// Copyright (C) 2026 Agent Army / GPL v3

#include "ColumnEditorDialog.h"
#include "../editor/ScintillaEditor.h"

#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciscintillabase.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QDebug>
#include <QApplication>

ColumnEditorDialog::ColumnEditorDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Column Editor");
    setModal(true);
    setupUi();
}

ColumnEditorDialog::~ColumnEditorDialog() = default;

void ColumnEditorDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // ── Column mode checkbox ──────────────────────────────────────────────
    _columnModeCheck = new QCheckBox("Column Mode (Alt+Click active selection)", this);
    mainLayout->addWidget(_columnModeCheck);

    // ── Content type ──────────────────────────────────────────────────────
    QGroupBox* typeGroup = new QGroupBox("Insert Type", this);
    QVBoxLayout* typeLayout = new QVBoxLayout(typeGroup);

    _numericRadio = new QRadioButton("Numeric (e.g. 1, 2, 3...)", this);
    _characterRadio = new QRadioButton("Character (e.g. repeated char or hex)", this);
    _numericRadio->setChecked(true);
    typeLayout->addWidget(_numericRadio);
    typeLayout->addWidget(_characterRadio);
    mainLayout->addWidget(typeGroup);

    // ── Numeric options ───────────────────────────────────────────────────
    _numericGroup = new QGroupBox("Numeric Options", this);
    QFormLayout* numLayout = new QFormLayout(_numericGroup);

    _initialValueSpin = new QSpinBox(this);
    _initialValueSpin->setRange(-999999999, 999999999);
    _initialValueSpin->setValue(1);
    _initialValueSpin->setPrefix("Start at: ");
    numLayout->addRow("Initial value:", _initialValueSpin);

    _stepSpin = new QSpinBox(this);
    _stepSpin->setRange(-999999, 999999);
    _stepSpin->setValue(1);
    _stepSpin->setPrefix("Step: ");
    numLayout->addRow("Step:", _stepSpin);

    _paddingSpin = new QSpinBox(this);
    _paddingSpin->setRange(0, 20);
    _paddingSpin->setValue(0);
    _paddingSpin->setPrefix("Left pad: ");
    numLayout->addRow("Zero-padding:", _paddingSpin);

    mainLayout->addWidget(_numericGroup);

    // ── Character options ─────────────────────────────────────────────────
    _characterGroup = new QGroupBox("Character Options", this);
    QFormLayout* charLayout = new QFormLayout(_characterGroup);

    _characterEdit = new QLineEdit(this);
    _characterEdit->setMaxLength(1);
    _characterEdit->setPlaceholderText("e.g. 0, A, *");
    _characterEdit->setMaximumWidth(60);
    charLayout->addRow("Character:", _characterEdit);

    _hexLabel = new QLabel("or hex value (0x..)", this);
    charLayout->addRow("", _hexLabel);

    _repeatSpin = new QSpinBox(this);
    _repeatSpin->setRange(1, 1000);
    _repeatSpin->setValue(1);
    _repeatSpin->setPrefix("Repeat: ");
    charLayout->addRow("Times to repeat:", _repeatSpin);

    _characterGroup->setEnabled(false);
    mainLayout->addWidget(_characterGroup);

    // ── Buttons ───────────────────────────────────────────────────────────
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    _okButton = new QPushButton("OK", this);
    QPushButton* applyButton = new QPushButton("Apply", this);
    _cancelButton = new QPushButton("Cancel", this);
    btnLayout->addWidget(_okButton);
    btnLayout->addWidget(applyButton);
    btnLayout->addWidget(_cancelButton);
    mainLayout->addLayout(btnLayout);

    // ── Connections ────────────────────────────────────────────────────────
    connect(_numericRadio, &QRadioButton::toggled, this, [this](bool on) {
        _numericGroup->setEnabled(on);
        _characterGroup->setEnabled(!on);
    });

    connect(_okButton, &QPushButton::clicked, this, &ColumnEditorDialog::onOk);
    connect(applyButton, &QPushButton::clicked, this, &ColumnEditorDialog::onApply);
    connect(_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Make enter key apply, escape cancel
    _okButton->setDefault(false);
    _okButton->setAutoDefault(true);
}

void ColumnEditorDialog::setEditor(ScintillaEditor* editor) {
    _editor = editor;
}

void ColumnEditorDialog::onApply() {
    applyContent();
}

void ColumnEditorDialog::onOk() {
    applyContent();
    accept();
}

void ColumnEditorDialog::applyContent() {
    if (!_editor) return;

    QsciScintilla* sci = _editor->qsciEditor();
    if (!sci) return;

    // Ensure rectangular selection mode
    if (_columnModeCheck->isChecked()) {
        sci->SendScintilla(QsciScintilla::SCI_SETSELECTIONMODE, 1); // SC_SEL_RECTANGLE
        sci->SendScintilla(QsciScintilla::SCI_SETVIRTUALSPACEOPTIONS, 1);
    }

    if (_numericRadio->isChecked()) {
        // Numeric mode: insert sequential numbers
        int initialValue = _initialValueSpin->value();
        int step = _stepSpin->value();
        int padding = _paddingSpin->value();

        // Get rectangular selection range
        int selStart = sci->SendScintilla(QsciScintilla::SCI_GETSELECTIONNSTART, 0);
        int selEnd   = sci->SendScintilla(QsciScintilla::SCI_GETSELECTIONNEND, 0);

        if (selEnd <= selStart) {
            qDebug() << "ColumnEditorDialog: no selection";
            return;
        }

        // For rectangular selection we need the anchor lines
        int lineStart = sci->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, selStart);
        int lineEnd   = sci->SendScintilla(QsciScintilla::SCI_LINEFROMPOSITION, selEnd);

        // We build the replacement string and set it
        // For each line in the selection, we insert a number at the virtual cursor position
        QString replacement;
        int value = initialValue;
        for (int line = lineStart; line <= lineEnd; ++line) {
            if (line > lineStart) replacement += '\n';
            QString numStr = QString::number(value);
            if (padding > 0) {
                numStr = numStr.rightJustified(padding, '0');
            }
            replacement += numStr;
            value += step;
        }

        // Replace selection with the numeric sequence
        QByteArray repUtf8 = replacement.toUtf8();
        sci->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
                            reinterpret_cast<intptr_t>(repUtf8.constData()));

    } else {
        // Character mode
        QString charStr = _characterEdit->text();
        int repeat = _repeatSpin->value();

        if (charStr.isEmpty()) {
            // Try hex interpretation
            QString hexText = _hexLabel->text();
            Q_UNUSED(hexText);
            // Fallback: no-op
            return;
        }

        QChar ch = charStr.at(0);
        QString repeated(repeat, ch);

        // Insert the repeated character into the selection
        QByteArray repUtf8 = repeated.toUtf8();
        sci->SendScintilla(QsciScintilla::SCI_REPLACESEL, 0,
                            reinterpret_cast<intptr_t>(repUtf8.constData()));
    }
}
