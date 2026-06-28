// Semantic Lift: Win32 Splitter → Qt6 Splitter widget
// Original: PowerEditor/src/WinControls/SplitterContainer/Splitter.h
// Target: npp-qt/src/WinControls/SplitterContainer/Splitter.h
//
// This header defines the Splitter class API and related flags/enums.
// The concrete Qt6 implementation is in SplitterContainer.h/cpp which
// combines Splitter + SplitterContainer into a single cohesive file.

#pragma once

#include <QWidget>
#include <QRect>
#include <QPoint>
#include <QPaintDevice>
#include <cstdint>

// Splitter orientation and behaviour flags
constexpr unsigned int SV_HORIZONTAL       = 0x00000001;
constexpr unsigned int SV_VERTICAL         = 0x00000002;
constexpr unsigned int SV_FIXED            = 0x00000004;
constexpr unsigned int SV_ENABLERDBLCLK    = 0x00000008;  // right dbl-click → go to end
constexpr unsigned int SV_ENABLELDBLCLK    = 0x00000010;  // left dbl-click  → go to start
constexpr unsigned int SV_RESIZEWTHPERCNT  = 0x00000020;  // resize proportionally

// Splitter message IDs (mirrors Win32 RegisterWindowMessage)
constexpr int SPLITTER_USER              = 9000;
constexpr int WM_GETSPLITTER_X           = SPLITTER_USER + 1;
constexpr int WM_GETSPLITTER_Y           = SPLITTER_USER + 2;
constexpr int WM_DOPOPUPMENU             = SPLITTER_USER + 3;
constexpr int WM_RESIZE_CONTAINER        = SPLITTER_USER + 4;

// Arrow directions (for collapse indicators)
enum class Arrow { left, up, right, down };

// Width/height query
enum class WH { height, width };

// Zone type for draw regions
enum class ZONE_TYPE { bottomRight, topLeft };

// Splitter fix mode
enum class SplitterMode : quint8 {
    DYNAMIC,
    LEFT_FIX,
    RIGHT_FIX
};

// Forward declaration — concrete class lives in SplitterContainer.h
class Splitter;
