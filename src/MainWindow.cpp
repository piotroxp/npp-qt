// Semantic lift: Main window implementation
// Original: PowerEditor/src/Notepad_plus_Window.cpp
// Target: npp-qt/src/MainWindow.cpp

#include "MainWindow.h"
#include "Buffer.h"
#include "ScintillaComponent.h"
#include "FindReplaceDlg.h"
#include "NppSciCompat.h"

#include <QDir>
#include <Qsci/qsciscintilla.h>
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
    show();  // Qt6: show the main window (Win32 ::ShowWindow equivalent)
}

void MainWindow::setupUi()
{
    setWindowTitle("Notepad++ (Qt)");
    resize(1200, 800);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDocumentPane();   // creates _pDocTabBar with Scintilla editors
    createPanels();

    // Initialize Find/Replace dialog (editor set lazily via setCurrentEditor())
    _pFindReplaceDlg = new FindReplaceDlg();
    _pFindReplaceDlg->setParent(this);
    _pFindReplaceDlg->hide();

    // _pDocTabBar (QTabWidget) is the central widget — holds Scintilla editor tabs
    setCentralWidget(_pDocTabBar);

    // Qt6: QTabWidget::currentChanged and tabCloseRequested are protected signals.
    // Connect via the public QTabBar base class signals.
    QTabBar* docTabBar = qobject_cast<QTabBar*>(_pDocTabBar);
    connect(docTabBar, &QTabBar::currentChanged, this, &MainWindow::onTabChanged);
    connect(docTabBar, &QTabBar::tabCloseRequested, this, [this](int i) { closeFile(i); });
}

