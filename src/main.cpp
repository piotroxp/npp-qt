#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion")); // Modern look
    
    QApplication app(argc, argv);
    app.setApplicationName("npp-qt");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Jaisiu");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}