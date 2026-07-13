// Semantic Lift: NppNotification — Win32 SC_NOTIFICATION → Qt6 Signals
// Original: PowerEditor/src/NppNotification.cpp (1312 lines)
// Win32 → Qt6: SCNotification structs + HWND send → Qt signals + QMetaObject

#include "NppNotification.h"
#include "ScintillaWidget.h"
#include "Parameters.h"
#include <QApplication>

// Convert Scintilla notification to Qt signal
void NppNotification::ScintillaNotify::fromScintilla(int id, void* pSC)
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

    // Forward to registered handlers
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

// NppNotificationHub — central notification dispatcher
NppNotificationHub::NppNotificationHub(QObject* parent)
    : QObject(parent)
{
}

void NppNotificationHub::notify(SCNotification* scn)
{
    emit nppNotification(static_cast<void*>(scn));
}
