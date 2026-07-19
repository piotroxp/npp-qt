// Ported to Qt6: Comprehensive N++ Notification System Implementation
#include "NppNotification.h"
#include <QCoreApplication>
#include <QReadLocker>
#include <QWriteLocker>
#include <QMetaMethod>
#include <QDebug>

// =============================================================================
// NppNotification Static Factories
// =============================================================================

NppNotification NppNotification::bufferOpened(int bufferId, const QString& path) {
    NppNotification n(NppNotificationType::BufferOpened, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("path", path);
    return n;
}

NppNotification NppNotification::bufferClosed(int bufferId) {
    NppNotification n(NppNotificationType::BufferClosed, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    return n;
}

NppNotification NppNotification::bufferActivated(int bufferId, int viewId) {
    NppNotification n(NppNotificationType::BufferActivated, NppNotificationPriority::High);
    n.data.bufferId = bufferId;
    n.data.viewId = viewId;
    return n;
}

NppNotification NppNotification::bufferModified(int bufferId, bool dirty) {
    NppNotification n(dirty ? NppNotificationType::BufferDirty : NppNotificationType::BufferClean,
                      NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("dirty", dirty);
    return n;
}

NppNotification NppNotification::bufferEncodingChanged(int bufferId, const QString& oldEnc, const QString& newEnc) {
    NppNotification n(NppNotificationType::BufferEncodingChanged, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("oldEncoding", oldEnc);
    n.data.set("newEncoding", newEnc);
    return n;
}

NppNotification NppNotification::bufferLanguageChanged(int bufferId, const QString& oldLang, const QString& newLang) {
    NppNotification n(NppNotificationType::BufferLanguageChanged, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("oldLanguage", oldLang);
    n.data.set("newLanguage", newLang);
    return n;
}

NppNotification NppNotification::fileSaved(int bufferId, const QString& path) {
    NppNotification n(NppNotificationType::FileSaved, NppNotificationPriority::Normal);
    n.data.bufferId = bufferId;
    n.data.set("path", path);
    return n;
}

NppNotification NppNotification::fileDeleted(const QString& path) {
    NppNotification n(NppNotificationType::FileDeleted, NppNotificationPriority::High);
    n.data.set("path", path);
    return n;
}

NppNotification NppNotification::fileRenamed(const QString& oldPath, const QString& newPath) {
    NppNotification n(NppNotificationType::FileRenamed, NppNotificationPriority::High);
    n.data.set("oldPath", oldPath);
    n.data.set("newPath", newPath);
    return n;
}

NppNotification NppNotification::fileExternallyModified(const QString& path) {
    NppNotification n(NppNotificationType::FileExternallyModified, NppNotificationPriority::High);
    n.data.set("path", path);
    return n;
}

NppNotification NppNotification::sessionLoaded(const QString& sessionFile) {
    NppNotification n(NppNotificationType::SessionLoaded, NppNotificationPriority::High);
    n.data.set("sessionFile", sessionFile);
    return n;
}

NppNotification NppNotification::sessionSaved(const QString& sessionFile) {
    NppNotification n(NppNotificationType::SessionSaved, NppNotificationPriority::Normal);
    n.data.set("sessionFile", sessionFile);
    return n;
}

NppNotification NppNotification::sessionError(const QString& error) {
    NppNotification n(NppNotificationType::SessionLoadError, NppNotificationPriority::Critical);
    n.data.set("error", error);
    return n;
}

NppNotification NppNotification::pluginLoaded(const QString& name) {
    NppNotification n(NppNotificationType::PluginLoaded, NppNotificationPriority::Normal);
    n.data.set("pluginName", name);
    return n;
}

NppNotification NppNotification::pluginUnloaded(const QString& name) {
    NppNotification n(NppNotificationType::PluginUnloaded, NppNotificationPriority::Normal);
    n.data.set("pluginName", name);
    return n;
}

NppNotification NppNotification::pluginError(const QString& name, const QString& error) {
    NppNotification n(NppNotificationType::PluginError, NppNotificationPriority::Critical);
    n.data.set("pluginName", name);
    n.data.set("error", error);
    return n;
}

NppNotification NppNotification::themeChanged(const QString& themeName) {
    NppNotification n(NppNotificationType::ThemeChanged, NppNotificationPriority::High);
    n.data.set("themeName", themeName);
    return n;
}

NppNotification NppNotification::zoomChanged(int zoom) {
    NppNotification n(NppNotificationType::ZoomChanged, NppNotificationPriority::Low);
    n.data.set("zoom", zoom);
    return n;
}

NppNotification NppNotification::applicationError(const QString& error, const QString& details) {
    NppNotification n(NppNotificationType::ApplicationError, NppNotificationPriority::Critical);
    n.data.set("error", error);
    n.data.set("details", details);
    return n;
}

NppNotification NppNotification::custom(const QString& eventType, const NppNotificationData& extraData) {
    NppNotification n(NppNotificationType::Custom, NppNotificationPriority::Normal);
    n.data.set("eventType", eventType);
    for (auto it = extraData.metadata.constBegin(); it != extraData.metadata.constEnd(); ++it) n.data.metadata.insert(it.key(), it.value());
    return n;
}

// =============================================================================
// NppNotificationFilter
// =============================================================================

bool NppNotificationFilter::matches(const NppNotification& notification) const {
    // Check priority
    if (notification.priority < minimumPriority) {
        return false;
    }

    // Check type inclusion (if any types specified)
    if (!acceptedTypes.isEmpty() && !acceptedTypes.contains(notification.type)) {
        return false;
    }

    // Check type exclusion
    if (rejectedTypes.contains(notification.type)) {
        return false;
    }

    // Check source filter
    if (!sourceFilter.isEmpty() && !notification.data.source.isEmpty() &&
        !notification.data.source.contains(sourceFilter, Qt::CaseInsensitive)) {
        return false;
    }

    return true;
}

NppNotificationFilter NppNotificationFilter::all() {
    return NppNotificationFilter{};
}

NppNotificationFilter NppNotificationFilter::bufferOnly() {
    NppNotificationFilter f;
    f.acceptedTypes = {
        NppNotificationType::BufferOpened, NppNotificationType::BufferClosed,
        NppNotificationType::BufferActivated, NppNotificationType::BufferCreated,
        NppNotificationType::BufferDeleted, NppNotificationType::BufferModified,
        NppNotificationType::BufferReloaded, NppNotificationType::BufferSaved,
        NppNotificationType::BufferDirty, NppNotificationType::BufferClean,
        NppNotificationType::BufferEncodingChanged, NppNotificationType::BufferLanguageChanged,
        NppNotificationType::BufferReadOnlyChanged, NppNotificationType::BufferEolChanged
    };
    return f;
}

NppNotificationFilter NppNotificationFilter::fileOnly() {
    NppNotificationFilter f;
    f.acceptedTypes = {
        NppNotificationType::FileOpened, NppNotificationType::FileClosed,
        NppNotificationType::FileSaved, NppNotificationType::FileDeleted,
        NppNotificationType::FileRenamed, NppNotificationType::FileMoved,
        NppNotificationType::FilePermissionsChanged, NppNotificationType::FileMonitorTriggered,
        NppNotificationType::FileExternallyModified, NppNotificationType::FileExternallyDeleted,
        NppNotificationType::FileConflict
    };
    return f;
}

NppNotificationFilter NppNotificationFilter::uiOnly() {
    NppNotificationFilter f;
    f.acceptedTypes = {
        NppNotificationType::ThemeChanged, NppNotificationType::ThemeLoading,
        NppNotificationType::StyleChanged, NppNotificationType::LayoutChanged,
        NppNotificationType::PanelVisibilityChanged, NppNotificationType::PanelDocked,
        NppNotificationType::PanelUndocked, NppNotificationType::ZoomChanged,
        NppNotificationType::DpiChanged, NppNotificationType::FontChanged,
        NppNotificationType::LanguageChanged, NppNotificationType::EditorFocused,
        NppNotificationType::EditorLostFocus, NppNotificationType::SelectionChanged,
        NppNotificationType::CursorPositionChanged, NppNotificationType::ScrollChanged
    };
    return f;
}

NppNotificationFilter NppNotificationFilter::highPriorityOnly() {
    NppNotificationFilter f;
    f.minimumPriority = NppNotificationPriority::High;
    return f;
}

// =============================================================================
// NppNotificationHandler Singleton
// =============================================================================

NppNotificationHandler::NppNotificationHandler()
    : QObject(nullptr) {
    // Private constructor - singleton
}

NppNotificationHandler::~NppNotificationHandler() {
    // Ensure all pending notifications are flushed
    flushPending();
}

NppNotificationHandler& NppNotificationHandler::instance() {
    static NppNotificationHandler instance;
    return instance;
}

// =============================================================================
// Observer Management
// =============================================================================

void NppNotificationHandler::registerObserver(NppNotificationObserver* observer, const QString& name) {
    if (!observer) return;

    QWriteLocker locker(&_observersLock);
    ObserverEntry entry;
    entry.observer = observer;
    entry.name = name.isEmpty() ? QString("observer_%1").arg(reinterpret_cast<quintptr>(observer)) : name;
    _observers.append(entry);
}

void NppNotificationHandler::unregisterObserver(NppNotificationObserver* observer) {
    QWriteLocker locker(&_observersLock);
    for (auto it = _observers.begin(); it != _observers.end(); ) {
        if (it->observer == observer) {
            it = _observers.erase(it);
        } else {
            ++it;
        }
    }
}

void NppNotificationHandler::registerObserver(NppNotificationCallback callback, const QString& name) {
    QWriteLocker locker(&_observersLock);
    CallbackEntry entry;
    entry.callback = std::move(callback);
    entry.name = name.isEmpty() ? QString("callback_%1").arg(_callbackObservers.size()) : name;
    _callbackObservers.append(std::move(entry));
}

void NppNotificationHandler::unregisterObserver(const QString& name) {
    QWriteLocker locker(&_observersLock);
    _observers.erase(std::remove_if(_observers.begin(), _observers.end(),
        [&name](const ObserverEntry& e) { return e.name == name; }), _observers.end());
    _callbackObservers.erase(std::remove_if(_callbackObservers.begin(), _callbackObservers.end(),
        [&name](const CallbackEntry& e) { return e.name == name; }), _callbackObservers.end());
}

template<typename T>
void NppNotificationHandler::registerObserver(
    T* obj, bool (T::*method)(const NppNotification&), const QString& name) {
    // Wrap member function as callback
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
// Notification Dispatch
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

    // Check global filter
    {
        QReadLocker locker(&_filterLock);
        if (!_globalFilter.matches(notification)) {
            return;
        }
    }

    // Add to history
    {
        QMutexLocker locker(&_historyMutex);
        _history.append(notification);
        // Trim history if too large
        if (_history.size() > 1000) {
            _history.erase(_history.begin(), _history.begin() + 100);
        }
    }

    // Update aggregation
    if (_aggregationEnabled) {
        updateAggregation(notification);
    }

    emit notificationPosted(notification);

    // Queue for batch processing if thread-safe mode
    if (_threadSafe) {
        QMutexLocker locker(&_pendingMutex);
        _pendingQueue.append(notification);
        // Don't process immediately in thread-safe mode
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
    QVector<ObserverEntry> observersCopy;
    {
        QReadLocker locker(&_observersLock);
        observersCopy = _observers;
    }

    for (const auto& entry : observersCopy) {
        NppNotificationObserver* observer = entry.observer;
        if (!observer) continue;

        // Check priority filter
        if (notification.priority < observer->minimumPriority()) {
            continue;
        }

        // Check type filter
        if (!observer->acceptsNotification(notification.type)) {
            continue;
        }

        bool handled = observer->onNotification(notification);
        if (handled) {
            {
                QWriteLocker locker(&_statsLock);
                ++_totalHandled;
            }
        }
    }
}

void NppNotificationHandler::dispatchToCallbacks(const NppNotification& notification) {
    QVector<CallbackEntry> callbacksCopy;
    {
        QReadLocker locker(&_observersLock);
        callbacksCopy = _callbackObservers;
    }

    for (const auto& entry : callbacksCopy) {
        if (entry.callback) {
            bool handled = entry.callback(notification);
            if (handled) {
                QWriteLocker locker(&_statsLock);
                ++_totalHandled;
            }
        }
    }
}

void NppNotificationHandler::updateAggregation(const NppNotification& notification) {
    QMutexLocker locker(&_aggregationMutex);

    // Find existing aggregate or create new one
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
// Convenience Notification Methods
// =============================================================================

void NppNotificationHandler::notifyBufferOpened(int bufferId, const QString& path) {
    NppNotificationData data;
    data.bufferId = bufferId;
    data.set("path", path);
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

void NppNotificationHandler::notifyBufferEncodingChanged(int bufferId, const QString& oldEnc, const QString& newEnc) {
    postNotification(NppNotification::bufferEncodingChanged(bufferId, oldEnc, newEnc));
}

void NppNotificationHandler::notifyBufferLanguageChanged(int bufferId, const QString& oldLang, const QString& newLang) {
    postNotification(NppNotification::bufferLanguageChanged(bufferId, oldLang, newLang));
}

void NppNotificationHandler::notifyBufferSaved(int bufferId, const QString& path) {
    postNotification(NppNotification::fileSaved(bufferId, path));
    emit bufferSaved(bufferId);
}

void NppNotificationHandler::notifyFileDeleted(const QString& path) {
    postNotification(NppNotification::fileDeleted(path));
    emit fileDeleted(path);
}

void NppNotificationHandler::notifyFileRenamed(const QString& oldPath, const QString& newPath) {
    postNotification(NppNotification::fileRenamed(oldPath, newPath));
    emit fileRenamed(oldPath, newPath);
}

void NppNotificationHandler::notifyFileExternallyModified(const QString& path) {
    postNotification(NppNotification(NppNotificationType::FileExternallyModified,
        NppNotificationPriority::High));
    emit fileExternallyModified(path);
}

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

void NppNotificationHandler::notifyPluginLoaded(const QString& name) {
    postNotification(NppNotification::pluginLoaded(name));
    emit pluginLoaded(name);
}

void NppNotificationHandler::notifyPluginUnloaded(const QString& name) {
    postNotification(NppNotification::pluginUnloaded(name));
    emit pluginUnloaded(name);
}

void NppNotificationHandler::notifyPluginError(const QString& name, const QString& error) {
    postNotification(NppNotification::pluginError(name, error));
    emit pluginError(name, error);
}

void NppNotificationHandler::notifyThemeChanged(const QString& themeName) {
    postNotification(NppNotification::themeChanged(themeName));
    emit themeChanged(themeName);
}

void NppNotificationHandler::notifyZoomChanged(int zoom) {
    postNotification(NppNotification::zoomChanged(zoom));
    emit zoomChanged(zoom);
}

void NppNotificationHandler::notifyApplicationError(const QString& error, const QString& details) {
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
    for (int i = start; i < _history.size(); ++i) {
        result.append(_history[i]);
    }
    return result;
}

QVector<NppNotification> NppNotificationHandler::history(NppNotificationType type, int maxCount) const {
    QMutexLocker locker(&_historyMutex);
    QVector<NppNotification> result;
    for (int i = _history.size() - 1; i >= 0 && result.size() < maxCount; --i) {
        if (_history[i].type == type) {
            result.prepend(_history[i]);
        }
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
        if (agg.type == type && agg.lastSeen >= cutoff) {
            result.append(agg);
        }
    }
    return result;
}

// =============================================================================
// Flush Pending
// =============================================================================

void NppNotificationHandler::flushPending() {
    QVector<NppNotification> queue;
    {
        QMutexLocker locker(&_pendingMutex);
        if (_pendingQueue.isEmpty()) return;
        queue = std::move(_pendingQueue);
        _pendingQueue.clear();
    }

    for (const auto& n : queue) {
        dispatchNotification(n);
    }
}

// =============================================================================
// Enable/Disable & Thread Safety
// =============================================================================

void NppNotificationHandler::setEnabled(bool enabled) {
    QMutexLocker locker(&_enabledMutex);
    _enabled = enabled;
}

void NppNotificationHandler::enableThreadSafety(bool enabled) {
    _threadSafe = enabled;
}

// =============================================================================
