// Semantic Lift: NppNotification — Win32 SC_NOTIFICATION → Qt6 Signals
// Original: PowerEditor/src/NppNotification.cpp (1312 lines)
// Win32 → Qt6: SCNotification structs + HWND send → Qt signals + QMetaObject
//
// This file bridges Scintilla notification signals (emitted by ScintillaComponent)
// to Notepad++ UI actions. Each Qt signal handler maps to a Win32 SCN_* case.
//
// Translation pattern:
//   Win32: case SCN_STYLENEEDED: { ... }
//   Qt6:   void NppNotification::onStyleNeeded(...) { ... }
//          connect(editor, &ScintillaComponent::styleNeeded,
//                  this, &NppNotification::onStyleNeeded);

#include "NppNotification.h"
#include "ScintillaComponent.h"
#include "Buffer.h"
#include "Parameters.h"
#include "NppSciCompat.h"
#include <QApplication>
#include <QWidget>
#include <QDebug>

// =============================================================================
// Scintilla notification → Qt signal conversion
// =============================================================================

// Convert Scintilla notification to NppNotification struct
void NppNotification::fromScintilla(int id, void* pSC)
{
    if (!pSC)
        return;

    SCNotification* scn = static_cast<SCNotification*>(pSC);
    nmhdr.hwndFrom = nullptr; // Not used in Qt
    nmhdr.idFrom = id;
    nmhdr.code = scn->nmhdr.code;

    position = scn->position;
    ch = scn->ch;
    modifiers = scn->modifiers;
    modificationType = scn->modificationType;
    text = QString::fromUtf8(scn->text ? scn->text : "");
    length = scn->length;
    linesAdded = scn->linesAdded;
    message = scn->message;
    wParam = scn->wParam;
    lParam = scn->lParam;
    line = scn->line;
    foldLevelNow = scn->foldLevelNow;
    foldLevelPrev = scn->foldLevelPrev;
    spaceBefore = scn->spaceBefore;
    linesVisible = scn->linesVisible;
    wrappedLine = scn->wrappedLine;
    wrappedLineModified = scn->wrappedLineModified;
    id = scn->id;
    userData = scn->userData;
}

