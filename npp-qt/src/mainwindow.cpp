#include "mainwindow.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QInputDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QToolBar>
#include <QLabel>
#include <QStatusBar>
#include <QMdiArea>
#include <QTabWidget>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QApplication>
#include <QDir>
#include <QFileInfo>

#ifdef HAVE_QSCINTILLA
#include <Qsci/qsciscintilla.h>
#endif

// ============================================================================
// C++ Keywords for syntax highlighting
static const QStringList CPP_KEYWORDS = {
    "int", "char", "void", "bool", "float", "double", "long", "short",
    "unsigned", "signed", "const", "static", "volatile", "typedef",
    "struct", "class", "union", "enum", "typedef", "namespace", "using",
    "public", "private", "protected", "virtual", "override", "final",
    "if", "else", "for", "while", "do", "switch", "case", "break", "continue",
    "return", "goto", "try", "catch", "throw", "new", "delete", "this",
    "nullptr", "true", "false", "sizeof", "typeid", "const_cast",
    "static_cast", "dynamic_cast", "reinterpret_cast", "template", "typename",
    "explicit", "friend", "inline", "extern", "register", "auto", "mutable",
    "operator", "and", "or", "not", "bitand", "bitor", "xor", "compl",
    "cin", "cout", "cerr", "clog", "std", "string", "vector", "map", "set"
};

static const QStringList CPP_TYPES = {
    "int", "char", "void", "bool", "float", "double", "long", "short",
    "size_t", "ssize_t", "int8_t", "int16_t", "int32_t", "int64_t",
    "uint8_t", "uint16_t", "uint32_t", "uint64_t", "wchar_t", "char16_t",
    "char32_t", "string", "wstring", "u16string", "u32string"
};

// ============================================================================
// SyntaxHighlighter Implementation
SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    setDarkTheme();
}

void SyntaxHighlighter::setTheme(const QString &theme)
{
    if (theme.toLower() == "light") {
        setLightTheme();
    } else {
        setDarkTheme();
    }
}

void SyntaxHighlighter::setDarkTheme()
{
    keywordFormat.setForeground(QColor("#569CD6"));
    keywordFormat.setFontWeight(QFont::Bold);
    stringFormat.setForeground(QColor("#CE9178"));
    commentFormat.setForeground(QColor("#6A9955"));
    commentFormat.setFontItalic(true);
    numberFormat.setForeground(QColor("#B5CEA8"));
    functionFormat.setForeground(QColor("#DCDCAA"));
    typeFormat.setForeground(QColor("#4EC9B0"));
}

void SyntaxHighlighter::setLightTheme()
{
    keywordFormat.setForeground(QColor("#0000FF"));
    keywordFormat.setFontWeight(QFont::Bold);
    stringFormat.setForeground(QColor("#A31515"));
    commentFormat.setForeground(QColor("#008000"));
    commentFormat.setFontItalic(true);
    numberFormat.setForeground(QColor("#098658"));
    functionFormat.setForeground(QColor("#74531F"));
    typeFormat.setForeground(QColor("#267B99"));
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    QRegularExpressionMatchIterator it;
    
    // Functions
    QRegularExpression funcExpr("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\s*\\()");
    it = funcExpr.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), functionFormat);
    }
    
    // Strings
    QRegularExpression strExpr("\"[^\"]*\"");
    it = strExpr.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), stringFormat);
    }
    
    // Char literals
    QRegularExpression charExpr("'[^']*'");
    it = charExpr.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), stringFormat);
    }
    
    // Single-line comments
    QRegularExpression slCommentExpr("//.*$");
    it = slCommentExpr.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
    }
    
    // Multi-line comments (simplified)
    QRegularExpression mlCommentExpr("/\\*[\\s\\S]*?\\*/");
    it = mlCommentExpr.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
    }
    
    // Numbers
    QRegularExpression numExpr("\\b[+-]?[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?\\b");
    it = numExpr.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), numberFormat);
    }
    
    // Preprocessor
    QRegularExpression preprocExpr("#\\w+");
    it = preprocExpr.globalMatch(text);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
    }
    
    // Keywords
    for (const QString &keyword : CPP_KEYWORDS) {
        QRegularExpression expr("\\b" + keyword + "\\b");
        it = expr.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
        }
    }
    
    // Types
    for (const QString &type : CPP_TYPES) {
        QRegularExpression expr("\\b" + type + "\\b");
        it = expr.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), typeFormat);
        }
    }
}

