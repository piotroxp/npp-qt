// NppSciNotificationBridge.cpp — bridges Scintilla SCNotification events to N++ NppNotification
// Ported from: PowerEditor/src/ScintillaComponent/Npp.cpp (SCNotification callbacks)
#include "MISC/Common/NppNotification.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciscintillabase.h>
#include <QDebug>

// =============================================================================
// Construction
// =============================================================================

NppSciNotificationBridge::NppSciNotificationBridge(QObject* parent)
    : QObject(parent) {}

NppSciNotificationBridge::~NppSciNotificationBridge() = default;

// =============================================================================
// Main entry: convert SCNotification → NppNotification + dispatch
// =============================================================================

bool NppSciNotificationBridge::handleScintillaNotification(
    const SCNotification* notification, int bufferId) {

    if (!notification) return false;

    const unsigned int code = notification->npNmhdr.npCode;

    emit scintillaNotification(notification, bufferId);

    switch (code) {
        case SCN_MODIFIED:
            return handleModified(notification, bufferId);
        case SCN_SAVEPOINTREACHED:
        case SCN_SAVEPOINTLEFT:
            return handleSavePoint(notification, bufferId);
        case SCN_MARGINCLICK:
            return handleMarginClick(notification, bufferId);
        case SCN_CHARADDED:
            return handleCharAdded(notification, bufferId);
        case SCN_DOUBLECLICK:
            return handleDoubleClick(notification, bufferId);
        case SCN_UPDATEUI:
            return handleUpdateUI(notification, bufferId);
        case SCN_STYLENEEDED:
            return handleStyleNeeded(notification, bufferId);
        case SCN_ZOOM:
            return handleZoom(notification, bufferId);
        case SCN_PAINTED:
            return handlePainted(notification, bufferId);
        case SCN_AUTOCSELECTION:
        case SCN_AUTOCCANCELLED:
        case SCN_AUTOCCOMPLETED:
            return handleAutoComplete(notification, bufferId);
        case SCN_DWELLSTART:
        case SCN_DWELLEND:
            return handleDwell(notification, bufferId);
        case SCN_CALLTIPCLICK:
            return handleCallTipClick(notification, bufferId);
        case SCN_MACRORECORD:
            return handleMacroRecord(notification, bufferId);
        case SCN_MARGINRIGHTCLICK:
            return handleMarginRightClick(notification, bufferId);
        case SCN_NEEDSHOWN:
            return handleNeedsShown(notification, bufferId);
        case SCN_HOTSPOTCLICK:
        case SCN_HOTSPOTDOUBLECLICK:
        case SCN_HOTSPOTRELEASECLICK:
            return handleHotspot(notification, bufferId);
        case SCN_INDICATORCLICK:
        case SCN_INDICATORRELEASE:
            return handleIndicator(notification, bufferId);
        case SCN_MODIFYATTEMPTRO:
            return handleModifyAttemptRO(notification, bufferId);
        case SCN_USERLISTSELECTION:
            return handleUserListSelection(notification, bufferId);
        case SCN_URIDROPPED:
            return handleUriDropped(notification, bufferId);
        default:
            return false;
    }
}

// =============================================================================
// Handle N++ native notifications (WM_NOTIFY style)
// =============================================================================

bool NppSciNotificationBridge::handleNppNotification(
    void*, unsigned int code,
    uintptr_t wParam, intptr_t lParam) {

    emit nppNotification(code, wParam, lParam);
    Q_UNUSED(code);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);
    return false;
}

// =============================================================================
// Convert SCNotification → NppNotification
// =============================================================================

