// CommandPaletteDialog.cpp - Quick command palette (Ctrl+Shift+P)
// Copyright (C) 2026 Agent Army | GPL v3

#include "CommandPaletteDialog.h"
#include "core/Application.h"
#include "core/EditorCommandManager.h"
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QWindow>
#include <QLabel>
#include <QTimer>
#include <algorithm>
#include <cctype>

CommandPaletteDialog::CommandPaletteDialog(QWidget* parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::Dialog)
{
    setModal(false);
    setSizeGripEnabled(false);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->setContentsMargins(0, 0, 0, 0);
    main->setSpacing(0);

    // Dark background frame
    QFrame* frame = new QFrame(this);
    frame->setObjectName("paletteFrame");
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    frame->setStyleSheet(R"(
        #paletteFrame {
            background: #1e1e1e;
            border: 1px solid #555;
            border-radius: 4px;
        }
        QLabel { color: #d4d4d4; }
        QLineEdit {
            background: #2d2d2d;
            color: #d4d4d4;
            border: none;
            border-bottom: 1px solid #444;
            padding: 8px 12px;
            font-size: 14px;
        }
        QLineEdit:focus { border-bottom: 1px solid #007acc; }
        QListWidget {
            background: #1e1e1e;
            color: #d4d4d4;
            border: none;
            outline: none;
            font-size: 13px;
        }
        QListWidget::item {
            padding: 4px 12px;
            border-radius: 2px;
        }
        QListWidget::item:selected,
        QListWidget::item:hover {
            background: #094771;
        }
    )");
    main->addWidget(frame);

    QVBoxLayout* inner = new QVBoxLayout(frame);

    // Search input
    _searchBox = new QLineEdit(frame);
    _searchBox->setPlaceholderText("Type a command name...");
    inner->addWidget(_searchBox);

    // Command list
    _commandList = new QListWidget(frame);
    _commandList->setMaximumHeight(300);
    _commandList->setSpacing(2);
    inner->addWidget(_commandList);

    // Build command registry — try to use registered commands from EditorCommandManager,
    // falling back to the static list below
    buildCommandRegistry();

    populateAll();

    connect(_searchBox, &QLineEdit::textChanged, this, &CommandPaletteDialog::onTextChanged);
    connect(_searchBox, &QLineEdit::returnPressed, this, [this]() {
        if (_commandList->currentItem()) {
            onItemActivated(_commandList->currentItem());
        } else if (_commandList->count() > 0) {
            // Activate the first item on Enter with no selection
            onItemActivated(_commandList->item(0));
        }
    });
    connect(_commandList, &QListWidget::itemActivated, this, &CommandPaletteDialog::onItemActivated);

    // Arrow key navigation
    _commandList->setFocusPolicy(Qt::NoFocus);

    // Focus search box
    QTimer::singleShot(0, this, [this]() {
        _searchBox->setFocus();
    });
}

CommandPaletteDialog::~CommandPaletteDialog() = default;

void CommandPaletteDialog::buildCommandRegistry() {
    // Try to load commands from the EditorCommandManager
    QVector<EditorCommandManager::CommandEntry> registeredCmds =
        app().commandManager()->getAllCommands();

    if (!registeredCmds.isEmpty()) {
        for (const auto& cmd : registeredCmds) {
            QString name = QString::fromStdString(cmd.name);
            // Skip internal commands that shouldn't appear in the palette
            if (name.startsWith("_") || name.isEmpty()) continue;
            _allCommands.append({name, QString::number(cmd.id)});
        }
        // Sort alphabetically
        std::sort(_allCommands.begin(), _allCommands.end(),
                  [](const QPair<QString, QString>& a, const QPair<QString, QString>& b) {
                      return a.first.localeAwareCompare(b.first) < 0;
                  });
        return;
    }

    // Fall back to the static list
    _allCommands = {
        // File
        {"New File", "1001"},
        {"Open File...", "1002"},
        {"Save", "1003"},
        {"Save As...", "1004"},
        {"Save All", "1005"},
        {"Close", "1006"},
        {"Close All", "1007"},
        {"Print...", "1008"},
        {"Exit", "1009"},
        // Edit
        {"Undo", "1101"},
        {"Redo", "1102"},
        {"Cut", "1103"},
        {"Copy", "1104"},
        {"Paste", "1105"},
        {"Select All", "1106"},
        {"Find...", "1107"},
        {"Replace...", "1108"},
        {"Go to Line...", "1109"},
        {"Delete Line", "1110"},
        {"Column Editor", "1111"},
        // Search
        {"Find Next", "1201"},
        {"Find Previous", "1202"},
        {"Find in Files...", "1203"},
        {"Find Incremental...", "1204"},
        {"Count Occurrences", "1205"},
        {"Mark All", "1206"},
        {"Unmark All", "1207"},
        // View
        {"Zoom In", "1301"},
        {"Zoom Out", "1302"},
        {"Zoom Reset", "1303"},
        {"Word Wrap", "1304"},
        {"Toggle Tab Bar", "1305"},
        {"Toggle Full Screen", "1306"},
        {"Toggle Distraction-Free", "1307"},
        {"Document Map", "1308"},
        {"Function List", "1309"},
        {"File Browser", "1310"},
        // Encoding
        {"Convert to UTF-8", "1401"},
        {"Convert to UTF-8 BOM", "1402"},
        {"Convert to UTF-16 LE", "1403"},
        {"Convert to UTF-16 BE", "1404"},
        {"Convert to ANSI", "1405"},
        // Language
        {"Normal Text", "1501"},
        {"Fold Current", "1502"},
        {"Unfold Current", "1503"},
        {"Fold All", "1504"},
        {"Unfold All", "1505"},
        // Settings
        {"Preferences...", "1601"},
        {"Shortcut Mapper...", "1602"},
        {"Command Palette", "1603"},
        {"Toggle Toolbar", "1604"},
        {"Toggle Status Bar", "1605"},
        // Run
        {"Launch", "1701"},
        {"Run in Browser", "1702"},
        {"Validate", "1703"},
        // Help
        {"About Notepad--", "1801"},
        {"Documentation", "1802"},
        {"Check for Updates", "1803"},
    };
}

void CommandPaletteDialog::populateAll() {
    _commandList->clear();
    for (const auto& cmd : _allCommands) {
        auto* item = new QListWidgetItem(cmd.first, _commandList);
        item->setData(Qt::UserRole, cmd.second);
    }
    if (_commandList->count() > 0) {
        _commandList->setCurrentRow(0);
    }
}

// ---------------------------------------------------------------------------
// Fuzzy matching
// ---------------------------------------------------------------------------

// Score a fuzzy match between pattern (lowercase) and text (lowercase).
// Returns -1 if no match. Higher score = better match.
// Scoring criteria:
//   - Consecutive character matches get bonus
//   - Match at start of word gets bonus
//   - Match at start of string gets bonus
//   - Separator characters (., -, _, space) allow word-boundary bonus
static int fuzzyScore(const QString& pattern, const QString& text) {
    if (pattern.isEmpty()) return 0;

    int score = 0;
    int pi = 0;
    int textLen = text.length();
    bool prevMatched = false;
    bool prevSeparator = true; // start of string counts as separator

    for (int ti = 0; ti < textLen && pi < pattern.length(); ++ti) {
        if (text[ti].toLower() == pattern[pi]) {
            // Bonus for matching at start of string
            if (ti == 0) {
                score += 15;
            }
            // Bonus for matching at start of a word (after separator)
            else if (prevSeparator) {
                score += 10;
            }
            // Bonus for consecutive matches
            else if (prevMatched) {
                score += 5;
            }
            // Base score for any match
            score += 1;

            prevMatched = true;
            ++pi;
        } else {
            prevMatched = false;
        }

        // Check if current char is a word separator
        QChar ch = text[ti];
        prevSeparator = !ch.isLetterOrNumber() && ch != '_';
    }

    // All pattern chars must be matched
    if (pi < pattern.length()) return -1;
    return score;
}

void CommandPaletteDialog::filterCommands(const QString& text) {
    _commandList->clear();

    if (text.isEmpty()) {
        populateAll();
        return;
    }

    QString lower = text.toLower();

    // Build list of (score, command) pairs
    QVector<QPair<int, QPair<QString, QString>>> scored;

    for (const auto& cmd : _allCommands) {
        QString nameLower = cmd.first.toLower();

        // Exact substring match gets a high base score
        if (nameLower.contains(lower)) {
            int score = fuzzyScore(lower, nameLower);
            if (score < 0) score = 0;
            // Bonus for substring match at start
            if (nameLower.startsWith(lower)) {
                score += 50;
            }
            // Bonus for exact match
            if (nameLower == lower) {
                score = 1000;
            }
            scored.append({score, cmd});
        } else {
            // Fuzzy match
            int score = fuzzyScore(lower, nameLower);
            if (score > 0) {
                scored.append({score, cmd});
            }
        }
    }

    // Sort by score descending
    std::sort(scored.begin(), scored.end(),
              [](const QPair<int, QPair<QString, QString>>& a,
                 const QPair<int, QPair<QString, QString>>& b) {
                  return a.first > b.first;
              });

    for (const auto& entry : scored) {
        auto* item = new QListWidgetItem(entry.second.first, _commandList);
        item->setData(Qt::UserRole, entry.second.second);
    }

    if (_commandList->count() > 0) {
        _commandList->setCurrentRow(0);
    }
}

void CommandPaletteDialog::onTextChanged(const QString& text) {
    filterCommands(text);
}

void CommandPaletteDialog::onItemActivated(QListWidgetItem* item) {
    if (!item) return;
    _selectedCommand = item->data(Qt::UserRole).toString();
    hide();
    executeCommand(_selectedCommand);
}

void CommandPaletteDialog::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hide();
        event->accept();
        return;
    }

    // Navigate list with arrow keys, but let search box handle typing
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_PageDown) {
        if (!_searchBox->hasFocus()) {
            _commandList->setFocus();
        }
        int row = _commandList->currentRow();
        int count = _commandList->count();
        if (event->key() == Qt::Key_Down) {
            _commandList->setCurrentRow((row + 1) % count);
        } else {
            _commandList->setCurrentRow(qMin(row + 10, count - 1));
        }
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_PageUp) {
        int row = _commandList->currentRow();
        int count = _commandList->count();
        if (event->key() == Qt::Key_Up) {
            _commandList->setCurrentRow(row > 0 ? row - 1 : count - 1);
        } else {
            _commandList->setCurrentRow(qMax(row - 10, 0));
        }
        event->accept();
        return;
    }

    // Let Tab complete the current selection (common in VS Code style)
    if (event->key() == Qt::Key_Tab) {
        if (_commandList->currentItem()) {
            onItemActivated(_commandList->currentItem());
        }
        event->accept();
        return;
    }

    QDialog::keyPressEvent(event);
}

QString CommandPaletteDialog::executeCommand(const QString& cmd) {
    // cmd is the command ID (string number) or command name
    bool ok = false;
    int cmdId = cmd.toInt(&ok);
    if (ok) {
        app().commandManager()->execute(cmdId);
    } else {
        app().onMenuCommand(cmd);
    }
    return cmd;
}
