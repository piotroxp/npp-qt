// FindReplaceDialog.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FindReplaceDialog.h"
#include "core/Application.h"
#include "../editor/ScintillaEditor.h"
#include "../NppConstants.h"
#include "ui/MainWindow.h"
#include <Qsci/qsciscintilla.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QSettings>
#include <QLineEdit>

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

    // Wire search-as-you-type highlight - use textChanged on the line edit within the combobox
    QLineEdit* findEdit = _findCombo->lineEdit();
    if (findEdit) {
        connect(findEdit, &QLineEdit::textChanged, this, &FindReplaceDialog::onSearchTextChanged);
    }

    // Also handle when user selects from history (same combobox entry chosen)
    // Use currentTextChanged which fires on both typing and selection
    connect(_findCombo, &QComboBox::currentTextChanged,
            this, &FindReplaceDialog::onSearchTextChanged);
}

void FindReplaceDialog::readSettings() {
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP);
    restoreGeometry(settings.value(SETTINGS_GEOMETRY).toByteArray());
    _matchCaseCheck->setChecked(settings.value(SETTINGS_MATCH_CASE, false).toBool());
    _wholeWordCheck->setChecked(settings.value(SETTINGS_WHOLE_WORD, false).toBool());
    _wrapCheck->setChecked(settings.value(SETTINGS_WRAP, true).toBool());
    _regexCheck->setChecked(settings.value(SETTINGS_REGEX, false).toBool());
    _highlightingCheck->setChecked(settings.value(SETTINGS_HIGHLIGHT, true).toBool());
    _promptsCheck->setChecked(settings.value(SETTINGS_PROMPTS, true).toBool());
    _hideDialogCheck->setChecked(settings.value(SETTINGS_HIDE_DIALOG, true).toBool());
    int dir = settings.value(SETTINGS_DIRECTION, 0).toInt();
    if (auto* btn = _directionGroup->button(dir)) btn->setChecked(true);
    settings.endGroup();
    loadSearchHistory();
    loadReplaceHistory();
}

