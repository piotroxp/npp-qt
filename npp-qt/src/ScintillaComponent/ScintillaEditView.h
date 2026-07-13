// npp-qt: ScintillaEditView type alias for ScintillaComponent subdirectory
//
// Original: PowerEditor/src/ScintillaComponent/ScintillaEditView.h (36KB)
// Target:   npp-qt/src/ScintillaComponent/ScintillaEditView.h
//
// In the Win32 original, ScintillaEditView is the concrete editor class.
// In the Qt6 lift, ScintillaComponent IS that class.
// ScintillaComponent.h (in src/ root) defines:
//     using ScintillaEditView = ScintillaComponent;
//
// This header is kept in the ScintillaComponent/ subdirectory for include-path
// compatibility so that #include "ScintillaComponent/ScintillaEditView.h"
// resolves correctly in the original include hierarchy.
#pragma once
#include "ScintillaComponent.h"
