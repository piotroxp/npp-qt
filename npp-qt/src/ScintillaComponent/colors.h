// npp-qt: Win32→Qt6 semantic lift
// Original: PowerEditor/src/ScintillaComponent/colors.h
// Target:   npp-qt/src/ScintillaComponent/colors.h
//
// Color constants translated from Win32 RGB() macros to QRgb values.
// QRgb is a 0xAARRGGBB format (premultiplied with alpha).
// In Qt6, use QColor for color management.

#pragma once

#include <QColor>

// --- Basic colors as QRgb ---
// In Win32: COLORREF = RGB(r,g,b) → 0x00BBGGRR
// In Qt6: QRgb = qRgb(r,g,b) → 0xFFRRGGBB (with alpha FF)
// We use QColor for API compatibility, and QRgb for numeric constants.

// Red spectrum
static const QRgb red           = qRgb(0xFF, 0x00, 0x00);
static const QRgb darkRed       = qRgb(0x80, 0x00, 0x00);

// White/Grey spectrum
static const QRgb offWhite      = qRgb(0xFF, 0xFB, 0xF0);
static const QRgb white         = qRgb(0xFF, 0xFF, 0xFF);
static const QRgb grey          = qRgb(0x80, 0x80, 0x80);
static const QRgb liteGrey      = qRgb(0xC0, 0xC0, 0xC0);
static const QRgb veryLiteGrey  = qRgb(0xE0, 0xE0, 0xE0);
static const QRgb darkGrey     = qRgb(0x40, 0x40, 0x40);

// Green spectrum
static const QRgb darkGreen     = qRgb(0x00, 0x80, 0x00);
static const QRgb liteGreen     = qRgb(0x00, 0xFF, 0x00);
static const QRgb midGreen      = qRgb(0x00, 0xA0, 0x00);
static const QRgb blueGreen     = qRgb(0x00, 0x80, 0x80);

// Blue spectrum
static const QRgb liteBlueGreen = qRgb(0xAA, 0xFF, 0xC8);
static const QRgb liteBlue      = qRgb(0xA6, 0xCA, 0xF0);
static const QRgb veryLiteBlue   = qRgb(0xC4, 0xF9, 0xFD);
static const QRgb extremeLiteBlue= qRgb(0xF2, 0xF4, 0xFF);
static const QRgb darkBlue      = qRgb(0x00, 0x00, 0x80);
static const QRgb blue          = qRgb(0x00, 0x00, 0xFF);
static const QRgb darkCyan      = qRgb(0x40, 0xA0, 0xBF);

// Yellow/Orange spectrum
static const QRgb liteRed       = qRgb(0xFF, 0xAA, 0xAA);
static const QRgb yellow        = qRgb(0xFF, 0xFF, 0x00);
static const QRgb darkYellow    = qRgb(0xFF, 0xC0, 0x00);
static const QRgb lightYellow   = qRgb(0xFF, 0xFF, 0xD5);
static const QRgb yellowGreen   = qRgb(0xA0, 0xC0, 0x00);
static const QRgb orange        = qRgb(0xFF, 0x80, 0x00);

// Purple/Pink
static const QRgb purple        = qRgb(0x80, 0x00, 0xFF);
static const QRgb deepPurple    = qRgb(0x87, 0x13, 0x97);
static const QRgb extremeLitePurple= qRgb(0xF8, 0xE8, 0xFF);
static const QRgb veryLitePurple = qRgb(0xE7, 0xD8, 0xE9);
static const QRgb liteBerge      = qRgb(0xFE, 0xFC, 0xF5);
static const QRgb berge         = qRgb(0xFD, 0xF8, 0xE3);

// Brown/Other
static const QRgb brown         = qRgb(0x80, 0x40, 0x00);
static const QRgb black         = qRgb(0x00, 0x00, 0x00);
static const QRgb cyan          = qRgb(0x00, 0xFF, 0xFF);
