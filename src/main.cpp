#include "NppApplication.h"
#include "Parameters.h"
#include "Notepad_plus_Window.h"
#include "ScintillaComponents/scintilla/qt/ScintillaEdit/ScintillaEdit.h"

#include <QCoreApplication>
#include <QEvent>
#include <QMessageBox>
#include <QSize>
#include <QString>

#include <memory>
#include <string>

int main(int argc, char* argv[])
{
    NppApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(QStringLiteral("npp-qt"));
    app.setApplicationVersion(QStringLiteral("8.9.4"));
    app.setOrganizationName(QStringLiteral("Notepad++"));

    NppParameters::getInstance().load();
    app.loadSettings();

    CmdLineParams cmdLineParams;
    std::wstring cmdLine;
    for (int i = 1; i < argc; ++i) {
        if (i > 1)
            cmdLine += L' ';
        cmdLine += QString::fromLocal8Bit(argv[i]).toStdWString();
    }

    std::unique_ptr<Notepad_plus_Window> nppWindow;
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&nppWindow]() {
        nppWindow.reset();
        if (QCoreApplication::instance())
            QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    });

    nppWindow = std::make_unique<Notepad_plus_Window>();
    try {
        nppWindow->init(reinterpret_cast<HINSTANCE>(&app), nullptr,
            cmdLine.empty() ? L"" : cmdLine.c_str(), &cmdLineParams);
    } catch (const std::exception& ex) {
        QMessageBox::critical(nullptr, QStringLiteral("npp-qt"),
            QString::fromUtf8(ex.what()));
        return 1;
    }

    if (HWND hwnd = nppWindow->getHSelf()) {
        SendMessageW(hwnd, WM_SIZE, 0, 0);
        hwnd->raise();
        hwnd->activateWindow();
        ScintillaEdit* focusEditor = nullptr;
        int bestArea = 0;
        for (ScintillaEdit* editor : hwnd->findChildren<ScintillaEdit*>()) {
            const QSize s = editor->size();
            const int area = s.width() * s.height();
            if (area > bestArea) {
                bestArea = area;
                focusEditor = editor;
            }
        }
        if (focusEditor) {
            focusEditor->raise();
            focusEditor->setFocus(Qt::OtherFocusReason);
        }
    }

    return app.exec();
}
