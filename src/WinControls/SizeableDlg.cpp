// Semantic Lift: Win32 SizeableDlg → Qt6 SizeableDlg
// Original: PowerEditor/src/WinControls/WindowsDlg/SizeableDlg.cpp
// Target: npp-qt/src/WinControls/WindowsDlg.cpp

#include "SizeableDlg.h"

SizeableDlg::SizeableDlg(WINRECT* pWinMap)
    : StaticDialog()
    , _winMgr(pWinMap)
{
}

void SizeableDlg::onInitDialog()
{
    // Mirrors Win32 SizeableDlg::onInitDialog():
    // Initialize TOFIT sizes from current positions, then apply layout
    if (window()) {
        _winMgr.InitToFitSizeFromCurrent(window());
    }
}

void SizeableDlg::onSize(QSize newSize)
{
    // Mirrors Win32 SizeableDlg::onSize():
    // Recalculate layout for new widget size
    if (window()) {
        _winMgr.CalcLayout(newSize.width(), newSize.height(), window());
        _winMgr.SetWindowPositions(window());
    }
}

void SizeableDlg::onGetMinMaxInfo(QSize& minSize, QSize& maxSize)
{
    if (window()) {
        _winMgr.GetMinMaxInfo(window(), minSize, maxSize);
    }
}

int SizeableDlg::onWinMgr(int wp, int lp)
{
    Q_UNUSED(wp);
    Q_UNUSED(lp);
    return 0;
}

intptr_t SizeableDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG:
        case WM_SHOWWINDOW:
            if (!_initialized) {
                _initialized = true;
                onInitDialog();
            }
            onInit();
            return TRUE;

        case WM_SIZE: {
            int cx = static_cast<int>(wParam);
            int cy = static_cast<int>(lParam);
            Q_UNUSED(cx);
            Q_UNUSED(cy);
            onSize(size());
            onSizeEvent(size());
            return TRUE;
        }

        case WM_GETMINMAXINFO: {
            // lParam is pointer to MINMAXINFO (in Win32)
            // In Qt6, we return via reference
            QSize minSize, maxSize = SIZEMAX;
            onGetMinMaxInfo(minSize, maxSize);
            onGetMinMaxInfoEvent(minSize, maxSize);
            return TRUE;
        }

        default:
            if (message == WM_WINMGR)
                return onWinMgrEvent(static_cast<int>(wParam), static_cast<int>(lParam));
            break;
    }
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}