NppNotification NppSciNotificationBridge::toNotification(
    const SCNotification* sciNotification, int bufferId) const {

    NppNotification n(NppNotificationType::Unknown);
    if (!sciNotification) return n;

    const unsigned int code = sciNotification->npNmhdr.npCode;

    switch (code) {
        case SCN_UPDATEUI:
            n = NppNotification(NppNotificationType::StyleChanged,
                                NppNotificationPriority::Low);
            n.data.set("updated", QVariant::fromValue(static_cast<long long>(sciNotification->npWParam));
            break;

        case SCN_MODIFIED:
            n = NppNotification(NppNotificationType::BufferModified,
                                NppNotificationPriority::Normal);
            n.data.set("modificationType", sciNotification->npModificationType);
            if (sciNotification->npText && sciNotification->npLength > 0) {
                n.data.set("text",
                    QString::fromUtf8(sciNotification->npText,
                                      sciNotification->npLength));
            }
            n.data.set("linesAdded", sciNotification->npLinesAdded);
            break;

        case SCN_SAVEPOINTREACHED:
            n = NppNotification(NppNotificationType::BufferClean,
                                NppNotificationPriority::Normal);
            break;

        case SCN_SAVEPOINTLEFT:
            n = NppNotification(NppNotificationType::BufferDirty,
                                NppNotificationPriority::Normal);
            break;

        case SCN_CHARADDED:
            n = NppNotification(NppNotificationType::CharAdded,
                                NppNotificationPriority::Normal);
            n.data.set("ch", sciNotification->npCh);
            break;

        case SCN_STYLENEEDED:
            n = NppNotification(NppNotificationType::StyleNeeded,
                                NppNotificationPriority::Normal);
            n.data.set("position", sciNotification->npPosition);
            break;

        case SCN_ZOOM:
            n = NppNotification(NppNotificationType::ZoomChanged,
                                NppNotificationPriority::Low);
            n.data.set("zoom", static_cast<int>(sciNotification->npWParam));
            break;

        case SCN_MARGINCLICK:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "marginClick");
            n.data.set("margin", sciNotification->npMargin);
            n.data.set("position", sciNotification->npPosition);
            n.data.set("modifiers", sciNotification->npModifiers);
            break;

        case SCN_DOUBLECLICK:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "doubleClick");
            n.data.set("position", sciNotification->npPosition);
            n.data.set("line", sciNotification->npLine);
            break;

        case SCN_PAINTED:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Low);
            n.data.set("eventType", "painted");
            break;

        case SCN_AUTOCSELECTION:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "autoCompleteSelection");
            n.data.set("text", sciNotification->npText
                                        ? QString::fromUtf8(sciNotification->npText)
                                        : QString());
            break;

        case SCN_AUTOCCOMPLETED:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "autoCompleteCompleted");
            n.data.set("text", sciNotification->npText
                                        ? QString::fromUtf8(sciNotification->npText)
                                        : QString());
            break;

        case SCN_DWELLSTART:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Low);
            n.data.set("eventType", "dwellStart");
            n.data.set("position", sciNotification->npPosition);
            n.data.set("x", sciNotification->npX);
            n.data.set("y", sciNotification->npY);
            break;

        case SCN_DWELLEND:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Low);
            n.data.set("eventType", "dwellEnd");
            break;

        case SCN_HOTSPOTCLICK:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "hotspotClick");
            n.data.set("position", sciNotification->npPosition);
            n.data.set("modifiers", sciNotification->npModifiers);
            break;

        case SCN_HOTSPOTDOUBLECLICK:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "hotspotDoubleClick");
            n.data.set("position", sciNotification->npPosition);
            break;

        case SCN_INDICATORCLICK:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Low);
            n.data.set("eventType", "indicatorClick");
            n.data.set("position", sciNotification->npPosition);
            break;

        case SCN_MODIFYATTEMPTRO:
            n = NppNotification(NppNotificationType::BufferReadOnlyChanged,
                                NppNotificationPriority::High);
            n.data.set("readOnly", true);
            break;

        case SCN_USERLISTSELECTION:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "userListSelection");
            n.data.set("listType", sciNotification->npListType);
            n.data.set("text", sciNotification->npText
                                        ? QString::fromUtf8(sciNotification->npText)
                                        : QString());
            break;

        case SCN_MACRORECORD:
            n = NppNotification(NppNotificationType::MacroRecordingStarted,
                                NppNotificationPriority::Normal);
            n.data.set("message", static_cast<int>(sciNotification->npMessage));
            n.data.set("wParam", QVariant::fromValue(static_cast<long long>(sciNotification->npWParam));
            n.data.set("lParam", QVariant::fromValue(static_cast<long long>(sciNotification->npLParam));
            break;

        case SCN_CALLTIPCLICK:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "callTipClick");
            n.data.set("position", sciNotification->npPosition);
            break;

        case SCN_NEEDSHOWN:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "needShown");
            n.data.set("position", sciNotification->npPosition);
            n.data.set("length", sciNotification->npLength);
            break;

        case SCN_URIDROPPED:
            n = NppNotification(NppNotificationType::Custom,
                                NppNotificationPriority::Normal);
            n.data.set("eventType", "uriDropped");
            n.data.set("text", sciNotification->npText
                                        ? QString::fromUtf8(sciNotification->npText)
                                        : QString());
            break;

        default:
            break;
    }

    n.data.bufferId = bufferId;
    return n;
}

