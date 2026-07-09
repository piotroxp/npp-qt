// NppCommands.cpp — Menu command handlers for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3

#include "NppCommands.h"
#include "Application.h"
#include "core/FileManager.h"
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>

NppCommands::NppCommands(Application* app, QObject* parent)
    : QObject(parent), m_app(app)
{}

// ============================================================================
// Dispatch
// ============================================================================

void NppCommands::command(int id) {
    Q_EMIT commandExecuted(id);
    switch (id) {
    case 40001: newFile(); break;
    case 40002: openFile(); break;
    case 40003: closeFile(); break;
    case 40004: save(); break;
    case 40005: saveAs(); break;
    case 40006: saveAll(); break;
    case 40007: reload(); break;
    case 40010: print(); break;
    case 40015: closeAll(); break;
    case 40016: closeAllButCurrent(); break;
    case 40019: loadSession(); break;
    case 40020: saveSession(); break;
    case 40110: undo(); break;
    case 40111: redo(); break;
    case 40120: cut(); break;
    case 40121: copy(); break;
    case 40122: paste(); break;
    case 40123: /* delete */ break;
    case 40130: selectAll(); break;
    case 40140: deleteLine(); break;
    case 40150: joinLines(); break;
    case 40160: trimTrailing(); break;
    case 40161: trimLeading(); break;
    case 40180: sortLinesLocaleAsc(); break;
    case 40181: sortLinesLocaleDesc(); break;
    case 40184: sortLinesLengthAsc(); break;
    case 40185: sortLinesLengthDesc(); break;
    case 40190: openInFolder(); break;
    case 40192: searchOnInternet(); break;
    case 40200: showFind(); break;
    case 40201: showReplace(); break;
    case 40202: findNext(); break;
    case 40203: findPrevious(); break;
    case 40210: goToLine(); break;
    case 40220: markAll({}); break;
    case 40221: unmarkAll(); break;
    case 40230: findInFiles({}, {}); break;
    case 40350: toggleDocumentMap(); break;
    case 40351: toggleFunctionList(); break;
    case 40352: toggleFileBrowser(); break;
    case 40353: /* fullscreen */ break;
    case 40360: switchToOtherView(); break;
    case 40361: switchToOtherView(); break;
    case 40362: cloneToOtherView(); break;
    case 40363: moveToOtherView(); break;
    case 40500: convertToAnsi(); break;
    case 40501: convertToUtf8(); break;
    case 40502: convertToUtf8Bom(); break;
    case 40600: convertToWindowsEol(); break;
    case 40601: convertToUnixEol(); break;
    case 40602: convertToMacEol(); break;
    case 44001: showRunDialog(); break;
    default: qDebug() << "[NppCommands] Unknown command:" << id; break;
    }
}

// ============================================================================
// File
// ============================================================================

void NppCommands::newFile()  { if (m_app) m_app->newFile(); }

void NppCommands::openFile() {
    QString path = QFileDialog::getOpenFileName(nullptr, "Open File");
    if (!path.isEmpty() && m_app) m_app->openFile(path.toStdString());
}

void NppCommands::save() {
    if (!m_app) return;
    BufferID id = m_app->getActiveBuffer();
    if (id) m_app->saveFile(id);
}

void NppCommands::saveAs() {
    if (!m_app) return;
    QString path = QFileDialog::getSaveFileName(nullptr, "Save As");
    if (!path.isEmpty()) {
        BufferID id = m_app->getActiveBuffer();
        if (id) m_app->saveFile(id, path.toStdString());
    }
}

void NppCommands::saveAll()  { if (m_app) m_app->saveAllFiles(); }

void NppCommands::reload() {
    if (!m_app) return;
    BufferID id = m_app->getActiveBuffer();
    if (id) {
        QString path = FileManager::instance().getBufferPath(id);
        if (!path.isEmpty()) m_app->openFile(path.toStdString());
    }
}

void NppCommands::closeFile() {
    if (!m_app) return;
    BufferID id = m_app->getActiveBuffer();
    if (id) m_app->closeFile(id, false);
}

void NppCommands::closeAll()  { if (m_app) m_app->closeAllFiles(); }
void NppCommands::closeAllButCurrent() { /* TODO */ }
void NppCommands::print()    { qDebug() << "[NppCommands] print()"; }

// ============================================================================
// Edit
// ============================================================================

void NppCommands::undo()  { if (m_app) m_app->onUndo(); }
void NppCommands::redo()  { if (m_app) m_app->onRedo(); }
void NppCommands::cut()   { if (m_app) m_app->onCut(); }
void NppCommands::copy()  { if (m_app) m_app->onCopy(); }
void NppCommands::paste() { if (m_app) m_app->onPaste(); }
void NppCommands::selectAll() { if (m_app) m_app->onSelectAll(); }

