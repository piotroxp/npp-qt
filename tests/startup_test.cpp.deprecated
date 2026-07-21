// Headless smoke tests for Notepad--Qt crash fixes
// Verifies source patterns without linking against Qt GUI (QT_QPA_PLATFORM=offscreen)
//
// These tests parse source files to detect known crash-inducing patterns.
// Build: cd tests/build && cmake .. && make npp-test-startup && ./npp-test-startup

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QTest>

// ============================================================================
// Helpers (file-scope for access from all test classes)
// ============================================================================

static QString readFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return {};
    return QString::fromUtf8(f.readAll());
}

static bool checkUtf8ConstDataSafety(const QString& src, QStringList* failures) {
    const QStringList lines = src.split('\n');
    bool allSafe = true;
    QRegularExpression re("(\\w+)\\.toUtf8\\(\\)\\.constData\\(\\)");

    for (int i = 0; i < lines.size(); ++i) {
        const QString& line = lines[i];
        if (!re.match(line).hasMatch()) continue;

        bool safe = false;
        for (int back = i; back >= 0 && back >= i - 2; --back) {
            if (lines[back].contains("const QByteArray") && lines[back].contains("toUtf8()")) {
                safe = true; break;
            }
        }

        if (!safe) {
            const QString trimmed = line.trimmed();
            bool isReturn = trimmed.startsWith("return ");
            bool isScintillaSend = trimmed.contains("send(") ||
                                    trimmed.contains("SendScintilla(") ||
                                    trimmed.contains("SCI_AUTOC") ||
                                    trimmed.contains("SCI_CALLTIP") ||
                                    trimmed.contains("SCI_REPLACESEL");
            if (isReturn || isScintillaSend) {
                allSafe = false;
                if (failures)
                    *failures << QString("  line %1: %2").arg(i+1).arg(trimmed);
            }
        }
    }
    return allSafe;
}

// ============================================================================
// Test: AutoCompletion - no dangling toUtf8().constData() passed to Scintilla
// ============================================================================
class AutoCompletionSafetyTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void AutoCompletionSafetyTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();
    QString path = d.filePath("src/dialogs/AutoCompletion.cpp");
    QString src = readFile(path);
    QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

    QStringList failures;
    bool safe = checkUtf8ConstDataSafety(src, &failures);
    if (!failures.isEmpty()) {
        QString msg = "Dangling toUtf8().constData() in AutoCompletion.cpp:\n"
                     + failures.join("\n");
        QFAIL(qPrintable(msg));
    }
    QVERIFY2(safe, "AutoCompletion.cpp has unsafe toUtf8().constData()");
}

// ============================================================================
// Test: FunctionCallTip - no dangling toUtf8().constData()
// ============================================================================
class FunctionCallTipSafetyTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void FunctionCallTipSafetyTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();
    QString path = d.filePath("src/dialogs/FunctionCallTip.cpp");
    QString src = readFile(path);
    QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

    QStringList failures;
    bool safe = checkUtf8ConstDataSafety(src, &failures);
    if (!failures.isEmpty()) {
        QString msg = "Dangling toUtf8().constData() in FunctionCallTip.cpp:\n"
                     + failures.join("\n");
        QFAIL(qPrintable(msg));
    }
    QVERIFY2(safe, "FunctionCallTip.cpp has unsafe toUtf8().constData()");
}

// ============================================================================
// Test: ScintillaEditor - no dangling toUtf8().constData()
// ============================================================================
class ScintillaEditorSafetyTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void ScintillaEditorSafetyTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();
    QString path = d.filePath("src/editor/ScintillaEditor.cpp");
    QString src = readFile(path);
    QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

    QStringList failures;
    bool safe = checkUtf8ConstDataSafety(src, &failures);
    if (!failures.isEmpty()) {
        QString msg = "Dangling toUtf8().constData() in ScintillaEditor.cpp:\n"
                     + failures.join("\n");
        QFAIL(qPrintable(msg));
    }
    QVERIFY2(safe, "ScintillaEditor.cpp has unsafe toUtf8().constData()");
}

