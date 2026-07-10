// CommandPaletteDialog.cpp - Quick command palette (Ctrl+Shift+P)
// Copyright (C) 2026 Agent Army | GPL v3

#include "CommandPaletteDialog.h"
#include "core/Application.h"
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QWindow>
#include <QLabel>

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

    // Build command registry
    _allCommands = {
        {"New File", "file.new"},
        {"Open File...", "file.open"},
        {"Save", "file.save"},
        {"Save As...", "file.saveAs"},
        {"Save All", "file.saveAll"},
        {"Close", "file.close"},
        {"Close All", "file.closeAll"},
        {"Print...", "file.print"},
        {"Exit", "file.exit"},
        {"Undo", "edit.undo"},
        {"Redo", "edit.redo"},
        {"Cut", "edit.cut"},
        {"Copy", "edit.copy"},
        {"Paste", "edit.paste"},
        {"Select All", "edit.selectAll"},
        {"Find...", "search.find"},
        {"Replace...", "search.replace"},
        {"Find Next", "search.findNext"},
        {"Find Previous", "search.findPrev"},
        {"Go to Line...", "search.gotoLine"},
        {"Mark All", "search.markAll"},
        {"Unmark All", "search.unmarkAll"},
        {"Find in Files...", "search.findInFiles"},
        {"Count Occurrences", "search.count"},
        {"Zoom In", "view.zoomIn"},
        {"Zoom Out", "view.zoomOut"},
        {"Zoom Reset", "view.zoomRestore"},
        {"Word Wrap", "view.toggleWordWrap"},
        {"Toggle Tab Bar", "view.toggleTabBar"},
        {"Toggle Full Screen", "view.fullscreen"},
        {"Toggle Distraction-Free", "view.distractionFree"},
        {"Document Map", "view.documentMap"},
        {"Function List", "view.functionList"},
        {"File Browser", "view.fileBrowser"},
        {"Convert to UTF-8", "encoding.utf8"},
        {"Convert to UTF-8 BOM", "encoding.utf8bom"},
        {"Convert to UTF-16 LE", "encoding.utf16le"},
        {"Convert to ANSI", "encoding.ansi"},
        {"Preferences...", "settings.preferences"},
        {"Shortcut Mapper...", "settings.shortcutMapper"},
        {"Command Palette", "settings.commandPalette"},
        {"About Notepad--", "help.about"},
    };

    populateAll();

    connect(_searchBox, &QLineEdit::textChanged, this, &CommandPaletteDialog::onTextChanged);
    connect(_searchBox, &QLineEdit::returnPressed, this, [this]() {
        if (_commandList->currentItem()) {
            onItemActivated(_commandList->currentItem());
        }
    });
    connect(_commandList, &QListWidget::itemActivated, this, &CommandPaletteDialog::onItemActivated);

    // Focus search box
    _searchBox->setFocus();
}

CommandPaletteDialog::~CommandPaletteDialog() = default;

void CommandPaletteDialog::populateAll() {
    _commandList->clear();
    for (const auto& cmd : _allCommands) {
        auto* item = new QListWidgetItem(cmd.first, _commandList);
        item->setData(Qt::UserRole, cmd.second);
    }
}

void CommandPaletteDialog::filterCommands(const QString& text) {
    _commandList->clear();
    QString lower = text.toLower();
    for (const auto& cmd : _allCommands) {
        if (text.isEmpty() || cmd.first.toLower().contains(lower)) {
            auto* item = new QListWidgetItem(cmd.first, _commandList);
            item->setData(Qt::UserRole, cmd.second);
        }
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
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_PageDown) {
        _commandList->setFocus();
        QDialog::keyPressEvent(event);
        return;
    }
    QDialog::keyPressEvent(event);
}

QString CommandPaletteDialog::executeCommand(const QString& cmd) {
    Application::instance().onMenuCommand(cmd);
    return cmd;
}
