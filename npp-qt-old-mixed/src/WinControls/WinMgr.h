// Semantic Lift: Win32 WinMgr → Qt6 WinMgr layout engine
// Original: PowerEditor/src/WinControls/WindowsDlg/WinMgr.h + WinRect.cpp
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
#include <cstring>  // for memset

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
    (static_cast<unsigned int>(static_cast<unsigned short>(b)) << 16)))

class WINRECT {
public:
    // Constructor (mirrors Win32 WINRECT::WINRECT)
    WINRECT(unsigned short f = 0, int id = -1, long p = 0) {
        memset(this, 0, sizeof(WINRECT));
        flags = f;
        nID = id;
        param = p;
    }

    // Type() — returns the WRCT_* type (mirrors Win32 Type())
    unsigned short Type() const { return static_cast<unsigned short>(flags & WRCF_TYPEMASK); }

    // Navigation (mirrors Win32 next/prev pointers, now inline)
    WINRECT* Prev() { return prev; }
    WINRECT* Next() { return next; }
    WINRECT* Children() { return IsGroup() ? this + 1 : nullptr; }
    WINRECT* Parent();

    // Getters/setters (mirrors Win32)
    unsigned short GetFlags() const { return flags; }
    unsigned short SetFlags(unsigned short f) { return flags = f; }
    long GetParam() const { return param; }
    long SetParam(long p) { return param = p; }
    int GetID() const { return nID; }
    int SetID(int id) { return nID = id; }

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
    int GetParam() { return param; }  // overload for const version above
    QRect& GetRect() { return rc; }
    void SetRect(const QRect& r) { rc = r; }

    // For TOFIT types, param is the TOFIT size (mirrors Win32)
    bool HasToFitSize() const { return param != 0; }
    QSize GetToFitSize() const { return QSize(static_cast<int>(static_cast<quint16>(param & 0xFFFF)), static_cast<int>(static_cast<quint16>((param >> 16) & 0xFFFF))); }
    void SetToFitSize(QSize sz) { param = static_cast<long>((static_cast<quint32>(sz.width()) & 0xFFFF) | ((static_cast<quint32>(sz.height()) & 0xFFFF) << 16)); }

    // Margins (mirrors Win32)
    bool GetMargins(int& w, int& h) {
        if (IsGroup()) {
            w = static_cast<short>(static_cast<quint16>(param & 0xFFFF));
            h = static_cast<short>(static_cast<quint16>((param >> 16) & 0xFFFF));
            return true;
        }
        w = h = 0;
        return false;
    }

    // InitMap — set up next/prev linked list (mirrors Win32 WINRECT::InitMap)
    static WINRECT* InitMap(WINRECT* pWinMap, const WINRECT* parent = nullptr);

protected:
    // Linked-list pointers (mirrors Win32 next/prev — essential for traversal)
    WINRECT* next = nullptr;
    WINRECT* prev = nullptr;

    // Data (mirrors Win32)
    QRect rc;                     // current rectangle position/size
    unsigned short flags = 0;    // WRCT_* + WRCF_* flags
    int nID = -1;                 // window ID
    long param = 0;               // type-dependent parameter
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

    // Advance to next WINRECT in group
    CWinGroupIterator& operator++() {
        if (pCur && !pCur->IsEndGroup()) {
            if (pCur->IsGroup()) {
                // Skip into group then past it
                pCur++;
                while (!pCur->IsEndGroup()) {
                    if (pCur->IsGroup())
                        pCur++;
                    else
                        pCur++;
                }
            }
            pCur++;
        } else {
            pCur = nullptr;
        }
        return *this;
    }

    WINRECT* Next() {
        if (pCur && !pCur->IsEndGroup()) {
            if (pCur->IsGroup()) {
                pCur++;
                while (!pCur->IsEndGroup()) {
                    if (pCur->IsGroup())
                        pCur++;
                    else
                        pCur++;
                }
            }
            pCur++;
        } else {
            return nullptr;
        }
        return pCur;
    }

    WINRECT* pWINRECT() { return pCur; }
    explicit operator bool() const { return pCur != nullptr; }
    WINRECT* operator->() { return pCur; }
    WINRECT& operator*() { return *pCur; }

private:
    WINRECT* pCur = nullptr;
    friend class CWinMgr;
};
