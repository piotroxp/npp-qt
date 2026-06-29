// Semantic Lift: Win32 Shell_NotifyIcon → Qt6 QSystemTrayIcon
// Original: PowerEditor/src/WinControls/TrayIcon/*.{h,cpp}
// Target: npp-qt/src/WinControls/TrayIcon.h

#pragma once

#include <QWidget>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QString>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QTimer>

// =============================================================================
// TrayIconController — system tray icon (lifted from trayIconControler.h)
// Translates Shell_NotifyIcon(NIM_ADD/NIM_DELETE) → QSystemTrayIcon
// =============================================================================

class TrayIconController : public QObject
{
    Q_OBJECT

public:
    // Constructor mirrors Win32: pass parent window, icon ID, callback msg, icon, tooltip
    explicit TrayIconController(QWidget* parent, const QString& tooltip = QString());
    ~TrayIconController() override;

    // Show/hide tray icon
    void addToTray();
    void removeFromTray();
    bool isInTray() const;

    // Re-add after tray icon was lost (e.g. explorer restart)
    int reAddTrayIcon();

    // Set tooltip
    void setToolTip(const QString& tip);

    // Set icon
    void setIcon(const QIcon& icon);

    // Context menu
    void setContextMenu(QMenu* menu);

    // Visibility
    void setVisible(bool visible);

    // Win32 compat stub: doTrayIcon(REMOVE) → removeFromTray()
    void doTrayIcon(int action);

    // Handle tray icon activation
    void handleActivation(QSystemTrayIcon::ActivationReason reason);

signals:
    void trayIconClicked();
    void trayIconDoubleClicked();
    void trayIconRightClicked();
    void trayIconMiddleClicked();

private:
    QSystemTrayIcon* _trayIcon = nullptr;
    QWidget* _parentWindow = nullptr;
    bool _isIconShown = false;
    QString _toolTip;
    QIcon _icon;
};