// =============================================================================
// Individual notification handlers
// =============================================================================

bool NppSciNotificationBridge::handleModified(
    const SCNotification* notification, int bufferId) {

    const int modType = notification->npModificationType;

    emit contentChanged(bufferId);

    NppNotification n = toNotification(notification, bufferId);

    if (modType & SC_MOD_INSERTTEXT) n.data.set("inserted", true);
    if (modType & SC_MOD_DELETETEXT)  n.data.set("deleted", true);
    if (modType & SC_MOD_CHANGESTYLE) n.data.set("styleChanged", true);
    if (modType & SC_MOD_CHANGEFOLD)  n.data.set("foldChanged", true);
    if (modType & SC_MOD_CHANGELINESTATE) n.data.set("lineStateChanged", true);

    NppNotificationHandler::instance().emitNotification(n);
    return true;
}

bool NppSciNotificationBridge::handleSavePoint(
    const SCNotification* notification, int bufferId) {

    if (notification->npNmhdr.npCode == SCN_SAVEPOINTLEFT) {
        NppNotificationHandler::instance().notifyBufferModified(bufferId, true);
    } else {
        NppNotificationHandler::instance().notifyBufferModified(bufferId, false);
    }
    return true;
}

bool NppSciNotificationBridge::handleMarginClick(
    const SCNotification* notification, int bufferId) {

    NppNotification n = toNotification(notification, bufferId);

    if (notification->npMargin == 2) {
        n.data.set("eventType", "foldToggle");
        n.data.set("line", notification->npLine);
    }

    NppNotificationHandler::instance().emitNotification(n);
    return true;
}

bool NppSciNotificationBridge::handleCharAdded(
    const SCNotification* notification, int bufferId) {

    const int ch = notification->npCh;

    NppNotification n = toNotification(notification, bufferId);
    if (ch == '\r' || ch == '\n') n.data.set("eol", true);

    NppNotificationHandler::instance().emitNotification(n);
    return false;
}

bool NppSciNotificationBridge::handleDoubleClick(
    const SCNotification* notification, int bufferId) {

    NppNotification n = toNotification(notification, bufferId);
    n.data.set("line", notification->npLine);
    NppNotificationHandler::instance().emitNotification(n);
    return true;
}

bool NppSciNotificationBridge::handleUpdateUI(
    const SCNotification* notification, int bufferId) {

    const unsigned int updated = static_cast<unsigned int>(notification->npWParam);

    NppNotificationType type = NppNotificationType::StyleChanged;
    if (updated & SC_UPDATE_SELECTION) type = NppNotificationType::SelectionChanged;
    else if (updated & SC_UPDATE_CONTENT) type = NppNotificationType::StyleChanged;
    else if (updated & (SC_UPDATE_V_SCROLL | SC_UPDATE_H_SCROLL))
        type = NppNotificationType::ScrollChanged;

    NppNotification n(type, NppNotificationPriority::Low);
    n.data.bufferId = bufferId;
    n.data.set("updated", static_cast<int>(updated));

    NppNotificationHandler::instance().emitNotification(n);

    if (updated & SC_UPDATE_SELECTION)
        emit selectionChanged();

    return true;
}

bool NppSciNotificationBridge::handleStyleNeeded(
    const SCNotification* notification, int bufferId) {

    NppNotification n = toNotification(notification, bufferId);
    n.data.bufferId = bufferId;
    n.data.set("position", notification->npPosition);
    NppNotificationHandler::instance().emitNotification(n);
    return false;
}

bool NppSciNotificationBridge::handleZoom(
    const SCNotification* notification, int bufferId) {

    const int zoom = static_cast<int>(notification->npWParam);

    NppNotification n = NppNotification(NppNotificationType::ZoomChanged,
                                          NppNotificationPriority::Low);
    n.data.bufferId = bufferId;
    n.data.set("zoom", zoom);

    NppNotificationHandler::instance().emitNotification(n);
    emit zoomChanged(zoom);
    return true;
}

bool NppSciNotificationBridge::handlePainted(
    const SCNotification*, int) {
    // Low-priority; only dispatch if aggregation is enabled
    if (!NppNotificationHandler::instance().isAggregationEnabled())
        return false;

    NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Low);
    n.data.set("eventType", "painted");
    NppNotificationHandler::instance().emitNotification(n);
    return false;
}

