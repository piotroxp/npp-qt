// ToolBar.cpp - Main toolbar with full Notepad++ functionality
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ToolBar.h"
#include "DpiManager.h"
#include <QAction>
#include <QIcon>
#include <QStyle>
#include <QApplication>
#include <QSize>
#include <QMenu>
#include <QWidgetAction>
#include <QLineEdit>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QDrag>
#include <QMimeData>
#include <QStyleOptionToolBar>
#include <QStylePainter>

ToolBar::ToolBar(QWidget* parent) : QToolBar(parent) {
    setMovable(true);
    setFloatable(true);
    setIconSize(DefaultIconSize);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    setupActions();
    setupMenus();
    applyDpiScaling();
}

ToolBar::~ToolBar() = default;

void ToolBar::setupActions() {
    // =========================================================================
    // FILE ACTIONS
    // =========================================================================
    QAction* newAction = new QAction(style()->standardIcon(QStyle::SP_FileIcon), "New", this);
    newAction->setData("file.new");
    newAction->setToolTip("New (Ctrl+N)");
    newAction->setShortcut(QKeySequence::New);
    m_actions["file.new"] = newAction;
    addAction(newAction);
    
    QAction* openAction = new QAction(style()->standardIcon(QStyle::SP_DirOpenIcon), "Open", this);
    openAction->setData("file.open");
    openAction->setToolTip("Open (Ctrl+O)");
    openAction->setShortcut(QKeySequence::Open);
    m_actions["file.open"] = openAction;
    addAction(openAction);
    
    QAction* saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save", this);
    saveAction->setData("file.save");
    saveAction->setToolTip("Save (Ctrl+S)");
    saveAction->setShortcut(QKeySequence::Save);
    m_actions["file.save"] = saveAction;
    addAction(saveAction);
    
    QAction* saveAsAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save As", this);
    saveAsAction->setData("file.saveAs");
    saveAsAction->setToolTip("Save As... (Ctrl+Shift+S)");
    saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    m_actions["file.saveAs"] = saveAsAction;
    addAction(saveAsAction);
    
    QAction* closeAction = new QAction(style()->standardIcon(QStyle::SP_TitleBarCloseButton), "Close", this);
    closeAction->setData("file.close");
    closeAction->setToolTip("Close (Ctrl+W)");
    closeAction->setShortcut(QKeySequence("Ctrl+W"));
    m_actions["file.close"] = closeAction;
    addAction(closeAction);
    
    addSeparator();
    
    // =========================================================================
    // EDIT ACTIONS
    // =========================================================================
    QAction* undoAction = new QAction(style()->standardIcon(QStyle::SP_ArrowBack), "Undo", this);
    undoAction->setData("edit.undo");
    undoAction->setToolTip("Undo (Ctrl+Z)");
    undoAction->setShortcut(QKeySequence::Undo);
    m_actions["edit.undo"] = undoAction;
    addAction(undoAction);
    
    QAction* redoAction = new QAction(style()->standardIcon(QStyle::SP_ArrowForward), "Redo", this);
    redoAction->setData("edit.redo");
    redoAction->setToolTip("Redo (Ctrl+Y)");
    redoAction->setShortcut(QKeySequence::Redo);
    m_actions["edit.redo"] = redoAction;
    addAction(redoAction);
    
    addSeparator();
    
    // =========================================================================
    // CLIPBOARD ACTIONS
    // =========================================================================
    QAction* cutAction = new QAction(style()->standardIcon(QStyle::SP_TitleBarMinButton), "Cut", this);
    cutAction->setData("edit.cut");
    cutAction->setToolTip("Cut (Ctrl+X)");
    cutAction->setShortcut(QKeySequence::Cut);
    m_actions["edit.cut"] = cutAction;
    addAction(cutAction);
    
    QAction* copyAction = new QAction(style()->standardIcon(QStyle::SP_TitleBarMaxButton), "Copy", this);
    copyAction->setData("edit.copy");
    copyAction->setToolTip("Copy (Ctrl+C)");
    copyAction->setShortcut(QKeySequence::Copy);
    m_actions["edit.copy"] = copyAction;
    addAction(copyAction);
    
    QAction* pasteAction = new QAction(style()->standardIcon(QStyle::SP_TitleBarNormalButton), "Paste", this);
    pasteAction->setData("edit.paste");
    pasteAction->setToolTip("Paste (Ctrl+V)");
    pasteAction->setShortcut(QKeySequence::Paste);
    m_actions["edit.paste"] = pasteAction;
    addAction(pasteAction);
    
    addSeparator();
    
    // =========================================================================
    // SEARCH ACTIONS
    // =========================================================================
    QAction* findAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogContentsView), "Find", this);
    findAction->setData("edit.find");
    findAction->setToolTip("Find (Ctrl+F)");
    findAction->setShortcut(QKeySequence::Find);
    m_actions["edit.find"] = findAction;
    addAction(findAction);
    
    QAction* replaceAction = new QAction(style()->standardIcon(QStyle::SP_DesktopIcon), "Replace", this);
    replaceAction->setData("edit.replace");
    replaceAction->setToolTip("Replace (Ctrl+H)");
    replaceAction->setShortcut(QKeySequence("Ctrl+H"));
    m_actions["edit.replace"] = replaceAction;
    addAction(replaceAction);
    
    QAction* gotoAction = new QAction(style()->standardIcon(QStyle::SP_CommandLink), "Go to Line", this);
    gotoAction->setData("edit.goto");
    gotoAction->setToolTip("Go to Line (Ctrl+G)");
    gotoAction->setShortcut(QKeySequence("Ctrl+G"));
    m_actions["edit.goto"] = gotoAction;
    addAction(gotoAction);
    
    addSeparator();
    
    // =========================================================================
    // COMMENT ACTIONS
    // =========================================================================
    QAction* commentAction = new QAction(style()->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton), "Comment", this);
    commentAction->setData("edit.toggleComment");
    commentAction->setToolTip("Toggle Comment (Ctrl+Q)");
    commentAction->setShortcut(QKeySequence("Ctrl+Q"));
    m_actions["edit.toggleComment"] = commentAction;
    addAction(commentAction);
    
    QAction* blockCommentAction = new QAction(style()->standardIcon(QStyle::SP_ToolBarVerticalExtensionButton), "Block Comment", this);
    blockCommentAction->setData("edit.blockComment");
    blockCommentAction->setToolTip("Block Comment (Ctrl+Shift+Q)");
    blockCommentAction->setShortcut(QKeySequence("Ctrl+Shift+Q"));
    m_actions["edit.blockComment"] = blockCommentAction;
    addAction(blockCommentAction);
    
    addSeparator();
    
    // =========================================================================
    // INDENT ACTIONS
    // =========================================================================
    QAction* indentAction = new QAction(style()->standardIcon(QStyle::SP_ArrowRight), "Indent", this);
    indentAction->setData("edit.indent");
    indentAction->setToolTip("Indent (Tab)");
    indentAction->setShortcut(QKeySequence::MoveToNextWord);
    m_actions["edit.indent"] = indentAction;
    addAction(indentAction);
    
    QAction* outdentAction = new QAction(style()->standardIcon(QStyle::SP_ArrowLeft), "Outdent", this);
    outdentAction->setData("edit.outdent");
    outdentAction->setToolTip("Outdent (Shift+Tab)");
    outdentAction->setShortcut(QKeySequence::MoveToPreviousWord);
    m_actions["edit.outdent"] = outdentAction;
    addAction(outdentAction);
    
    addSeparator();
    
    // =========================================================================
    // MACRO ACTIONS
    // =========================================================================
    QAction* recordMacroAction = new QAction(QIcon::fromTheme("media-record", QIcon(":/icons/media-record")), "Record Macro", this);
    recordMacroAction->setData("macro.record");
    recordMacroAction->setToolTip("Record Macro (Ctrl+Shift+R)");
    recordMacroAction->setShortcut(QKeySequence("Ctrl+Shift+R"));
    m_actions["macro.record"] = recordMacroAction;
    addAction(recordMacroAction);
    
    QAction* stopMacroAction = new QAction(QIcon::fromTheme("media-stop", QIcon(":/icons/media-stop")), "Stop Macro", this);
    stopMacroAction->setData("macro.stop");
    stopMacroAction->setToolTip("Stop Recording Macro (Ctrl+Shift+R)");
    stopMacroAction->setEnabled(false);
    m_actions["macro.stop"] = stopMacroAction;
    addAction(stopMacroAction);
    
    QAction* playMacroAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), "Play Macro", this);
    playMacroAction->setData("macro.play");
    playMacroAction->setToolTip("Play Macro (Ctrl+Shift+P)");
    playMacroAction->setShortcut(QKeySequence("Ctrl+Shift+P"));
    m_actions["macro.play"] = playMacroAction;
    addAction(playMacroAction);
    
    addSeparator();
    
    // =========================================================================
    // VIEW/TOGGLE ACTIONS
    // =========================================================================
    QAction* wordWrapAction = new QAction(style()->standardIcon(QStyle::SP_TitleBarShadeButton), "Word Wrap", this);
    wordWrapAction->setData("view.wordWrap");
    wordWrapAction->setToolTip("Toggle Word Wrap");
    wordWrapAction->setCheckable(true);
    m_actions["view.wordWrap"] = wordWrapAction;
    addAction(wordWrapAction);
    
    QAction* showAllCharsAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogInfoView), "Show All Characters", this);
    showAllCharsAction->setData("view.showAllChars");
    showAllCharsAction->setToolTip("Show All Characters (显示所有字符)");
    showAllCharsAction->setCheckable(true);
    m_actions["view.showAllChars"] = showAllCharsAction;
    addAction(showAllCharsAction);
    
    QAction* readOnlyAction = new QAction(QIcon::fromTheme("lock-locked", QIcon(":/icons/lock-locked")), "Read Only", this);
    readOnlyAction->setData("view.readOnly");
    readOnlyAction->setToolTip("Toggle Read Only Mode");
    readOnlyAction->setCheckable(true);
    m_actions["view.readOnly"] = readOnlyAction;
    addAction(readOnlyAction);
    
    addSeparator();
    
    // =========================================================================
    // ZOOM ACTIONS
    // =========================================================================
    QAction* zoomInAction = new QAction(style()->standardIcon(QStyle::SP_ArrowUp), "Zoom In", this);
    zoomInAction->setData("view.zoomIn");
    zoomInAction->setToolTip("Zoom In (Ctrl+Keypad+)");
    zoomInAction->setShortcut(QKeySequence("Ctrl+KeypadPlus"));
    m_actions["view.zoomIn"] = zoomInAction;
    addAction(zoomInAction);
    
    QAction* zoomOutAction = new QAction(style()->standardIcon(QStyle::SP_ArrowDown), "Zoom Out", this);
    zoomOutAction->setData("view.zoomOut");
    zoomOutAction->setToolTip("Zoom Out (Ctrl+Keypad-)");
    zoomOutAction->setShortcut(QKeySequence("Ctrl+KeypadMinus"));
    m_actions["view.zoomOut"] = zoomOutAction;
    addAction(zoomOutAction);
    
    QAction* zoomResetAction = new QAction(style()->standardIcon(QStyle::SP_BrowserReload), "Zoom Reset", this);
    zoomResetAction->setData("view.zoomReset");
    zoomResetAction->setToolTip("Zoom Reset (Ctrl+Keypad/)");
    zoomResetAction->setShortcut(QKeySequence("Ctrl+KeypadDivide"));
    m_actions["view.zoomReset"] = zoomResetAction;
    addAction(zoomResetAction);
    
    addSeparator();
    
    // =========================================================================
    // RUN ACTIONS
    // =========================================================================
    QAction* runAction = new QAction(style()->standardIcon(QStyle::SP_CommandLink), "Run", this);
    runAction->setData("tools.run");
    runAction->setToolTip("Run (F5)");
    runAction->setShortcut(QKeySequence("F5"));
    m_actions["tools.run"] = runAction;
    addAction(runAction);
    
    QAction* printAction = new QAction(QIcon::fromTheme("document-print", QIcon(":/icons/printer")), "Print", this);
    printAction->setData("tools.print");
    printAction->setToolTip("Print (Ctrl+P)");
    printAction->setShortcut(QKeySequence("Ctrl+P"));
    m_actions["tools.print"] = printAction;
    addAction(printAction);
    
    addSeparator();
    
    // =========================================================================
    // PANEL ACTIONS
    // =========================================================================
    QAction* docMapAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogListView), "Document Map", this);
    docMapAction->setData("view.docMap");
    docMapAction->setToolTip("Document Map");
    docMapAction->setCheckable(true);
    docMapAction->setChecked(true);
    m_actions["view.docMap"] = docMapAction;
    addAction(docMapAction);
    
    QAction* funcListAction = new QAction(style()->standardIcon(QStyle::SP_FileDialogDetailedView), "Function List", this);
    funcListAction->setData("view.funcList");
    funcListAction->setToolTip("Function List");
    funcListAction->setCheckable(true);
    funcListAction->setChecked(true);
    m_actions["view.funcList"] = funcListAction;
    addAction(funcListAction);
    
    QAction* fileBrowserAction = new QAction(style()->standardIcon(QStyle::SP_DirIcon), "File Browser", this);
    fileBrowserAction->setData("view.fileBrowser");
    fileBrowserAction->setToolTip("File Browser");
    fileBrowserAction->setCheckable(true);
    fileBrowserAction->setChecked(true);
    m_actions["view.fileBrowser"] = fileBrowserAction;
    addAction(fileBrowserAction);
    
    // =========================================================================
    // CONNECT ALL ACTIONS
    // =========================================================================
    for (QAction* action : actions()) {
        if (!action->data().isNull()) {
            connect(action, &QAction::triggered, this, &ToolBar::onActionTriggered);
        }
    }
}

