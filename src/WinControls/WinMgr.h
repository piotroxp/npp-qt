// Semantic Lift: Win32 WinMgr → Qt6 WinMgr layout engine
// Original: PowerEditor/src/WinControls/WindowsDlg/WinMgr.h
// Target: npp-qt/src/WinControls/WinMgr.h
//
// WinMgr provides declarative row/column layout for dialogs.
// Ported to Qt6: uses QVBoxLayout/QHBoxLayout + QLayout::setSizeConstraint
// while preserving the original BEGIN_WINDOW_MAP / END_WINDOW_MAP macro API.

#pragma once

#include <QWidget>
#include <QBoxLayout>
#include <QLayout>
#include <QSizePolicy>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QDebug>
#include <vector>
#include <cassert>
#include <algorithm>

// ============================================================================
// Geometry helpers (mirrors Win32 RECT/SIZE/POINT)
// ============================================================================

inline int rectWidth(const QRect& rc) { return rc.width(); }
inline int rectHeight(const QRect& rc) { return rc.height(); }

inline QSize rectToSize(const QRect& rc) { return rc.size(); }
inline QPoint rectToPoint(const QRect& rc) { return rc.topLeft(); }
inline QPoint sizeToPoint(QSize sz) { return QPoint(sz.width(), sz.height()); }

constexpr QSize SIZEZERO{ 0, 0 };
constexpr QSize SIZEMAX{ SHRT_MAX, SHRT_MAX };

inline QSize minsize(QSize a, QSize b) {
    return QSize(std::min(a.width(), b.width()), std::min(a.height(), b.height()));
}
inline QSize maxsize(QSize a, QSize b) {
    return QSize(std::max(a.width(), b.width()), std::max(a.height(), b.height()));
}

// ============================================================================
// WINRECT — rectangle descriptor (mirrors Win32 WINRECT)
// ============================================================================

// WINRECT type flags (mirrors WRCT_*)
#define WRCT_END      0
#define WRCT_FIXED    0x0001
#define WRCT_PCT      0x0002
#define WRCT_REST     0x0003
#define WRCT_TOFIT    0x0004
#define WRCF_TYPEMASK 0x000F

// Group flags (mirrors WRCF_*)
#define WRCF_ROWGROUP 0x0010
#define WRCF_COLGROUP 0x0020
#define WRCF_ENDGROUP 0x00F0
#define WRCF_GROUPMASK 0x00F0

// Macros (mirrors Win32 WinMgr macros)
#define RCMARGINS(w, h) ((w) | ((h) << 16))
// MAKELONG: pack two 16-bit values into a 32-bit long
#define MAKELONG(a, b) (static_cast<long>((static_cast<unsigned int>(static_cast<unsigned short>(a))) | \
    (static_cast<unsigned int>(static_cast<unsigned short>(b)) << 16))

class WINRECT {
public:
    unsigned short flags = 0;
    int nID = -1;
    long param = 0;

    QRect rc; // current rectangle

    WINRECT(unsigned short f, int id, long p) : flags(f), nID(id), param(p) {}

    bool IsGroup() const { return (flags & WRCF_GROUPMASK) && (flags & WRCF_GROUPMASK) != WRCF_ENDGROUP; }
    bool IsEndGroup() const { return flags == 0 || flags == WRCF_ENDGROUP; }
    bool IsEnd() const { return flags == WRCT_END; }
    bool IsWindow() const { return nID > 0; }
    bool IsRowGroup() const { return (flags & WRCF_GROUPMASK) == WRCF_ROWGROUP; }
    bool IsColGroup() const { return (flags & WRCF_GROUPMASK) == WRCF_COLGROUP; }

    int GetHeight() const { return rc.height(); }
    int GetWidth() const { return rc.width(); }
    void SetHeight(int h) { rc.setHeight(h); }
    void SetWidth(int w) { rc.setWidth(w); }
    int HeightOrWidth(bool bHeight) const { return bHeight ? rc.height() : rc.width(); }
    void SetHeightOrWidth(int hw, bool bHeight) { bHeight ? SetHeight(hw) : SetWidth(hw); }
    int GetParam() const { return param; }
    int GetID() const { return nID; }
    QRect& GetRect() { return rc; }
    void SetRect(const QRect& r) { rc = r; }
};

