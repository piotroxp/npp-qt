// Semantic Lift: TrayIcon — Win32 Shell_NotifyIcon + NIM_ADD → Qt6 QSystemTrayIcon
// Original: PowerEditor/src/WinControls/TrayIcon/trayIconControler.cpp (60 lines)
// Win32 → Qt6: Shell_NotifyIcon(NIM_ADD) + WM_COMMAND → QSystemTrayIcon

#include "TrayIconController.h"
#include <QIcon>
#include <QApplication>

TrayIconController::TrayIconController(QObject* parent)
    : QObject(parent)
    , _trayIcon(nullptr)
    , _parent(nullptr)
    , _isAdded(false)
{
}

TrayIconController::~TrayIconController()
{
    destroy();
}

void TrayIconController::init(QWidget* parent)
{
    _parent = parent;
    _trayIcon = new QSystemTrayIcon(this);

    connect(_trayIcon, &QSystemTrayIcon::activated,
            this, &TrayIconController::trayIconClicked);

    // Set a default icon
    _trayIcon->setIcon(QApplication::windowIcon());
    _trayIcon->setToolTip(QStringLiteral("Notepad++ (Qt)"));
}

void TrayIconController::destroy()
{
    if (_trayIcon) {
        _trayIcon->hide();
        delete _trayIcon;
        _trayIcon = nullptr;
    }
    _isAdded = false;
}

void TrayIconController::addToTray()
{
    if (_trayIcon && !_isAdded) {
        _trayIcon->show();
        _isAdded = true;
    }
}

void TrayIconController::removeFromTray()
{
    if (_trayIcon && _isAdded) {
        _trayIcon->hide();
        _isAdded = false;
    }
}

void TrayIconController::setIcon(const QString& iconPath)
{
    if (_trayIcon) {
        _trayIcon->setIcon(QIcon(iconPath));
    }
}

void TrayIconController::setToolTip(const QString& tip)
{
    if (_trayIcon) {
        _trayIcon->setToolTip(tip);
    }
}

void TrayIconController::setMenu(QMenu* menu)
{
    if (_trayIcon) {
        _trayIcon->setContextMenu(menu);
    }
}

void TrayIconController::trayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            // Single click
            break;
        case QSystemTrayIcon::DoubleClick:
            emit trayIconLeftDoubleClicked();
            break;
        case QSystemTrayIcon::MiddleClick:
            emit trayIconMiddleClicked();
            break;
        case QSystemTrayIcon::Context:
            emit trayIconRightClicked();
            break;
        default:
            break;
    }
}
