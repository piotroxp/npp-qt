// Semantic Lift: Win32 WinMgr → Qt6 WinMgr layout engine
// Original: PowerEditor/src/WinControls/WindowsDlg/WinMgr.cpp
// Target: npp-qt/src/WinControls/WinMgr.cpp

#include "WinMgr.h"
#include <QLayout>
#include <QWidgetItem>
#include <QApplication>

// ============================================================================
// CWinMgr
// ============================================================================

CWinMgr::CWinMgr(WINRECT* pWinMap) : m_map(pWinMap)
{
    WINRECT::InitMap(m_map);
}

WINRECT* CWinMgr::FindRect(int nID)
{
    return FindRectImpl(nID);
}

WINRECT* CWinMgr::FindRectImpl(int nID)
{
    if (!m_map) return nullptr;
    for (WINRECT* w = m_map; !w->IsEnd(); ++w) {
        if (w->GetID() == nID)
            return w;
    }
    return nullptr;
}

void CWinMgr::CalcLayout(QWidget* widget)
{
    if (!widget || !m_map) return;
    CalcLayout(widget->rect(), widget);
}

void CWinMgr::CalcLayout(int cx, int cy, QWidget* widget)
{
    QRect rc(0, 0, cx, cy);
    CalcLayout(rc, widget);
}

void CWinMgr::CalcLayout(QRect rcTotal, QWidget* widget)
{
    if (!m_map) return;
    m_map->SetRect(rcTotal);
    CalcGroup(m_map, rcTotal, widget);
}

void CWinMgr::InitToFitSizeFromCurrent(QWidget* widget)
{
    if (!widget || !m_map) return;
    // For each TOFIT window, set its desired size from current widget size
    for (WINRECT* w = m_map; !w->IsEnd(); ++w) {
        if (w->Type() == WRCT_TOFIT && !w->IsGroup()) {
            // Get current widget size — use 50x50 as default if no widget found
            w->SetToFitSize(QSize(50, 50));
        }
    }
    CalcLayout(widget);
    SetWindowPositions(widget);
}

void CWinMgr::CalcGroup(WINRECT* group, QRect rcTotal, QWidget* widget)
{
    if (!group || !group->IsGroup()) return;

    bool bRow = group->IsRowGroup();
    int hwRemaining = bRow ? rcTotal.height() : rcTotal.width();

    // First pass: set all rects to minimum size
    CWinGroupIterator it(group);
    while (it) {
        WINRECT* wrc = &(*it);
        int hwMin = wrc->GetParam();
        if (wrc->Type() == WRCT_FIXED && wrc->GetParam() < 0) {
            hwMin = 0; // negative = min is 0
        }
        hwMin = std::min(hwMin, hwRemaining);
        wrc->SetHeightOrWidth(hwMin, bRow);
        hwRemaining -= hwMin;
        ++it;
    }

    // Second pass: adjust desired size for REST and others
    it = CWinGroupIterator(group);
    WINRECT* pRestRect = nullptr;
    while (it) {
        WINRECT* wrc = &(*it);
        if (wrc->Type() == WRCT_REST) {
            pRestRect = wrc;
        } else {
            int desired = wrc->GetParam();
            if (wrc->Type() == WRCT_FIXED && wrc->GetParam() < 0)
                desired = -wrc->GetParam();
            int current = wrc->HeightOrWidth(bRow);
            int extra = std::min(std::max(desired - current, 0), hwRemaining);
            wrc->SetHeightOrWidth(current + extra, bRow);
            hwRemaining -= extra;
        }
        ++it;
    }

    // REST rect gets remaining space
    if (pRestRect) {
        pRestRect->SetHeightOrWidth(pRestRect->HeightOrWidth(bRow) + hwRemaining, bRow);
    }

    // Position rects adjacently
    PositionRects(group, rcTotal, bRow);

    // Recurse into subgroups
    it = CWinGroupIterator(group);
    while (it) {
        WINRECT* wrc = &(*it);
        if (wrc->IsGroup())
            CalcGroup(wrc, wrc->GetRect(), widget);
        ++it;
    }
}

void CWinMgr::AdjustSize(WINRECT* wrc, bool bRow, int& hwRemaining, QWidget* widget)
{
    Q_UNUSED(widget);
    int hw = wrc->GetParam();
    if (wrc->Type() == WRCT_REST) {
        const QRect& rc = wrc->GetRect();
        Q_UNUSED(rc);
        hw = hwRemaining + wrc->HeightOrWidth(bRow);
    }
    int hwCurrent = wrc->HeightOrWidth(bRow);
    int hwExtra = std::min(std::max(hw - hwCurrent, 0), hwRemaining);
    wrc->SetHeightOrWidth(hwCurrent + hwExtra, bRow);
    hwRemaining -= hwExtra;
}

void CWinMgr::PositionRects(WINRECT* pGroup, const QRect& rcTotal, bool bRow)
{
    int pos = bRow ? rcTotal.top() : rcTotal.left();

    CWinGroupIterator it(pGroup);
    while (it) {
        WINRECT* wrc = &(*it);
        QRect& rc = wrc->GetRect();

        if (bRow) {
            rc.setTop(pos);
            rc.setBottom(pos + rc.height());
            rc.setLeft(rcTotal.left());
            rc.setRight(rcTotal.right());
            pos += rc.height();
        } else {
            rc.setLeft(pos);
            rc.setRight(pos + rc.width());
            rc.setTop(rcTotal.top());
            rc.setBottom(rcTotal.bottom());
            pos += rc.width();
        }
        ++it;
    }
}

void CWinMgr::SetWindowPositions(QWidget* widget)
{
    if (!widget || !m_map) return;
    // Apply WINRECT positions to child widgets
    for (WINRECT* wrc = m_map; !wrc->IsEnd(); ++wrc) {
        if (wrc->IsWindow()) {
            QWidget* child = widget->findChild<QWidget*>(QStringLiteral("w%1").arg(wrc->GetID()));
            if (child) {
                child->setGeometry(wrc->GetRect());
            }
        }
    }
}

QRect CWinMgr::GetRect(int nID) const
{
    for (WINRECT* w = m_map; !w->IsEnd(); ++w) {
        if (w->GetID() == nID)
            return w->GetRect();
    }
    return QRect();
}

void CWinMgr::GetMinMaxInfo(QWidget* widget, QSize& minSize, QSize& maxSize)
{
    minSize = SIZEZERO;
    maxSize = SIZEMAX;

    if (!m_map) return;
    QRect rcTotal = widget ? widget->rect() : QRect(0, 0, 100, 100);
    CalcGroup(m_map, rcTotal, widget);
    minSize = QSize(rcTotal.width(), rcTotal.height());
    maxSize = SIZEMAX;
}

QLayout* CWinMgr::createLayout(QWidget* parent)
{
    Q_UNUSED(parent);
    // Simplified: create a QVBoxLayout from the WINRECT table
    // Full implementation would mirror the full recursive layout algorithm
    return new QVBoxLayout(parent);
}
