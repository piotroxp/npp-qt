// ScintillaComponent.h — minimal Scintilla send() interface
// Used by AutoCompletion and other components that need to issue
// SCI_* messages without coupling to the full ScintillaEditor.
// Copyright (C) 2026 Agent Army / GPL v3
#pragma once

#include <QByteArray>
#include <QWidget>
#include <QtGlobal>  // qMax
#include "NppSciCompat.h"  // SCI_GETSELECTION, SCI_GETTEXTRANGE, etc.

// Scintilla API integer types (compatible with Scintilla.h / QsciScintilla)
using Sci_Position = long;
using Sci_PositionCR = long;
using sptr_t = intptr_t;
using uptr_t = uintptr_t;

// Scintilla API structs (compatible with QsciScintilla)
struct Sci_CharacterRange {
    Sci_PositionCR cpMin;
    Sci_PositionCR cpMax;
};

struct Sci_TextRange {
    Sci_CharacterRange chrg;
    char* lpstrText;
};

class ScintillaComponent {
public:
    virtual ~ScintillaComponent() = default;

    /// Primary send — derived classes delegate to QsciScintilla::SendScintilla
    virtual intptr_t send(int message, int wParam = 0, intptr_t lParam = 0) = 0;

    /// Convenience: single integer param (lParam only, wParam=0)
    intptr_t send(int message, intptr_t lParam) {
        return send(message, 0, lParam);
    }

    /// Convenience: string lParam (cast to sptr_t)
    intptr_t send(int message, int wParam, const char* s) {
        return send(message, wParam, reinterpret_cast<intptr_t>(s));
    }

    /// Convenience: string lParam (QByteArray overload)
    intptr_t send(int message, int wParam, const QByteArray& s) {
        return send(message, wParam, s.constData());
    }

    /// Get current selection range
    Sci_CharacterRange getSelection() const {
        Sci_CharacterRange cr;
        const_cast<ScintillaComponent*>(this)->send(SCI_GETSELECTION, 0, reinterpret_cast<intptr_t>(&cr));
        return cr;
    }

    /// Retrieve text in [start, end] range
    QByteArray textRange(int start, int end) const {
        Sci_TextRange tr;
        tr.chrg.cpMin = static_cast<Sci_PositionCR>(start);
        tr.chrg.cpMax = static_cast<Sci_PositionCR>(end);
        QByteArray buf(qMax(0, end - start) + 1, Qt::Uninitialized);
        tr.lpstrText = buf.data();
        const_cast<ScintillaComponent*>(this)->send(SCI_GETTEXTRANGE, 0, reinterpret_cast<intptr_t>(&tr));
        buf.truncate(static_cast<int>(strlen(buf.data())));
        return buf;
    }

    /// Returns the underlying widget, or nullptr if not available.
    /// Override in derived classes that have a widget.
    virtual QWidget* widget() const { return nullptr; }

    /// Returns the window containing the widget, or nullptr if not available.
    /// Override in derived classes that have a widget.
    virtual QWidget* window() const { return nullptr; }
};