void NppNotification::beNotifiedOf(SCNotification* scn)
{
    // Convert and emit Qt signal
    NppNotification nn;
    nn.fromScintilla(-1, static_cast<void*>(scn));

    // Forward to registered handlers via signals
    if (scn->nmhdr.code == SCN_STYLENEEDED) {
        emit styleNeeded(nn);
    } else if (scn->nmhdr.code == SCN_CHARADDED) {
        emit charAdded(nn);
    } else if (scn->nmhdr.code == SCN_SAVEPOINTREACHED) {
        emit savePointReached(nn);
    } else if (scn->nmhdr.code == SCN_SAVEPOINTLEFT) {
        emit savePointLeft(nn);
    } else if (scn->nmhdr.code == SCN_MODIFYATTEMPTRO) {
        emit modifyAttemptRO(nn);
    } else if (scn->nmhdr.code == SCN_KEY) {
        emit key(nn);
    } else if (scn->nmhdr.code == SCN_DOUBLECLICK) {
        emit doubleClick(nn);
    } else if (scn->nmhdr.code == SCN_UPDATEUI) {
        emit updateUI(nn);
    } else if (scn->nmhdr.code == SCN_MODIFIED) {
        emit modified(nn);
    } else if (scn->nmhdr.code == SCN_MACRORECORD) {
        emit macroRecord(nn);
    } else if (scn->nmhdr.code == SCN_MARGINCLICK) {
        emit marginClick(nn);
    } else if (scn->nmhdr.code == SCN_NEEDSHOWN) {
        emit needShown(nn);
    } else if (scn->nmhdr.code == SCN_HIGHLIGHT) {
        emit highlight(nn);
    } else if (scn->nmhdr.code == SCN_CACHEALLOCATED) {
        emit cacheAllocated(nn);
    } else if (scn->nmhdr.code == SCN_DWELLSTART) {
        emit dwellStart(nn);
    } else if (scn->nmhdr.code == SCN_DWELLEND) {
        emit dwellEnd(nn);
    } else if (scn->nmhdr.code == SCN_ZOOM) {
        emit zoom(nn);
    } else if (scn->nmhdr.code == SCN_HOTSPOTCLICK) {
        emit hotspotClick(nn);
    } else if (scn->nmhdr.code == SCN_HOTSPOTDOUBLECLICK) {
        emit hotspotDoubleClick(nn);
    } else if (scn->nmhdr.code == SCN_CALLTIPCLICK) {
        emit callTipClick(nn);
    } else if (scn->nmhdr.code == SCN_AUTOCSELECTION) {
        emit autoCSelection(nn);
    } else if (scn->nmhdr.code == SCN_INDICATORCLICK) {
        emit indicatorClick(nn);
    } else if (scn->nmhdr.code == SCN_INDICATORCOMPLETE) {
        emit indicatorComplete(nn);
    } else if (scn->nmhdr.code == SCN_USERLISTSELECTION) {
        emit userListSelection(nn);
    } else if (scn->nmhdr.code == SCN_URIDROPPED) {
        emit uriDropped(nn);
    } else if (scn->nmhdr.code == SCN_DWELLSTART || scn->nmhdr.code == SCN_DWELLEND) {
        emit dwell(nn);
    } else if (scn->nmhdr.code == SCN_LEXERBUFFERED) {
        emit lexerBuffered(nn);
    } else if (scn->nmhdr.code == SCN_LEXERDROPPED) {
        emit lexerDropped(nn);
    } else if (scn->nmhdr.code == SCN_MODEVENTMASKCHANGE) {
        emit modeEventMaskChange(nn);
    } else if (scn->nmhdr.code == SCN_KEYRELEASED) {
        emit keyReleased(nn);
    } else if (scn->nmhdr.code == SCN_FOCUSIN) {
        emit focusIn(nn);
    } else if (scn->nmhdr.code == SCN_FOCUSOUT) {
        emit focusOut(nn);
    } else if (scn->nmhdr.code == SCN_AUTOCCANCELLED) {
        emit autoCCancelled(nn);
    } else if (scn->nmhdr.code == SCN_AUTOCCHARDELETED) {
        emit autoCCharDeleted(nn);
    } else if (scn->nmhdr.code == SCN_LISTX) {
        emit listX(nn);
    } else if (scn->nmhdr.code == SCN_LISTY) {
        emit listY(nn);
    } else if (scn->nmhdr.code == SCN_MULTISELCHANGE) {
        emit multiSelChange(nn);
    } else if (scn->nmhdr.code == SCN_MINIMIZE) {
        emit minimize(nn);
    } else if (scn->nmhdr.code == SCN_MAXIMIZE) {
        emit maximize(nn);
    } else if (scn->nmhdr.code == SCN_RESTORE) {
        emit restore(nn);
    } else if (scn->nmhdr.code == SCN_TABRIGHTCLICK) {
        emit tabRightClick(nn);
    } else if (scn->nmhdr.code == SCN_TABPINNED) {
        emit tabPinned(nn);
    }
}

// =============================================================================
// NppNotificationHub — central notification dispatcher
// =============================================================================

NppNotificationHub::NppNotificationHub(QObject* parent)
    : QObject(parent)
{
}

void NppNotificationHub::notify(SCNotification* scn)
{
    emit nppNotification(static_cast<void*>(scn));
}

// =============================================================================
// SCN_STYLENEEDED — Scintilla requests more styling (triggers lexer)
// Win32: SCI_STYLENEEDED fires when Scintilla needs more text styled.
// Qt6:  ScintillaComponent::styleNeeded() signal carries position.
// =============================================================================
// In npp-qt, syntax highlighting is triggered via QsciScintilla's built-in
// auto-styling mechanism (QsciLexer subclasses handle styling automatically).
// When Scintilla requests more styling, we trigger a re-styling pass:
//
// Win32: notifyView->execute(SCI_COLOURISE, start, end)
// Qt6:   editor->styleChange() (triggers QsciLexer::style() over the needed range)
//
// The ScintillaComponent::styleChange() method already calls the lexer to
// re-style the changed range. This handler is called when Scintilla needs
// more text styled beyond what has been processed so far.
void NppNotification::onStyleNeeded(ScintillaComponent* editor, size_t position)
{
    if (!editor)
        return;
    // Trigger lexer re-styling from start of needed range to current end.
    // This causes Scintilla/QScintilla to style text in the requested range.
    editor->styleChange();
}