// ============================================================================
// Layout macros (mirrors Win32 WinMgr macros)
// ============================================================================

#define BEGIN_WINDOW_MAP(name)    WINRECT name[] = {
#define END_WINDOW_MAP()          WINRECT(WRCT_END, -1, 0) };

#define BEGINROWS(type, id, m)    WINRECT(WRCF_ROWGROUP | (type), id, m),
#define BEGINCOLS(type, id, m)    WINRECT(WRCF_COLGROUP | (type), id, m),
#define ENDGROUP()                WINRECT(WRCF_ENDGROUP, -1, 0),

#define RCFIXED(id, val)         WINRECT(WRCT_FIXED, id, val),
#define RCPERCENT(id, val)        WINRECT(WRCT_PCT, id, val),
#define RCREST(id)                WINRECT(WRCT_REST, id, 0),
#define RCTOFIT(id)               WINRECT(WRCT_TOFIT, id, 0),
#define RCSPACE(val)              RCFIXED(-1, val)

// ============================================================================
// NMWINMGR — notification struct (mirrors Win32 NMWINMGR)
// ============================================================================

struct NMWINMGR {
    enum Code { GET_SIZEINFO = 1, SIZEBAR_MOVED = 2 };

    int code = 0;
    int idFrom = 0;

    struct SizeInfo {
        QSize szAvail{ 0, 0 };
        QSize szDesired{ 0, 0 };
        QSize szMin{ 0, 0 };
        QSize szMax{ SHRT_MAX, SHRT_MAX };
    } sizeinfo;

    bool processed = false;
};

// ============================================================================
// CWinMgr — window layout manager
// Mirrors Win32 CWinMgr: reads WINRECT tables and applies layout to Qt widgets.
// Uses QLayout internally; the macro-based table API is preserved.
// ============================================================================

class CWinMgr {
public:
    explicit CWinMgr(WINRECT* map);

    // Apply layout to a widget's current client area
    void CalcLayout(QWidget* widget);
    void CalcLayout(int cx, int cy, QWidget* widget = nullptr);
    void CalcLayout(QRect rcTotal, QWidget* widget = nullptr);

    // Set window positions from WINRECT map
    void SetWindowPositions(QWidget* widget);

    // Get window rect for a given ID
    QRect GetRect(int nID) const;

    // Find WINRECT by ID
    WINRECT* FindRect(int nID);

    // Get min/max size
    void GetMinMaxInfo(QWidget* widget, QSize& minSize, QSize& maxSize);

    // Initialize TOFIT sizes from current widget positions
    void InitToFitSizeFromCurrent(QWidget* widget);

    // Add layout to a widget
    QLayout* createLayout(QWidget* parent);

private:
    WINRECT* m_map = nullptr;

    void CalcGroup(WINRECT* group, QRect rcTotal, QWidget* widget);
    void AdjustSize(WINRECT* wrc, bool bRow, int& hwRemaining, QWidget* widget);
    void PositionRects(WINRECT* pGroup, const QRect& rcTotal, bool bRow);
    WINRECT* FindRectImpl(int nID);
};

// ============================================================================
// CWinGroupIterator — iterate entries in a group
// ============================================================================

class CWinGroupIterator {
public:
    CWinGroupIterator() : pCur(nullptr) {}
    explicit CWinGroupIterator(WINRECT* pg) { if (pg->IsGroup()) pCur = pg + 1; }

    WINRECT* Next() { if (pCur && !pCur->IsEndGroup()) { pCur = pCur + 1; } else return nullptr; return pCur; }
    WINRECT* pWINRECT() { return pCur; }
    explicit operator bool() const { return pCur != nullptr; }
    WINRECT* operator->() { return pCur; }
    WINRECT& operator*() { return *pCur; }

private:
    WINRECT* pCur = nullptr;
    friend class CWinMgr;
};
