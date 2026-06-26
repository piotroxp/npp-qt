// Semantic lift: Main window implementation
// Original: PowerEditor/src/Notepad_plus_Window.cpp
// Target: npp-qt/src/MainWindow.cpp

#include "MainWindow.h"
#include "Buffer.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QFileInfo>
#include <QCloseEvent>
#include <QStyle>
#include <QStyleOption>

// Static member initialization
QWidget* MainWindow::gNppHWND = nullptr;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _settings(QSettings::IniFormat, QSettings::UserScope, "Notepad++", "Notepad++Qt")
{
    setAcceptDrops(true);
    setupUi();
    loadSettings();
    gNppHWND = this;  // register main window handle
}

MainWindow::~MainWindow()
{
    gNppHWND = nullptr;  // unregister on destruction
}

void MainWindow::init()
{
    newFile();
    loadSession();
}

void MainWindow::setupUi()
{
    setWindowTitle("Notepad++ (Qt)");
    resize(1200, 800);

    _pTabBar = new TabBarPlus(this);
    setCentralWidget(_pTabBar);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createTabBar();
    createDocumentPane();
    createPanels();

    connect(_pTabBar, &TabBarPlus::currentChanged, this, &MainWindow::onTabChanged);
    connect(_pTabBar, &TabBarPlus::tabDelete, this, &MainWindow::closeFile);
}

void MainWindow::createActions()
{
    // File actions
    QAction* newAct = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
    newAct->setShortcut(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    QAction* openAct = new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, qOverload<>(&MainWindow::openFile));

    QAction* saveAct = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    QAction* saveAsAct = new QAction(QIcon::fromTheme("document-save-as"), tr("Save &As..."), this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveFileAs);

    QAction* closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcut(QKeySequence::Close);
    connect(closeAct, &QAction::triggered, this, &MainWindow::closeFile);

    QAction* exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, this, &MainWindow::exit);

    // Edit actions
    QAction* undoAct = new QAction(QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    undoAct->setShortcut(QKeySequence::Undo);
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    QAction* redoAct = new QAction(QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence::Redo);
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    QAction* cutAct = new QAction(QIcon::fromTheme("edit-cut"), tr("Cu&t"), this);
    cutAct->setShortcut(QKeySequence::Cut);
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);

    QAction* copyAct = new QAction(QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    copyAct->setShortcut(QKeySequence::Copy);
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);

    QAction* pasteAct = new QAction(QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    pasteAct->setShortcut(QKeySequence::Paste);
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);

    QAction* selectAllAct = new QAction(tr("Select &All"), this);
    selectAllAct->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAct, &QAction::triggered, this, &MainWindow::selectAll);

    // View actions
    QAction* fullScreenAct = new QAction(tr("&Full Screen"), this);
    fullScreenAct->setShortcut(QKeySequence::FullScreen);
    fullScreenAct->setCheckable(true);
    connect(fullScreenAct, &QAction::triggered, this, &MainWindow::toggleFullScreen);

    QAction* wordWrapAct = new QAction(tr("&Word Wrap"), this);
    wordWrapAct->setCheckable(true);
    connect(wordWrapAct, &QAction::triggered, this, &MainWindow::toggleWordWrap);

    // Search actions
    QAction* findAct = new QAction(QIcon::fromTheme("edit-find"), tr("&Find..."), this);
    findAct->setShortcut(QKeySequence::Find);
    connect(findAct, &QAction::triggered, this, &MainWindow::find);

    QAction* replaceAct = new QAction(tr("&Replace..."), this);
    replaceAct->setShortcut(QKeySequence::Replace);
    connect(replaceAct, &QAction::triggered, this, &MainWindow::replace);
}

