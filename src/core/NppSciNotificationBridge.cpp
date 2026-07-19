#include "MISC/Common/NppNotification.h"

NppSciNotificationBridge::NppSciNotificationBridge(QObject* parent) : QObject(parent) {}
NppSciNotificationBridge::~NppSciNotificationBridge() = default;
bool NppSciNotificationBridge::handleScintillaNotification(const SCNotification*, int) { return false; }
bool NppSciNotificationBridge::handleNppNotification(void*, unsigned int, uintptr_t, intptr_t) { return false; }
NppNotification NppSciNotificationBridge::toNotification(const SCNotification*, int) const { return NppNotification{}; }
