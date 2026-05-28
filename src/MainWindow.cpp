// MainWindow.cpp - Qt6 main window implementation
#include "MainWindow.h"
#include "Notepad_plus.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _readOnly(false)
    , _noPlugin(false)
{
    setupUI();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setAcceptDrops(true);
    setWindowTitle("Notepad--");
    
    // Set minimum size
    setMinimumSize(400, 300);
    
    // Setup menu bar
    setupMenuBar();
    
    // Setup toolbar
    setupToolBar();
    
    // Setup status bar
    setupStatusBar();
    
    // Central widget is the Notepad_plus instance
    // The _npp object will be embedded
}

void MainWindow::setupMenuBar() {
    QMenuBar* menuBar = this->menuBar();
    
    // File menu
    _fileMenu = menuBar->addMenu(tr("&File"));
    _fileMenu->addAction(tr("New"), this, &MainWindow::onFileNew);
    _fileMenu->addAction(tr("&Open..."), this, &MainWindow::onFileOpen);
    _fileMenu->addSeparator();
    _fileMenu->addAction(tr("&Save"), this, &MainWindow::onFileSave);
    _fileMenu->addAction(tr("Save &As..."), this, &MainWindow::onFileSaveAs);
    _fileMenu->addAction(tr("Save A&ll"), this, &MainWindow::onFileSaveAll);
    _fileMenu->addSeparator();
    _fileMenu->addAction(tr("&Close"), this, &MainWindow::onFileClose);
    _fileMenu->addAction(tr("Close Al&l"), this, &MainWindow::onFileCloseAll);
    _fileMenu->addSeparator();
    _fileMenu->addAction(tr("&Print..."), this, &MainWindow::onFilePrint);
    _fileMenu->addSeparator();
    _fileMenu->addAction(tr("E&xit"), this, &QMainWindow::close);
    
    // Edit menu
    _editMenu = menuBar->addMenu(tr("&Edit"));
    _editMenu->addAction(tr("&Undo"), this, &MainWindow::onEditUndo);
    _editMenu->addAction(tr("&Redo"), this, &MainWindow::onEditRedo);
    _editMenu->addSeparator();
    _editMenu->addAction(tr("Cu&t"), this, &MainWindow::onEditCut);
    _editMenu->addAction(tr("&Copy"), this, &MainWindow::onEditCopy);
    _editMenu->addAction(tr("&Paste"), this, &MainWindow::onEditPaste);
    _editMenu->addAction(tr("&Delete"), this, &MainWindow::onEditDelete);
    _editMenu->addSeparator();
    _editMenu->addAction(tr("Select &All"), this, &MainWindow::onEditSelectAll);
    _editMenu->addSeparator();
    _editMenu->addAction(tr("&Find..."), this, &MainWindow::onEditFind);
    _editMenu->addAction(tr("&Replace..."), this, &MainWindow::onEditReplace);
    
    // Search menu
    _searchMenu = menuBar->addMenu(tr("&Search"));
    _searchMenu->addAction(tr("Find &Next"), this, &MainWindow::onSearchFindNext);
    _searchMenu->addAction(tr("Find &Previous"), this, &MainWindow::onSearchFindPrev);
    _searchMenu->addSeparator();
    _searchMenu->addAction(tr("&Go to Line..."), this, &MainWindow::onSearchGoToLine);
    
    // View menu
    _viewMenu = menuBar->addMenu(tr("&View"));
    _viewMenu->addAction(tr("Zoom &In"), this, &MainWindow::onViewZoomIn);
    _viewMenu->addAction(tr("Zoom &Out"), this, &MainWindow::onViewZoomOut);
    _viewMenu->addSeparator();
    _viewMenu->addAction(tr("&Full Screen"), this, &MainWindow::onViewFullScreen);
    _viewMenu->addAction(tr("&Post It"), this, &MainWindow::onViewPostIt);
    
    // Encoding menu
    _encodingMenu = menuBar->addMenu(tr("&Encoding"));
    QAction* utf8Action = _encodingMenu->addAction(tr("Convert to UTF-8"));
    QAction* ansiAction = _encodingMenu->addAction(tr("Convert to ANSI"));
    Q_UNUSED(utf8Action);
    Q_UNUSED(ansiAction);
    
    // Tools menu
    _toolsMenu = menuBar->addMenu(tr("&Tools"));
    _toolsMenu->addAction(tr("&Run..."), this, &MainWindow::onRunCommand);
    
    // Macro menu
    _macroMenu = menuBar->addMenu(tr("M&acro"));
    _macroMenu->addAction(tr("Start Recording"), this, &MainWindow::onMacroStartRecord);
    _macroMenu->addAction(tr("Stop Recording"), this, &MainWindow::onMacroStopRecord);
    _macroMenu->addAction(tr("Play Recording"), this, &MainWindow::onMacroPlayRecord);
    
    // Run menu
    _runMenu = menuBar->addMenu(tr("&Run"));
    _runMenu->addAction(tr("&Run..."), this, &MainWindow::onRunCommand);
    
    // Window menu
    _windowMenu = menuBar->addMenu(tr("&Window"));
    _windowMenu->addAction(tr("Always on &Top"), this, [this](bool checked) {
        setWindowFlag(Qt::WindowStaysOnTopHint, checked);
    });
    
    // Help menu
    _helpMenu = menuBar->addMenu(tr("&Help"));
    _helpMenu->addAction(tr("&About Notepad--"), this, [this]() {
        QMessageBox::about(this, tr("About Notepad--"),
            tr("Notepad-- (Qt6 port of Notepad++)\nVersion 8.9.4\n\n"
               "A free, open-source text editor for Windows/Linux."));
    });
}