void FindReplaceDialog::writeSettings() {
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP);
    settings.setValue(SETTINGS_GEOMETRY, saveGeometry());
    settings.setValue(SETTINGS_MATCH_CASE, _matchCaseCheck->isChecked());
    settings.setValue(SETTINGS_WHOLE_WORD, _wholeWordCheck->isChecked());
    settings.setValue(SETTINGS_WRAP, _wrapCheck->isChecked());
    settings.setValue(SETTINGS_REGEX, _regexCheck->isChecked());
    settings.setValue(SETTINGS_HIGHLIGHT, _highlightingCheck ? _highlightingCheck->isChecked() : true);
    settings.setValue(SETTINGS_PROMPTS, _promptsCheck ? _promptsCheck->isChecked() : true);
    settings.setValue(SETTINGS_HIDE_DIALOG, _hideDialogCheck ? _hideDialogCheck->isChecked() : true);
    settings.setValue(SETTINGS_DIRECTION, _directionGroup->checkedId());
    settings.setValue(SETTINGS_RECENT_FINDS, _searchHistory);
    settings.setValue(SETTINGS_RECENT_REPLACES, _replaceHistory);
    settings.endGroup();
}

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

            // Search in target - returns -1 if not found, or position
            QByteArray findBa = findText.toUtf8();
            int foundPos = sci->SendScintilla(QsciScintilla::SCI_SEARCHINTARGET,
                                              findBa.size(),
                                              findBa.constData());

            if (foundPos < 0) break;

            // Get the matched range
            (void)sci->SendScintilla(QsciScintilla::SCI_GETTARGETEND);

            // For backreferences, we need to retrieve matched groups.
            // SCI_GETTAG(n) returns the text of capture group n (0 = entire match).
            // The replacement string may contain \1, \2 etc. which we expand here.
            // SCI_REPLACETARGETRE handles this natively in Scintilla.
            QByteArray replaceBa = replaceText.toUtf8();
            int newLen = sci->SendScintilla(QsciScintilla::SCI_REPLACETARGETRE,
                                             replaceBa.size(),
                                             replaceBa.constData());

            replacedCount++;

            // Next search starts after the replaced text
            targetStart = foundPos + newLen;
        }

        sci->SendScintilla(QsciScintilla::SCI_ENDUNDOACTION);
    } else {
        // Plain text replace all - use the simple loop
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

void FindReplaceDialog::setMatchCount(int count) {
    if (_statusLabel) {
        if (count >= 0) {
            _statusLabel->setText(QString("  %1 match%2")
                .arg(count).arg(count == 1 ? "" : "s"));
        } else {
            _statusLabel->setText(QString());
        }
    }
}

void FindReplaceDialog::setStatusMessage(const QString& msg) {
    if (_statusLabel)
        _statusLabel->setText(QString("  %1").arg(msg));
}

// === Stubs ===
void FindReplaceDialog::restoreAfterSearch() {
    if (!_editor) return;
    // Clear temporary find markers via the underlying QsciScintilla
    QsciScintilla* sci = _editor->qsciEditor();
    sci->markerDeleteAll(MARKER_FINDMARK_ALL);
    for (int i = 11; i <= 15; ++i) {
        sci->markerDeleteAll(i);
    }
    // Restore cursor visibility and clear indicator highlight
    clearHighlight();
}

void FindReplaceDialog::onFindPrevious() {
    QString text = _findCombo->currentText();
    if (text.isEmpty()) return;
    _lastSearchText = text;
    _lastSearchOptions = currentOptions();

    if (_editor) {
        _editor->findPrevious(text, currentOptions());
    }
    clearHighlight();
    emit findPrevRequested(text, currentOptions());
}

void FindReplaceDialog::onReplaceAllInAll() {
    QString findText = _findCombo->currentText();
    QString replaceText = _replaceCombo->currentText();
    if (findText.isEmpty()) return;

    _lastSearchText = findText;
    _lastSearchOptions = currentOptions();

    FindOption opts = currentOptions();

    // Get MainWindow and iterate all open editors
    auto* mw = Application::instance().mainWindow();
    if (!mw) return;

    int totalReplaced = 0;
    for (int i = 0; i < mw->editorCount(); ++i) {
        if (auto* ed = mw->editorAt(i)) {
            int n = ed->replaceAll(findText, replaceText, opts);
            totalReplaced += n;
        }
    }

    QString msg = (totalReplaced == 1)
        ? QString("Replaced 1 occurrence of \"%1\" in all open documents").arg(findText)
        : QString("Replaced %1 occurrences of \"%2\" in all open documents").arg(totalReplaced).arg(findText);
    _statusLabel->setText(msg);
    Application::instance().setStatusBarText(msg.toStdString());

    emit replaceAllInAllRequested(findText, replaceText, opts);
}

void FindReplaceDialog::onMarkAll() {
    QString text = _findCombo->currentText();
    if (text.isEmpty() || !_editor) return;

    _lastSearchText = text;
    _lastSearchOptions = currentOptions();

    // Clear previous mark markers
    onUnmarkAll();

    FindOption opts = currentOptions();
    // We need to iterate through all matches and mark their lines.
    // Use a dedicated marker number for "mark all" highlights.
    int markerNum = MARKER_FINDMARK_ALL;

    // Configure marker color on the editor
    QsciScintilla* sci = _editor->qsciEditor();
    // Set marker background color for MARKER_FINDMARK_ALL
    sci->SendScintilla(QsciScintilla::SCI_MARKERDEFINE, markerNum, QsciScintilla::SC_MARK_BACKGROUND);
    sci->SendScintilla(QsciScintilla::SCI_MARKERSETBACK, markerNum,
                       static_cast<long>(QColor(_markColor).rgb()) & 0x00FFFFFF);

    int count = 0;
    
    // Use findFirst loop to walk all matches and mark each line.
    // QsciScintilla::findFirst returns true if found; keep calling
    // findFirst with the same args to advance to next match.
    bool cs = (opts & FindOption::MatchCase) != FindOption::None;
    bool wo = (opts & FindOption::WholeWord) != FindOption::None;
    bool re = (opts & FindOption::Regex) != FindOption::None;

    bool found = sci->findFirst(text, re, cs, wo, true /*forward*/,
                                true /*wrap*/, 0, 0, false, false, false);
    while (found) {
        int line = 0, col = 0;
        sci->getCursorPosition(&line, &col);
        sci->markerAdd(line, markerNum);
        ++count;
        // Advance to next match using findFirst with the same pattern
        found = sci->findFirst(text, re, cs, wo, true, true, 0, 0, false, false, false);
    }

    QString msg = (count == 1)
        ? QString("Marked 1 match of \"%1\"").arg(text)
        : (count == 0 ? QString("No matches of \"%1\"").arg(text)
                       : QString("Marked %1 matches of \"%2\"").arg(count).arg(text));
    _statusLabel->setText(msg);

    emit markRequested(text, opts, _markColor, _markStyle);
}

void FindReplaceDialog::onUnmarkAll() {
    if (!_editor) return;
    QsciScintilla* sci = _editor->qsciEditor();
    sci->markerDeleteAll(MARKER_FINDMARK_ALL);
    for (int i = 11; i <= 15; ++i) {
        sci->markerDeleteAll(i);
    }
    emit unmarkRequested(_findCombo->currentText(), currentOptions());
}

void FindReplaceDialog::onPurge() {
    // Clear search history
    _searchHistory.clear();
    _replaceHistory.clear();
    _findCombo->clear();
    _replaceCombo->clear();

    // Clear all mark markers
    if (_editor) {
        QsciScintilla* sci = _editor->qsciEditor();
        sci->markerDeleteAll(MARKER_FINDMARK_ALL);
        for (int i = 11; i <= 15; ++i) {
            sci->markerDeleteAll(i);
        }
    }

    // Clear highlights
    clearHighlight();

    emit purgeRequested(_findCombo->currentText(), currentOptions());
}

void FindReplaceDialog::onBookmark() {
    if (!_editor) return;
    int line = _editor->currentLine();
    _editor->toggleBookmark(line);
    _statusLabel->setText(_editor->bookmarks().contains(line)
        ? QString("Bookmark added on line %1").arg(line + 1)
        : QString("Bookmark removed from line %1").arg(line + 1));
}

void FindReplaceDialog::onSelectAndFind() {
    if (!_editor) return;
    QString selected = _editor->selectedText();
    if (selected.isEmpty()) {
        // Nothing selected - nothing to do
        return;
    }
    // Put selected text into find box and trigger search
    {
        QSignalBlocker blocker(_findCombo);
        _findCombo->setEditText(selected);
    }
    onSearchTextChanged(selected);
    // Also highlight the selection for the find
    saveSearchToHistory(selected);
    onFindNext();
}

void FindReplaceDialog::onFindInFiles() {
    QString text = _findCombo->currentText();
    emit openFindInFilesRequested(text);
}

void FindReplaceDialog::onReplaceInFiles() {
    QString findText = _findCombo->currentText();
    QString replaceText = _replaceCombo->currentText();
    emit openReplaceInFilesRequested(findText, replaceText);
}

void FindReplaceDialog::onReplaceTextChanged(const QString& text) {
    bool hasText = !text.isEmpty();
    if (_replaceBtn) _replaceBtn->setEnabled(hasText && _editor);
    if (_replaceAllBtn) _replaceAllBtn->setEnabled(hasText && _editor);
    if (_replaceAllInAllBtn) _replaceAllInAllBtn->setEnabled(hasText && _editor);
}

void FindReplaceDialog::onModeChanged() {
    // Sync UI state based on current search mode (normal / regex / extended).
    // This is called when regex, whole-word, or case options change.
    // Could update the status bar or dialog appearance.
    FindOption opts = currentOptions();
    if ((opts & FindOption::Regex) != FindOption::None) {
        _statusLabel->setText("  Regex mode");
    } else {
        _statusLabel->clear();
    }
}

void FindReplaceDialog::onReplacePrompt(const QString& find, const QString& replace, int count) {
    if (count <= 0) return;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Replace",
        QString("Replace %1 occurrence%2 of \"%3\" with \"%4\"?").arg(count).arg(count == 1 ? "" : "s").arg(find).arg(replace),
        QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if (reply == QMessageBox::Yes) {
        // Replace one at current position
        if (_editor) _editor->replace(replace);
    } else if (reply == QMessageBox::YesToAll) {
        // Replace all without further prompting
        onReplaceAll();
    }
    // Cancel: do nothing
}

void FindReplaceDialog::updateHighlight() {
    // Re-apply visible find-result highlights for the current search term
    if (!_editor) return;
    QString text = _findCombo->currentText();
    if (text.isEmpty()) {
        clearHighlight();
        return;
    }
    onSearchTextChanged(text);
}

void FindReplaceDialog::applyHighlightToggle() {
    // Toggle showing/hiding find highlights based on _highlightingEnabled
    if (!_editor) return;
    if (_highlightingEnabled) {
        updateHighlight();
    } else {
        clearHighlight();
    }
}

void FindReplaceDialog::saveSearchToHistory(const QString& text) {
    if (text.isEmpty()) return;
    // Remove duplicate if exists (to avoid repeats)
    _searchHistory.removeAll(text);
    // Insert at front
    _searchHistory.prepend(text);
    // Trim to max size
    while (_searchHistory.size() > MAX_HISTORY) {
        _searchHistory.removeLast();
    }
    // Update combo box
    {
        QSignalBlocker blocker(_findCombo);
        _findCombo->removeItem(_findCombo->findText(text));
        _findCombo->insertItem(0, text);
        _findCombo->setCurrentIndex(0);
    }
}

void FindReplaceDialog::saveReplaceToHistory(const QString& text) {
    if (text.isEmpty()) return;
    _replaceHistory.removeAll(text);
    _replaceHistory.prepend(text);
    while (_replaceHistory.size() > MAX_HISTORY) {
        _replaceHistory.removeLast();
    }
    {
        QSignalBlocker blocker(_replaceCombo);
        _replaceCombo->removeItem(_replaceCombo->findText(text));
        _replaceCombo->insertItem(0, text);
        _replaceCombo->setCurrentIndex(0);
    }
}

void FindReplaceDialog::loadSearchHistory() {
    // Load search history from settings and populate _findCombo
    QSettings settings;
    settings.beginGroup("FindReplaceDialog");
    QStringList history = settings.value("searchHistory").toStringList();
    settings.endGroup();

    if (!history.isEmpty()) {
        _searchHistory = history;
        _findCombo->clear();
        _findCombo->addItems(history);
    }
}

void FindReplaceDialog::loadReplaceHistory() {
    // Load replace history from settings and populate _replaceCombo
    QSettings settings;
    settings.beginGroup("FindReplaceDialog");
    QStringList history = settings.value("replaceHistory").toStringList();
    settings.endGroup();

    if (!history.isEmpty()) {
        _replaceHistory = history;
        _replaceCombo->clear();
        _replaceCombo->addItems(history);
    }
}

void FindReplaceDialog::updateMarkColor(const QColor& color) {
    _markColor = color;
    // Re-apply mark all with new color if there's an active search
    QString text = _findCombo->currentText();
    if (!text.isEmpty() && _editor) {
        onUnmarkAll();
        onMarkAll();
    }
}

void FindReplaceDialog::scheduleHideForSearch() {
    // Hide the dialog briefly during search so it doesn't obscure results,
    // then show it again after a short delay.
    _wasHidden = !isVisible();
    if (!isVisible()) return;  // already hidden, nothing to do

    hide();
    QTimer::singleShot(100, this, [this]() {
        if (!_wasHidden) {
            show();
            activateWindow();
        }
    });
}

void FindReplaceDialog::keyPressEvent(QKeyEvent* event) {
    // Handle Up/Down for history navigation in combos
    QComboBox* combo = qobject_cast<QComboBox*>(focusWidget());
    if (combo) {
        if (event->key() == Qt::Key_Down && combo->currentIndex() == 0) {
            // Allow normal combo navigation
        }
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            // Enter in find combo: trigger find next
            if (combo == _findCombo) {
                onFindNext();
                event->accept();
                return;
            }
        }
    }

    // Escape closes the dialog
    if (event->key() == Qt::Key_Escape) {
        hide();
        clearHighlight();
        event->accept();
        return;
    }

    // F3 / Shift+F3: find next / previous without mouse
    if (event->key() == Qt::Key_F3) {
        if (event->modifiers() & Qt::ShiftModifier) {
            onFindPrevious();
        } else {
            onFindNext();
        }
        event->accept();
        return;
    }

    event->ignore();
}

void FindReplaceDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    // Populate history combos on show
    loadSearchHistory();
    loadReplaceHistory();
    // Set focus to find field
    if (_findCombo) {
        _findCombo->setFocus();
        _findCombo->lineEdit()->selectAll();
    }
    // Update button states
    onReplaceTextChanged(_replaceCombo->currentText());
}

// === Missing public/slot implementations ===

void FindReplaceDialog::countAll() {
    QString text = _findCombo->currentText();
    if (text.isEmpty() || !_editor) return;
    FindOption opts = currentOptions();
    int count = _editor->countMatches(text, opts);
    setMatchCount(count);
}

void FindReplaceDialog::markAll() {
    onMarkAll();
}

void FindReplaceDialog::unmarkAll() {
    onUnmarkAll();
}

void FindReplaceDialog::purgeMarkedLines() {
    onPurge();
}

void FindReplaceDialog::bookmarkAllMatches() {
    onBookmark();
}

void FindReplaceDialog::selectAndFind() {
    onSelectAndFind();
}

void FindReplaceDialog::findNextInSelection() {
    if (!_editor) return;
    QString text = _findCombo->currentText();
    if (text.isEmpty()) return;
    int selStart = _editor->send(SCI_GETSELECTIONNSTART, 0);
    int selEnd = _editor->send(SCI_GETSELECTIONNEND, 0);
    if (selStart == selEnd) return;
    _selectionAnchor = selStart;
    _selectionCaret = selEnd;
    FindOption opts = currentOptions();
    _editor->findNext(text, opts);
}

