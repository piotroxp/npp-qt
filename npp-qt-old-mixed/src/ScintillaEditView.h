// npp-qt: ScintillaEditView type alias
//
// In the Win32 original, ScintillaEditView is the concrete editor class.
// In the Qt6 lift, ScintillaComponent IS that class.
// ScintillaComponent.h defines:
//     using ScintillaEditView = ScintillaComponent;
//
// This header is kept for build compatibility so that
//     #include "ScintillaEditView.h"
// resolves without renaming every include throughout the codebase.
#pragma once
#include "ScintillaComponent.h"
