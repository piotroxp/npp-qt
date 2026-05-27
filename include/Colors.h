// Colors.h - Qt port stub for color constants
#pragma once
#include <QColor>

namespace Colors {
    // Color constants - will be populated from NppDarkMode integration
    const QColor nppRed(255, 80, 80);
    const QColor nppGreen(80, 255, 80);
    const QColor nppBlue(80, 160, 255);
    const QColor nppYellow(255, 255, 80);
    const QColor nppOrange(255, 180, 80);
    const QColor nppGray(160, 160, 160);
    const QColor nppDarkGray(80, 80, 80);

    // Editor colors
    const QColor selectionBg(50, 100, 150);
    const QColor currentLineBg(30, 40, 60);
    const QColor marginBg(45, 50, 55);

    // Status colors
    const QColor statusOk(QColor("#4CAF50"));
    const QColor statusWarning(QColor("#FF9800"));
    const QColor statusError(QColor("#F44336"));
}