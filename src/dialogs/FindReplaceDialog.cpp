// FindReplaceDialog.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FindReplaceDialog.h"
#include "core/Application.h"
#include "../editor/ScintillaEditor.h"
#include <Qsci/qsciscintilla.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSignalBlocker>

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

    // --- Find group ---
    QGroupBox* findGroup = new QGroupBox("Find", this);
    QFormLayout* findLayout = new QFormLayout(findGroup);
    _findCombo = new QComboBox(this);
    _findCombo->setEditable(true);
    _findCombo->setMinimumWidth(300);
    _findCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    findLayout->addRow("Find:", _findCombo);

    // --- Replace group ---
    QGroupBox* replaceGroup = new QGroupBox("Replace", this);
    QFormLayout* replaceLayout = new QFormLayout(replaceGroup);
    _replaceCombo = new QComboBox(this);
    _replaceCombo->setEditable(true);
    _replaceCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    replaceLayout->addRow("Replace:", _replaceCombo);

    // --- Direction toggle ---
    QGroupBox* dirGroup = new QGroupBox("Direction", this);
    QHBoxLayout* dirLayout = new QHBoxLayout(dirGroup);
    _directionGroup = new QButtonGroup(this);
    QPushButton* downBtn = new QPushButton("Down", this);
    downBtn->setCheckable(true);
    downBtn->setChecked(true);
    downBtn->setMinimumWidth(60);
    QPushButton* upBtn = new QPushButton("Up", this);
    upBtn->setCheckable(true);
    upBtn->setMinimumWidth(60);
    _directionGroup->addButton(downBtn, 0);
    _directionGroup->addButton(upBtn, 1);
    dirLayout->addWidget(downBtn);
    dirLayout->addWidget(upBtn);
    dirLayout->addStretch();
    dirLayout->setContentsMargins(4, 4, 4, 4);

    // --- Options row ---
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

    // --- Button row ---
    QHBoxLayout* btnLayout = new QHBoxLayout();
    _findNextBtn = new QPushButton("Find Next", this);
    _replaceBtn = new QPushButton("Replace", this);
    _replaceAllBtn = new QPushButton("Replace All", this);
    _countBtn = new QPushButton("Count", this);
    _closeBtn = new QPushButton("Close", this);
    btnLayout->addWidget(_findNextBtn);
    btnLayout->addWidget(_replaceBtn);
    btnLayout->addWidget(_replaceAllBtn);
    btnLayout->addWidget(_countBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(_closeBtn);

    _statusLabel = new QLabel(this);
    _statusLabel->setStyleSheet("QLabel { color: palette(highlight-text); font-weight: bold; }");

    mainLayout->addWidget(findGroup);
    mainLayout->addWidget(replaceGroup);
    mainLayout->addWidget(dirGroup);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(_statusLabel);

    // --- Connections ---
    connect(_findNextBtn, &QPushButton::clicked, this, &FindReplaceDialog::onFindNext);
    connect(_replaceBtn, &QPushButton::clicked, this, &FindReplaceDialog::onReplace);
    connect(_replaceAllBtn, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAll);
    connect(_countBtn, &QPushButton::clicked, this, &FindReplaceDialog::onCount);
    connect(_closeBtn, &QPushButton::clicked, this, &QDialog::hide);
    connect(_directionGroup, &QButtonGroup::idClicked, this, &FindReplaceDialog::onDirectionChanged);

    // Wire search-as-you-type highlight — use textChanged on the line edit within the combobox
    QLineEdit* findEdit = _findCombo->lineEdit();
    if (findEdit) {
        connect(findEdit, &QLineEdit::textChanged, this, &FindReplaceDialog::onSearchTextChanged);
    }

    // Also handle when user selects from history (same combobox entry chosen)
    // Use currentTextChanged which fires on both typing and selection
    connect(_findCombo, &QComboBox::currentTextChanged,
            this, &FindReplaceDialog::onSearchTextChanged);
}

