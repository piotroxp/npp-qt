// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// ScintillaCtrls implementation lives in ScintillaComponent.cpp.
// This file exists for build compatibility but contains no separate logic.

#include "ScintillaComponent.h"

void ScintillaCtrls::destroy()
{
    for (auto* editor : _editors)
        if (editor) editor->deleteLater();
    _editors.clear();
}