void ToolBar::setupMenus() {
    createEncodingMenu();
    createEolMenu();
    createLanguageMenu();
}

void ToolBar::createEncodingMenu() {
    m_encodingMenu = new QMenu(this);
    
    // Character Sets submenu
    QMenu* charSetsMenu = m_encodingMenu->addMenu("Character Sets");
    
    QStringList encodings = {
        "Western European (ISO-8859-1)",
        "Central European (ISO-8859-2)",
        "Cyrillic (ISO-8859-5)",
        "Greek (ISO-8859-7)",
        "Turkish (ISO-8859-9)",
        "Hebrew (ISO-8859-8)",
        "Arabic (ISO-8859-6)",
        "Baltic (ISO-8859-13)",
        "Greek (Windows-1253)",
        "Cyrillic (Windows-1251)",
        "Western European (Windows-1252)",
        "Turkish (Windows-1254)"
    };
    for (const QString& enc : encodings) {
        charSetsMenu->addAction(enc)->setData("encoding." + enc);
    }
    
    m_encodingMenu->addSeparator();
    
    m_encodingMenu->addAction("UTF-8")->setData("encoding.utf8");
    m_encodingMenu->addAction("UTF-8 BOM")->setData("encoding.utf8bom");
    m_encodingMenu->addAction("UTF-16 LE")->setData("encoding.utf16le");
    m_encodingMenu->addAction("UTF-16 BE")->setData("encoding.utf16be");
    m_encodingMenu->addAction("ANSI")->setData("encoding.ansi");
}

