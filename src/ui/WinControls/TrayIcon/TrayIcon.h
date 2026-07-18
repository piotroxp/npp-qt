// TrayIcon.h - Qt6 system tray icon
// INTENT: source uses Shell_NotifyIcon. Target uses QSystemTrayIcon.
#pragma once
#include <QWidget>
#include <QString>
#include <QSystemTrayIcon>
#include <QIcon>

class TrayIcon : public QObject {
    Q_OBJECT
public:
    static TrayIcon& instance();

    void init(QWidget* parent);
    void showMessage(const QString& title, const QString& msg,
                    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                    int timeout = 3000);
    void setIcon(const QIcon& icon);
    void quit();

signals:
    void activated(QSystemTrayIcon::ActivationReason reason);

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    TrayIcon();
    ~TrayIcon();

    QSystemTrayIcon* _trayIcon = nullptr;
    QWidget* _parent = nullptr;
};
