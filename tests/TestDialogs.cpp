// TestDialogs.cpp — Qt Test suite for dialog widgets
// Phase1 canonical sources:
//   src/dialogs/AboutDialog.cpp
//   src/dialogs/FileReloadDialog.cpp

#include <QtTest/QtTest>
#include <QCoreApplication>   // QApplication unnecessary — no GUI shown
#include <QDialog>
#include "dialogs/AboutDialog.h"
#include "dialogs/FileReloadDialog.h"

static int    s_argc    = 1;
static char*  s_argv[]  = { const_cast<char*>("test") };
static QCoreApplication s_app(s_argc, s_argv);

class TestDialogs : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void test_aboutDialog_construct();
    void test_fileReloadDialog_construct();

    void cleanup()
    {
        if (_parent) {
            delete _parent;
            _parent = nullptr;
        }
    }

private:
    QWidget* _parent = nullptr;
};

void TestDialogs::init()
{
    _parent = new QWidget();
}

void TestDialogs::test_aboutDialog_construct()
{
    AboutDialog dlg(_parent);
    QVERIFY2(dlg.windowTitle().contains("Notepad"),
             "About dialog should have Notepad title");
}

void TestDialogs::test_fileReloadDialog_construct()
{
    FileReloadDialog dlg("test.cpp", _parent);
    QVERIFY2(dlg.windowTitle().contains("Changed"),
             "Reload dialog title should say 'File Changed'");
}

QTEST_MAIN(TestDialogs)
#include "TestDialogs.moc"