void MainWindow::createActions()
{
    // File actions
    _pActNew = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
    _pActNew->setShortcut(QKeySequence::New);
    connect(_pActNew, &QAction::triggered, this, &MainWindow::newFile);

    _pActOpen = new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
    _pActOpen->setShortcut(QKeySequence::Open);
    connect(_pActOpen, &QAction::triggered, this, qOverload<>(&MainWindow::openFile));

    _pActSave = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
    _pActSave->setShortcut(QKeySequence::Save);
    connect(_pActSave, &QAction::triggered, this, &MainWindow::saveFile);

    QAction* saveAsAct = new QAction(QIcon::fromTheme("document-save-as"), tr("Save &As..."), this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveFileAs);

    QAction* closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcut(QKeySequence::Close);
    connect(closeAct, &QAction::triggered, this, [this]() { closeFile(); });

    QAction* exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, this, &MainWindow::exit);

    // Edit actions
    _pActUndo = new QAction(QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    _pActUndo->setShortcut(QKeySequence::Undo);
    connect(_pActUndo, &QAction::triggered, this, &MainWindow::undo);

    _pActRedo = new QAction(QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
    _pActRedo->setShortcut(QKeySequence::Redo);
    connect(_pActRedo, &QAction::triggered, this, &MainWindow::redo);

    _pActCut = new QAction(QIcon::fromTheme("edit-cut"), tr("Cu&t"), this);
    _pActCut->setShortcut(QKeySequence::Cut);
    connect(_pActCut, &QAction::triggered, this, &MainWindow::cut);

    _pActCopy = new QAction(QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    _pActCopy->setShortcut(QKeySequence::Copy);
    connect(_pActCopy, &QAction::triggered, this, &MainWindow::copy);

    _pActPaste = new QAction(QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    _pActPaste->setShortcut(QKeySequence::Paste);
    connect(_pActPaste, &QAction::triggered, this, &MainWindow::paste);

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
    _pActFind = new QAction(QIcon::fromTheme("edit-find"), tr("&Find..."), this);
    _pActFind->setShortcut(QKeySequence::Find);
    connect(_pActFind, &QAction::triggered, this, &MainWindow::find);

    _pActReplace = new QAction(tr("&Replace..."), this);
    _pActReplace->setShortcut(QKeySequence::Replace);
    connect(_pActReplace, &QAction::triggered, this, &MainWindow::replace);
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
    fileMenu->addAction(tr("&Close"), this, [this]() { closeFile(); }, QKeySequence::Close);
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
    _pFileToolBar->setMovable(false);
    _pFileToolBar->addAction(_pActNew);
    _pFileToolBar->addAction(_pActOpen);
    _pFileToolBar->addAction(_pActSave);

    _pEditToolBar = addToolBar(tr("Edit"));
    _pEditToolBar->setObjectName("EditToolBar");
    _pEditToolBar->setMovable(false);
    _pEditToolBar->addAction(_pActCut);
    _pEditToolBar->addAction(_pActCopy);
    _pEditToolBar->addAction(_pActPaste);
    _pEditToolBar->addSeparator();
    _pEditToolBar->addAction(_pActUndo);
    _pEditToolBar->addAction(_pActRedo);

    _pSearchToolBar = addToolBar(tr("Search"));
    _pSearchToolBar->setObjectName("SearchToolBar");
    _pSearchToolBar->setMovable(false);
    _pSearchToolBar->addAction(_pActFind);
    _pSearchToolBar->addAction(_pActReplace);
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
    // _pTabBar is created in setupUi() and used for the top-level tab bar.
    // Document tabs are managed by _pDocTabBar (Scintilla editor tabs).
    if (_pTabBar)
    {
        _pTabBar->init(this, false, false, 3);
        connect(_pTabBar, &TabBarPlus::tabDelete, this, [this]() { closeFile(); });
    }
}

void MainWindow::createDocumentPane()
{
    _pDocTabBar = new QTabWidget(this);
    _pDocTabBar->setTabsClosable(true);
    _pDocTabBar->setDocumentMode(true);
    // Qt6: use QTabBar base class public signals
    QTabBar* docTabBar = qobject_cast<QTabBar*>(_pDocTabBar);
    connect(docTabBar, &QTabBar::currentChanged, this, &MainWindow::onTabChanged);
    connect(docTabBar, &QTabBar::tabCloseRequested, this, [this](int i) { closeFile(i); });
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
    // Create a Scintilla editor widget for this document
    ScintillaComponent* editor = new ScintillaComponent(this);
    editor->init(this);
    // Show line number gutter by default
    editor->showMargin(SC_MARGE_LINENUMBER, true);
    int index = _pDocTabBar->addTab(editor, "Untitled");
    _pDocTabBar->setCurrentIndex(index);
    _scintillaEditors.append(editor);
    _pCurrentBuffer = buf;

    // Connect cursor movement to status bar update
    connect(editor, &QsciScintilla::cursorPositionChanged, this, &MainWindow::updateStatusBar);
    // Connect text modifications to status bar (length may change)
    connect(editor, &QsciScintilla::textChanged, this, &MainWindow::updateStatusBar);

    updateStatusBar();
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
    ScintillaComponent* editor = new ScintillaComponent(this);
    editor->init(this);
    // Show line number gutter by default
    editor->showMargin(SC_MARGE_LINENUMBER, true);
    editor->setText(content.toUtf8());
    int index = _pDocTabBar->addTab(editor, fi.fileName());
    _pDocTabBar->setCurrentIndex(index);
    _scintillaEditors.append(editor);
    _pCurrentBuffer = buf;

    // Connect cursor movement to status bar update
    connect(editor, &QsciScintilla::cursorPositionChanged, this, &MainWindow::updateStatusBar);
    // Connect text modifications to status bar (length may change)
    connect(editor, &QsciScintilla::textChanged, this, &MainWindow::updateStatusBar);

    updateStatusBar();
}

void MainWindow::saveFile()
{
    if (!_pCurrentBuffer) return;
    if (_pCurrentBuffer->filePath().isEmpty()) {
        saveFileAs();
        return;
    }
    ScintillaComponent* editor = currentScintilla();
    QString text = editor ? editor->text() : "";
    QFile file(_pCurrentBuffer->filePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << text;
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
    closeFile(_pDocTabBar ? _pDocTabBar->currentIndex() : -1);
}

void MainWindow::closeFile(int index)
{
    if (index < 0) index = _pDocTabBar ? _pDocTabBar->currentIndex() : -1;
    if (index < 0 || !_pDocTabBar) return;

    if (index < _scintillaEditors.size()) {
        delete _scintillaEditors[index];
        _scintillaEditors.remove(index);
    }
    if (index < _buffers.size()) {
        delete _buffers[index];
        _buffers.remove(index);
    }
    _pDocTabBar->removeTab(index);
}

void MainWindow::exit() { close(); }

void MainWindow::undo()
{
    if (auto* ed = currentScintilla()) ed->undo();
}

void MainWindow::redo()
{
    if (auto* ed = currentScintilla()) ed->redo();
}

void MainWindow::cut()
{
    if (auto* ed = currentScintilla()) ed->cut();
}

void MainWindow::copy()
{
    if (auto* ed = currentScintilla()) ed->copy();
}

void MainWindow::paste()
{
    if (auto* ed = currentScintilla()) ed->paste();
}

void MainWindow::selectAll()
{
    if (auto* ed = currentScintilla()) ed->selectAll();
}

void MainWindow::find()
{
    if (_pFindReplaceDlg) {
        _pFindReplaceDlg->setCurrentEditor(currentScintilla());
        _pFindReplaceDlg->doDialog(FIND_DLG);
    }
}

void MainWindow::replace()
{
    if (_pFindReplaceDlg) {
        _pFindReplaceDlg->setCurrentEditor(currentScintilla());
        _pFindReplaceDlg->doDialog(REPLACE_DLG);
    }
}

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
void MainWindow::toggleWordWrap()
{
    if (auto* ed = currentScintilla()) ed->wrapText(!ed->isWrap());
}

// TODO: pending NppCommands wiring
void MainWindow::zoomIn() { _currentZoomFactor += 10; }
// TODO: pending NppCommands wiring
void MainWindow::zoomOut() { _currentZoomFactor -= 10; }
// TODO: pending NppCommands wiring
void MainWindow::resetZoom() { _currentZoomFactor = 100; }

void MainWindow::onTabChanged(int index)
{
    if (index >= 0 && index < _buffers.size())
        _pCurrentBuffer = _buffers[index];
    else
        _pCurrentBuffer = nullptr;
    // Sync current editor to FindReplaceDlg so search works on active tab
    if (_pFindReplaceDlg)
        _pFindReplaceDlg->setCurrentEditor(currentScintilla());
    updateStatusBar();
    emit documentChanged(index);
}

void MainWindow::updateStatusBar()
{
    if (!_pStatusBar) return;
    ScintillaComponent* ed = currentScintilla();
    if (!ed) return;

    // Use QsciScintilla's cursor position helpers (1-indexed line, 1-indexed column)
    int line = 0, col = 0;
    ed->getCursorPosition(&line, &col);
    if (_pPositionLabel) _pPositionLabel->setText(QString("Ln %1, Col %2").arg(line + 1).arg(col + 1));

    // Document length
    if (_pLengthLabel) _pLengthLabel->setText(QString("Len %1").arg(ed->send(SCI_GETLENGTH)));

    // Encoding
    if (_pEncodingLabel) _pEncodingLabel->setText("UTF-8");

    // EOL mode
    int eolMode = ed->eolMode();
    QString eolStr = (eolMode == npp_sci::SC_EOL_CRLF) ? "Windows (CRLF)"
                      : (eolMode == npp_sci::SC_EOL_LF) ? "Unix (LF)"
                      : (eolMode == npp_sci::SC_EOL_CR) ? "Mac (CR)" : "Windows (CRLF)";
    if (_pEolLabel) _pEolLabel->setText(eolStr);

    // Language
    QString langStr = "Normal text";
    if (_pCurrentBuffer) {
        switch (_pCurrentBuffer->getLangType()) {
            case LangType::L_CPP:     langStr = "C++"; break;
            case LangType::L_JAVA:    langStr = "Java"; break;
            case LangType::L_CS:      langStr = "C#"; break;
            case LangType::L_HTML:    langStr = "HTML"; break;
            case LangType::L_XML:     langStr = "XML"; break;
            case LangType::L_PYTHON:  langStr = "Python"; break;
            case LangType::L_JSON:    langStr = "JSON"; break;
            case LangType::L_MAKEFILE:langStr = "Makefile"; break;
            case LangType::L_INI:     langStr = "INI"; break;
            case LangType::L_BATCH:   langStr = "Batch"; break;
            case LangType::L_JAVASCRIPT: langStr = "JavaScript"; break;
            case LangType::L_PHP:     langStr = "PHP"; break;
            case LangType::L_SQL:     langStr = "SQL"; break;
            case LangType::L_RC:      langStr = "Resource"; break;
            case LangType::L_TEXT:    langStr = "Normal text"; break;
            default:                  langStr = "Normal text"; break;
        }
    }
    if (_pLanguageLabel) _pLanguageLabel->setText(langStr);
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

ScintillaComponent* MainWindow::currentScintilla() const
{
    int idx = _pDocTabBar ? _pDocTabBar->currentIndex() : -1;
    if (idx >= 0 && idx < _scintillaEditors.size())
        return _scintillaEditors[idx];
    return nullptr;
}