void FindReplaceDialog::readSettings() { }
void FindReplaceDialog::writeSettings() { }

void FindReplaceDialog::setEditor(ScintillaEditor* editor) { _editor = editor; }

void FindReplaceDialog::setSearchText(const QString& text) {
    {
        QSignalBlocker blocker(_findCombo);
        _findCombo->setEditText(text);
    }
    onSearchTextChanged(text);
}

FindOption FindReplaceDialog::currentOptions() const {
    FindOption opts = FindOption::None;
    if (_matchCaseCheck->isChecked()) opts |= FindOption::MatchCase;
    if (_wholeWordCheck->isChecked()) opts |= FindOption::WholeWord;
    if (_wrapCheck->isChecked()) opts |= FindOption::WrapAround;
    if (_regexCheck->isChecked()) opts |= FindOption::Regex;
    return opts;
}

void FindReplaceDialog::onDirectionChanged(int directionId) {
    _searchForward = (directionId == 0);  // 0 = Down, 1 = Up
}

void FindReplaceDialog::onSearchTextChanged(const QString& text) {
    // Don't re-highlight if the text hasn't actually changed
    if (text == _prevHighlightTerm) return;
    _prevHighlightTerm = text;

    if (!_editor || text.isEmpty()) {
        clearHighlight();
        return;
    }

    // Store current cursor so we can restore it
    int curLine = 0, curCol = 0;
    _editor->getCursorPosition(&curLine, &curCol);

    // Clear previous highlights
    clearHighlight();

    // Mark all matches using box indicator (yellow background)
    int count = _editor->markAllMatches(text, currentOptions());

    // Restore cursor
    _editor->setCursorPosition(curLine, curCol);

    // Show live count in status label if non-zero
    if (count > 0) {
        _statusLabel->setText(QString("  %1 match%2")
            .arg(count)
            .arg(count == 1 ? "" : "es"));
    } else {
        _statusLabel->clear();
    }
}

void FindReplaceDialog::clearHighlight() {
    if (!_editor) return;
    // Clear indicator 1 (our highlight indicator) from entire document
    _editor->clearIndicatorRange(0, 0, _editor->lineCount(), 0, 1);
    _statusLabel->clear();
}

void FindReplaceDialog::hideEvent(QHideEvent* event) {
    clearHighlight();
    QDialog::hideEvent(event);
}

void FindReplaceDialog::onFindNext() {
    QString text = _findCombo->currentText();
    if (text.isEmpty()) return;
    _lastSearchText = text;
    _lastSearchOptions = currentOptions();

    if (!_editor) return;

    // Determine direction
    bool forward = _searchForward;
    // Shift modifier also reverses direction
    // (handled by keypress event in the editor, not here)

    if (forward) {
        _editor->findNext(text, currentOptions());
    } else {
        _editor->findPrevious(text, currentOptions());
    }

    // Clear auto-highlight when user explicitly searches
    clearHighlight();

    emit findNextRequested(text, currentOptions());
}

void FindReplaceDialog::onReplace() {
    if (!_editor) return;
    // replace() in ScintillaEditor replaces selection with replacement text
    _editor->replace(_replaceCombo->currentText());
    // After replacing, advance to next match
    onFindNext();
}

