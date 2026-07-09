// FindReplaceDialog.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FindReplaceDialog.h"
#include "../editor/ScintillaEditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QMessageBox>

FindReplaceDialog::FindReplaceDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Find / Replace");
    setModal(false);
    setupUi();
    readSettings();
}

FindReplaceDialog::~FindReplaceDialog() {
    writeSettings();
}

void FindReplaceDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QGroupBox* findGroup = new QGroupBox("Find", this);
    QFormLayout* findLayout = new QFormLayout(findGroup);
    _findCombo = new QComboBox(this);
    _findCombo->setEditable(true);
    _findCombo->setMinimumWidth(300);
    findLayout->addRow("Find:", _findCombo);

    QGroupBox* replaceGroup = new QGroupBox("Replace", this);
    QFormLayout* replaceLayout = new QFormLayout(replaceGroup);
    _replaceCombo = new QComboBox(this);
    _replaceCombo->setEditable(true);
    replaceLayout->addRow("Replace:", _replaceCombo);

    QHBoxLayout* optionsLayout = new QHBoxLayout();
    _matchCaseCheck = new QCheckBox("Match Case", this);
    _wholeWordCheck = new QCheckBox("Whole Word", this);
    _wrapCheck = new QCheckBox("Wrap Around", this);
    _wrapCheck->setChecked(true);
    _regexCheck = new QCheckBox("Regex", this);
    optionsLayout->addWidget(_matchCaseCheck);
    optionsLayout->addWidget(_wholeWordCheck);
    optionsLayout->addWidget(_wrapCheck);
    optionsLayout->addWidget(_regexCheck);
    optionsLayout->addStretch();

    replaceLayout->addRow("", optionsLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    _findNextBtn = new QPushButton("Find Next", this);
    _replaceBtn = new QPushButton("Replace", this);
    _replaceAllBtn = new QPushButton("Replace All", this);
    _closeBtn = new QPushButton("Close", this);
    btnLayout->addWidget(_findNextBtn);
    btnLayout->addWidget(_replaceBtn);
    btnLayout->addWidget(_replaceAllBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(_closeBtn);

    _statusLabel = new QLabel(this);

    mainLayout->addWidget(findGroup);
    mainLayout->addWidget(replaceGroup);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(_statusLabel);

    connect(_findNextBtn, &QPushButton::clicked, this, &FindReplaceDialog::onFindNext);
    connect(_replaceBtn, &QPushButton::clicked, this, &FindReplaceDialog::onReplace);
    connect(_replaceAllBtn, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAll);
    connect(_closeBtn, &QPushButton::clicked, this, &QDialog::hide);
}

void FindReplaceDialog::readSettings() { }
void FindReplaceDialog::writeSettings() { }

void FindReplaceDialog::setEditor(ScintillaEditor* editor) { _editor = editor; }

void FindReplaceDialog::setSearchText(const QString& text) {
    _findCombo->setEditText(text);
}

FindOption currentOptions() {
    return FindOption::None;
}

void FindReplaceDialog::onFindNext() {
    QString text = _findCombo->currentText();
    if (text.isEmpty()) return;
    if (_editor) {
        FindOption opts = FindOption::None;
        if (_matchCaseCheck->isChecked()) opts |= FindOption::MatchCase;
        if (_wholeWordCheck->isChecked()) opts |= FindOption::WholeWord;
        if (_wrapCheck->isChecked()) opts |= FindOption::WrapAround;
        _editor->findNext(text, opts);
    }
    emit findNextRequested(text, currentOptions());
}

void FindReplaceDialog::onReplace() {
    if (_editor) {
        QString rep = _replaceCombo->currentText();
        _editor->replace(rep);
    }
}

void FindReplaceDialog::onReplaceAll() {
    if (_editor) {
        QString find = _findCombo->currentText();
        QString rep = _replaceCombo->currentText();
        FindOption opts = FindOption::None;
        if (_matchCaseCheck->isChecked()) opts |= FindOption::MatchCase;
        if (_wholeWordCheck->isChecked()) opts |= FindOption::WholeWord;
        if (_wrapCheck->isChecked()) opts |= FindOption::WrapAround;
        _editor->replaceAll(find, rep, opts);
    }
}

void FindReplaceDialog::findNext() { onFindNext(); }
void FindReplaceDialog::findPrevious() { }
void FindReplaceDialog::replace() { onReplace(); }
void FindReplaceDialog::replaceAll() { onReplaceAll(); }