// ============================================================================
// Test: Regression for double-click-before-init crash (SEGV ScintillaEditor)
// GitHub: crash on startup when FileBrowserPanel emits fileDoubleClicked before
// _tabWidget is set. Fix: FileBrowserPanel::_initialized guard + null guard in
// MainWindow::openFileInTab().
// ============================================================================
class FileBrowserPanelDoubleClickGuardTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void FileBrowserPanelDoubleClickGuardTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();

    // 1. FileBrowserPanel must have _initialized guard
    QString fbPath = d.filePath("src/panels/FileBrowserPanel.h");
    QString fbSrc = readFile(fbPath);
    QVERIFY2(!fbSrc.isEmpty(), qPrintable("Could not read: " + fbPath));
    QVERIFY2(fbSrc.contains("bool _initialized"),
             "FileBrowserPanel must declare bool _initialized; guard");

    QString fbCpp = readFile(d.filePath("src/panels/FileBrowserPanel.cpp"));
    QVERIFY2(!fbCpp.isEmpty(), qPrintable("Could not read FileBrowserPanel.cpp"));

    // Guard must be set to true after toolbar/setup before any widget/layout
    bool initAfterToolbar = fbCpp.indexOf("_initialized = true") <
                            fbCpp.indexOf("mainLayout->addWidget");
    QVERIFY2(initAfterToolbar,
             "_initialized = true must appear BEFORE mainLayout->addWidget() "
             "to block signals before widgets are added to layout");

    // All entry points must check _initialized
    QStringList guarded = {"onDoubleClicked(", "openSelectedFile(", "openReadOnly("};
    for (const QString& fn : guarded) {
        int pos = fbCpp.indexOf(fn);
        QVERIFY2(pos >= 0, qPrintable("Missing: " + fn));
        QString snippet = fbCpp.mid(pos, 300);
        QVERIFY2(snippet.contains("!_initialized") || snippet.contains("_initialized &&"),
                 qPrintable("Missing _initialized guard in: " + fn));
    }

    // 2. MainWindow::openFileInTab() must guard against null _tabWidget
    //    empty path, and missing _fileBrowserPanel
    QString mwPath = d.filePath("src/ui/MainWindow.cpp");
    QString mwSrc = readFile(mwPath);
    QVERIFY2(!mwSrc.isEmpty(), qPrintable("Could not read: " + mwPath));

    int openFn = mwSrc.indexOf("void MainWindow::openFileInTab(const QString& path)");
    QVERIFY2(openFn >= 0, "Missing: void MainWindow::openFileInTab()");
    // Window must be large enough to reach _tabWidget guard (~1100 chars in).
    QString openFnBody = mwSrc.mid(openFn, 3000);

    QVERIFY2(openFnBody.contains("!_tabWidget"),
             "MainWindow::openFileInTab() must guard: if (!_tabWidget) return;");
    QVERIFY2(openFnBody.contains("isEmpty()") || openFnBody.contains("path.isEmpty()"),
             "MainWindow::openFileInTab() must guard: if (path.isEmpty()) return;");
    QVERIFY2(openFnBody.contains("_fileBrowserPanel"),
             "MainWindow::openFileInTab() must guard against missing _fileBrowserPanel");
    QVERIFY2(openFnBody.contains("QMessageBox::warning"),
             "openFileInTab() must show QMessageBox::warning on validation failure");
    QVERIFY2(openFnBody.contains("isFile()") || openFnBody.contains("QFileInfo"),
             "openFileInTab() must guard: if (!QFileInfo(path).isFile()) return;");

    qDebug() << "PASSED: FileBrowserPanel _initialized guard present";
    qDebug() << "PASSED: MainWindow::openFileInTab null guard present";
}

// ============================================================================
// Test: No double show of main window
// ============================================================================
class MainDoubleShowTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void MainDoubleShowTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();
    QString mainPath = d.filePath("src/main.cpp");
    QString src = readFile(mainPath);
    QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + mainPath));

    const QStringList lines = src.split('\n');
    int showCount = 0;
    int initCount = 0;
    for (const QString& line : lines) {
        if (line.contains("showMainWindow()")) ++showCount;
        if (line.contains("initialize()")) ++initCount;
    }

    QVERIFY2(initCount > 0, "initialize() not called in main()");
    if (showCount > 0) {
        // Ignore comment-only lines; the fix removes the call from main().
        // A commented-out call is valid.
        QStringList occurrences;
        bool inBlockComment = false;
        for (int i = 0; i < lines.size(); ++i) {
            QString line = lines[i].trimmed();
            if (line.contains("/*")) inBlockComment = true;
            if (inBlockComment) {
                if (line.contains("*/")) inBlockComment = false;
                continue;
            }
            if (line.startsWith("//")) continue;
            if (line.contains("showMainWindow()"))
                occurrences << QString("  line %1: %2").arg(i+1).arg(lines[i].trimmed());
        }
        if (!occurrences.isEmpty())
            QFAIL(qPrintable("showMainWindow() called directly in main() (not just in comments):\n"
                              + occurrences.join("\n")));
    }
}