// ============================================================================
// MainWindow Implementation
MainWindow::MainWindow()
    : currentFile(QString())
    , isUntitled(true)
    , currentEncoding("UTF-8")
    , wordWrapEnabled(true)
    , lineNumbersEnabled(true)
    , caseSensitive(false)
    , wholeWords(false)
    , regexSearch(false)
{
    setupUi();
    setupMenus();
    setupToolbars();
    setupStatusBar();
    setupConnections();
    
    QSettings settings("Jaisiu", "npp-qt");
    recentFiles = settings.value("recentFiles", QStringList()).toStringList();
}

MainWindow::~MainWindow()
{
    QSettings settings("Jaisiu", "npp-qt");
    settings.setValue("recentFiles", recentFiles);
}

void MainWindow::setupUi()
{
    setWindowTitle("Untitled - npp-qt");
    resize(900, 600);
    
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    setCentralWidget(tabWidget);
    
    QMdiArea *mdiArea = new QMdiArea(this);
    tabWidget->addTab(mdiArea, "Document 1");
    
#ifdef HAVE_QSCINTILLA
    editor = new QsciScintilla(mdiArea);
    editor->setLexer(nullptr);
#else
    editor = new QPlainTextEdit(mdiArea);
#endif
    
    QFont font("Consolas", 10);
    editor->setFont(font);
    
#ifdef HAVE_QSCINTILLA
    editor->setUtf8(true);
    editor->setAutoIndentStyle(QsciScintilla::AiClosing);
    editor->setIndentationsUseTabs(false);
    editor->setTabWidth(4);
    editor->setIndentationGuides(true);
    editor->setWrapMode(QsciScintilla::WrapWord);
    editor->setEolVisibility(true);
    editor->setWhitespaceVisibility(QsciScintilla::WsVisibleAfterIndent);
    editor->setMarginType(0, QsciScintilla::NumberMargin);
    editor->setMarginWidth(0, "00000");
    editor->setMarginLineNumbers(0, true);
#else
    editor->setTabStopDistance(40);
    editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
#endif

    editor->setLineWrapMode(wordWrapEnabled ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    
    highlighter = new SyntaxHighlighter(editor->document());
    highlighter->setTheme("dark");
    
    mdiArea->addSubWindow(editor);
    editor->show();
}

void MainWindow::setupMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    connect(fileMenu->addAction("&New"), &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->actions().last()->setShortcut(QKeySequence::New);
    
    connect(fileMenu->addAction("&Open..."), &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->actions().last()->setShortcut(QKeySequence::Open);
    
    fileMenu->addSeparator();
    
    connect(fileMenu->addAction("&Save"), &QAction::triggered, this, &MainWindow::saveFile);
    fileMenu->actions().last()->setShortcut(QKeySequence::Save);
    
    connect(fileMenu->addAction("Save &As..."), &QAction::triggered, this, &MainWindow::saveFileAs);
    fileMenu->actions().last()->setShortcut(QKeySequence::SaveAs);
    
    fileMenu->addSeparator();
    
    QMenu *recentMenu = fileMenu->addMenu("&Recent Files");
    updateRecentFilesMenu(recentMenu);
    
    QAction *clearRecentAction = fileMenu->addAction("&Clear Recent Files");
    connect(clearRecentAction, &QAction::triggered, [recentMenu, this]() {
        recentFiles.clear();
        updateRecentFilesMenu(recentMenu);
    });
    
    fileMenu->addSeparator();
    
    connect(fileMenu->addAction("Page &Setup..."), &QAction::triggered, this, &MainWindow::pageSetup);
    connect(fileMenu->addAction("&Print..."), &QAction::triggered, this, &MainWindow::printFile);
    
    fileMenu->addSeparator();
    
    connect(fileMenu->addAction("E&xit"), &QAction::triggered, this, &QMainWindow::close);
    
    // Edit Menu
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    
    connect(editMenu->addAction("&Undo"), &QAction::triggered, this, &MainWindow::undo);
    editMenu->actions().last()->setShortcut(QKeySequence::Undo);
    
    connect(editMenu->addAction("&Redo"), &QAction::triggered, this, &MainWindow::redo);
    editMenu->actions().last()->setShortcut(QKeySequence::Redo);
    
    editMenu->addSeparator();
    
    connect(editMenu->addAction("Cu&t"), &QAction::triggered, this, &MainWindow::cut);
    editMenu->actions().last()->setShortcut(QKeySequence::Cut);
    
    connect(editMenu->addAction("&Copy"), &QAction::triggered, this, &MainWindow::copy);
    editMenu->actions().last()->setShortcut(QKeySequence::Copy);
    
    connect(editMenu->addAction("&Paste"), &QAction::triggered, this, &MainWindow::paste);
    editMenu->actions().last()->setShortcut(QKeySequence::Paste);
    
    editMenu->addSeparator();
    
    connect(editMenu->addAction("Select &All"), &QAction::triggered, this, &MainWindow::selectAll);
    editMenu->actions().last()->setShortcut(QKeySequence::SelectAll);
    
    editMenu->addSeparator();
    
    connect(editMenu->addAction("&Find..."), &QAction::triggered, this, &MainWindow::find);
    editMenu->actions().last()->setShortcut(QKeySequence::Find);
    
    connect(editMenu->addAction("Find &Next"), &QAction::triggered, this, &MainWindow::findNext);
    editMenu->actions().last()->setData(QVariant::fromValue((QKeySequence::StandardKey)3)); // FindNext
    
    connect(editMenu->addAction("&Replace..."), &QAction::triggered, this, &MainWindow::replace);
    editMenu->actions().last()->setShortcut(QKeySequence::Replace);
    
    editMenu->addSeparator();
    
    connect(editMenu->addAction("&Go To..."), &QAction ::triggered, this, &MainWindow::goToLine);
    
    // View Menu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    
    QAction *wrapAction = viewMenu->addAction("&Word Wrap");
    wrapAction->setCheckable(true);
    wrapAction->setChecked(wordWrapEnabled);
    connect(wrapAction, &QAction::triggered, this, &MainWindow::toggleWordWrap);
    
    QAction *lineNumsAction = viewMenu->addAction("&Line Numbers");
    lineNumsAction->setCheckable(true);
    lineNumsAction->setChecked(lineNumbersEnabled);
    connect(lineNumsAction, &QAction::triggered, this, &MainWindow::toggleLineNumbers);
    
    viewMenu->addSeparator();
    
    connect(viewMenu->addAction("Zoom &In"), &QAction::triggered, this, &MainWindow::zoomIn);
    connect(viewMenu->addAction("Zoom &Out"), &QAction::triggered, this, &MainWindow::zoomOut);
    connect(viewMenu->addAction("&Reset Zoom"), &QAction::triggered, this, &MainWindow::resetZoom);
    
    viewMenu->addSeparator();
    
    QAction *fullScrAction = viewMenu->addAction("&Full Screen");
    fullScrAction->setCheckable(true);
    fullScrAction->setShortcut(QKeySequence::FullScreen);
    connect(fullScrAction, &QAction::triggered, this, &MainWindow::toggleFullScreen);
    
    // Format Menu
    QMenu *formatMenu = menuBar()->addMenu("F&ormat");
    
    connect(formatMenu->addAction("&Font..."), &QAction::triggered, this, &MainWindow::changeFont);
    formatMenu->addSeparator();
    
    connect(formatMenu->addAction("&Background Color..."), &QAction::triggered, this, &MainWindow::changeBackgroundColor);
    connect(formatMenu->addAction("&Text Color..."), &QAction::triggered, this, &MainWindow::changeTextColor);
    
    // Encoding Menu
    QMenu *encMenu = menuBar()->addMenu("&Encoding");
    
    QMenu *utf8Menu = encMenu->addMenu("UTF-8");
    connect(utf8Menu->addAction("UTF-8 with BOM"), &QAction::triggered, [this]() { setEncoding("UTF-8 BOM"); });
    connect(utf8Menu->addAction("UTF-8 without BOM"), &QAction::triggered, [this]() { setEncoding("UTF-8"); });
    
    QMenu *ansiMenu = encMenu->addMenu("ANSI");
    connect(ansiMenu->addAction("Windows-1250"), &QAction::triggered, [this]() { setEncoding("Windows-1250"); });
    
    QMenu *uniMenu = encMenu->addMenu("Unicode");
    connect(uniMenu->addAction("UTF-16 LE"), &QAction::triggered, [this]() { setEncoding("UTF-16 LE"); });
    
    // Window Menu
    QMenu *windowMenu = menuBar()->addMenu("&Window");
    
    connect(windowMenu->addAction("&New Window"), &QAction::triggered, this, &MainWindow::newWindow);
    connect(windowMenu->addAction("&Close"), &QAction::triggered, this, &MainWindow::closeTab);
    windowMenu->addSeparator();
    connect(windowMenu->addAction("&Next Tab"), &QAction::triggered, this, &MainWindow::nextTab);
    connect(windowMenu->addAction("&Previous Tab"), &QAction::triggered, this, &MainWindow::prevTab);
    
    // Help Menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    
    connect(helpMenu->addAction("&Documentation"), &QAction::triggered, this, &MainWindow::documentation);
    connect(helpMenu->addAction("&About npp-qt"), &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::updateRecentFilesMenu(QMenu *menu)
{
    menu->clear();
    
    if (recentFiles.isEmpty()) {
        QAction *emptyAction = menu->addAction("(empty)");
        emptyAction->setEnabled(false);
    } else {
        for (const QString &file : recentFiles) {
            QAction *action = menu->addAction(QFileInfo(file).fileName());
            action->setData(file);
            connect(action, &QAction::triggered, [this, file]() { loadFile(file); });
        }
    }
}

void MainWindow::setupToolbars()
{
    QToolBar *toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    
    QAction *newAct = new QAction("New", this);
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    toolbar->addAction(newAct);
    
    QAction *openAct = new QAction("Open", this);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    toolbar->addAction(openAct);
    
    toolbar->addSeparator();
    
    QAction *saveAct = new QAction("Save", this);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);
    toolbar->addAction(saveAct);
    
    toolbar->addSeparator();
    
    QAction *cutAct = new QAction("Cut", this);
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
    toolbar->addAction(cutAct);
    
    QAction *copyAct = new QAction("Copy", this);
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
    toolbar->addAction(copyAct);
    
    QAction *pasteAct = new QAction("Paste", this);
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
    toolbar->addAction(pasteAct);
    
    toolbar->addSeparator();
    
    QAction *findAct = new QAction("Find", this);
    connect(findAct, &QAction::triggered, this, &MainWindow::find);
    toolbar->addAction(findAct);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
    
    QLabel *encLabel = new QLabel("UTF-8");
    encLabel->setObjectName("encodingLabel");
    statusBar()->addPermanentWidget(encLabel);
    
    QLabel *posLabel = new QLabel("Ln 1, Col 1");
    posLabel->setObjectName("positionLabel");
    statusBar()->addPermanentWidget(posLabel);
}

void MainWindow::setupConnections()
{
    connect(editor, &EDITOR_WIDGET::textChanged, this, &MainWindow::textChangedSlot);
    
#ifdef HAVE_QSCINTILLA
    connect(editor, &QsciScintilla::cursorPositionChanged, this, [this](int line, int col) {
        QLabel *posLabel = findChild<QLabel *>("positionLabel");
        if (posLabel) {
            posLabel->setText(QString("Ln %1, Col %2").arg(line + 1).arg(col + 1));
        }
    });
#else
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, [this]() {
        QTextCursor cursor = editor->textCursor();
        QLabel *posLabel = findChild<QLabel *>("positionLabel");
        if (posLabel) {
            posLabel->setText(QString("Ln %1, Col %2").arg(cursor.blockNumber() + 1).arg(cursor.columnNumber() + 1));
        }
    });
#endif
}

// ============================================================================
// File Operations
void MainWindow::newFile()
{
    if (maybeSave()) {
        currentFile.clear();
        isUntitled = true;
        editor->setPlainText(QString());
        editor->document()->setModified(false);
        setWindowTitle("Untitled - npp-qt");
    }
}

void MainWindow::openFile()
{
    if (!maybeSave()) return;
    
    QString fileName = QFileDialog::getOpenFileName(
        this, "Open File", QDir::homePath(),
        "All Files (*.*);;Text Files (*.txt);;C++ Files (*.cpp *.h);;HTML Files (*.html *.htm)"
    );
    
    if (!fileName.isEmpty()) {
        loadFile(fileName);
    }
}

bool MainWindow::saveFile()
{
    if (isUntitled) {
        return saveFileAs();
    }
    return saveToFile(currentFile);
}

bool MainWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, "Save File As", QDir::homePath(),
        "All Files (*.*);;Text Files (*.txt);;C++ Files (*.cpp *.h);;HTML Files (*.html *.htm)"
    );
    
    if (!fileName.isEmpty()) {
        return saveToFile(fileName);
    }
    return false;
}

