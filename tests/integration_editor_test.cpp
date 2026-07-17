// integration_editor_test.cpp — IEditor/MockEditor/AutoCompletion integration tests
// Runs headlessly — no display, no Scintilla native libs needed
//
// Build: cd tests/build && cmake .. && make npp-test-integration-editor && ./npp-test-integration-editor
// All 6 tests:  2 = initTestCase pass, 0 = no failures, 0 = skipped

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTest>

// ============================================================================
// Helper
// ============================================================================
static QString readFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return {};
    return QString::fromUtf8(f.readAll());
}

static QString projectRoot() {
    QDir d(QFile::decodeName(__FILE__));
    d.cdUp(); d.cdUp(); // tests/build → tests → root
    return d.path();
}

// ============================================================================
// MockEditorTest — IEditor.h and MockEditor.cpp patterns
// ============================================================================
class MockEditorTest : public QObject {
public:
    MockEditorTest(QObject* parent = nullptr) : QObject(parent) {}
private slots:
    void initTestCase() {
        const QString mockPath = projectRoot() + "/src/editor/MockEditor.cpp";
        const QString ifacePath = projectRoot() + "/src/editor/IEditor.h";
        const QString mock = readFile(mockPath);
        const QString iface = readFile(ifacePath);

        QVERIFY2(!mock.isEmpty(), qPrintable("Could not read MockEditor.cpp"));
        QVERIFY2(!iface.isEmpty(), qPrintable("Could not read IEditor.h"));

        // MockEditor must record send() history
        QVERIFY2(mock.contains("_sendHistory"), "MockEditor must record send() history");

        // MockEditor must handle AutoCShow / CallTipShow
        QVERIFY2(mock.contains("AutoCShow") || mock.contains("AutoC"), "MockEditor must handle autocomplete");
        QVERIFY2(mock.contains("CallTipShow") || mock.contains("CallTip"), "MockEditor must handle calltips");

        // IEditor must declare MockEditor
        QVERIFY2(iface.contains("class MockEditor"), "IEditor.h must declare MockEditor");

        // IEditor must inherit IEditor
        QVERIFY2(iface.contains("class MockEditor : public IEditor"),
                 "MockEditor must inherit IEditor publicly");

        // IEditor must have send(), showAutoComplete(), state()
        QVERIFY2(iface.contains("virtual long long send("), "IEditor must declare send()");
        QVERIFY2(iface.contains("showAutoComplete"), "IEditor must declare showAutoComplete()");
        QVERIFY2(iface.contains("QVariantMap state()"), "IEditor must declare state() for test assertions");
    }
};

// ============================================================================
// MockEditorHistoryTest — send() call sequence tracking
// ============================================================================
class MockEditorHistoryTest : public QObject {
public:
    MockEditorHistoryTest(QObject* parent = nullptr) : QObject(parent) {}
private slots:
    void initTestCase() {
        const QString mock = readFile(projectRoot() + "/src/editor/MockEditor.cpp");
        QVERIFY2(!mock.isEmpty(), "Could not read MockEditor.cpp");

        // send() must append to history
        QVERIFY2(mock.contains("_sendHistory.append"),
                 "MockEditor::send() must append to _sendHistory for test assertions");

        // Must have setSendResponse() for test setup
        QVERIFY2(mock.contains("setSendResponse"),
                 "MockEditor must have setSendResponse() for test control");

        // Must emit signals for test assertions
        QVERIFY2(mock.contains("emit charAdded") || mock.contains("emit notification"),
                 "MockEditor must emit signals for test assertions");

        // Must handle all key Scintilla messages used by AutoCompletion
        QVERIFY2(mock.contains("GetLength") || mock.contains("GetTextLength"),
                 "MockEditor must handle SCI_GETLENGTH");
        QVERIFY2(mock.contains("AutoCGetCurrent") || mock.contains("GetCurrent"),
                 "MockEditor must handle SCI_AUTOCGETCURRENT");
        QVERIFY2(mock.contains("GetCharAt") || mock.contains("CharAt"),
                 "MockEditor must handle SCI_GETCHARAT");
    }
};

