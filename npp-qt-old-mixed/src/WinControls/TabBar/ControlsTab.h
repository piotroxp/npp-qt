// Semantic Lift: Win32 ControlsTab → Qt6 implementation
// Original: PowerEditor/src/WinControls/TabBar/ControlsTab.h
// Target: npp-qt/src/WinControls/TabBar/ControlsTab.h
//
// ControlsTab is a TabBar that manages multiple docked panel windows.
// Mirrors Win32 TabBar-based panel switcher for Document Map, Function List, etc.

#pragma once

#include "WinControls/TabBar.h"
#include <QWidget>
#include <QString>
#include <QRect>
#include <vector>

// =============================================================================
// DlgInfo — wrapper for a docked panel window
// Mirrors Win32 DlgInfo struct.
// =============================================================================

struct DlgInfo {
    IWindow* _dlg = nullptr;
    QString _name;
    QString _internalName;

    DlgInfo() = default;
    DlgInfo(IWindow* dlg, const QString& name, const QString& internalName = QString{})
        : _dlg(dlg), _name(name), _internalName(internalName) {}
};

using WindowVector = std::vector<DlgInfo>;

// =============================================================================
// ControlsTab — tab bar that switches between docked panels
// Mirrors Win32 ControlsTab (subclass of TabBar).
// =============================================================================

class ControlsTab final : public TabBar
{
    Q_OBJECT

public:
    ControlsTab(QWidget* parent = nullptr);
    ~ControlsTab() override = default;

    // init — mirrors Win32 ControlsTab::init(HINSTANCE, HWND)
    // Note: TabBar base has init(QWidget*, bool, bool); this hides it with a 1-arg variant.
    void init(QWidget* parent);

    // createTabs — mirrors Win32 ControlsTab::createTabs(WindowVector&)
    void createTabs(WindowVector& winVector);

    // reSizeTo — mirrors Win32 ControlsTab::reSizeTo(RECT&)
    void reSizeTo(QRect& rc);

    // activateWindowAt — mirrors Win32 list selection → panel switch
    void activateWindowAt(int index);

    // clickedUpdate — mirrors TCM_GETCURSEL → activateWindowAt
    void clickedUpdate() {
        activateWindowAt(currentIndex());
    }

    // renameTab — mirrors Win32 tab rename by index or internal name
    void renameTab(size_t index, const QString& newName);
    bool renameTab(const QString& internalName, const QString& newName);

    WindowVector* windowVector() const { return _pWinVector; }

signals:
    void windowActivated(int index);
    void windowRenamed(int index, const QString& newName);

private:
    WindowVector* _pWinVector = nullptr;
    int _current = 0;

    using TabBar::init;  // hide base init
};