// ScintillaComponent.h — minimal Scintilla send() interface
// Used by AutoCompletion and other components that need to issue
// SCI_* messages without coupling to the full ScintillaEditor.
// Copyright (C) 2026 Agent Army / GPL v3
#pragma once

// Forward declarations for QsciScintilla symbols used in send() signatures
// (sptr_t / string casts are compatible with QsciScintilla::SendScintilla)
using sptr_t = intptr_t;
using uptr_t = uintptr_t;

class ScintillaComponent {
public:
    virtual ~ScintillaComponent() = default;

    /// Primary send — derived classes delegate to QsciScintilla::SendScintilla
    virtual intptr_t send(int message, int wParam = 0, intptr_t lParam = 0) = 0;

    /// Convenience: single integer param (lParam only, wParam=0)
    intptr_t send(int message, intptr_t lParam) {
        return send(message, 0, lParam);
    }
};
