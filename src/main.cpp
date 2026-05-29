#include "MainWindow.h"
#include "NppApplication.h"
#include "Parameters.h"

#include <QCommandLineParser>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    NppApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("npp-qt"));
    app.setApplicationVersion(QStringLiteral("8.9.4"));
    app.setOrganizationName(QStringLiteral("Notepad++"));
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Notepad++ Qt port"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    NppParameters::getInstance().load();
    app.loadSettings();

    MainWindow window;
    window.show();

    return app.exec();
}