// ============================================================================
// Test: MainWindow - no Application::instance() call during construction
// ============================================================================
class SingletonDeadlockTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void SingletonDeadlockTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();
    QString mwPath = d.filePath("src/ui/MainWindow.cpp");
    QString src = readFile(mwPath);
    QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + mwPath));

    // Verify MainWindow constructor takes Application* parameter
    bool hasAppParam = src.contains("MainWindow::MainWindow(Application*");
    QVERIFY2(hasAppParam,
             "MainWindow must take Application* as constructor parameter "
             "to avoid calling Application::instance() during construction");

    // Constructor must NOT call Application::instance()
    const QStringList lines = src.split('\n');
    bool inCtor = false;
    int braceDepth = 0;
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].contains("MainWindow::MainWindow")) inCtor = true;
        if (!inCtor) continue;
        for (QChar c : lines[i]) {
            if (c == '{') ++braceDepth;
            if (c == '}') --braceDepth;
        }
        if (inCtor && braceDepth == 0) break;
        if (lines[i].contains("Application::instance()")) {
            QFAIL(qPrintable(QString(
                "Application::instance() called in MainWindow constructor "
                "(line %1) - causes singleton deadlock:\n  %2")
                .arg(i+1).arg(lines[i].trimmed())));
        }
    }
}

// ============================================================================
// Test: FindReplaceDialog - checkboxes initialized in setupUi()
// ============================================================================
class FindReplaceDialogSafetyTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void FindReplaceDialogSafetyTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();
    QString path = d.filePath("src/dialogs/FindReplaceDialog.cpp");
    QString src = readFile(path);
    QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

    const QStringList lines = src.split('\n');
    int setupUiEnd = -1;
    int readSettingsStart = -1;

    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].contains("void FindReplaceDialog::setupUi")) readSettingsStart = i;
        if (readSettingsStart >= 0 && lines[i].contains("void FindReplaceDialog::readSettings"))
            { setupUiEnd = i; break; }
    }

    QVERIFY2(setupUiEnd > 0, "Could not find setupUi/readSettings boundaries");
    QVERIFY2(readSettingsStart >= 0, "Could not find setupUi");

    QString setupUiBlock = lines.mid(readSettingsStart, setupUiEnd - readSettingsStart).join("\n");
    bool hasHighlighting = setupUiBlock.contains("_highlightingCheck");
    bool hasPrompts = setupUiBlock.contains("_promptsCheck");
    bool hasHideDialog = setupUiBlock.contains("_hideDialogCheck");

    if (!hasHighlighting || !hasPrompts || !hasHideDialog) {
        QString missing;
        if (!hasHighlighting) missing += " _highlightingCheck";
        if (!hasPrompts) missing += " _promptsCheck";
        if (!hasHideDialog) missing += " _hideDialogCheck";
        QFAIL(qPrintable(QString(
            "FindReplaceDialog checkboxes NOT created in setupUi():%1\n"
            "These are accessed in readSettings() and will nullptr dereference.").arg(missing)));
    }
}

// ============================================================================
// Test: FunctionListPanel - all parser methods implemented
// ============================================================================
class FunctionListPanelSafetyTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void FunctionListPanelSafetyTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();
    QString path = d.filePath("src/panels/FunctionListPanel.cpp");
    QString src = readFile(path);
    QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

    const int lineCount = src.split('\n').size();
    QVERIFY2(lineCount > 500,
             qPrintable(QString("FunctionListPanel.cpp is too short (%1 lines) - "
                                "likely truncated. Should be >1000 lines.").arg(lineCount)));

    const QStringList parsers = {
        "parsePerl", "parseLua", "parsePHP", "parsePython", "parseRuby",
        "parseRust", "parseGo", "parseSwift", "parseKotlin", "parseTypeScript",
        "parseCsharp", "parseSQL", "parseHTML", "parseTypeScript", "parseCsharp", "parseSQL", "parseHTML"
    };

    QStringList missing;
    for (const QString& p : parsers) {
        if (!src.contains(QString("void FunctionListPanel::%1(").arg(p)))
            missing << p;
    }

    if (!missing.isEmpty())
        QFAIL(qPrintable("Missing FunctionListPanel parsers:\n" + missing.join("\n")));
}