void ToolBar::createEolMenu() {
    m_eolMenu = new QMenu(this);
    
    m_eolMenu->addAction("Windows (CRLF)")->setData("eol.windows");
    m_eolMenu->addAction("Unix (LF)")->setData("eol.unix");
    m_eolMenu->addAction("Mac (CR)")->setData("eol.mac");
}

void ToolBar::createLanguageMenu() {
    m_languageMenu = new QMenu(this);
    
    QStringList languages = {
        "Normal Text", "C", "C++", "C#", "Java", "JavaScript",
        "HTML", "XML", "XAML", "PHP", "Python", "Ruby",
        "Perl", "Lua", "CSS", "JSON", "YAML", "Makefile",
        "Batch", "INI", "Markdown", "SQL", "Bash"
    };
    
    for (const QString& lang : languages) {
        m_languageMenu->addAction(lang)->setData("language." + lang.toLower().replace(' ', '_'));
    }
}

void ToolBar::applyDpiScaling() {
    DpiManager& dpi = DpiManager::instance();
    
    if (dpi.scaleFactor() > 1.0) {
        int scaledSize = dpi.scale(DefaultIconSize);
        setIconSize(scaledSize);
    }
}

void ToolBar::setIconSize(int size) {
    m_iconSize = size;
    QToolBar::setIconSize(QSize(size, size));
}

