// MainWindow.h - Qt6 main window wrapper for Notepad++
#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include "Notepad_plus.h"
#include "Parameters.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    // File operations
    void openFiles(const QStringList& files);
    void loadSession(const QString& sessionFile);

    // Mode flags
    void setReadOnly(bool ro) { _readOnly = ro; }
    void setNoPlugin(bool np) { _noPlugin = np; }

    // Editor access
    ScintillaEditView* activeEditor();

public slots:
    // File menu slots
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileSaveAll();
    void onFileClose();
    void onFileCloseAll();
    void onFilePrint();

    // Edit menu slots
    void onEditUndo();
    void onEditRedo();
    void onEditCut();
    void onEditCopy();
    void onEditPaste();
    void onEditDelete();
    void onEditSelectAll();
    void onEditFind();
    void onEditReplace();

    // Search menu slots
    void onSearchFindNext();
    void onSearchFindPrev();
    void onSearchGoToLine();

    // View menu slots
    void onViewZoomIn();
    void onViewZoomOut();
    void onViewFullScreen();
    void onViewPostIt();

    // Tools menu slots
    void onRunCommand();

    // Macro menu slots
    void onMacroStartRecord();
    void onMacroStopRecord();
    void onMacroPlayRecord();

signals:
    void fileOpened(const QString& path);
    void sessionLoaded(const QString& session);

protected:
    // Qt events
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void createTrayIcon();

    Notepad_plus _npp;
    bool _readOnly = false;
    bool _noPlugin = false;

    QMenu* _fileMenu = nullptr;
    QMenu* _editMenu = nullptr;
    QMenu* _searchMenu = nullptr;
    QMenu* _viewMenu = nullptr;
    QMenu* _encodingMenu = nullptr;
    QMenu* _toolsMenu = nullptr;
    QMenu* _macroMenu = nullptr;
    QMenu* _runMenu = nullptr;
    QMenu* _windowMenu = nullptr;
    QMenu* _helpMenu = nullptr;
};