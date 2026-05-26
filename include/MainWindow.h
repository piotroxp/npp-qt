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