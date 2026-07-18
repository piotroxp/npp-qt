// TrayIcon.cpp
#include "TrayIcon.h"
#include <QMenu>
#include <QAction>
#include <QApplication>

TrayIcon::TrayIcon()
    : QObject() {
}

TrayIcon::~TrayIcon() {
    delete _trayIcon;
}

TrayIcon& TrayIcon::instance() {
    static TrayIcon inst;
    return inst;
}

void TrayIcon::init(QWidget* parent) {
    _parent = parent;
    if (!_trayIcon) {
        _trayIcon = new QSystemTrayIcon(parent);
        connect(_trayIcon, &QSystemTrayIcon::activated,
                this, &TrayIcon::onActivated);
        _trayIcon->setIcon(QIcon::fromTheme("accessories-text-editor"));
        _trayIcon->show();
    }
}

void TrayIcon::showMessage(const QString& title, const QString& msg,
                          QSystemTrayIcon::MessageIcon icon, int timeout) {
    if (_trayIcon) {
        _trayIcon->showMessage(title, msg, icon, timeout);
    }
}

void TrayIcon::setIcon(const QIcon& icon) {
    if (_trayIcon) _trayIcon->setIcon(icon);
}

void TrayIcon::quit() {
    if (_trayIcon) {
        _trayIcon->hide();
        delete _trayIcon;
        _trayIcon = nullptr;
    }
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason) {
    emit activated(reason);
}