void MainWindow::printFile()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    
    if (dialog.exec() == QDialog::Accepted) {
#ifdef HAVE_QSCINTILLA
        printer.printRange(editor);
#else
        QPainter painter;
        if (painter.begin(&printer)) {
            QFont font = editor->font();
            font.setPointSize(12);
            painter.setFont(font);
            painter.drawText(100, 100, editor->toPlainText());
            painter.end();
        }
#endif
    }
}

void MainWindow::pageSetup()
{
    QPrinter printer;
    QPageSetupDialog dialog(&printer, this);
    dialog.exec();
}

void MainWindow::recentFilesTriggered()
{
    // Handled via lambda in updateRecentFilesMenu
}

// ============================================================================
// Edit Operations
void MainWindow::undo()
{
#ifdef HAVE_QSCINTILLA
    editor->undo();
#endif
}

void MainWindow::redo()
{
#ifdef HAVE_QSCINTILLA
    editor->redo();
#endif
}

void MainWindow::cut()
{
    editor->cut();
}

void MainWindow::copy()
{
    editor->copy();
}

void MainWindow::paste()
{
    editor->paste();
}

void MainWindow::selectAll()
{
    editor->selectAll();
}

void MainWindow::find()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Find", "Find:", QLineEdit::Normal, lastSearch, &ok);
    if (ok && !text.isEmpty()) {
        lastSearch = text;
        
#ifdef HAVE_QSCINTILLA
        bool found = editor->findFirst(text, caseSensitive, wholeWords, regexSearch, false);
        if (!found) {
            QMessageBox::information(this, "Find", "Text not found: " + text);
        }
#else
        QTextDocument *doc = editor->document();
        QTextCursor cursor = doc->find(text);
        if (cursor.hasSelection()) {
            editor->setTextCursor(cursor);
        } else {
            QMessageBox::information(this, "Find", "Text not found: " + text);
        }
#endif
    }
}

