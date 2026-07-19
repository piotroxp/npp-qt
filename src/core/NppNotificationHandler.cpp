// NppNotificationHandler.cpp — full Qt6 implementation
// Ported from: PowerEditor/src/ScintillaComponent/Npp.cpp
#include "MISC/Common/NppNotification.h"
#include <QCoreApplication>
#include <QReadLocker>
#include <QWriteLocker>
#include <QMetaMethod>
#include <QDebug>
#include <algorithm>

// =============================================================================
// NppNotificationHandler — Singleton
// =============================================================================

NppNotificationHandler::NppNotificationHandler()
    : QObject(nullptr) {}

NppNotificationHandler::~NppNotificationHandler() {
    flushPending();
}

NppNotificationHandler& NppNotificationHandler::instance() {
    static NppNotificationHandler inst;
    return inst;
}

// =============================================================================
// Observer Management
// =============================================================================

void NppNotificationHandler::registerObserver(
    NppNotificationObserver* observer, const QString& name) {
    if (!observer) return;
    QWriteLocker locker(&_observersLock);
    ObserverEntry entry;
    entry.observer = observer;
    entry.name = name.isEmpty()
        ? QString("observer_%1").arg(reinterpret_cast<quintptr>(observer))
        : name;
    _observers.append(entry);
}

void NppNotificationHandler::unregisterObserver(NppNotificationObserver* observer) {
    QWriteLocker locker(&_observersLock);
    _observers.erase(
        std::remove_if(_observers.begin(), _observers.end(),
            [observer](const ObserverEntry& e) { return e.observer == observer; }),
        _observers.end());
}

void NppNotificationHandler::registerObserver(
    NppNotificationCallback callback, const QString& name) {
    QWriteLocker locker(&_observersLock);
    CallbackEntry entry;
    entry.callback = std::move(callback);
    entry.name = name.isEmpty()
        ? QString("callback_%1").arg(_callbackObservers.size())
        : name;
    _callbackObservers.append(std::move(entry));
}

void NppNotificationHandler::unregisterObserver(const QString& name) {
    QWriteLocker locker(&_observersLock);
    _observers.erase(
        std::remove_if(_observers.begin(), _observers.end(),
            [&name](const ObserverEntry& e) { return e.name == name; }),
        _observers.end());
    _callbackObservers.erase(
        std::remove_if(_callbackObservers.begin(), _callbackObservers.end(),
            [&name](const CallbackEntry& e) { return e.name == name; }),
        _callbackObservers.end());
}

template<typename T>
void NppNotificationHandler::registerObserver(
    T* obj, bool (T::*method)(const NppNotification&), const QString& name) {
    auto wrapper = [obj, method](const NppNotification& n) -> bool {
        return (obj->*method)(n);
    };
    registerObserver(static_cast<NppNotificationCallback>(wrapper), name);
}

// =============================================================================
// Filter Management
// =============================================================================

void NppNotificationHandler::setGlobalFilter(const NppNotificationFilter& filter) {
    QWriteLocker locker(&_filterLock);
    _globalFilter = filter;
}

void NppNotificationHandler::clearGlobalFilter() {
    QWriteLocker locker(&_filterLock);
    _globalFilter = NppNotificationFilter{};
}

// =============================================================================
// Notification Dispatch (thread-safe)
// =============================================================================

void NppNotificationHandler::postNotification(const NppNotification& notification) {
    {
        QMutexLocker locker(&_enabledMutex);
        if (!_enabled) return;
    }

    {
        QWriteLocker locker(&_statsLock);
        ++_totalPosted;
    }

    {
        QReadLocker locker(&_filterLock);
        if (!_globalFilter.matches(notification)) return;
    }

    {
        QMutexLocker locker(&_historyMutex);
        _history.append(notification);
        if (_history.size() > 1000)
            _history.erase(_history.begin(), _history.begin() + 100);
    }

    if (_aggregationEnabled)
        updateAggregation(notification);

    emit notificationPosted(notification);

    if (_threadSafe) {
        QMutexLocker locker(&_pendingMutex);
        _pendingQueue.append(notification);
    } else {
        dispatchNotification(notification);
    }
}