void ToolBar::setIconSizeSmall() {
    setIconSize(SmallIconSize);
}

void ToolBar::setIconSizeLarge() {
    setIconSize(LargeIconSize);
}

void ToolBar::setToolBarStyle(ToolBarStyle style) {
    m_style = style;
    
    switch (style) {
        case IconOnly:
            setToolButtonStyle(Qt::ToolButtonIconOnly);
            break;
        case TextOnly:
            setToolButtonStyle(Qt::ToolButtonTextOnly);
            break;
        case TextBesideIcon:
            setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            break;
        case TextBelowIcon:
            setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            break;
    }
    
    emit toolBarStyleChanged(style);
}

QAction* ToolBar::actionForCommand(const QString& command) const {
    return m_actions.value(command, nullptr);
}

void ToolBar::setActionEnabled(const QString& command, bool enabled) {
    if (QAction* action = m_actions.value(command)) {
        action->setEnabled(enabled);
    }
}

void ToolBar::updateActionStates(const QMap<QString, bool>& states) {
    for (auto it = states.constBegin(); it != states.constEnd(); ++it) {
        setActionEnabled(it.key(), it.value());
    }
}

void ToolBar::zoomIn() {
    updateZoomLevel(1);
}

void ToolBar::zoomOut() {
    updateZoomLevel(-1);
}