void MainWindow::findNext()
{
    if (lastSearch.isEmpty()) {
        find();
        return;
    }
    
#ifdef HAVE_QSCINTILLA
    editor->findNext();
#else
    QTextDocument *doc = editor->document();
    QTextCursor cursor = doc->find(lastSearch, editor->textCursor());
    if (cursor.hasSelection()) {
        editor->setTextCursor(cursor);
    }
#endif
}

void MainWindow::replace()
{
    bool ok;
    QString searchText = QInputDialog::getText(this, "Replace", "Find:", QLineEdit::Normal, lastSearch, &ok);
    
    if (ok && !searchText.isEmpty()) {
        lastSearch = searchText;
        
        QString replaceText = QInputDialog::getText(this, "Replace", "Replace with:", QLineEdit::Normal, "", &ok);
        
        if (ok) {
            QString content = editor->toPlainText();
            content.replace(lastSearch, replaceText);
            editor->setPlainText(content);
        }
    }
}

void MainWindow::goToLine()
{
    bool ok;
    int line = QInputDialog::getInt(this, "Go to Line", "Line number:", 1, 1, editor->document()->blockCount(), 1, &ok);
    
    if (ok) {
#ifdef HAVE_QSCINTILLA
        editor->setFirstVisibleLine(line - 1);
#else
        QTextBlock block = editor->document()->findBlockByLineNumber(line - 1);
        QTextCursor cursor(block);
        editor->setTextCursor(cursor);
#endif
    }
}

