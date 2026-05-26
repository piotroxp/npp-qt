#include <QApplication>
#include <QMessageBox>
#include <QStyleFactory>
#include <QPalette>
#include <QFile>
#include <QDir>
#include <QCommandLineParser>
#include <QStandardPaths>
#include <QTranslator>
#include <QLocale>
#include "MainWindow.h"
#include "NppApplication.h"

#ifdef Q_OS_LINUX
#include "X11Support.h"
#endif

int main(int argc, char* argv[])
{
    // ─── High-DPI scaling ────────────────────────────────────────────────────
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    NppApplication app(argc, argv);

    // ─── App metadata ─────────────────────────────────────────────────────────
    QCoreApplication::setApplicationName("Notepad--");
    QCoreApplication::setApplicationVersion("8.9.4");
    QCoreApplication::setOrganizationName("Notepad-- Project");
    QCoreApplication::setOrganizationDomain("notepad-minus-minus.org");
    QCoreApplication::setDesktopFileName("org.notepad_minus_minus.desktop");

    // ─── Translations ─────────────────────────────────────────────────────────
    QTranslator qtTranslator;
    qtTranslator.load(QLocale::system(),
                      "qt", "_",
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator nppTranslator;
    QString nppQmPath = QStandardPaths::locate(
        QStandardPaths::AppDataLocation,
        QString("i18n/npp-%1.qm").arg(QLocale::system().name()),
        QStandardPaths::LocateFile);
    if (!nppQmPath.isEmpty())
        nppTranslator.load(nppQmPath);
    app.installTranslator(&nppTranslator);

    // ─── CLI parser ───────────────────────────────────────────────────────────
    QCommandLineParser parser;
    parser.setApplicationDescription("Notepad-- (Qt port of Notepad++)");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption multiInstOpt("multiInst",
        QObject::tr("Allow multiple instances"));
    QCommandLineOption noPluginOpt("noPlugin",
        QObject::tr("Start without loading plugins"));
    QCommandLineOption readOnlyOpt("ro",
        QObject::tr("Open files in read-only mode"));
    QCommandLineOption sessionOpt("session",
        QObject::tr("Load a session file"), "file");
    QCommandLineOption lineNumOpt("l",
        QObject::tr("Go to line number"), "line");
    QCommandLineOption columnOpt("c",
        QObject::tr("Go to column number"), "column");
    QCommandLineOption alwaysOnTopOpt("top",
        QObject::tr("Keep window always on top"));

    parser.addOption(multiInstOpt);
    parser.addOption(noPluginOpt);
    parser.addOption(readOnlyOpt);
    parser.addOption(sessionOpt);
    parser.addOption(lineNumOpt);
    parser.addOption(columnOpt);
    parser.addOption(alwaysOnTopOpt);
    parser.addPositionalArgument("files",
        QObject::tr("Files to open"), "[files...]");

    parser.process(app);

    // ─── Load settings ────────────────────────────────────────────────────────
    app.loadSettings();

    // ─── Dark mode setup ───────────────────────────────────────────────────────
    if (app.settings()->value("Preferences/Theme/DarkMode", false).toBool()) {
        app.setStyle("Fusion");
        QPalette darkPal;
        darkPal.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPal.setColor(QPalette::WindowText, Qt::white);
        darkPal.setColor(QPalette::Base, QColor(35, 35, 35));
        darkPal.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPal.setColor(QPalette::ToolTipBase, Qt::white);
        darkPal.setColor(QPalette::ToolTipText, Qt::white);
        darkPal.setColor(QPalette::Text, Qt::white);
        darkPal.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPal.setColor(QPalette::ButtonText, Qt::white);
        darkPal.setColor(QPalette::BrightText, Qt::red);
        darkPal.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPal.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPal.setColor(QPalette::HighlightedText, Qt::black);
        app.setPalette(darkPal);
    }

    // ─── Main window ───────────────────────────────────────────────────────────
    MainWindow mainWin;
    mainWin.setReadOnly(parser.isSet(readOnlyOpt));
    mainWin.setNoPlugin(parser.isSet(noPluginOpt));

    if (parser.isSet(alwaysOnTopOpt))
        mainWin.setWindowFlags(mainWin.windowFlags() | Qt::WindowStaysOnTopHint);

    // ─── Open files / session ──────────────────────────────────────────────────
    const QStringList files = parser.positionalArguments();
    QString sessionFile;
    if (parser.isSet(sessionOpt))
        sessionFile = parser.value(sessionOpt);
    else if (!files.isEmpty())
        sessionFile = files.first();

    if (!sessionFile.isEmpty()) {
        if (sessionFile.endsWith(".xml", Qt::CaseInsensitive))
            mainWin.loadSession(sessionFile);
        else
            mainWin.openFiles(files);
    }

    // Go to line/col if specified
    if (parser.isSet(lineNumOpt)) {
        bool ok = false;
        int line = parser.value(lineNumOpt).toInt(&ok);
        if (ok)
            mainWin.activeEditor()->goToLine(line - 1,
                parser.isSet(columnOpt)
                    ? parser.value(columnOpt).toInt() - 1 : 0);
    }

    mainWin.show();
    return app.exec();
}
