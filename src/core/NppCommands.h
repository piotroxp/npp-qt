// NppCommands.h — Menu command handlers for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3
#pragma once

#include <QObject>
#include <QString>
#include "common/Types.h"

class Application;
class ScintillaEditor;

// Entry point: routes integer command IDs to handler methods.
// NppBigSwitch dispatches by ID → calls NppCommands::command(int).
class NppCommands : public QObject {
    Q_OBJECT
public:
    explicit NppCommands(Application* app, QObject* parent = nullptr);
    Application* app() const { return m_app; }

    // Dispatch by ID
    void command(int id);

    // ── File ───────────────────────────────────────────────────────────────
    void newFile();
    void openFile();
    void save();
    void saveAs();
    void saveAll();
    void reload();
    void closeFile();
    void closeAll();
    void closeAllButCurrent();
    void print();

    // ── Edit ──────────────────────────────────────────────────────────────
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void deleteLine();
    void joinLines();
    void trimTrailing();
    void trimLeading();
    void sortLinesLengthAsc();
    void sortLinesLengthDesc();
    void sortLinesLocaleAsc();
    void sortLinesLocaleDesc();

    // ── Search ─────────────────────────────────────────────────────────────
    void showFind();
    void showReplace();
    void findNext();
    void findPrevious();
    void goToLine();
    void markAll(const QString& text, int styleId = 0);
    void unmarkAll(int styleId = 0);
    void findInFiles(const QString& dir, const QString& pattern);

    // ── View ───────────────────────────────────────────────────────────────
    void toggleDocumentMap();
    void toggleFunctionList();
    void toggleFileBrowser();
    void switchToOtherView();
    void cloneToOtherView();
    void moveToOtherView();

    // ── Encoding ───────────────────────────────────────────────────────────
    void convertToAnsi();
    void convertToUtf8();
    void convertToUtf8Bom();

    // ── EOL ────────────────────────────────────────────────────────────────
    void convertToWindowsEol();
    void convertToUnixEol();
    void convertToMacEol();

    // ── Session / Shell ────────────────────────────────────────────────────
    void loadSession();
    void saveSession();
    void openContainingFolder();
    void openFolderAsWorkspace();
    void openInFolder();
    void searchOnInternet();
    void showRunDialog();

Q_SIGNALS:
    void commandExecuted(int id);
    void statusMessage(const QString& msg);

private:
    Application* m_app = nullptr;
};
