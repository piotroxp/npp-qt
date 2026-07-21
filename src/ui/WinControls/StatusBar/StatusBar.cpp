// StatusBar.cpp
#include "StatusBar.h"
#include <QLabel>

StatusBar::StatusBar(QWidget* parent)
    : QStatusBar(parent) {
    _encodingLabel = new QLabel(this);
    _encodingLabel->setMinimumWidth(60);
    addPermanentWidget(_encodingLabel);

    _eolLabel = new QLabel(this);
    _eolLabel->setMinimumWidth(30);
    addPermanentWidget(_eolLabel);

    _positionLabel = new QLabel(this);
    _positionLabel->setMinimumWidth(100);
    addPermanentWidget(_positionLabel);

    _lengthLabel = new QLabel(this);
    _lengthLabel->setMinimumWidth(60);
    addPermanentWidget(_lengthLabel);

    _infoLabel = new QLabel(this);
    _infoLabel->setMinimumWidth(100);
    addWidget(_infoLabel);
}

void StatusBar::setEncoding(const QString& enc) {
    if (_encodingLabel) _encodingLabel->setText(enc);
}

void StatusBar::setEol(const QString& eol) {
    if (_eolLabel) _eolLabel->setText(eol);
}

void StatusBar::setPosition(int line, int col) {
    if (_positionLabel)
        _positionLabel->setText(tr("Ln %1, Col %2").arg(line).arg(col));
}

void StatusBar::setLength(int len) {
    if (_lengthLabel)
        _lengthLabel->setText(tr("Len %1").arg(len));
}

void StatusBar::setInfo(const QString& info) {
    if (_infoLabel) showMessage(info, 0);
}

void StatusBar::setReadOnly(bool ro) {
    if (ro) {
        showMessage(tr("Read only"), 3000);
    }
}

void StatusBar::setModified(bool modified) {
    if (modified) {
        showMessage(tr("Modified"), 0);
    } else {
        clearMessage();
    }
}
