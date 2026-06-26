// Semantic Lift: Win32 Shell_NotifyIcon → Qt6 QSystemTrayIcon
// Original: PowerEditor/src/WinControls/TrayIcon/*.{h,cpp}
// Target: npp-qt/src/WinControls/TrayIcon.cpp

#include "TrayIcon.h"
#include <QApplication>
#include <QStyle>

TrayIconController::TrayIconController(QWidget* parent, const QString& tooltip)
    : QObject(parent)
    , _parentWindow(parent)
    , _isIconShown(false)
    , _toolTip(tooltip)
{
    _trayIcon = new QSystemTrayIcon(this);
    _trayIcon->setObjectName(QStringLiteral("TrayIcon"));

    // Connect activation signal
    connect(_trayIcon, &QSystemTrayIcon::activated,
            this, &TrayIconController::handleActivation);

    // Default icon if none set — use system-standard "document" icon
    if (_icon.isNull()) {
        _icon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    }
    _trayIcon->setIcon(_icon);
    _trayIcon->setToolTip(_toolTip);
}

TrayIconController::~TrayIconController()
{
    removeFromTray();
}

void TrayIconController::addToTray()
{
    if (_isIconShown) return; // Already shown

    _trayIcon->setIcon(_icon);
    _trayIcon->setToolTip(_toolTip);
    _trayIcon->show();
    _isIconShown = true;
}

void TrayIconController::removeFromTray()
{
    if (!_isIconShown) return; // Already hidden

    _trayIcon->hide();
    _isIconShown = false;
}

bool TrayIconController::isInTray() const
{
    return _isIconShown;
}

int TrayIconController::reAddTrayIcon()
{
    if (!_isIconShown)
        return -1;

    // Hide first, then re-show
    _trayIcon->hide();
    _isIconShown = false;
    addToTray();
    return 0;
}

void TrayIconController::setToolTip(const QString& tip)
{
    _toolTip = tip;
    _trayIcon->setToolTip(tip);
}

void TrayIconController::setIcon(const QIcon& icon)
{
    _icon = icon;
    if (_isIconShown) {
        _trayIcon->setIcon(icon);
    }
}

void TrayIconController::setContextMenu(QMenu* menu)
{
    _trayIcon->setContextMenu(menu);
}

void TrayIconController::setVisible(bool visible)
{
    if (visible) {
        addToTray();
    } else {
        removeFromTray();
    }
}

void TrayIconController::handleActivation(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            emit trayIconClicked();
            break;
        case QSystemTrayIcon::DoubleClick:
            emit trayIconDoubleClicked();
            break;
        case QSystemTrayIcon::Context:
            emit trayIconRightClicked();
            break;
        case QSystemTrayIcon::MiddleClick:
            emit trayIconMiddleClicked();
            break;
        default:
            break;
    }
}
