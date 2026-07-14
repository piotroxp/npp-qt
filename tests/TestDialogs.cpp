// TestDialogs.cpp — Qt Test suite for dialog widgets
// Phase1 canonical sources:
//   src/dialogs/PreferenceDialog.cpp
//   src/dialogs/AboutDialog.cpp
//   src/dialogs/FileReloadDialog.cpp

#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QDialog>
#include "dialogs/PreferenceDialog.h"
#include "dialogs/AboutDialog.h"
#include "dialogs/FileReloadDialog.h"

class TestDialogs : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

    void test_preferenceDialog_construct();
    void test_aboutDialog_construct();
    void test_fileReloadDialog_construct();

private:
    QWidget* _parent = nullptr;
};

void TestDialogs::initTestCase()
{
    static int argc = 1;
    static char* argv[] = { const_cast<char*>("test") };
    static QCoreApplication app(argc, argv);
}

void TestDialogs::init()
{
    // Parentless widgets for dialog tests
    _parent = new QWidget();
}

void TestDialogs::test_preferenceDialog_construct()
{
    PreferenceDialog dlg;  // default parent=nullptr
    dlg.show();
    // Basic smoke test: dialog constructs and shows
}

void TestDialogs::test_aboutDialog_construct()
{
    AboutDialog dlg(_parent);
    dlg.show();
}

void TestDialogs::test_fileReloadDialog_construct()
{
    FileReloadDialog dlg("test.cpp", _parent);
    dlg.show();
}

QTEST_MAIN(TestDialogs)
#include "TestDialogs.moc"
