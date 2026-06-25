// Ported to Qt6
#include "NppNotification.h"
#include <QDebug>

NppNotificationHandler::NppNotificationHandler(QObject* parent) : QObject(parent) {}
NppNotificationHandler::~NppNotificationHandler() = default;

bool NppNotificationHandler::handleScintillaNotification(const SCNotification* notification)
{
    if (!notification) return false;
    switch (notification->nmhdr.code) {
        case SCN_MODIFIED: return handleModified(notification);
        case SCN_SAVEPOINTREACHED:
        case SCN_SAVEPOINTLEFT: return handleSavePoint(notification);
        case SCN_MARGINCLICK: return handleMarginClick(notification);
        case SCN_CHARADDED: return handleCharAdded(notification);
        case SCN_DOUBLECLICK: return handleDoubleClick(notification);
        case SCN_UPDATEUI: return handleUpdateUI(notification);
        case SCN_ZOOM: return handleZoom(notification);
        case SCN_PAINTED: return handlePainted(notification);
        default: return false;
    }
}

bool NppNotificationHandler::handleNppNotification(void*, unsigned int, uintptr_t, intptr_t) { return false; }

bool NppNotificationHandler::handleModified(const SCNotification*) { return false; }
bool NppNotificationHandler::handleSavePoint(const SCNotification*) { return false; }
bool NppNotificationHandler::handleMarginClick(const SCNotification*) { return false; }
bool NppNotificationHandler::handleCharAdded(const SCNotification*) { return false; }
bool NppNotificationHandler::handleDoubleClick(const SCNotification*) { return false; }

bool NppNotificationHandler::handleUpdateUI(const SCNotification*)
{
    emit selectionChanged();
    return false;
}

bool NppNotificationHandler::handleZoom(const SCNotification*)
{
    emit zoomChanged(0);
    return false;
}

bool NppNotificationHandler::handlePainted(const SCNotification*) { return false; }
