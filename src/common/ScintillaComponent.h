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

    /// Retrieve text in [start, end] range.
    /// Returns an empty QByteArray if start > end or either position is negative,
    /// preventing heap corruption in SCI_GETTEXTRANGE when Scintilla writes
    /// beyond an undersized buffer.
    QByteArray textRange(int start, int end) const {
        // Guard against invalid ranges: start must be <= end, both must be >= 0.
        if (start < 0 || end < 0 || start > end)
            return QByteArray();

        // Clamp to document length to prevent Scintilla from writing past the buffer.
        const int docLen = static_cast<int>(const_cast<ScintillaComponent*>(this)->send(SCI_GETLENGTH));
        if (start >= docLen)
            return QByteArray();

        // Cap end at docLen to ensure buffer is always large enough.
        const int safeEnd = qMin(end, docLen);
        const int rangeLen = safeEnd - start;
        if (rangeLen <= 0)
            return QByteArray();

        Sci_TextRange tr;
        tr.chrg.cpMin = static_cast<Sci_PositionCR>(start);
        tr.chrg.cpMax = static_cast<Sci_PositionCR>(safeEnd);
        QByteArray buf(rangeLen + 1, Qt::Uninitialized);
        tr.lpstrText = buf.data();
        const_cast<ScintillaComponent*>(this)->send(SCI_GETTEXTRANGE, 0, reinterpret_cast<intptr_t>(&tr));
        buf.truncate(static_cast<int>(strlen(buf.data())));
        return buf;
    }

    /// Returns the underlying widget, or nullptr if not available.
    /// Override in derived classes that have a widget.
    virtual QWidget* widget() const { return nullptr; }
};