void MainWindow::createMenus()
{
    _pMenuBar = menuBar();

    QMenu* fileMenu = _pMenuBar->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), this, &MainWindow::newFile, QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, [this]() { openFile(); }, QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Save"), this, &MainWindow::saveFile, QKeySequence::Save);
    fileMenu->addAction(tr("Save &As..."), this, &MainWindow::saveFileAs, QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Close"), this, &MainWindow::closeFile, QKeySequence::Close);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), this, &MainWindow::exit, QKeySequence::Quit);

    QMenu* editMenu = _pMenuBar->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Undo"), this, &MainWindow::undo, QKeySequence::Undo);
    editMenu->addAction(tr("&Redo"), this, &MainWindow::redo, QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction(tr("Cu&t"), this, &MainWindow::cut, QKeySequence::Cut);
    editMenu->addAction(tr("&Copy"), this, &MainWindow::copy, QKeySequence::Copy);
    editMenu->addAction(tr("&Paste"), this, &MainWindow::paste, QKeySequence::Paste);
    editMenu->addSeparator();
    editMenu->addAction(tr("Select &All"), this, &MainWindow::selectAll, QKeySequence::SelectAll);

    QMenu* searchMenu = _pMenuBar->addMenu(tr("&Search"));
    searchMenu->addAction(tr("&Find..."), this, &MainWindow::find, QKeySequence::Find);
    searchMenu->addAction(tr("&Replace..."), this, &MainWindow::replace, QKeySequence::Replace);

    QMenu* viewMenu = _pMenuBar->addMenu(tr("&View"));
    viewMenu->addAction(tr("&Full Screen"), this, &MainWindow::toggleFullScreen, QKeySequence::FullScreen);
    viewMenu->addAction(tr("&Word Wrap"), this, &MainWindow::toggleWordWrap);
}

void MainWindow::createToolBars()
{
    _pFileToolBar = addToolBar(tr("File"));
    _pFileToolBar->setObjectName("FileToolBar");
    _pFileToolBar->addAction(tr("New"), this, &MainWindow::newFile);
    _pFileToolBar->addAction(tr("Open"), this, [this]() { openFile(); });
    _pFileToolBar->addAction(tr("Save"), this, &MainWindow::saveFile);

    _pEditToolBar = addToolBar(tr("Edit"));
    _pEditToolBar->setObjectName("EditToolBar");
    _pEditToolBar->addAction(tr("Undo"), this, &MainWindow::undo);
    _pEditToolBar->addAction(tr("Redo"), this, &MainWindow::redo);

    _pSearchToolBar = addToolBar(tr("Search"));
    _pSearchToolBar->setObjectName("SearchToolBar");
    _pSearchToolBar->addAction(tr("Find"), this, &MainWindow::find);
    _pSearchToolBar->addAction(tr("Replace"), this, &MainWindow::replace);
}

void MainWindow::createStatusBar()
{
    _pStatusBar = statusBar();

    _pPositionLabel = new QLabel(tr("Ln 1, Col 1"), this);
    _pStatusBar->addPermanentWidget(_pPositionLabel);

    _pLengthLabel = new QLabel(tr("0 chars"), this);
    _pStatusBar->addPermanentWidget(_pLengthLabel);

    _pEncodingLabel = new QLabel(tr("UTF-8"), this);
    _pStatusBar->addPermanentWidget(_pEncodingLabel);

    _pEolLabel = new QLabel(tr("Windows (CRLF)"), this);
    _pStatusBar->addPermanentWidget(_pEolLabel);

    _pLanguageLabel = new QLabel(tr("Normal text"), this);
    _pStatusBar->addPermanentWidget(_pLanguageLabel);
}

void MainWindow::createTabBar()
{
    _pTabBar->init(this, false, false, 3);
    connect(_pTabBar, &TabBarPlus::tabDelete, this, &MainWindow::closeFile);
}

void MainWindow::createDocumentPane()
{
    _pDocTabBar = new QTabWidget(this);
    _pDocTabBar->setTabsClosable(true);
    _pDocTabBar->setDocumentMode(true);
}

void MainWindow::createPanels()
{
    _pFileBrowserDock = new QDockWidget(tr("File Browser"), this);
    _pFileBrowserDock->setObjectName("FileBrowser");
    addDockWidget(Qt::LeftDockWidgetArea, _pFileBrowserDock);

    _pFunctionListDock = new QDockWidget(tr("Function List"), this);
    _pFunctionListDock->setObjectName("FunctionList");
    addDockWidget(Qt::RightDockWidgetArea, _pFunctionListDock);

    _pDocMapDock = new QDockWidget(tr("Document Map"), this);
    _pDocMapDock->setObjectName("DocumentMap");
    addDockWidget(Qt::RightDockWidgetArea, _pDocMapDock);

    _pProjectPanelDock = new QDockWidget(tr("Project"), this);
    _pProjectPanelDock->setObjectName("ProjectPanel");
    addDockWidget(Qt::LeftDockWidgetArea, _pProjectPanelDock);

    _pFileBrowserDock->hide();
    _pFunctionListDock->hide();
    _pDocMapDock->hide();
    _pProjectPanelDock->hide();
}

void MainWindow::newFile()
{
    Buffer* buf = createNewBuffer();
    int index = _pTabBar->insertAtEnd("Untitled");
    _pTabBar->activateAt(index);
    _pCurrentBuffer = buf;
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, tr("Open File"), QDir::homePath(),
        tr("All Files (*.*);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));
    if (!filePath.isEmpty()) {
        openFile(filePath);
    }
}

void MainWindow::openFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open file: %1").arg(path));
        return;
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    Buffer* buf = createNewBuffer();
    buf->setContent(content);
    buf->setFilePath(path);

    QFileInfo fi(path);
    int index = _pTabBar->insertAtEnd(fi.fileName());
    _pTabBar->activateAt(index);
    _pCurrentBuffer = buf;
}

void MainWindow::saveFile()
{
    if (!_pCurrentBuffer) return;
    if (_pCurrentBuffer->filePath().isEmpty()) {
        saveFileAs();
        return;
    }
    QFile file(_pCurrentBuffer->filePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << _pCurrentBuffer->content();
        file.close();
        _pCurrentBuffer->setDirty(false);
    }
}

void MainWindow::saveFileAs()
{
    if (!_pCurrentBuffer) return;
    QString filePath = QFileDialog::getSaveFileName(
        this, tr("Save File As"), QDir::homePath(),
        tr("All Files (*.*);;Text Files (*.txt)"));
    if (!filePath.isEmpty()) {
        _pCurrentBuffer->setFilePath(filePath);
        saveFile();
    }
}

void MainWindow::closeFile()
{
    int index = _pTabBar->getCurrentTabIndex();
    if (index >= 0) {
        _pTabBar->deleteItemAt(index);
    }
}

void MainWindow::exit() { close(); }


// TODO: pending NppCommands wiring
void MainWindow::undo() {}
// TODO: pending NppCommands wiring
void MainWindow::redo() {}
// TODO: pending NppCommands wiring
void MainWindow::cut() {}
// TODO: pending NppCommands wiring
void MainWindow::copy() {}
// TODO: pending NppCommands wiring
void MainWindow::paste() {}
// TODO: pending NppCommands wiring
void MainWindow::selectAll() {}

// TODO: pending NppCommands wiring
void MainWindow::find() {}
// TODO: pending NppCommands wiring
void MainWindow::replace() {}

void MainWindow::toggleFullScreen()
{
    if (_isFullScreen) {
        showNormal();
    } else {
        showFullScreen();
    }
    _isFullScreen = !_isFullScreen;
}

// TODO: pending NppCommands wiring
void MainWindow::toggleWordWrap() {}

// TODO: pending NppCommands wiring
void MainWindow::zoomIn() { _currentZoomFactor += 10; }
// TODO: pending NppCommands wiring
void MainWindow::zoomOut() { _currentZoomFactor -= 10; }
// TODO: pending NppCommands wiring
void MainWindow::resetZoom() { _currentZoomFactor = 100; }

void MainWindow::onTabChanged(int index)
{
    Q_UNUSED(index);
    Buffer* buf = getBufferFromIndex(index);
    if (buf) {
        _pCurrentBuffer = buf;
        updateStatusBar();
    }
    emit documentChanged(index);
}

void MainWindow::onTabCloseRequested(int index)
{
    _pTabBar->deleteItemAt(index);
}

void MainWindow::updateStatusBar()
{
    if (_pCurrentBuffer) {
        // Update position, length, encoding, EOL, language
    }
}

void MainWindow::updateTabBar() {}

Buffer* MainWindow::createNewBuffer()
{
    Buffer* buf = new Buffer();
    _buffers.append(buf);
    return buf;
}

Buffer* MainWindow::getBufferFromIndex(int index) const
{
    if (index >= 0 && index < _buffers.size()) {
        return _buffers[index];
    }
    return nullptr;
}

void MainWindow::enableDarkMode(bool enable)
{
    _isDarkMode = enable;
    if (enable) {
        applyDarkModeStyle();
    } else {
        applyLightModeStyle();
    }
}

void MainWindow::applyDarkModeStyle()
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);
}

void MainWindow::applyLightModeStyle()
{
    qApp->setStyle(QStyleFactory::create("Windows"));
    qApp->setPalette(qApp->style()->standardPalette());
}

void MainWindow::loadSettings()
{
    _settings.beginGroup("General");
    restoreGeometry(_settings.value("geometry").toByteArray());
    restoreState(_settings.value("windowState").toByteArray());
    _settings.endGroup();
}

void MainWindow::saveSettings()
{
    _settings.beginGroup("General");
    _settings.setValue("geometry", saveGeometry());
    _settings.setValue("windowState", saveState());
    _settings.endGroup();
}

void MainWindow::loadSession() {}
void MainWindow::saveSession() {}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveSettings();
    saveSession();
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        for (const QUrl& url : mimeData->urls()) {
            if (url.isLocalFile()) {
                openFile(url.toLocalFile());
            }
        }
    }
}

bool MainWindow::event(QEvent* event)
{
    return QMainWindow::event(event);
}