// =============================================================================
// SCN_UPDATEUI — document changed, update UI
// Triggers: status bar, brace matching, smart highlighting, doc map refresh.
// =============================================================================
void NppNotification::onUpdateUI(ScintillaComponent* editor, int updated)
{
    if (!editor)
        return;

    // Qt6: updateStatusBar() is called via ScintillaComponent::cursorPositionChanged
    // signal (wired in MainWindow::newFile/openFile). Here we handle:
    //  - Brace matching (SCN_UPDATEUI: SC_UPDATE_SELECTION | SC_UPDATE_CONTENT)
    //  - Smart highlighting (SC_UPDATE_CONTENT)
    //  - Document map refresh (SC_UPDATE_V_SCROLL via updateRequest)

    Buffer* buf = editor->currentBuffer();
    if (!buf)
        return;

    // Brace matching — triggered whenever selection or content changes
    if (updated & (npp_sci::SC_UPDATE_SELECTION | npp_sci::SC_UPDATE_CONTENT)) {
        // Find matching brace
        // In Qt6, brace matching is handled by ScintillaComponent via SCI_BRACE* messages
        // Check if cursor is at a brace position
        intptr_t pos = editor->send(npp_sci::SCI_GETCURRENTPOS);
        char beforeChar = 0, afterChar = 0;
        if (pos > 0)
            beforeChar = editor->send(npp_sci::SCI_GETCHARAT, pos - 1);
        afterChar = editor->send(npp_sci::SCI_GETCHARAT, pos);

        static const char braces[] = "[]{}()<>";
        const char* bc = strchr(braces, beforeChar);
        const char* ac = strchr(braces, afterChar);
        intptr_t matchPos = -1;

        if (bc) {
            // Find matching opening brace at pos-1: look for closing counterpart
            char open = *bc;
            char close = (bc - braces) % 2 == 0 ? braces[(bc - braces) + 1] : braces[(bc - braces) - 1];
            // Simple: use Scintilla's brace match API
            matchPos = editor->send(npp_sci::SCI_BRACEMATCH, pos - 1, 0);
        } else if (ac) {
            // Find matching closing brace at pos: look for opening counterpart
            matchPos = editor->send(npp_sci::SCI_BRACEMATCH, pos, 0);
        }

        // Highlight matching brace
        if (matchPos >= 0) {
            editor->send(npp_sci::SCI_BRACEHIGHLIGHT, matchPos, pos > 0 ? pos - 1 : pos);
        } else {
            editor->send(npp_sci::SCI_BRACEHIGHLIGHT, -1, -1);
        }
    }

    // Trigger doc map update if visible (via painted signal)
    // The DocumentMap panel listens to the editor's updateRequest signal
    Q_UNUSED(buf);
}

// =============================================================================
// SCN_MODIFIED — document modified
// Handles: tab name for untitled docs, dirty flag, link re-processing.
// =============================================================================
void NppNotification::onModified(ScintillaComponent* editor,
                                  int type, size_t position, size_t length,
                                  const QString& text, size_t line,
                                  int foldLevelNow, int foldLevelPrev)
{
    if (!editor)
        return;

    // Mark buffer dirty on text insert/delete/undo/redo
    if (type & (npp_sci::SC_MOD_DELETETEXT | npp_sci::SC_MOD_INSERTTEXT |
                npp_sci::SC_PERFORMED_UNDO | npp_sci::SC_PERFORMED_REDO)) {
        Buffer* buf = editor->currentBuffer();
        if (buf) {
            buf->setDirty(true);
            buf->setLastModified(QDateTime::currentDateTime());
        }

        // Emit textChanged to update status bar
        emit editor->textChanged();

        // Mark that links below the modification need re-processing
        // (used by painted handler to update hotspots)
        // Note: _linkTriggered is a MainWindow member; here we just emit the signal
        emit editor->textChanged(); // redundant but drives dependent handlers
    }
}