void FindReplaceDialog::onReplaceAll() {
    if (!_editor) return;

    QString findText = _findCombo->currentText();
    QString replaceText = _replaceCombo->currentText();
    if (findText.isEmpty()) return;

    _lastSearchText = findText;
    _lastSearchOptions = currentOptions();

    FindOption opts = currentOptions();
    bool isRegex = (opts & FindOption::Regex) != FindOption::None;

    int replacedCount = 0;

    if (isRegex) {
        // Use the SCI_TARGET approach for regex with backreference support.
        // This mirrors the NPP pattern: searchInTarget + replaceTargetRegExMode.
        QsciScintilla* sci = _editor->qsciEditor();
        int docLength = sci->length();

        // Set search flags
        int flags = 0;
        if ((opts & FindOption::MatchCase) != FindOption::None) flags |= 0x04;   // SCFIND_MATCHCASE
        if ((opts & FindOption::WholeWord) != FindOption::None) flags |= 0x02;   // SCFIND_WHOLEWORD
        if ((opts & FindOption::Regex) != FindOption::None) flags |= 0x00200000; // SCFIND_REGEXP
        // Note: SCFIND_REGEXP_SKIPCRLFASONE also set in NPP for consistent handling
        if ((opts & FindOption::Regex) != FindOption::None) flags |= 0x00020000; // SCFIND_REGEXP_SKIPCRLFASONE

        sci->SendScintilla(QsciScintilla::SCI_SETSEARCHFLAGS, flags);
        sci->SendScintilla(QsciScintilla::SCI_BEGINUNDOACTION);

        int targetStart = 0;
        int targetEnd = docLength;

        while (targetStart < docLength) {
            // Set target range for this iteration
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETSTART, targetStart);
            sci->SendScintilla(QsciScintilla::SCI_SETTARGETEND, targetEnd);

            // Search in target — returns -1 if not found, or position
            QByteArray findBa = findText.toUtf8();
            int foundPos = sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                              findBa.size(),
                                              findBa.constData());

            if (foundPos < 0) break;

            // Get the matched range
            int foundEnd = sci->SendScintilla(QsciScintilla::SCI_GETTARGETEND);

            // For backreferences, we need to retrieve matched groups.
            // SCI_GETTAG(n) returns the text of capture group n (0 = entire match).
            // The replacement string may contain \1, \2 etc. which we expand here.
            // SCI_REPLACETARGETRE handles this natively in Scintilla.
            QByteArray replaceBa = replaceText.toUtf8();
            int newLen = sci->SendScintilla(QsciScintilla::SCI_REPLACETARGETRE,
                                             replaceBa.size(),
                                             replaceBa.constData());

            int matchLen = foundEnd - foundPos;
            int delta = newLen - matchLen;
            replacedCount++;

            // Next search starts after the replaced text
            targetStart = foundPos + newLen;
        }

        sci->SendScintilla(QsciScintilla::SCI_ENDUNDOACTION);
    } else {
        // Plain text replace all — use the simple loop
        replacedCount = _editor->replaceAll(findText, replaceText, opts);
    }

    // Update status
    QString msg = (replacedCount == 1)
        ? QString("Replaced 1 occurrence of \"%1\"").arg(findText)
        : QString("Replaced %1 occurrences of \"%2\"").arg(replacedCount).arg(findText);
    _statusLabel->setText(msg);

    // Also show in status bar
    if (auto* app = qobject_cast<QWidget*>(parent())) {
        app->findChild<QStatusBar*>();
    }
    // Use Application instance to set status bar
    Application::instance().setStatusBarText(msg.toStdString());
}

void FindReplaceDialog::onCount() {
    QString text = _findCombo->currentText();
    if (text.isEmpty()) {
        _statusLabel->setText("Count: no search term");
        return;
    }

    _lastSearchText = text;
    _lastSearchOptions = currentOptions();

    emit countRequested(text, currentOptions());
}

void FindReplaceDialog::findNext() { onFindNext(); }

void FindReplaceDialog::findPrevious() {
    QString text = _findCombo->currentText();
    if (text.isEmpty()) return;
    _lastSearchText = text;
    _lastSearchOptions = currentOptions();
    if (_editor) {
        _editor->findPrevious(text, currentOptions());
    }
    clearHighlight();
    emit findNextRequested(text, currentOptions());
}

void FindReplaceDialog::replace() { onReplace(); }
void FindReplaceDialog::replaceAll() { onReplaceAll(); }
