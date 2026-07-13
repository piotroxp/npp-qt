// npp-qt: Qt6 port of Notepad++
// Main entry point
// Semantic lift from: PowerEditor/src/winmain.cpp, Notepad_plus.cpp

#include <QApplication>
#include <QSurfaceFormat>
#include <QStyleFactory>
#include <QFile>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    // Enable high DPI scaling
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Notepad++ (Qt)");
    app.setApplicationDisplayName("Notepad++ (Qt)");
    app.setApplicationVersion("8.9.6");
    app.setOrganizationName("Notepad++");
    app.setOrganizationDomain("notepad-plus-plus.org");

    // Set default surface format for OpenGL
    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    // Load dark/light theme preference
    bool isDarkMode = false;

    // Create and show main window
    MainWindow mainWindow;
    
    if (isDarkMode) {
        mainWindow.enableDarkMode();
    }

    mainWindow.show();

    return app.exec();
}
