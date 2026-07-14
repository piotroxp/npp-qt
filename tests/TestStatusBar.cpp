// TestStatusBar.cpp — Qt Test suite for StatusBar widget
// Phase1 canonical source: src/ui/StatusBar.cpp

#include <QtTest/QtTest>
#include <QCoreApplication>
#include "ui/StatusBar.h"

class TestStatusBar : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

    void test_setPosition();
    void test_setEncoding();
    void test_setEol();
    void test_setLanguage();
    void test_setMessage();
    void test_setSelection();

private:
    StatusBar* _bar = nullptr;
};

void TestStatusBar::initTestCase()
{
    static int argc = 1;
    static char* argv[] = { const_cast<char*>("test") };
    static QCoreApplication app(argc, argv);
}

void TestStatusBar::init()
{
    _bar = new StatusBar();
}

void TestStatusBar::test_setPosition()
{
    _bar->setPosition(1, 10);  // line 1, col 10
    _bar->setPosition(100, 5);
}

void TestStatusBar::test_setEncoding()
{
    _bar->setEncoding("UTF-8");
    _bar->setEncoding("Windows-1252");
    _bar->setEncoding("ANSI");
}

void TestStatusBar::test_setEol()
{
    _bar->setEol("Windows (CRLF)");
    _bar->setEol("Unix (LF)");
    _bar->setEol("Mac (CR)");
}

void TestStatusBar::test_setLanguage()
{
    _bar->setLanguage("C++");
    _bar->setLanguage("Python");
    _bar->setLanguage("Normal text");
}

void TestStatusBar::test_setMessage()
{
    _bar->setMessage("Test message");
    _bar->clearMessage();
}

void TestStatusBar::test_setSelection()
{
    _bar->setSelection(0, 0);     // no selection
    _bar->setSelection(10, 2);    // 10 chars, 2 lines selected
}

QTEST_MAIN(TestStatusBar)
#include "TestStatusBar.moc"