void FindReplaceDialog::replaceInAllOpenDocuments() {
    onReplaceAllInAll();
}

void FindReplaceDialog::setReplaceText(const QString& text) {
    if (!_replaceCombo) return;
    QSignalBlocker blocker(_replaceCombo);
    _replaceCombo->setEditText(text);
}

void FindReplaceDialog::setOptions(FindOption opts) {
    if (_matchCaseCheck) _matchCaseCheck->setChecked((opts & FindOption::MatchCase) != FindOption::None);
    if (_wholeWordCheck) _wholeWordCheck->setChecked((opts & FindOption::WholeWord) != FindOption::None);
    if (_wrapCheck) _wrapCheck->setChecked((opts & FindOption::WrapAround) != FindOption::None);
    if (_regexCheck) _regexCheck->setChecked((opts & FindOption::Regex) != FindOption::None);
}

void FindReplaceDialog::setCurrentMatch(int current, int total) {
    if (_matchLabel) {
        if (total > 0)
            _matchLabel->setText(QString("  %1 of %2").arg(current).arg(total));
        else
            _matchLabel->clear();
    }
}

void FindReplaceDialog::openFindInFiles() {
    onFindInFiles();
}

void FindReplaceDialog::openReplaceInFiles() {
    onReplaceInFiles();
}