void ToolBar::zoomReset() {
    m_zoomLevel = 0;
    emit zoomChanged(m_zoomLevel);
    emit toolBarCommand("view.zoomReset");
}

void ToolBar::updateZoomLevel(int delta) {
    m_zoomLevel = qBound(-5, m_zoomLevel + delta, 20);
    emit zoomChanged(m_zoomLevel);
    
    if (delta > 0) {
        emit toolBarCommand("view.zoomIn");
    } else if (delta < 0) {
        emit toolBarCommand("view.zoomOut");
    }
}

void ToolBar::setDragHandleVisible(bool visible) {
    m_dragHandleVisible = visible;
    update();
}

void ToolBar::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
    
    // Style options
    QMenu* styleMenu = menu.addMenu("Style");
    QAction* iconOnlyAction = styleMenu->addAction("Icons Only");
    iconOnlyAction->setCheckable(true);
    iconOnlyAction->setChecked(m_style == IconOnly);
    
    QAction* textOnlyAction = styleMenu->addAction("Text Only");
    textOnlyAction->setCheckable(true);
    textOnlyAction->setChecked(m_style == TextOnly);
    
    QAction* textBesideAction = styleMenu->addAction("Text Beside Icon");
    textBesideAction->setCheckable(true);
    textBesideAction->setChecked(m_style == TextBesideIcon);
    
    QAction* textBelowAction = styleMenu->addAction("Text Below Icon");
    textBelowAction->setCheckable(true);
    textBelowAction->setChecked(m_style == TextBelowIcon);
    
    menu.addSeparator();
    
    QMenu* sizeMenu = menu.addMenu("Icon Size");
    QAction* smallAction = sizeMenu->addAction("Small");
    smallAction->setCheckable(true);
    smallAction->setChecked(m_iconSize == SmallIconSize);
    
    QAction* largeAction = sizeMenu->addAction("Large");
    largeAction->setCheckable(true);
    largeAction->setChecked(m_iconSize == LargeIconSize);
    
    menu.addSeparator();
    
    QAction* customizeAction = menu.addAction("Customize...");
    
    menu.addSeparator();
    
    QAction* lockAction = menu.addAction(isMovable() ? "Lock Toolbar" : "Unlock Toolbar");
    lockAction->setCheckable(true);
    lockAction->setChecked(!isMovable());
    
    QAction* action = menu.exec(event->globalPos());
    
    if (action == iconOnlyAction) {
        setToolBarStyle(IconOnly);
    } else if (action == textOnlyAction) {
        setToolBarStyle(TextOnly);
    } else if (action == textBesideAction) {
        setToolBarStyle(TextBesideIcon);
    } else if (action == textBelowAction) {
        setToolBarStyle(TextBelowIcon);
    } else if (action == smallAction) {
        setIconSizeSmall();
    } else if (action == largeAction) {
        setIconSizeLarge();
    } else if (action == customizeAction) {
        showCustomizationDialog();
    } else if (action == lockAction) {
        setMovable(!isMovable());
    }
}