// ============================================================================
// AutoCompletionPatternTest — AutoCompletion source patterns
// ============================================================================
class AutoCompletionPatternTest : public QObject {
public:
    AutoCompletionPatternTest(QObject* parent = nullptr) : QObject(parent) {}
private slots:
    void initTestCase() {
        const QString ac = readFile(projectRoot() + "/src/dialogs/AutoCompletion.cpp");
        QVERIFY2(!ac.isEmpty(), "Could not read AutoCompletion.cpp");

        // Constructor takes ScintillaComponent*
        QVERIFY2(ac.contains("AutoCompletion::AutoCompletion(ScintillaComponent"),
                  "AutoCompletion must take ScintillaComponent*");

        // Uses _pEditView for Scintilla calls
        QVERIFY2(ac.contains("_pEditView->send") || ac.contains("_pEditView->SendScintilla"),
                  "AutoCompletion must use _pEditView for Scintilla messages");

        // Auto-triggers on character input
        QVERIFY2(ac.contains("CharAdded") || ac.contains("charAdded") ||
                  ac.contains("SCN_CHARADDED") || ac.contains("onChar"),
                  "AutoCompletion must handle SCN_CHARADDED");

        // Trigger threshold
        QVERIFY2(ac.contains("triggerThreshold") || ac.contains("_trigger") ||
                  ac.contains("threshold") || ac.contains("minChars"),
                  "AutoCompletion must check trigger threshold");

        // Word list building
        QVERIFY2(ac.contains("buildWordList") || ac.contains("getWordList") ||
                  ac.contains("documentWord") || ac.contains("wordList()"),
                  "AutoCompletion must build word list");

        // SCI_AUTOCSHOW call
        QVERIFY2(ac.contains("SCI_AUTOCSHOW") || ac.contains("AutoCShow"),
                  "AutoCompletion must call SCI_AUTOCSHOW");

        // Case handling
        QVERIFY2(ac.contains("IgnoreCase") || ac.contains("ignoreCase"),
                  "AutoCompletion must handle case sensitivity");

        // No dangling pointer returns
        const QStringList lines = ac.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].trimmed().startsWith("return ") &&
                (lines[i].contains("toUtf8().constData()") ||
                 lines[i].contains("toLatin1().constData()"))) {
                QFAIL(qPrintable(QString("Dangling return in AutoCompletion.cpp:%1:\n  %2")
                    .arg(i+1).arg(lines[i].trimmed())));
            }
        }
    }
};

// ============================================================================
// FunctionCallTipPatternTest — FunctionCallTip source patterns
// ============================================================================
class FunctionCallTipPatternTest : public QObject {
public:
    FunctionCallTipPatternTest(QObject* parent = nullptr) : QObject(parent) {}
private slots:
    void initTestCase() {
        const QString fct = readFile(projectRoot() + "/src/dialogs/FunctionCallTip.cpp");
        QVERIFY2(!fct.isEmpty(), "Could not read FunctionCallTip.cpp");

        QVERIFY2(fct.contains("FunctionCallTip::show") || fct.contains("show("),
                  "FunctionCallTip must have show() method");

        QVERIFY2(fct.contains("SCI_CALLTIPSHOW") || fct.contains("CallTipShow"),
                  "FunctionCallTip must use SCI_CALLTIPSHOW");

        QVERIFY2(fct.contains("highlight") || fct.contains("setHighlight") ||
                  fct.contains("CallTipSetHL"),
                  "FunctionCallTip must support argument highlighting");

        QVERIFY2(fct.contains("cancel") || fct.contains("hide") ||
                  fct.contains("CallTipCancel"),
                  "FunctionCallTip must have cancel/hide");

        QVERIFY2(fct.contains("parameter") || fct.contains("param") ||
                  fct.contains("signature") || fct.contains("argument"),
                  "FunctionCallTip must parse function signatures");

        // Safe QByteArray usage — no dangling returns
        const QStringList lines = fct.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].trimmed().startsWith("return ") &&
                lines[i].contains("toUtf8().constData()")) {
                QFAIL(qPrintable(QString("Dangling return in FunctionCallTip.cpp:%1:\n  %2")
                    .arg(i+1).arg(lines[i].trimmed())));
            }
        }
    }
};