void FindReplaceDialog::showMinimizedOrFocus() {
    if (!isVisible() || isMinimized()) {
        show();
        activateWindow();
        raise();
    } else {
        activateWindow();
        raise();
    }
    if (_findCombo) {
        _findCombo->setFocus();
        _findCombo->lineEdit()->selectAll();
    }
}

bool FindReplaceDialog::isRegex() const {
    return _regexCheck && _regexCheck->isChecked();
}

bool FindReplaceDialog::isWrapAround() const {
    return _wrapCheck && _wrapCheck->isChecked();
}

bool FindReplaceDialog::isPromptsOnReplace() const {
    return _promptsCheck && _promptsCheck->isChecked();
}

void FindReplaceDialog::syncOptionsFromUI() {
    // currentOptions() reads UI directly; this method exists for external callers.
}

int FindReplaceDialog::performFindNext(const QString& text, FindOption opts) {
    if (!_editor) return -1;
    // Store options and delegate to slot which returns bool via findNext.
    // Since findNext is void, we return 1 to indicate the search was attempted.
    Q_UNUSED(text);
    _lastSearchText = text;
    _lastSearchOptions = opts;
    _editor->findNext(text, opts);
    return 1;
}

int FindReplaceDialog::performFindPrevious(const QString& text, FindOption opts) {
    if (!_editor) return -1;
    _lastSearchText = text;
    _lastSearchOptions = opts;
    _editor->findPrevious(text, opts);
    return 1;
}

int FindReplaceDialog::performCount(const QString& text, FindOption opts) {
    if (!_editor) return 0;
    return _editor->countMatches(text, opts);
}

void FindReplaceDialog::setupShortcuts() {
    // Shortcut registration is handled at MainWindow level via ShortcutManager.
}

// Static const definitions
const QString FindReplaceDialog::SETTINGS_GROUP = QStringLiteral("FindReplaceDialog");
const QString FindReplaceDialog::SETTINGS_RECENT_FINDS = QStringLiteral("recentFinds");
const QString FindReplaceDialog::SETTINGS_RECENT_REPLACES = QStringLiteral("recentReplaces");
const QString FindReplaceDialog::SETTINGS_MATCH_CASE = QStringLiteral("matchCase");
const QString FindReplaceDialog::SETTINGS_WHOLE_WORD = QStringLiteral("wholeWord");
const QString FindReplaceDialog::SETTINGS_WRAP = QStringLiteral("wrapAround");
const QString FindReplaceDialog::SETTINGS_REGEX = QStringLiteral("regex");
const QString FindReplaceDialog::SETTINGS_HIGHLIGHT = QStringLiteral("highlighting");
const QString FindReplaceDialog::SETTINGS_HIDE_DIALOG = QStringLiteral("hideDialog");
const QString FindReplaceDialog::SETTINGS_PROMPTS = QStringLiteral("promptsOnReplace");
const QString FindReplaceDialog::SETTINGS_MARK_COLOR = QStringLiteral("markColor");
const QString FindReplaceDialog::SETTINGS_DIRECTION = QStringLiteral("direction");
const QString FindReplaceDialog::SETTINGS_GEOMETRY = QStringLiteral("geometry");

