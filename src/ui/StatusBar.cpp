// StatusBar.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "StatusBar.h"
#include <QLabel>
#include <QTimer>

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent) {
    _posLabel = new QLabel("Ln 1, Col 1", this);
    _encLabel = new QLabel("UTF-8", this);
    _eolLabel = new QLabel("Unix (LF)", this);
    _langLabel = new QLabel("Normal", this);
    _typeLabel = new QLabel("Plain Text", this);
    _msgLabel = new QLabel(this);
    _msgLabel->setStyleSheet("QLabel { color: palette(highlight); font-weight: bold; }");
    _msgLabel->setMinimumWidth(200);
    _msgLabel->setVisible(false);

    addWidget(_msgLabel, 2);
    addPermanentWidget(_posLabel, 1);
    addPermanentWidget(_encLabel, 1);
    addPermanentWidget(_eolLabel, 1);
    addPermanentWidget(_langLabel, 1);
    addPermanentWidget(_typeLabel, 1);

    setSizeGripEnabled(true);
}

StatusBar::~StatusBar() = default;

void StatusBar::setPosition(int line, int col) {
    _posLabel->setText(QString("Ln %1, Col %2").arg(line + 1).arg(col + 1));
}

void StatusBar::setEncoding(const QString& enc) {
    _encLabel->setText(enc);
}

void StatusBar::setEol(const QString& eol) {
    _eolLabel->setText(eol);
}

void StatusBar::setLanguage(const QString& lang) {
    _langLabel->setText(lang);
}

void StatusBar::setFileType(const QString& type) {
    _typeLabel->setText(type);
}

void StatusBar::setMessage(const QString& msg) {
    _msgLabel->setText(msg);
    _msgLabel->setVisible(!msg.isEmpty());
    if (!msg.isEmpty()) {
        // Auto-clear after 5 seconds using a one-shot timer
        QTimer* timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setProperty("label", QVariant::fromValue<void*>(_msgLabel));
        connect(timer, &QTimer::timeout, this, [this, timer]() {
            _msgLabel->clear();
            _msgLabel->setVisible(false);
            timer->deleteLater();
        });
        timer->start(5000);
    }
}

void StatusBar::clearMessage() {
    _msgLabel->clear();
    _msgLabel->setVisible(false);
}

void StatusBar::setSelection(int start, int end) {
    int selLength = end - start;
    if (selLength > 0) {
        _selLabel->setText(QString("Sel: %1").arg(selLength));
    } else {
        _selLabel->clear();
    }
}