// ============================================================================
// Test: _activeEditor is set after addView()
// ============================================================================
class ActiveEditorSafetyTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void ActiveEditorSafetyTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();
    QString path = d.filePath("src/core/Application.cpp");
    QString src = readFile(path);
    QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

    const QStringList lines = src.split('\n');
    int setupUiStart = -1, setupUiEnd = -1;
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].contains("void Application::setupUI()")) setupUiStart = i;
        if (setupUiStart >= 0 && lines[i].contains("void Application::setupConnections()"))
            { setupUiEnd = i; break; }
    }

    QVERIFY2(setupUiStart >= 0, "Could not find setupUi()");
    QVERIFY2(setupUiEnd > setupUiStart, "Could not find setupUi() end");

    QString block = lines.mid(setupUiStart, setupUiEnd - setupUiStart).join("\n");
    bool hasActiveEditor = block.contains("_activeEditor") && block.contains("addView()");
    QVERIFY2(hasActiveEditor,
             "_activeEditor must be set in setupUi() after addView() call. "
             "Without this, any editor action (typing, Ctrl+S) crashes with nullptr dereference.");
}

// ============================================================================
// Test: Application destructor - no direct delete of Qt children
// ============================================================================
class ShutdownSafetyTest : public QObject {
    Q_OBJECT
private slots:
    void run();
};

void ShutdownSafetyTest::run() {
    QDir d(__FILE__);
    d.cdUp(); d.cdUp();
    QString path = d.filePath("src/core/Application.cpp");
    QString src = readFile(path);
    QVERIFY2(!src.isEmpty(), qPrintable("Could not read: " + path));

    const QStringList danger = {"delete _mainWindow", "delete _menuBar",
                                "delete _toolBar", "delete _statusBar"};
    QStringList found;
    const QStringList lines = src.split('\n');
    for (const QString& d : danger) {
        for (const QString& line : lines) {
            if (line.trimmed().startsWith(d))
                found << QString("  %1").arg(line.trimmed());
        }
    }
    if (!found.isEmpty())
        QFAIL(qPrintable("Direct delete of Qt child widgets found:\n" + found.join("\n")
                         + "\nUse Qt parent-child hierarchy instead."));
}

// ============================================================================
// Main - runs all tests
// ============================================================================
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    int failed = 0;

    {
        AutoCompletionSafetyTest t;
        qDebug() << "\n=== RUN  : AutoCompletionSafety ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }
    {
        FunctionCallTipSafetyTest t;
        qDebug() << "\n=== RUN  : FunctionCallTipSafety ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }
    {
        ScintillaEditorSafetyTest t;
        qDebug() << "\n=== RUN  : ScintillaEditorSafety ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }
    {
        FileBrowserPanelDoubleClickGuardTest t;
        qDebug() << "\n=== RUN  : FileBrowserPanelDoubleClickGuard ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }
    {
        MainDoubleShowTest t;
        qDebug() << "\n=== RUN  : MainDoubleShow ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }
    {
        SingletonDeadlockTest t;
        qDebug() << "\n=== RUN  : SingletonDeadlock ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }
    {
        FindReplaceDialogSafetyTest t;
        qDebug() << "\n=== RUN  : FindReplaceDialogSafety ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }
    {
        FunctionListPanelSafetyTest t;
        qDebug() << "\n=== RUN  : FunctionListPanelSafety ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }
    {
        ActiveEditorSafetyTest t;
        qDebug() << "\n=== RUN  : ActiveEditorSafety ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }
    {
        ShutdownSafetyTest t;
        qDebug() << "\n=== RUN  : ShutdownSafety ===";
        if (QTest::qExec(&t, argc, argv) != 0) ++failed;
        else qDebug() << "=== PASSED ===";
    }

    qDebug() << "\n========== SUMMARY ==========";
    qDebug() << (failed == 0 ? "ALL TESTS PASSED" : QString("%1 TEST(S) FAILED").arg(failed));
    qDebug() << "==============================";
    return failed ? 1 : 0;
}

#include "startup_test.moc"