void ToolBar::showCustomizationDialog() {
    // TODO: Show a dialog to customize toolbar buttons
    emit toolBarCommand("settings.toolbarCustomize");
}

void ToolBar::onNew() { emit toolBarCommand("file.new"); }
void ToolBar::onOpen() { emit toolBarCommand("file.open"); }
void ToolBar::onSave() { emit toolBarCommand("file.save"); }
void ToolBar::onSaveAs() { emit toolBarCommand("file.saveAs"); }
void ToolBar::onClose() { emit toolBarCommand("file.close"); }
void ToolBar::onUndo() { emit toolBarCommand("edit.undo"); }
void ToolBar::onRedo() { emit toolBarCommand("edit.redo"); }
void ToolBar::onCut() { emit toolBarCommand("edit.cut"); }
void ToolBar::onCopy() { emit toolBarCommand("edit.copy"); }
void ToolBar::onPaste() { emit toolBarCommand("edit.paste"); }
void ToolBar::onDelete() { emit toolBarCommand("edit.delete"); }
void ToolBar::onFind() { emit toolBarCommand("edit.find"); }
void ToolBar::onReplace() { emit toolBarCommand("edit.replace"); }
void ToolBar::onGotoLine() { emit toolBarCommand("edit.goto"); }
void ToolBar::onToggleComment() { emit toolBarCommand("edit.toggleComment"); }
void ToolBar::onBlockComment() { emit toolBarCommand("edit.blockComment"); }
void ToolBar::onIndent() { emit toolBarCommand("edit.indent"); }
void ToolBar::onOutdent() { emit toolBarCommand("edit.outdent"); }
void ToolBar::onRecordMacro() { emit toolBarCommand("macro.record"); }
void ToolBar::onStopMacro() { emit toolBarCommand("macro.stop"); }
void ToolBar::onPlayMacro() { emit toolBarCommand("macro.play"); }
void ToolBar::onRun() { emit toolBarCommand("tools.run"); }
void ToolBar::onPrint() { emit toolBarCommand("tools.print"); }
void ToolBar::onWordWrap() { emit toolBarCommand("view.wordWrap"); }
void ToolBar::onShowAllCharacters() { emit toolBarCommand("view.showAllChars"); }
void ToolBar::onToggleReadOnly() { emit toolBarCommand("view.readOnly"); }
void ToolBar::onZoomIn() { zoomIn(); }
void ToolBar::onZoomOut() { zoomOut(); }
void ToolBar::onZoomReset() { zoomReset(); }
void ToolBar::onDocMap() { emit toolBarCommand("view.docMap"); }
void ToolBar::onFuncList() { emit toolBarCommand("view.funcList"); }
void ToolBar::onFileBrowser() { emit toolBarCommand("view.fileBrowser"); }
void ToolBar::onEncodingMenu() { emit toolBarCommand("encoding.menu"); }
void ToolBar::onEolMenu() { emit toolBarCommand("eol.menu"); }
void ToolBar::onLanguageMenu() { emit toolBarCommand("language.menu"); }

void ToolBar::onEncodingAction(QAction* action) {
    QString cmd = action->data().toString();
    if (!cmd.isEmpty()) {
        emit toolBarCommand(cmd);
    }
}

void ToolBar::onEolAction(QAction* action) {
    QString cmd = action->data().toString();
    if (!cmd.isEmpty()) {
        emit toolBarCommand(cmd);
    }
}