// File menu slots
void MainWindow::onFileNew() { if (_npp) _npp->onFileNew(); }
void MainWindow::onFileOpen() { if (_npp) _npp->onFileOpen(); }
void MainWindow::onFileSave() { if (_npp) _npp->onFileSave(); }
void MainWindow::onFileSaveAs() { if (_npp) _npp->onFileSaveAs(); }
void MainWindow::onFileSaveAll() { if (_npp) _npp->onFileSaveAll(); }
void MainWindow::onFileClose() { if (_npp) _npp->onFileClose(); }
void MainWindow::onFileCloseAll() { if (_npp) _npp->onFileCloseAll(); }
void MainWindow::onFilePrint() { if (_npp) _npp->onFilePrint(); }

// Edit menu slots
void MainWindow::onEditUndo() { /* handled by scintilla */ }
void MainWindow::onEditRedo() { /* handled by scintilla */ }
void MainWindow::onEditCut() { /* handled by scintilla */ }
void MainWindow::onEditCopy() { /* handled by scintilla */ }
void MainWindow::onEditPaste() { /* handled by scintilla */ }
void MainWindow::onEditDelete() { /* handled by scintilla */ }
void MainWindow::onEditSelectAll() { /* handled by scintilla */ }
void MainWindow::onEditFind() { if (_npp) _npp->onEditFind(); }
void MainWindow::onEditReplace() { if (_npp) _npp->onEditReplace(); }

// Search menu slots
void MainWindow::onSearchFindNext() { if (_npp) _npp->onSearchFindNext(); }
void MainWindow::onSearchFindPrev() { if (_npp) _npp->onSearchFindPrev(); }
void MainWindow::onSearchGoToLine() { /* open dialog */ }

// View menu slots
void MainWindow::onViewZoomIn() { if (_npp) _npp->onViewZoomIn(); }
void MainWindow::onViewZoomOut() { if (_npp) _npp->onViewZoomOut(); }
void MainWindow::onViewFullScreen() { /* toggle fullscreen */ }
void MainWindow::onViewPostIt() { /* toggle post-it mode */ }

// Tools menu slots
void MainWindow::onRunCommand() { if (_npp) _npp->onRunCommand(); }

// Macro menu slots
void MainWindow::onMacroStartRecord() { if (_npp) _npp->onMacroStartRecord(); }
void MainWindow::onMacroStopRecord() { if (_npp) _npp->onMacroStopRecord(); }
void MainWindow::onMacroPlayRecord() { if (_npp) _npp->onMacroPlayRecord(); }

void MainWindow::setupToolBar() {
    // Placeholder toolbar - actual implementation would connect to _npp methods
    QToolBar* toolbar = addToolBar(tr("Main Toolbar"));
    toolbar->setMovable(false);
    
    // File operations - stubs (actual actions would call _npp methods)
    toolbar->addAction(tr("New"), this, []() { /* TODO: call _npp.onFileNew */ });
    toolbar->addAction(tr("Open"), this, []() { /* TODO: call _npp.onFileOpen */ });
    toolbar->addAction(tr("Save"), this, []() { /* TODO: call _npp.onFileSave */ });
    toolbar->addSeparator();
    
    // Edit operations
    toolbar->addAction(tr("Undo"), this, []() { /* TODO */ });
    toolbar->addAction(tr("Redo"), this, []() { /* TODO */ });
    toolbar->addSeparator();
    
    // Search
    toolbar->addAction(tr("Find"), this, []() { /* TODO */ });
}

void MainWindow::setupStatusBar() {
    QStatusBar* statusBar = this->statusBar();
    statusBar->showMessage(tr("Ready"));
}

ScintillaEditView* MainWindow::activeEditor() {
    return _npp ? _npp->getCurrentEditView() : nullptr;
}

void MainWindow::openFiles(const QStringList& files) {
    for (const QString& file : files) {
        // Load file into Npp
        Q_UNUSED(file);
    }
}

void MainWindow::loadSession(const QString& sessionFile) {
    Q_UNUSED(sessionFile);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // Check for unsaved changes
    if (_npp && _npp->getNbDirtyBuffer(0) > 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, tr("Save Changes?"),
            tr("Some files have unsaved changes. Do you want to save them?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Save) {
            _npp->fileSaveAllConfirm();
        } else if (reply == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }
    
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasUrls()) {
        QStringList files;
        for (const QUrl& url : event->mimeData()->urls()) {
            if (url.isLocalFile()) {
                files.append(url.toLocalFile());
            }
        }
        if (!files.isEmpty()) {
            openFiles(files);
        }
        event->acceptProposedAction();
    }
}