void NppCommands::deleteLine() { if (m_app) m_app->onMenuCommand("deleteLine"); }
void NppCommands::joinLines()  { if (m_app) m_app->onMenuCommand("joinLines"); }
void NppCommands::trimTrailing() { if (m_app) m_app->onMenuCommand("trimTrailing"); }
void NppCommands::trimLeading()  { if (m_app) m_app->onMenuCommand("trimLeading"); }
void NppCommands::sortLinesLengthAsc()  { if (m_app) m_app->onMenuCommand("sortLengthAsc"); }
void NppCommands::sortLinesLengthDesc() { if (m_app) m_app->onMenuCommand("sortLengthDesc"); }
void NppCommands::sortLinesLocaleAsc()  { if (m_app) m_app->onMenuCommand("sortAsc"); }
void NppCommands::sortLinesLocaleDesc() { if (m_app) m_app->onMenuCommand("sortDesc"); }

// ============================================================================
// Search
// ============================================================================

void NppCommands::showFind()    { if (m_app) m_app->onFind(); }
void NppCommands::showReplace() { if (m_app) m_app->onReplace(); }
void NppCommands::findNext()   { if (m_app) m_app->onFindNext(); }
void NppCommands::findPrevious(){ if (m_app) m_app->onFindPrev(); }

void NppCommands::goToLine() {
    if (!m_app) return;
    bool ok;
    int line = QInputDialog::getInt(nullptr, "Go to Line", "Line:", 1, 1, 9999999, 1, &ok);
    if (ok) m_app->onMenuCommand("goto:" + QString::number(line));
}

void NppCommands::markAll(const QString& text, int styleId) {
    Q_UNUSED(text); Q_UNUSED(styleId);
    if (m_app) m_app->onMarkAll();
}

void NppCommands::unmarkAll(int styleId) {
    Q_UNUSED(styleId);
    if (m_app) m_app->onMenuCommand("unmarkAll");
}

void NppCommands::findInFiles(const QString& dir, const QString& pattern) {
    Q_UNUSED(dir); Q_UNUSED(pattern);
    if (m_app) m_app->onFindInFiles();
}

// ============================================================================
// View
// ============================================================================

void NppCommands::toggleDocumentMap()  { if (m_app) m_app->onMenuCommand("toggleDocumentMap"); }
void NppCommands::toggleFunctionList(){ if (m_app) m_app->onMenuCommand("toggleFunctionList"); }
void NppCommands::toggleFileBrowser() { if (m_app) m_app->onMenuCommand("toggleFileBrowser"); }
void NppCommands::switchToOtherView(){ if (m_app) m_app->switchToView(1); }
void NppCommands::cloneToOtherView() { if (m_app) m_app->onMenuCommand("cloneToOtherView"); }
void NppCommands::moveToOtherView()  { if (m_app) m_app->onMenuCommand("moveToOtherView"); }

// ============================================================================
// Encoding
// ============================================================================

void NppCommands::convertToAnsi()   { if (m_app) m_app->onConvertEncoding(EncodingType::ANSI); }
void NppCommands::convertToUtf8()   { if (m_app) m_app->onConvertEncoding(EncodingType::UTF_8); }
void NppCommands::convertToUtf8Bom(){ if (m_app) m_app->onConvertEncoding(EncodingType::UTF_8_BOM); }

// ============================================================================
// EOL
// ============================================================================

void NppCommands::convertToWindowsEol() { if (m_app) m_app->onMenuCommand("eol:CRLF"); }
void NppCommands::convertToUnixEol()    { if (m_app) m_app->onMenuCommand("eol:LF"); }
void NppCommands::convertToMacEol()    { if (m_app) m_app->onMenuCommand("eol:CR"); }

// ============================================================================
// Session / Shell
// ============================================================================

void NppCommands::loadSession() {
    QString path = QFileDialog::getOpenFileName(nullptr, "Load Session", QString(), "Session (*.session)");
    if (!path.isEmpty() && m_app) m_app->loadSession(path.toStdString());
}

void NppCommands::saveSession() {
    QString path = QFileDialog::getSaveFileName(nullptr, "Save Session", QString(), "Session (*.session)");
    if (!path.isEmpty() && m_app) m_app->saveSession(path.toStdString());
}

void NppCommands::openContainingFolder() {
    if (!m_app) return;
    BufferID id = m_app->getActiveBuffer();
    QString path = FileManager::instance().getBufferPath(id);
    if (!path.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absolutePath()));
}

void NppCommands::openFolderAsWorkspace() {
    QString dir = QFileDialog::getExistingDirectory(nullptr, "Open Folder as Workspace");
    Q_UNUSED(dir);
}

void NppCommands::openInFolder()    { openContainingFolder(); }
void NppCommands::searchOnInternet() {
    QClipboard* cb = QApplication::clipboard();
    QString text = cb->text().trimmed();
    if (!text.isEmpty())
        QDesktopServices::openUrl(QUrl("https://www.google.com/search?q=" + text));
}

void NppCommands::showRunDialog() {
    QString cmd = QInputDialog::getText(nullptr, "Run", "Command:").trimmed();
    if (!cmd.isEmpty()) QProcess::startDetached("sh", {"-c", cmd});
}