void ToolBar::onLanguageAction(QAction* action) {
    QString cmd = action->data().toString();
    if (!cmd.isEmpty()) {
        emit toolBarCommand(cmd);
    }
}

void ToolBar::onStyleAction(QAction* action) {
    QString data = action->data().toString();
    if (data == "iconOnly") setToolBarStyle(IconOnly);
    else if (data == "textOnly") setToolBarStyle(TextOnly);
    else if (data == "textBeside") setToolBarStyle(TextBesideIcon);
    else if (data == "textBelow") setToolBarStyle(TextBelowIcon);
}

void ToolBar::dragEnterEvent(QDragEnterEvent* event) {
    QToolBar::dragEnterEvent(event);
}

void ToolBar::dragMoveEvent(QDragMoveEvent* event) {
    QToolBar::dragMoveEvent(event);
}

void ToolBar::dropEvent(QDropEvent* event) {
    QToolBar::dropEvent(event);
}

// === Stubs ===
void ToolBar::onActionTriggered() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action) {
        return;
    }

    QString cmd = action->data().toString();
    if (cmd.isEmpty()) {
        return;
    }

    // Extract simple command name from namespaced format (e.g., "file.new" -> "new")
    QString simpleCmd = cmd;
    if (cmd.contains('.')) {
        simpleCmd = cmd.section('.', -1);
    }

    // Dispatch to appropriate handler
    if (cmd == "file.new") {
        onNew();
    } else if (cmd == "file.open") {
        onOpen();
    } else if (cmd == "file.save") {
        onSave();
    } else if (cmd == "file.saveAs") {
        onSaveAs();
    } else if (cmd == "file.close") {
        onClose();
    } else if (cmd == "edit.undo") {
        onUndo();
    } else if (cmd == "edit.redo") {
        onRedo();
    } else if (cmd == "edit.cut") {
        onCut();
    } else if (cmd == "edit.copy") {
        onCopy();
    } else if (cmd == "edit.paste") {
        onPaste();
    } else if (cmd == "edit.delete") {
        onDelete();
    } else if (cmd == "edit.find") {
        onFind();
    } else if (cmd == "edit.replace") {
        onReplace();
    } else if (cmd == "edit.goto") {
        onGotoLine();
    } else if (cmd == "edit.toggleComment") {
        onToggleComment();
    } else if (cmd == "edit.blockComment") {
        onBlockComment();
    } else if (cmd == "edit.indent") {
        onIndent();
    } else if (cmd == "edit.outdent") {
        onOutdent();
    } else if (cmd == "macro.record") {
        onRecordMacro();
    } else if (cmd == "macro.stop") {
        onStopMacro();
    } else if (cmd == "macro.play") {
        onPlayMacro();
    } else if (cmd == "tools.run") {
        onRun();
    } else if (cmd == "tools.print") {
        onPrint();
    } else if (cmd == "view.wordWrap") {
        onWordWrap();
    } else if (cmd == "view.showAllChars") {
        onShowAllCharacters();
    } else if (cmd == "view.readOnly") {
        onToggleReadOnly();
    } else if (cmd == "view.zoomIn") {
        onZoomIn();
    } else if (cmd == "view.zoomOut") {
        onZoomOut();
    } else if (cmd == "view.zoomReset") {
        onZoomReset();
    } else if (cmd == "view.docMap") {
        onDocMap();
    } else if (cmd == "view.funcList") {
        onFuncList();
    } else if (cmd == "view.fileBrowser") {
        onFileBrowser();
    } else if (cmd == "encoding.menu" || cmd.startsWith("encoding.")) {
        onEncodingMenu();
    } else if (cmd.startsWith("eol.")) {
        onEolMenu();
    } else if (cmd.startsWith("language.")) {
        onLanguageMenu();
    } else if (cmd == "settings.toolbarCustomize") {
        showCustomizationDialog();
    }

    // Emit the command for any additional listeners
    emit toolBarCommand(cmd);
}

void ToolBar::updateForDpi() {
    DpiManager& dpi = DpiManager::instance();
    // Rescale icon size for the new DPI
    int scaledSize = dpi.iconSize();
    setIconSize(scaledSize);
}