// ============================================================================
// View Operations
void MainWindow::toggleWordWrap()
{
    wordWrapEnabled = !wordWrapEnabled;
    
#ifdef HAVE_QSCINTILLA
    editor->setWrapMode(wordWrapEnabled ? QsciScintilla::WrapWord : QsciScintilla::WrapNone);
#else
    editor->setLineWrapMode(wordWrapEnabled ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
#endif
}

void MainWindow::toggleLineNumbers()
{
    lineNumbersEnabled = !lineNumbersEnabled;
    
#ifdef HAVE_QSCINTILLA
    editor->setMarginLineNumbers(0, lineNumbersEnabled);
#endif
}

void MainWindow::zoomIn()
{
#ifdef HAVE_QSCINTILLA
    editor->zoomIn();
#else
    QFont font = editor->font();
    font.setPointSize(font.pointSize() + 1);
    editor->setFont(font);
#endif
}

void MainWindow::zoomOut()
{
#ifdef HAVE_QSCINTILLA
    editor->zoomOut();
#else
    QFont font = editor->font();
    if (font.pointSize() > 6) {
        font.setPointSize(font.pointSize() - 1);
        editor->setFont(font);
    }
#endif
}

void MainWindow::resetZoom()
{
#ifdef HAVE_QSCINTILLA
    editor->zoomTo(0);
#else
    QFont font = editor->font();
    font.setPointSize(10);
    editor->setFont(font);
#endif
}

void MainWindow::toggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

// ============================================================================
// Format Operations
void MainWindow::changeFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, editor->font(), this);
    if (ok) {
        editor->setFont(font);
    }
}

