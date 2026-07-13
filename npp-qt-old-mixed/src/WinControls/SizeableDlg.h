// Semantic Lift: Win32 SizeableDlg → Qt6 SizeableDlg
// Original: PowerEditor/src/WinControls/WindowsDlg/SizeableDlg.h
// Target: npp-qt/src/WinControls/WindowsDlg.h
//
// SizeableDlg is a base class for dialogs with dynamic layout.
// Uses CWinMgr for declarative row/column layout.
// Ported to Qt6: uses QDialog + QLayout with CWinMgr for the macro API.

#pragma once

#include "StaticDialog.h"
#include "WinMgr.h"
#include <QDialog>
#include <QVBoxLayout>

// WM_WINMGR custom message constant
#define WM_WINMGR (0x0400 + 900)

// SizeableDlg mirrors Win32 SizeableDlg:
// uses CWinMgr to manage child widget positions during resize.
// The BEGIN_WINDOW_MAP / END_WINDOW_MAP macros define the layout.

class SizeableDlg : public StaticDialog
{
    Q_OBJECT

public:
    explicit SizeableDlg(WINRECT* pWinMap);

    // Qt6 replacement for onInitDialog()
    void onInitDialog();

    // Qt6 replacement for onSize()
    void onSize(QSize newSize);

    // Qt6 replacement for onGetMinMaxInfo
    void onGetMinMaxInfo(QSize& minSize, QSize& maxSize);

    // Qt6 replacement for onWinMgr
    int onWinMgr(int wp, int lp);

protected:
    CWinMgr _winMgr; // Window manager

    // run_dlgProc mirrors Win32 CALLBACK DlgProc
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

    // Called after WM_INITDIALOG / showEvent
    virtual bool onInit() { return true; }
    // Called after WM_SIZE
    virtual void onSizeEvent(QSize newSize) { Q_UNUSED(newSize); }
    // Called when WM_GETMINMAXINFO
    virtual void onGetMinMaxInfoEvent(QSize& minSize, QSize& maxSize) {
        Q_UNUSED(minSize); Q_UNUSED(maxSize);
    }
    // Called when WM_WINMGR
    virtual int onWinMgrEvent(int wp, int lp) { Q_UNUSED(wp); Q_UNUSED(lp); return 0; }

private:
    bool _initialized = false;
};