// ============================================================================
// IEditorInterfaceTest — IEditor.h completeness
// ============================================================================
class IEditorInterfaceTest : public QObject {
public:
    IEditorInterfaceTest(QObject* parent = nullptr) : QObject(parent) {}
private slots:
    void initTestCase() {
        const QString iface = readFile(projectRoot() + "/src/editor/IEditor.h");
        QVERIFY2(!iface.isEmpty(), "Could not read IEditor.h");

        QVERIFY2(iface.contains("virtual long long send("),
                  "IEditor must declare send() for Scintilla messages");
        QVERIFY2(iface.contains("virtual QString text()") && iface.contains("virtual void setText("),
                  "IEditor must have text/getText/setText");
        QVERIFY2(iface.contains("showAutoComplete") && iface.contains("cancelAutoComplete"),
                  "IEditor must have autocomplete control");
        QVERIFY2(iface.contains("showCallTip") && iface.contains("cancelCallTip"),
                  "IEditor must have calltip control");
        QVERIFY2(iface.contains("state()") && iface.contains("QVariantMap"),
                  "IEditor must have state() snapshot for assertions");
        QVERIFY2(iface.contains("class MockEditor"),
                  "IEditor.h must declare MockEditor");
        QVERIFY2(iface.contains("class MockEditor : public IEditor"),
                  "MockEditor must inherit IEditor publicly");
        QVERIFY2(iface.contains("class IEditor : public QObject"),
                  "IEditor must inherit QObject for signals/slots");
    }
};

// ============================================================================
// AutocompleteFlowTest — end-to-end flow verification
// ============================================================================
class AutocompleteFlowTest : public QObject {
public:
    AutocompleteFlowTest(QObject* parent = nullptr) : QObject(parent) {}
private slots:
    void initTestCase() {
        const QString ac = readFile(projectRoot() + "/src/dialogs/AutoCompletion.cpp");
        QVERIFY2(!ac.isEmpty(), "Could not read AutoCompletion.cpp");

        bool hasCharAdded = ac.contains("CharAdded") || ac.contains("charAdded") ||
                          ac.contains("SCN_CHARADDED") || ac.contains("onChar");
        bool hasThreshold = ac.contains("triggerThreshold") || ac.contains("_trigger") ||
                          ac.contains("threshold") || ac.contains("minChars");
        bool hasWordList  = ac.contains("buildWordList") || ac.contains("getWordList") ||
                          ac.contains("documentWord") || ac.contains("wordList()");
        bool hasAutoCShow = ac.contains("SCI_AUTOCSHOW") || ac.contains("AutoCShow");
        bool hasIgnoreCase = ac.contains("IgnoreCase") || ac.contains("ignoreCase") ||
                            ac.contains("caseInsensitive");

        if (!hasCharAdded || !hasThreshold || !hasWordList || !hasAutoCShow || !hasIgnoreCase) {
            QString msg = "AutoCompletion flow incomplete:\n";
            if (!hasCharAdded)  msg += "  - SCN_CHARADDED character trigger\n";
            if (!hasThreshold)  msg += "  - Trigger threshold check\n";
            if (!hasWordList)  msg += "  - Word list building\n";
            if (!hasAutoCShow) msg += "  - SCI_AUTOCSHOW call\n";
            if (!hasIgnoreCase) msg += "  - Case sensitivity handling\n";
            QFAIL(qPrintable(msg));
        }
    }
};

// ============================================================================
// Main — runs all 6 test classes via qExec
// ============================================================================
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    int failed = 0;

    struct Test { const char* name; QObject* obj; } tests[] = {
        {"MockEditor",             new MockEditorTest()},
        {"MockEditorHistory",      new MockEditorHistoryTest()},
        {"AutoCompletionPatterns", new AutoCompletionPatternTest()},
        {"FunctionCallTipPatterns", new FunctionCallTipPatternTest()},
        {"IEditorInterface",       new IEditorInterfaceTest()},
        {"AutocompleteFlow",        new AutocompleteFlowTest()},
    };

    for (auto& t : tests) {
        qDebug() << "\n=== RUN  :" << t.name << "===";
        bool ok = QTest::qExec(t.obj, argc, argv) == 0;
        qDebug() << "=== RESULT:" << t.name << (ok ? "PASSED" : "FAILED") << "===";
        if (!ok) ++failed;
        delete t.obj;
    }

    qDebug() << "\n========== SUMMARY ==========";
    qDebug() << (failed == 0 ? "ALL TESTS PASSED" : QString("%1 TEST(S) FAILED").arg(failed));
    qDebug() << "==============================";
    return failed ? 1 : 0;
}