void MainWindow::changeFontSize()
{
    bool ok;
    int size = QInputDialog::getInt(this, "Font Size", "Size:", editor->font().pointSize(), 6, 72, 1, &ok);
    if (ok) {
        QFont font = editor->font();
        font.setPointSize(size);
        editor->setFont(font);
    }
}

void MainWindow::changeTextColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        QPalette palette = editor->palette();
        palette.setColor(QPalette::Text, color);
        editor->setPalette(palette);
    }
}

void MainWindow::changeBackgroundColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this);
    if (color.isValid()) {
        QPalette palette = editor->palette();
        palette.setColor(QPalette::Base, color);
        editor->setPalette(palette);
    }
}

// ============================================================================
// Encoding
void MainWindow::setEncoding(const QString &encoding)
{
    currentEncoding = encoding;
    QLabel *encLabel = findChild<QLabel *>("encodingLabel");
    if (encLabel) {
        encLabel->setText(encoding);
    }
    statusBar()->showMessage("Encoding: " + encoding, 3000);
}

// ============================================================================
// Settings
void MainWindow::preferences()
{
    QMessageBox::information(this, "Preferences", "Preferences dialog - coming soon");
}

// ============================================================================
// Window Operations
void MainWindow::newWindow()
{
    MainWindow *newWindow = new MainWindow();
    newWindow->show();
}

void MainWindow::closeTab()
{
    if (maybeSave()) {
        editor->setPlainText(QString());
        editor->document()->setModified(false);
        currentFile.clear();
        isUntitled = true;
        setWindowTitle("Untitled - npp-qt");
    }
}

void MainWindow::nextTab()
{
    QTabWidget *tabWidget = qobject_cast<QTabWidget *>(centralWidget());
    if (tabWidget) {
        int next = (tabWidget->currentIndex() + 1) % tabWidget->count();
        tabWidget->setCurrentIndex(next);
    }
}

void MainWindow::prevTab()
{
    QTabWidget *tabWidget = qobject_cast<QTabWidget *>(centralWidget());
    if (tabWidget) {
        int prev = tabWidget->currentIndex() - 1;
        if (prev < 0) prev = tabWidget->count() - 1;
        tabWidget->setCurrentIndex(prev);
    }
}

// ============================================================================
// Help
void MainWindow::about()
{
    QMessageBox::about(this, "About npp-qt",
        "<h3>npp-qt</h3>"
        "<p>Version 1.0.0</p>"
        "<p>A Qt/C++ port of Notepad++ functionality.</p>"
        "<p>Semantic lift from notepad-plus-plus-translation.</p>"
        "<p>Built with Qt6 and QScintilla (when available).</p>"
    );
}

