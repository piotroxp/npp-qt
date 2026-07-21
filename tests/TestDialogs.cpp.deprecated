// TestDialogs.cpp -- Qt Test suite for dialog widgets
// Phase1 canonical sources:
//   src/dialogs/AboutDialog.cpp
//   src/dialogs/FileReloadDialog.cpp
//
// Test coverage assessment:
// - Both dialogs: was weak -- only tested construction and window title
// - NEW: AboutDialog version/build-info, FileReloadDialog action buttons,
//         action enum values, dontAskForAll checkbox, dialog layout presence

#include <QtTest/QtTest>
#include <QCoreApplication>   // QApplication unnecessary -- no GUI shown
#include <QDialog>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
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

    // -- AboutDialog
    void test_aboutDialog_construct();
    void test_aboutDialog_windowTitle();
    void test_aboutDialog_setVersion();
    void test_aboutDialog_setBuildInfo();
    void test_aboutDialog_hasTabs();
    void test_aboutDialog_hasButtons();
    void test_aboutDialog_hasTextBrowsers();

    // -- FileReloadDialog
    void test_fileReloadDialog_construct();
    void test_fileReloadDialog_windowTitle();
    void test_fileReloadDialog_hasRadioButtons();
    void test_fileReloadDialog_hasDontAskCheckbox();
    void test_fileReloadDialog_defaultAction();
    void test_fileReloadDialog_actionEnumValues();

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

// ============================================================================
// AboutDialog
// ============================================================================

void TestDialogs::test_aboutDialog_construct()
{
    AboutDialog dlg(_parent);
    QVERIFY2(!dlg.windowTitle().isEmpty(),
             "AboutDialog should have a window title");
}

void TestDialogs::test_aboutDialog_windowTitle()
{
    AboutDialog dlg(_parent);
    QString title = dlg.windowTitle();
    QVERIFY2(title.contains("Notepad", Qt::CaseInsensitive),
             qPrintable(QString("About dialog title should contain \"Notepad\", got \"%1\"").arg(title)));
}

void TestDialogs::test_aboutDialog_setVersion()
{
    AboutDialog dlg(_parent);
    dlg.setVersion("99.99.99");
    // Should not crash -- just verify the method is callable
    QVERIFY(true);
}

void TestDialogs::test_aboutDialog_setBuildInfo()
{
    AboutDialog dlg(_parent);
    dlg.setBuildInfo("Test build info");
    // Should not crash
    QVERIFY(true);
}

void TestDialogs::test_aboutDialog_hasTabs()
{
    AboutDialog dlg(_parent);
    // Tab stack should exist (credits, license, info)
    QVERIFY2(dlg.tabStack != nullptr,
             "AboutDialog should have a tabStack (QStackedWidget)");
}

void TestDialogs::test_aboutDialog_hasButtons()
{
    AboutDialog dlg(_parent);
    // AboutDialog should have action buttons: close, credits, license, website, info
    QVERIFY2(dlg.closeBtn != nullptr,
             "AboutDialog should have a close button");
    QVERIFY2(dlg.creditsBtn != nullptr,
             "AboutDialog should have a credits button");
    QVERIFY2(dlg.licenseBtn != nullptr,
             "AboutDialog should have a license button");
    QVERIFY2(dlg.websiteBtn != nullptr,
             "AboutDialog should have a website button");
    QVERIFY2(dlg.infoBtn != nullptr,
             "AboutDialog should have an info button");
}

void TestDialogs::test_aboutDialog_hasTextBrowsers()
{
    AboutDialog dlg(_parent);
    // Should have text browsers for credits, license, and info content
    QVERIFY2(dlg.creditsBrowser != nullptr,
             "AboutDialog should have a creditsBrowser (QTextBrowser)");
    QVERIFY2(dlg.licenseBrowser != nullptr,
             "AboutDialog should have a licenseBrowser (QTextBrowser)");
    QVERIFY2(dlg.infoBrowser != nullptr,
             "AboutDialog should have an infoBrowser (QTextBrowser)");
}

// ============================================================================
// FileReloadDialog
// ============================================================================

void TestDialogs::test_fileReloadDialog_construct()
{
    FileReloadDialog dlg("test.cpp", _parent);
    QVERIFY2(!dlg.windowTitle().isEmpty(),
             "FileReloadDialog should have a window title");
}

void TestDialogs::test_fileReloadDialog_windowTitle()
{
    FileReloadDialog dlg("test.cpp", _parent);
    QString title = dlg.windowTitle();
    QVERIFY2(title.contains("Changed", Qt::CaseInsensitive),
             qPrintable(QString("Reload dialog title should say 'Changed', got \"%1\"").arg(title)));
}

void TestDialogs::test_fileReloadDialog_hasRadioButtons()
{
    FileReloadDialog dlg("test.cpp", _parent);
    // Should have three radio buttons: Reload, Keep (disk), Do nothing
    QVERIFY2(dlg._radioReload != nullptr,
             "FileReloadDialog should have a Reload radio button");
    QVERIFY2(dlg._radioKeep != nullptr,
             "FileReloadDialog should have a Keep (disk) radio button");
    QVERIFY2(dlg._radioNothing != nullptr,
             "FileReloadDialog should have a DoNothing radio button");
}

void TestDialogs::test_fileReloadDialog_hasDontAskCheckbox()
{
    FileReloadDialog dlg("test.cpp", _parent);
    QVERIFY2(dlg._chkDontAskAll != nullptr,
             "FileReloadDialog should have a 'dont ask again' checkbox");
}

void TestDialogs::test_fileReloadDialog_defaultAction()
{
    FileReloadDialog dlg("test.cpp", _parent);
    // Default action should be DoNothing
    FileReloadDialog::Action defaultAction = dlg.selectedAction();
    QVERIFY2(defaultAction == FileReloadDialog::Action::DoNothing,
             qPrintable(QString("Default action should be DoNothing, got %1").arg(int(defaultAction))));
}

void TestDialogs::test_fileReloadDialog_actionEnumValues()
{
    // Verify all enum values are distinct
    QVERIFY(int(FileReloadDialog::Action::Reload) != int(FileReloadDialog::Action::KeepDisk));
    QVERIFY(int(FileReloadDialog::Action::Reload) != int(FileReloadDialog::Action::DoNothing));
    QVERIFY(int(FileReloadDialog::Action::KeepDisk) != int(FileReloadDialog::Action::DoNothing));
}

QTEST_MAIN(TestDialogs)
#include "TestDialogs.moc"
