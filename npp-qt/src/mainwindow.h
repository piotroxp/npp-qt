#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QFontDialog>
#include <QColorDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QPageSetupDialog>
#include <QMdiArea>
#include <QTabWidget>
#include <QCompleter>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QSettings>
#include <QCloseEvent>
#include <QLabel>
#include <QInputDialog>
#include <QLineEdit>
#include <QFileInfo>
#include <QDir>

// Editor component - use QScintilla if available
#ifdef HAVE_QSCINTILLA
#include <Qsci/qsciscintilla.h>
#define EDITOR_WIDGET QsciScintilla
#else
#define EDITOR_WIDGET QPlainTextEdit
#endif

// Our highlighter
class SyntaxHighlighter;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // File operations
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void printFile();
    void pageSetup();
    void recentFilesTriggered();
    
    // Edit operations
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void find();
    void findNext();
    void replace();
    void goToLine();
    
    // View operations
    void toggleWordWrap();
    void toggleLineNumbers();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void toggleFullScreen();
    
    // Format operations
    void changeFont();
    void changeFontSize();
    void changeTextColor();
    void changeBackgroundColor();
    
    // Encoding
    void setEncoding(const QString &encoding);
    
    // Settings
    void preferences();
    
    // Window operations
    void newWindow();
    void closeTab();
    void nextTab();
    void prevTab();
    
    // Help
    void about();
    void documentation();
    
    // Editor signals
    void textChangedSlot();

private:
    void setupUi();
    void setupMenus();
    void setupToolbars();
    void setupStatusBar();
    void setupConnections();
    void updateRecentFilesMenu(QMenu *menu);
    
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveToFile(const QString &fileName);
    void updateRecentFiles(const QString &fileName);
    QStringList getRecentFiles() const;
    void saveRecentFiles(const QStringList &files);
    QString detectEncoding(const QByteArray &data);
    void updateWindowTitle();
    void setEditorContent(const QString &content);
    QString getEditorContent() const;

    // Editor widget
#ifdef HAVE_QSCINTILLA
    QsciScintilla *editor;
#else
    QPlainTextEdit *editor;
#endif

    // Highlighter - works with both
    SyntaxHighlighter *highlighter;
    
    // State
    QString currentFile;
    bool isUntitled;
    QString currentEncoding;
    
    // Recent files
    QStringList recentFiles;
    static const int MAX_RECENT_FILES = 10;
    
    // Find/replace
    QString lastSearch;
    bool caseSensitive;
    bool wholeWords;
    bool regexSearch;
    
    // View settings
    bool wordWrapEnabled;
    bool lineNumbersEnabled;
};

// Simple syntax highlighter for QPlainTextEdit (when QScintilla unavailable)
class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);
    
    void setTheme(const QString &theme);
    void highlightBlock(const QString &text) override;
    
private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    
    QVector<HighlightingRule> rules;
    QTextCharFormat keywordFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat typeFormat;
    
    void setDarkTheme();
    void setLightTheme();
};

#endif // MAINWINDOW_H