void MainWindow::documentation()
{
    QMessageBox::information(this, "Documentation",
        "npp-qt Documentation\n\n"
        "Keyboard shortcuts:\n"
        "Ctrl+N - New file\n"
        "Ctrl+O - Open file\n"
        "Ctrl+S - Save file\n"
        "Ctrl+F - Find\n"
        "Ctrl+H - Replace\n"
        "Ctrl+G - Go to line"
    );
}

// ============================================================================
// Editor signals
void MainWindow::textChangedSlot()
{
    if (!isUntitled) {
        setWindowTitle("*" + windowTitle());
    }
}

// ============================================================================
// Protected/Private helpers
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::maybeSave()
{
    if (!editor->document()->isModified()) {
        return true;
    }
    
    QMessageBox::StandardButton ret = QMessageBox::question(
        this,
        "Unsaved Changes",
        "Do you want to save your changes?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );
    
    if (ret == QMessageBox::Save) {
        return saveFile();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file: " + fileName);
        return;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    QString content = in.readAll();
    file.close();
    
    // Detect encoding
    QFile rawFile(fileName);
    if (rawFile.open(QFile::ReadOnly)) {
        QByteArray data = rawFile.read(4096);
        rawFile.close();
        currentEncoding = detectEncoding(data);
    }
    
    editor->document()->setPlainText(content);
    editor->document()->setModified(false);
    currentFile = fileName;
    isUntitled = false;
    
    updateWindowTitle();
    updateRecentFiles(fileName);
    
    statusBar()->showMessage("Loaded: " + fileName, 3000);
}

bool MainWindow::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Error", "Could not save file: " + fileName);
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    out << editor->toPlainText();
    file.close();
    
    editor->document()->setModified(false);
    currentFile = fileName;
    isUntitled = false;
    
    updateWindowTitle();
    updateRecentFiles(fileName);
    
    statusBar()->showMessage("Saved: " + fileName, 3000);
    return true;
}

void MainWindow::updateRecentFiles(const QString &fileName)
{
    if (!fileName.isEmpty()) {
        recentFiles.removeAll(fileName);
        recentFiles.prepend(fileName);
        
        while (recentFiles.size() > MAX_RECENT_FILES) {
            recentFiles.removeLast();
        }
    }
}

QStringList MainWindow::getRecentFiles() const
{
    return recentFiles;
}

void MainWindow::saveRecentFiles(const QStringList &files)
{
    recentFiles = files;
    QSettings settings("Jaisiu", "npp-qt");
    settings.setValue("recentFiles", recentFiles);
}

QString MainWindow::detectEncoding(const QByteArray &data)
{
    // BOM detection
    if (data.size() >= 3 && (unsigned char)data[0] == 0xef && (unsigned char)data[1] == 0xbb && (unsigned char)data[2] == 0xbf) {
        return "UTF-8 BOM";
    }
    if (data.size() >= 2 && (unsigned char)data[0] == 0xff && (unsigned char)data[1] == 0xfe) {
        return "UTF-16 LE";
    }
    if (data.size() >= 2 && (unsigned char)data[0] == 0xfe && (unsigned char)data[1] == 0xff) {
        return "UTF-16 BE";
    }
    
    // Check for valid UTF-8
    bool isUtf8 = true;
    for (int i = 0; i < data.size() && isUtf8; i++) {
        if ((unsigned char)data[i] > 127) {
            int bytes = 0;
            if (((unsigned char)data[i] & 0xE0) == 0xC0) bytes = 2;
            else if (((unsigned char)data[i] & 0xF0) == 0xE0) bytes = 3;
            else if (((unsigned char)data[i] & 0xF8) == 0xF0) bytes = 4;
            else isUtf8 = false;
        }
    }
    
    return isUtf8 ? "UTF-8" : "ANSI";
}

void MainWindow::updateWindowTitle()
{
    QString title = isUntitled ? "Untitled" : QFileInfo(currentFile).fileName();
    if (!isUntitled && editor->document()->isModified()) {
        title = "*" + title;
    }
    title += " - npp-qt";
    setWindowTitle(title);
}

QString MainWindow::getEditorContent() const
{
    return editor->toPlainText();
}

void MainWindow::setEditorContent(const QString &content)
{
    editor->setPlainText(content);
}
