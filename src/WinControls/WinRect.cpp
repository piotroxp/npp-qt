// Semantic Lift: Win32 WinRect.cpp → Qt6 WinRect.cpp
// Original: PowerEditor/src/WinControls/WindowsDlg/WinRect.cpp
// Target: npp-qt/src/WinControls/WinRect.cpp

#include "WinMgr.h"
#include <cassert>

//////////////////
// Initialize map: set up all the next/prev pointers. This converts the
// linear array to a convenient linked list. Called from CWinMgr constructor.
//
WINRECT* WINRECT::InitMap(WINRECT* pWinMap, const WINRECT* parent)
{
    assert(pWinMap);

    WINRECT* pwrc = pWinMap;
    WINRECT* prev = nullptr;

    while (!pwrc->IsEndGroup()) {
        pwrc->prev = prev;
        pwrc->next = nullptr;
        if (prev)
            prev->next = pwrc;
        prev = pwrc;
        if (pwrc->IsGroup()) {
            pwrc = InitMap(pwrc + 1, pwrc); // recurse! Returns end-of-grp
            assert(pwrc->IsEndGroup());
        }
        ++pwrc;
    }
    // safety checks
    assert(pwrc->IsEndGroup());
    assert(prev);
    assert(prev->next == nullptr);
    return parent ? const_cast<WINRECT*>(pwrc) : nullptr;
}

//////////////////
// Get the parent of a given WINRECT. To find the parent, chase the prev
// pointer to the start of the list, then take the item before that in
// memory.
//
WINRECT* WINRECT::Parent()
{
    WINRECT* pEntry = this;
    while (pEntry->Prev())
        pEntry = pEntry->Prev();

    // The entry before the first child is the group
    WINRECT* parent = pEntry - 1;
    assert(parent->IsGroup());
    return parent;
}