void NppNotificationHandler::postNotification(
    NppNotificationType type, const NppNotificationData& data,
    NppNotificationPriority priority) {
    postNotification(NppNotification(type, data, priority));
}

void NppNotificationHandler::emitNotification(const NppNotification& notification) {
    {
        QWriteLocker locker(&_statsLock);
        ++_totalEmitted;
    }
    dispatchNotification(notification);
    emit notificationEmitted(notification);
}

void NppNotificationHandler::dispatchNotification(const NppNotification& notification) {
    dispatchToObservers(notification);
    dispatchToCallbacks(notification);
}

void NppNotificationHandler::dispatchToObservers(const NppNotification& notification) {
    QVector<ObserverEntry> copy;
    {
        QReadLocker locker(&_observersLock);
        copy = _observers;
    }

    for (const auto& entry : copy) {
        if (!entry.observer) continue;
        if (notification.priority < entry.observer->minimumPriority()) continue;
        if (!entry.observer->acceptsNotification(notification.type)) continue;

        bool handled = entry.observer->onNotification(notification);
        if (handled) {
            QWriteLocker locker(&_statsLock);
            ++_totalHandled;
        }
    }
}

void NppNotificationHandler::dispatchToCallbacks(const NppNotification& notification) {
    QVector<CallbackEntry> copy;
    {
        QReadLocker locker(&_observersLock);
        copy = _callbackObservers;
    }

    for (const auto& entry : copy) {
        if (!entry.callback) continue;
        bool handled = entry.callback(notification);
        if (handled) {
            QWriteLocker locker(&_statsLock);
            ++_totalHandled;
        }
    }
}

void NppNotificationHandler::updateAggregation(const NppNotification& notification) {
    QMutexLocker locker(&_aggregationMutex);
    auto it = std::find_if(_aggregates.begin(), _aggregates.end(),
        [&notification](const NppNotificationAggregate& agg) {
            return agg.type == notification.type;
        });

    if (it != _aggregates.end()) {
        it->add(notification);
    } else {
        _aggregates.append(NppNotificationAggregate(notification.type));
        _aggregates.last().add(notification);
    }
}

// =============================================================================
// Convenience Buffer Notifications
// =============================================================================

void NppNotificationHandler::notifyBufferOpened(int bufferId, const QString& path) {
    postNotification(NppNotification::bufferOpened(bufferId, path));
    emit bufferOpened(bufferId);
}

void NppNotificationHandler::notifyBufferClosed(int bufferId) {
    postNotification(NppNotification::bufferClosed(bufferId));
    emit bufferClosed(bufferId);
}

void NppNotificationHandler::notifyBufferActivated(int bufferId, int viewId) {
    postNotification(NppNotification::bufferActivated(bufferId, viewId));
    emit bufferActivated(bufferId, viewId);
}

void NppNotificationHandler::notifyBufferModified(int bufferId, bool dirty) {
    postNotification(NppNotification::bufferModified(bufferId, dirty));
    emit bufferModified(bufferId, dirty);
}

void NppNotificationHandler::notifyBufferEncodingChanged(
    int bufferId, const QString& oldEnc, const QString& newEnc) {
    postNotification(NppNotification::bufferEncodingChanged(bufferId, oldEnc, newEnc));
}

void NppNotificationHandler::notifyBufferLanguageChanged(
    int bufferId, const QString& oldLang, const QString& newLang) {
    postNotification(NppNotification::bufferLanguageChanged(bufferId, oldLang, newLang));
}

void NppNotificationHandler::notifyBufferSaved(int bufferId, const QString& path) {
    postNotification(NppNotification::fileSaved(bufferId, path));
    emit bufferSaved(bufferId);
}

// =============================================================================
// Convenience File Notifications
// =============================================================================

void NppNotificationHandler::notifyFileDeleted(const QString& path) {
    postNotification(NppNotification::fileDeleted(path));
    emit fileDeleted(path);
}

void NppNotificationHandler::notifyFileRenamed(const QString& oldPath,
                                               const QString& newPath) {
    postNotification(NppNotification::fileRenamed(oldPath, newPath));
    emit fileRenamed(oldPath, newPath);
}

