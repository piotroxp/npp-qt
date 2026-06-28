// Semantic Lift: Win32 ControlsTab → Qt6 implementation
// Original: PowerEditor/src/WinControls/TabBar/ControlsTab.cpp
// Target: npp-qt/src/WinControls/TabBar/ControlsTab.cpp

#include "WinControls/TabBar/ControlsTab.h"
#include "WinControls/TabBar.h"
#include <QTabBar>

// =============================================================================
// ControlsTab — constructor
// =============================================================================

ControlsTab::ControlsTab(QWidget* parent)
    : TabBar(parent)
{
    setTabsClosable(false);
    setMovable(false);
    connect(this, &QTabWidget::currentChanged,
            this, &ControlsTab::activateWindowAt);
}

// =============================================================================
// init — mirrors Win32 ControlsTab::init(HINSTANCE, HWND)
// isVertical=false, isMultiLine=false from Win32 call
// =============================================================================

void ControlsTab::init(QWidget* parent)
{
    TabBar::init(parent, false, false);
}

// =============================================================================
// createTabs — mirrors Win32 ControlsTab::createTabs(WindowVector&)
// Adds a tab for each docked panel; hides all panels except the active one.
// =============================================================================

void ControlsTab::createTabs(WindowVector& winVector)
{
    _pWinVector = &winVector;

    // Remove existing tabs (but keep the TabBar widget)
    while (count() > 0)
        removeTab(0);

    for (const DlgInfo& info : winVector)
    {
        if (info._dlg)
        {
            int index = addTab(info._name);
            Q_UNUSED(index);
            // Panel is shown/hidden by activateWindowAt
            info._dlg->display(false);
        }
    }

    if (!winVector.empty())
        activateWindowAt(0);
}

// =============================================================================
// reSizeTo — mirrors Win32 ControlsTab::reSizeTo(RECT&)
// Resizes the tab bar to fit the given rect.
// =============================================================================

void ControlsTab::reSizeTo(QRect& rc)
{
    TabBar::reSizeTo(rc);
}

// =============================================================================
// activateWindowAt — mirrors Win32 tab selection → panel visibility switch
// Hides all panels, then shows the one at `index`.
// =============================================================================

void ControlsTab::activateWindowAt(int index)
{
    if (!_pWinVector)
        return;

    int count = static_cast<int>(_pWinVector->size());
    if (index < 0 || index >= count)
        return;

    // Hide all panels
    for (int i = 0; i < count; ++i)
    {
        DlgInfo& info = (*_pWinVector)[static_cast<size_t>(i)];
        if (info._dlg)
            info._dlg->display(false);
    }

    // Show selected panel
    DlgInfo& active = (*_pWinVector)[static_cast<size_t>(index)];
    if (active._dlg)
    {
        active._dlg->display(true);
        active._dlg->getHSelf()->raise();
    }

    _current = index;
    emit windowActivated(index);
}

// =============================================================================
// renameTab — rename tab by index
// Mirrors Win32 ListView_SetItemText equivalent via tab text update.
// =============================================================================

void ControlsTab::renameTab(size_t index, const QString& newName)
{
    if (!_pWinVector || index >= _pWinVector->size())
        return;

    if (index < static_cast<size_t>(count()))
        setTabText(static_cast<int>(index), newName);

    (*_pWinVector)[index]._name = newName;
    emit windowRenamed(static_cast<int>(index), newName);
}

// =============================================================================
// renameTab — rename tab by internal name lookup
// Mirrors Win32 ControlsTab::renameTab(wchar_t*, wchar_t*).
// =============================================================================

bool ControlsTab::renameTab(const QString& internalName, const QString& newName)
{
    if (!_pWinVector)
        return false;

    for (size_t i = 0; i < _pWinVector->size(); ++i)
    {
        if ((*_pWinVector)[i]._internalName == internalName)
        {
            renameTab(i, newName);
            return true;
        }
    }
    return false;
}