// StaticDialog.cpp - Qt6 base class for all Notepad--Qt dialogs
#include "StaticDialog.h"
#include <QShowEvent>
#include <QHideEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QApplication>
#include <QScreen>
#include <QCheckBox>
#include <QRadioButton>

StaticDialog::StaticDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f) {
    // Accept focus by default for dialogs
    setAttribute(Qt::WA_QuitOnClose, false);
    // Query initial DPI
    if (QGuiApplication::primaryScreen()) {
        _dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
    }
}

StaticDialog::~StaticDialog() = default;

void StaticDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    if (!_isInitialized) {
        _isInitialized = true;
        _initialRect = geometry();
    }
    emit dialogShown();
}

void StaticDialog::hideEvent(QHideEvent* event) {
    QDialog::hideEvent(event);
    emit dialogHidden();
}

void StaticDialog::moveEvent(QMoveEvent* event) {
    QDialog::moveEvent(event);
}

void StaticDialog::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
}

void StaticDialog::display(bool show) {
    if (show) {
        show();
        raise();
        activateWindow();
    } else {
        hide();
    }
}

void StaticDialog::displayEnhanced(bool show) {
    display(show);
}

bool StaticDialog::isChecked(const QString& objectName) const {
    auto* w = findChild<QWidget*>(objectName);
    if (!w) return false;
    // Try QCheckBox
    if (auto* cb = qobject_cast<QCheckBox*>(w))
        return cb->isChecked();
    // Try QRadioButton
    if (auto* rb = qobject_cast<QRadioButton*>(w))
        return rb->isChecked();
    return false;
}

void StaticDialog::setChecked(const QString& objectName, bool checked) {
    auto* w = findChild<QWidget*>(objectName);
    if (!w) return;
    if (auto* cb = qobject_cast<QCheckBox*>(w))
        cb->setChecked(checked);
    else if (auto* rb = qobject_cast<QRadioButton*>(w))
        rb->setChecked(checked);
}

void StaticDialog::updateDpi(int dpi) {
    _dpi = dpi;
}

QRect StaticDialog::childRect(const QString& objectName) const {
    auto* w = findChild<QWidget*>(objectName);
    if (!w) return QRect();
    return mapFromChild(w);
}

void StaticDialog::goToCenter() {
    if (parentWidget()) {
        QRect parentGeo = parentWidget()->geometry();
        move(parentGeo.center() - rect().center());
    } else {
        // Center on screen
        QRect screenGeo = QGuiApplication::primaryScreen()->geometry();
        move(screenGeo.center() - rect().center());
    }
}

QRect StaticDialog::dialogRect() const {
    return geometry();
}

QPoint StaticDialog::centerPoint() const {
    return rect().center();
}

QRect StaticDialog::mapChildToDialog(QWidget* child) const {
    if (!child) return QRect();
    QPoint topLeft = mapFromGlobal(child->mapToGlobal(QPoint(0, 0)));
    return QRect(topLeft, child->size());
}

void StaticDialog::redrawChild(const QString& objectName) {
    auto* w = findChild<QWidget*>(objectName);
    if (w) w->update();
}

void StaticDialog::moveForDpiChange(int newDpi) {
    Q_UNUSED(newDpi);
    // Subclasses can override to handle DPI-based repositioning
}