bool NppSciNotificationBridge::handleAutoComplete(
    const SCNotification* notification, int bufferId) {

    if (notification->npNmhdr.npCode == SCN_AUTOCCANCELLED) {
        NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Low);
        n.data.bufferId = bufferId;
        n.data.set("eventType", "autoCompleteCancelled");
        NppNotificationHandler::instance().emitNotification(n);
    }
    return false;
}

bool NppSciNotificationBridge::handleDwell(
    const SCNotification* notification, int bufferId) {

    if (notification->npNmhdr.npCode == SCN_DWELLSTART) {
        NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Low);
        n.data.bufferId = bufferId;
        n.data.set("eventType", "dwellStart");
        n.data.set("x", notification->npX);
        n.data.set("y", notification->npY);
        n.data.set("position", notification->npPosition);
        NppNotificationHandler::instance().emitNotification(n);
    }
    return false;
}

bool NppSciNotificationBridge::handleCallTipClick(
    const SCNotification* notification, int bufferId) {

    NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Low);
    n.data.bufferId = bufferId;
    n.data.set("eventType", "callTipClick");
    n.data.set("position", notification->npPosition);
    NppNotificationHandler::instance().emitNotification(n);
    return false;
}

bool NppSciNotificationBridge::handleMacroRecord(
    const SCNotification* notification, int bufferId) {

    NppNotification n(NppNotificationType::MacroRecordingStarted,
                      NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("message", static_cast<int>(notification->npMessage));
    n.data.set("wParam", notification->npWParam);
    n.data.set("lParam", notification->npLParam);
    NppNotificationHandler::instance().emitNotification(n);
    return false;
}

bool NppSciNotificationBridge::handleMarginRightClick(
    const SCNotification* notification, int bufferId) {

    NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("eventType", "marginRightClick");
    n.data.set("position", notification->npPosition);
    n.data.set("modifiers", notification->npModifiers);
    NppNotificationHandler::instance().emitNotification(n);
    return false;
}

bool NppSciNotificationBridge::handleNeedsShown(
    const SCNotification* notification, int bufferId) {

    NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("eventType", "needShown");
    n.data.set("position", notification->npPosition);
    n.data.set("length", notification->npLength);
    NppNotificationHandler::instance().emitNotification(n);
    return false;
}

bool NppSciNotificationBridge::handleHotspot(
    const SCNotification* notification, int bufferId) {

    const unsigned int code = notification->npNmhdr.npCode;
    QString eventType;

    if (code == SCN_HOTSPOTCLICK)       eventType = "hotspotClick";
    else if (code == SCN_HOTSPOTDOUBLECLICK) eventType = "hotspotDoubleClick";
    else if (code == SCN_HOTSPOTRELEASECLICK) eventType = "hotspotReleaseClick";

    NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("eventType", eventType);
    n.data.set("position", notification->npPosition);
    n.data.set("modifiers", notification->npModifiers);
    NppNotificationHandler::instance().emitNotification(n);
    return true;
}

bool NppSciNotificationBridge::handleIndicator(
    const SCNotification* notification, int bufferId) {

    const unsigned int code = notification->npNmhdr.npCode;
    QString eventType = (code == SCN_INDICATORCLICK)
        ? "indicatorClick" : "indicatorRelease";

    NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Low);
    n.data.bufferId = bufferId;
    n.data.set("eventType", eventType);
    n.data.set("position", notification->npPosition);
    NppNotificationHandler::instance().emitNotification(n);
    return false;
}

bool NppSciNotificationBridge::handleModifyAttemptRO(
    const SCNotification* notification, int bufferId) {

    NppNotification n(NppNotificationType::BufferReadOnlyChanged,
                        NppNotificationPriority::High);
    n.data.bufferId = bufferId;
    n.data.set("readOnly", true);
    n.data.set("eventType", "modifyAttemptRO");
    NppNotificationHandler::instance().emitNotification(n);
    return true;
}

bool NppSciNotificationBridge::handleUserListSelection(
    const SCNotification* notification, int bufferId) {

    NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("listType", notification->npListType);
    n.data.set("text", notification->npText
                              ? QString::fromUtf8(notification->npText)
                              : QString());
    NppNotificationHandler::instance().emitNotification(n);
    return true;
}

bool NppSciNotificationBridge::handleUriDropped(
    const SCNotification* notification, int bufferId) {

    NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("eventType", "uriDropped");
    if (notification->npText) {
        n.data.set("uri", QString::fromUtf8(notification->npText,
                                             notification->npLength));
    }
    NppNotificationHandler::instance().emitNotification(n);
    return true;
}