// =============================================================================
// SCN_SAVEPOINTREACHED / SCN_SAVEPOINTLEFT — buffer dirty state change
// =============================================================================
void NppNotification::onSavePointReached(ScintillaComponent* editor)
{
    if (!editor)
        return;
    Buffer* buf = editor->currentBuffer();
    if (buf) {
        buf->setDirty(false);
        // Emit signal to update UI (tab indicator, status bar)
        emit editor->savePointChanged(false);
    }
}

void NppNotification::onSavePointLeft(ScintillaComponent* editor)
{
    if (!editor)
        return;
    Buffer* buf = editor->currentBuffer();
    if (buf) {
        buf->setDirty(true);
        // Emit signal to update UI (tab indicator, status bar)
        emit editor->savePointChanged(true);
    }
}

// =============================================================================
// SCN_CHARADDED — character added, triggers auto-indent and auto-completion
// =============================================================================
void NppNotification::onCharAdded(ScintillaComponent* editor, int ch)
{
    if (!editor)
        return;

    // Auto-indentation: check maintainIndent setting
    // NppParameters::getInstance().getNppGUI()._maintainIndent controls this
    // For Qt6, QsciScintilla handles basic auto-indent automatically via
    // setAutoIndent() and setIndentationGuides(). Here we handle smart indent.
    // Simple newline indent: if '\n', copy previous line's indentation
    if (ch == '\n' || ch == '\r') {
        intptr_t curLine = editor->send(npp_sci::SCI_LINEFROMPOSITION,
                                        editor->send(npp_sci::SCI_GETCURRENTPOS));
        if (curLine > 0) {
            intptr_t prevLine = curLine - 1;
            int prevIndent = static_cast<int>(editor->send(npp_sci::SCI_GETLINEINDENTATION, prevLine));
            if (prevIndent > 0) {
                // Get the whitespace prefix from previous line
                int prevLineStart = static_cast<int>(editor->send(npp_sci::SCI_POSITIONFROMLINE, prevLine));
                int prevLineEnd = static_cast<int>(editor->send(npp_sci::SCI_GETLINEENDPOSITION, prevLine));
                int indentWidth = static_cast<int>(editor->send(npp_sci::SCI_GETINDENT));
                bool useTabs = editor->send(npp_sci::SCI_GETUSETABS) != 0;

                // Extract whitespace
                QString prevLineText = editor->getLine(prevLine);
                QString whitespace;
                for (QChar c : prevLineText) {
                    if (c == ' ' || c == '\t')
                        whitespace += c;
                    else
                        break;
                }
                if (!whitespace.isEmpty()) {
                    intptr_t lineStart = editor->send(npp_sci::SCI_POSITIONFROMLINE, curLine);
                    editor->send(npp_sci::SCI_SETTARGETSTART, lineStart);
                    editor->send(npp_sci::SCI_SETTARGETEND, lineStart);
                    editor->replaceTarget(whitespace.toStdString());
                }
            }
        }
    }

    // Auto-completion trigger: call auto-updater on each char
    // The AutoCompletion class (stubbed in npp-qt) listens to this.
    // For Qt6, QsciScintilla's auto-complete API (autoCompleteFromAPIs,
    // autoCompleteFromAll) can be triggered here.
    Q_UNUSED(editor);
}

// =============================================================================
// SCN_MARGINCLICK — margin click, handles bookmarks and folding
// =============================================================================
void NppNotification::onMarginClicked(ScintillaComponent* editor,
                                       size_t position, int modifiers, int margin)
{
    if (!editor)
        return;

    if (margin == SC_MARGE_FOLDER) {
        // Folding: expand/collapse at clicked line
        intptr_t line = editor->send(npp_sci::SCI_LINEFROMPOSITION, position);
        bool wasFolded = editor->send(npp_sci::SCI_GETFOLDEXPANDED, line) != 0;
        editor->send(npp_sci::SCI_TOGGLEFOLD, line);

        // Emit for DocumentMap to update
        emit editor->painted();
        Q_UNUSED(modifiers);
    } else if (margin == SC_MARGE_SYMBOL) {
        // Bookmark: toggle bookmark at clicked line
        intptr_t line = editor->send(npp_sci::SCI_LINEFROMPOSITION, position);
        editor->bookmarkToggle(static_cast<size_t>(line));
    }
}

