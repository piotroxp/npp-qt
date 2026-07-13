// Ported to Qt6: NppNotification handler
#pragma once
#include <QtCore>

// Minimal Scintilla notification types we use — no external Scintilla.h needed.
// This avoids macro conflicts with QsciScintillaBase's own SCI_* definitions.

// Scintilla notification codes (SCN_*)
#ifndef SCN_STYLENEEDED
#define SCN_STYLENEEDED 2000
#endif
#ifndef SCN_CHARADDED
#define SCN_CHARADDED 2001
#endif
#ifndef SCN_SAVEPOINTREACHED
#define SCN_SAVEPOINTREACHED 2002
#endif
#ifndef SCN_SAVEPOINTLEFT
#define SCN_SAVEPOINTLEFT 2003
#endif
#ifndef SCN_MODIFYATTEMPTRO
#define SCN_MODIFYATTEMPTRO 2004
#endif
#ifndef SCN_DOUBLECLICK
#define SCN_DOUBLECLICK 2006
#endif
#ifndef SCN_UPDATEUI
#define SCN_UPDATEUI 2007
#endif
#ifndef SCN_MODIFIED
#define SCN_MODIFIED 2008
#endif
#ifndef SCN_MARGINCLICK
#define SCN_MARGINCLICK 2010
#endif
#ifndef SCN_PAINTED
#define SCN_PAINTED 2013
#endif
#ifndef SCN_ZOOM
#define SCN_ZOOM 2018
#endif

// Use Scintilla.h's full Sci_NotifyHeader and SCNotification definitions
// to avoid redefinition conflicts when both NppNotification.h and Scintilla.h
// are included in the same translation unit (e.g. mocs_compilation.cpp).
// Scintilla.h defines these structs unconditionally — #pragma once prevents
// multiple inclusion of Scintilla.h itself, and the #ifndef guards for SCN_*
// prevent SCN_* redefinition warnings.
#include "../Scintilla.h"

#ifndef NPPNOTIFICATION_MINIMAL_DEFINED
#define NPPNOTIFICATION_MINIMAL_DEFINED
// Only if Scintilla.h structs aren't available (e.g. forward-declared in header-only usage):
// struct Sci_NotifyHeader { void *hwndFrom; uintptr_t idFrom; unsigned int code; };
// struct SCNotification { Sci_NotifyHeader nmhdr; /* minimal — add fields as needed */ };
#endif

class Buffer;

class NppNotificationHandler : public QObject {
    Q_OBJECT
public:
    explicit NppNotificationHandler(QObject* parent = nullptr);
    ~NppNotificationHandler() override;
    bool handleScintillaNotification(const SCNotification* notification);
    bool handleNppNotification(void* nmhdr, unsigned int code, uintptr_t wParam, intptr_t lParam);
signals:
    void fileModified(const QString& path);
    void bufferActivated(int bufferId);
    void zoomChanged(int zoom);
    void selectionChanged();
private:
    bool handleModified(const SCNotification* notification);
    bool handleSavePoint(const SCNotification* notification);
    bool handleMarginClick(const SCNotification* notification);
    bool handleCharAdded(const SCNotification* notification);
    bool handleDoubleClick(const SCNotification* notification);
    bool handleUpdateUI(const SCNotification* notification);
    bool handleZoom(const SCNotification* notification);
    bool handlePainted(const SCNotification* notification);
};
