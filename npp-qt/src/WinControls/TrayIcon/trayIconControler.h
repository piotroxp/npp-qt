#pragma once

#include <QObject>
#include <QString>
#include <QSystemTrayIcon>
#include <QMenu>

class TrayIconController : public QObject
{
    Q_OBJECT
public:
    explicit TrayIconController(QObject* parent = nullptr);
    ~TrayIconController() override;

    void init(QWidget* parent);
    void destroy();

    void addToTray();
    void removeFromTray();
    void setIcon(const QString& iconPath);
    void setToolTip(const QString& tip);
    void setMenu(QMenu* menu);

    bool isAdded() const { return _isAdded; }

public slots:
    void trayIconClicked(QSystemTrayIcon::ActivationReason reason);

signals:
    void trayIconLeftDoubleClicked();
    void trayIconRightClicked();
    void trayIconMiddleClicked();

private:
    QSystemTrayIcon* _trayIcon = nullptr;
    QWidget* _parent = nullptr;
    bool _isAdded = false;
};

