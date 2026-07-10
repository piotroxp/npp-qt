// DpiManager.cpp - High-DPI awareness and scaling for Notepad--Qt
// Copyright (C) 2026 Agent Army
// GPL v3

#include "DpiManager.h"
#include <QGuiApplication>
#include <QScreen>
#include <QFont>
#include <QFile>
#include <QRegularExpression>
#include <QDebug>

DpiManager& DpiManager::instance() {
    static DpiManager instance;
    return instance;
}

DpiManager::DpiManager(QObject* parent)
    : QObject(parent)
{
    // Get the primary screen and calculate initial DPI
    QList<QScreen*> screens = QGuiApplication::screens();
    if (!screens.isEmpty()) {
        _currentScreen = screens.first();
        recalculateDpi();
    }
}

DpiManager::~DpiManager() = default;

int DpiManager::detectScreenDpi() const {
    // First try the logical DPI from Qt
    if (_currentScreen) {
        int logicalDpi = qRound(_currentScreen->logicalDotsPerInch());
        if (logicalDpi > 0) {
            return logicalDpi;
        }
    }

    // Fallback to primary screen DPI
    QList<QScreen*> screens = QGuiApplication::screens();
    if (!screens.isEmpty()) {
        int dpi = qRound(screens.first()->logicalDotsPerInch());
        if (dpi > 0) {
            return dpi;
        }
    }

    // Ultimate fallback: 96 DPI (standard)
    return baseDpi();
}

void DpiManager::recalculateDpi() {
    int oldDpi = _dpi;
    _dpi = detectScreenDpi();
    _scaleFactor = static_cast<qreal>(_dpi) / static_cast<qreal>(baseDpi());

    if (_dpi != oldDpi) {
        qDebug() << "DPI changed:" << oldDpi << "->" << _dpi
                 << "(scale factor:" << _scaleFactor << ")";
        emit dpiChanged(_dpi, _scaleFactor);
    }
}

void DpiManager::updateFromScreen(QScreen* screen) {
    if (screen && screen != _currentScreen) {
        _currentScreen = screen;
        recalculateDpi();
    }
}

void DpiManager::onScreenChanged() {
    recalculateDpi();
}

void DpiManager::onScreensChanged() {
    QList<QScreen*> screens = QGuiApplication::screens();
    if (!screens.isEmpty()) {
        updateFromScreen(screens.first());
    }
}

QString DpiManager::findScaledIcon(const QString& baseName) const {
    if (!_currentScreen) {
        return baseName;
    }

    // Try @Nx variants based on device pixel ratio
    qreal ratio = _currentScreen->devicePixelRatio();
    int targetRatio = qRound(ratio);
    if (targetRatio >= 2) {
        QString scaledPath = baseName;
        int dotIndex = scaledPath.lastIndexOf('.');
        if (dotIndex > 0) {
            scaledPath = scaledPath.left(dotIndex) + "@" + QString::number(targetRatio) + "x" +
                         scaledPath.mid(dotIndex);
            if (QFile::exists(scaledPath)) {
                return scaledPath;
            }
        }
    }

    return baseName;
}

QString DpiManager::scaleStyleSheet(const QString& styleSheet) const {
    if (qFuzzyCompare(_scaleFactor, 1.0)) {
        return styleSheet;
    }

    QString result = styleSheet;

    // Scale pixel values: match patterns like "10px", "10 px", etc.
    QRegularExpression pxRegex(R"((\b\d+(?:\.\d+)?)\s*px\b)");
    QRegularExpressionMatchIterator it = pxRegex.globalMatch(result);

    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        bool ok = false;
        double value = m.captured(1).toDouble(&ok);
        if (ok) {
            int scaledValue = scale(static_cast<int>(value));
            QString replacement = m.captured(0);
            replacement.replace(m.captured(1), QString::number(scaledValue));
            result.replace(m.capturedStart(), m.capturedLength(), replacement);
        }
    }

    return result;
}

void DpiManager::applyScaledMinimumSize(QWidget* widget, int minW, int minH) {
    if (widget) {
        widget->setMinimumSize(scale(minW), scale(minH));
    }
}

void DpiManager::retranslateUi(QWidget* widget) {
    Q_UNUSED(widget);
    // Trigger a language change which will cause retranslation
    // This is handled by the Qt translation system
}