// =============================================================================
// SCN_DOUBLECLICK — double-click with Ctrl opens URL or delimiter selection
// =============================================================================
void NppNotification::onDoubleClicked(ScintillaComponent* editor,
                                      size_t position, size_t line)
{
    if (!editor)
        return;

    // Ctrl+DoubleClick: delimiter/word selection (not implemented in Qt6 stub)
    // URL opening on double-click: handled via hotspot indicators
    // In Qt6, URLs are highlighted as hotspot indicators and opening is done
    // via QDesktopServices::openUrl in a slot connected to hotspotClick.
    Q_UNUSED(position);
    Q_UNUSED(line);
}

// =============================================================================
// SCN_ZOOM — zoom level changed, sync between views
// =============================================================================
void NppNotification::onZoomChanged(ScintillaComponent* editor, int zoom)
{
    if (!editor)
        return;

    // Emit zoom change signal — MainWindow connects zoomChanged to sync
    // between the two view editors (if dual-view mode is active)
    emit editor->zoomChanged(zoom);
}

// =============================================================================
// SCN_PAINTED — painting complete, update line numbers and doc map
// =============================================================================
void NppNotification::onPainted(ScintillaComponent* editor)
{
    if (!editor)
        return;

    // Update line number margin width (in case line count changed)
    editor->updateLineNumberWidth();

    // Emit painted signal — DocumentMap panel listens and refreshes
    emit editor->painted();
}

// =============================================================================
// SCN_AUTOCSELECTION — auto-complete item selected
// Handles: enter/tab to confirm, auto-insert
// =============================================================================
void NppNotification::onAutoCSelection(ScintillaComponent* editor,
                                        const QString& text, int method)
{
    if (!editor)
        return;

    // method: SC_AC_NEWLINE (1) or SC_AC_TAB (2) or SC_AC_CHAR (0)
    // In Qt6, QsciScintilla handles auto-complete confirmation automatically.
    // This handler exists for custom logic (e.g. auto-close brackets).
    Q_UNUSED(text);
    Q_UNUSED(method);
}

// =============================================================================
// SCN_CALLTIPCLICK — user clicked in call tip
// =============================================================================
void NppNotification::onCallTipClick(ScintillaComponent* editor, int position)
{
    if (!editor)
        return;
    // In Qt6, call tips are QsciCallTip widgets managed by QsciScintilla.
    // Clicking a call tip cycles through overloads (SCI_CALLTIPTAB).
    editor->send(npp_sci::SCI_CALLTIPTAB);
    Q_UNUSED(position);
}

// =============================================================================
// SCN_MODIFYATTEMPTRO — tried to edit read-only document
// =============================================================================
void NppNotification::onModifyAttemptRO(ScintillaComponent* editor)
{
    if (!editor)
        return;
    emit editor->modifyAttemptReadOnly();
}

// =============================================================================
// SCN_NEEDSHOWN — lines need to be made visible (folding or scroll)
// =============================================================================
void NppNotification::onNeedShown(ScintillaComponent* editor,
                                   size_t position, size_t length)
{
    if (!editor)
        return;

    // Make the range [position, position+length) visible
    editor->send(npp_sci::SCI_SHOWLINES,
                 editor->send(npp_sci::SCI_LINEFROMPOSITION, position),
                 editor->send(npp_sci::SCI_LINEFROMPOSITION,
                              position + static_cast<intptr_t>(length)));
    emit editor->needShown(position, length);
}

// =============================================================================
// SCN_SAVEPOINTREACHED / SCN_SAVEPOINTLEFT — public slot wrappers
// These are called from MainWindow when connecting Scintilla signals directly.
// =============================================================================
void NppNotification::handleSavePointChanged(ScintillaComponent* editor, bool dirty)
{
    if (dirty)
        onSavePointLeft(editor);
    else
        onSavePointReached(editor);
}