void NppNotificationHandler::notifyFileExternallyModified(const QString& path) {
    postNotification(NppNotification::fileExternallyModified(path));
    emit fileExternallyModified(path);
}

// =============================================================================
// Session Notifications
// =============================================================================

void NppNotificationHandler::notifySessionLoaded(const QString& sessionFile) {
    postNotification(NppNotification::sessionLoaded(sessionFile));
    emit sessionLoaded(sessionFile);
}

void NppNotificationHandler::notifySessionSaved(const QString& sessionFile) {
    postNotification(NppNotification::sessionSaved(sessionFile));
}

void NppNotificationHandler::notifySessionError(const QString& error) {
    postNotification(NppNotification::sessionError(error));
    emit sessionError(error);
}

// =============================================================================
// Plugin Notifications
// =============================================================================

void NppNotificationHandler::notifyPluginLoaded(const QString& name) {
    postNotification(NppNotification::pluginLoaded(name));
    emit pluginLoaded(name);
}

void NppNotificationHandler::notifyPluginUnloaded(const QString& name) {
    postNotification(NppNotification::pluginUnloaded(name));
    emit pluginUnloaded(name);
}

void NppNotificationHandler::notifyPluginError(const QString& name,
                                                const QString& error) {
    postNotification(NppNotification::pluginError(name, error));
    emit pluginError(name, error);
}

// =============================================================================
// UI Notifications
// =============================================================================

void NppNotificationHandler::notifyThemeChanged(const QString& themeName) {
    postNotification(NppNotification::themeChanged(themeName));
    emit themeChanged(themeName);
}

void NppNotificationHandler::notifyZoomChanged(int zoom) {
    postNotification(NppNotification::zoomChanged(zoom));
    emit zoomChanged(zoom);
}

void NppNotificationHandler::notifyApplicationError(const QString& error,
                                                       const QString& details) {
    postNotification(NppNotification::applicationError(error, details));
    emit applicationError(error);
}

// =============================================================================
// History
// =============================================================================

QVector<NppNotification> NppNotificationHandler::history(int maxCount) const {
    QMutexLocker locker(&_historyMutex);
    QVector<NppNotification> result;
    int start = qMax(0, _history.size() - maxCount);
    for (int i = start; i < _history.size(); ++i)
        result.append(_history[i]);
    return result;
}

QVector<NppNotification> NppNotificationHandler::history(
    NppNotificationType type, int maxCount) const {
    QMutexLocker locker(&_historyMutex);
    QVector<NppNotification> result;
    for (int i = _history.size() - 1; i >= 0 && result.size() < maxCount; --i) {
        if (_history[i].type == type)
            result.prepend(_history[i]);
    }
    return result;
}

void NppNotificationHandler::clearHistory() {
    QMutexLocker locker(&_historyMutex);
    _history.clear();
}

// =============================================================================
// Aggregation
// =============================================================================

QVector<NppNotificationAggregate> NppNotificationHandler::getAggregates(
    NppNotificationType type, int windowSeconds) const {
    QMutexLocker locker(&_aggregationMutex);
    QVector<NppNotificationAggregate> result;
    QDateTime cutoff = QDateTime::currentDateTimeUtc().addSecs(-windowSeconds);
    for (const auto& agg : _aggregates) {
        if (agg.type == type && agg.lastSeen >= cutoff)
            result.append(agg);
    }
    return result;
}

// =============================================================================
// Flush Pending Queue
// =============================================================================

void NppNotificationHandler::flushPending() {
    QVector<NppNotification> queue;
    {
        QMutexLocker locker(&_pendingMutex);
        if (_pendingQueue.isEmpty()) return;
        queue = std::move(_pendingQueue);
        _pendingQueue.clear();
    }
    for (const auto& n : qAsConst(queue))
        dispatchNotification(n);
}

// =============================================================================
// Enable / Thread Safety
// =============================================================================

void NppNotificationHandler::setEnabled(bool enabled) {
    QMutexLocker locker(&_enabledMutex);
    _enabled = enabled;
}

void NppNotificationHandler::enableThreadSafety(bool enabled) {
    _threadSafe = enabled;
}
