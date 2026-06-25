// Ported to Qt6: NppNotification handler
#pragma once
// NppSciCompat.h undefs SC_UPDATE_*, SCEN_*, SCK_* before defining constexpr,
// then includes Scintilla.h which redefines them as macros. We undefine the
// conflicting ones again here so the constexpr/nppsci values take precedence.
#include "NppSciCompat.h"
#include <Scintilla.h>

#undef SC_UPDATE_NONE
#undef SC_UPDATE_CONTENT
#undef SC_UPDATE_SELECTION
#undef SC_UPDATE_V_SCROLL
#undef SC_UPDATE_H_SCROLL
#undef SCEN_CHANGE
#undef SCEN_SETFOCUS
#undef SCEN_KILLFOCUS
#undef SCK_DOWN
#undef SCK_UP
#undef SCK_LEFT
#undef SCK_RIGHT
#undef SCK_HOME
#undef SCK_END
#undef SCK_PRIOR
#undef SCK_NEXT
#undef SCK_DELETE
#undef SCK_INSERT
#undef SCK_ESCAPE
#undef SCK_BACK
#undef SCK_TAB
#undef SCK_RETURN
#undef SCK_ADD
#undef SCK_SUBTRACT
#undef SCK_DIVIDE
#undef SC_MOD_NONE
#undef SC_MOD_INSERTTEXT
#undef SC_MOD_DELETETEXT
#undef SC_MOD_CHANGESTYLE
#undef SC_MOD_CHANGEFOLD
#undef SC_MOD_CHANGEMARKER
#undef SC_MOD_BEFOREINSERT
#undef SC_MOD_BEFOREDELETE
#undef SC_MOD_CHANGEINDICATOR
#undef SC_MOD_CHANGELINESTATE
#undef SC_MOD_CHANGEMARGIN
#undef SC_MOD_CHANGEANNOTATION
#undef SC_MOD_CONTAINER
#undef SC_MOD_LEXERSTATE
#undef SC_MOD_INSERTCHECK
#undef SC_MOD_CHANGETABSTOPS
#undef SC_MOD_CHANGEEOLANNOTATION

// Note: SCNotification struct comes from Scintilla.h.
// For Qt6, we use npp_sci::SC_UPDATE_* and npp_sci::SCK_* values.

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
