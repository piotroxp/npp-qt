// Ported to Qt6: Comprehensive N++ Notification System
#pragma once
#include <QtCore>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QSet>
#include <QDateTime>
#include <QMutex>
#include <QWaitCondition>
#include <QReadWriteLock>

// Forward declarations
class Buffer;
struct Sci_NotifyHeader;
struct SCNotification;

// =============================================================================
// Notification Types
// =============================================================================

// Buffer notifications
enum class NppNotificationType {
    // Buffer lifecycle
    BufferOpened,
    BufferClosed,
    BufferActivated,
    BufferCreated,
    BufferDeleted,

    // Buffer content
    BufferModified,
    BufferReloaded,
    BufferSaved,
    BufferDirty,
    BufferClean,

    // Buffer properties
    BufferEncodingChanged,
    BufferLanguageChanged,
    BufferReadOnlyChanged,
    BufferEolChanged,
    BufferTimestampChanged,

    // File notifications
    FileOpened,
    FileClosed,
    FileSaved,
    FileDeleted,
    FileRenamed,
    FileMoved,
    FilePermissionsChanged,
    FileMonitorTriggered,

    // File conflict / external change
    FileExternallyModified,
    FileExternallyDeleted,
    FileConflict,

    // UI notifications
    ThemeChanged,
    ThemeLoading,
    StyleChanged,
    LayoutChanged,
    PanelVisibilityChanged,
    PanelDocked,
    PanelUndocked,
    ZoomChanged,
    DpiChanged,
    FontChanged,
    LanguageChanged,

    // Session notifications
    SessionLoaded,
    SessionSaved,
    SessionLoadError,
    SessionSaveError,
    SessionNew,

    // Plugin notifications
    PluginLoaded,
    PluginUnloaded,
    PluginError,
    PluginCommandInvoked,
    PluginMessage,

    // Application notifications
    ApplicationStarting,
    ApplicationStarted,
    ApplicationShutdown,
    ApplicationError,

    // Editor notifications
    EditorFocused,
    EditorLostFocus,
    SelectionChanged,
    CursorPositionChanged,
    ScrollChanged,

    // Macro notifications
    MacroRecordingStarted,
    MacroRecordingStopped,
    MacroPlaybackStarted,
    MacroPlaybackStopped,

    // Find/Replace notifications
    FindStarted,
    FindCompleted,
    ReplaceCompleted,

    // Custom
    Custom,
    Unknown = 999
};

// Notification priority levels
enum class NppNotificationPriority {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

// =============================================================================
// Notification Data Structures
// =============================================================================

struct NppNotificationData {
    // Common fields
    QString source;
    QString target;
    QVariantMap metadata;
    QDateTime timestamp;
    int bufferId = -1;
    int viewId = -1;

    NppNotificationData() : timestamp(QDateTime::currentDateTimeUtc()) {}

    template<typename T>
    T get(const QString& key, const T& defaultValue = T{}) const {
        return metadata.value(key).template value<T>();
    }

    void set(const QString& key, const QVariant& value) {
        metadata.insert(key, value);
    }

    bool has(const QString& key) const {
        return metadata.contains(key);
    }
};

// Main notification structure
struct NppNotification {
    NppNotificationType type;
    NppNotificationPriority priority;
    NppNotificationData data;
    bool handled = false;

    NppNotification(NppNotificationType t = NppNotificationType::Unknown,
                    NppNotificationPriority p = NppNotificationPriority::Normal)
        : type(t), priority(p), data() {}

    NppNotification(NppNotificationType t, const NppNotificationData& d,
                    NppNotificationPriority p = NppNotificationPriority::Normal)
        : type(t), priority(p), data(d) {}

    // Convenience factories
    static NppNotification bufferOpened(int bufferId, const QString& path);
    static NppNotification bufferClosed(int bufferId);
    static NppNotification bufferActivated(int bufferId, int viewId = -1);
    static NppNotification bufferModified(int bufferId, bool dirty);
    static NppNotification bufferEncodingChanged(int bufferId, const QString& oldEnc, const QString& newEnc);
    static NppNotification bufferLanguageChanged(int bufferId, const QString& oldLang, const QString& newLang);

    static NppNotification fileSaved(int bufferId, const QString& path);
    static NppNotification fileDeleted(const QString& path);
    static NppNotification fileRenamed(const QString& oldPath, const QString& newPath);
    static NppNotification fileExternallyModified(const QString& path);

    static NppNotification sessionLoaded(const QString& sessionFile);
    static NppNotification sessionSaved(const QString& sessionFile);
    static NppNotification sessionError(const QString& error);

    static NppNotification pluginLoaded(const QString& name);
    static NppNotification pluginUnloaded(const QString& name);
    static NppNotification pluginError(const QString& name, const QString& error);

    static NppNotification themeChanged(const QString& themeName);
    static NppNotification zoomChanged(int zoom);
    static NppNotification applicationError(const QString& error, const QString& details = QString());

    static NppNotification custom(const QString& eventType, const NppNotificationData& extraData = NppNotificationData());
};

// =============================================================================
// Observer / Callback Interface
// =============================================================================

class NppNotificationObserver {
public:
    virtual ~NppNotificationObserver() = default;

    // Return true if notification was handled and should stop propagating
    virtual bool onNotification(const NppNotification& notification) = 0;

    // Optional: filter by type
    virtual bool acceptsNotification(NppNotificationType type) const { Q_UNUSED(type); return true; }

    // Optional: filter by priority
    virtual NppNotificationPriority minimumPriority() const { return NppNotificationPriority::Low; }
};

// Callback type alias
using NppNotificationCallback = std::function<bool(const NppNotification&)>;

// =============================================================================
// Notification Filter
// =============================================================================

struct NppNotificationFilter {
    QSet<NppNotificationType> acceptedTypes;
    QSet<NppNotificationType> rejectedTypes;
    NppNotificationPriority minimumPriority = NppNotificationPriority::Low;
    QString sourceFilter;  // Empty = any source

    bool matches(const NppNotification& notification) const;

    static NppNotificationFilter all();
    static NppNotificationFilter bufferOnly();
    static NppNotificationFilter fileOnly();
    static NppNotificationFilter uiOnly();
    static NppNotificationFilter highPriorityOnly();
};

// =============================================================================
// Notification Aggregator
// =============================================================================

struct NppNotificationAggregate {
    NppNotificationType type;
    int count = 0;
    QDateTime firstSeen;
    QDateTime lastSeen;
    NppNotificationData lastData;

    NppNotificationAggregate(NppNotificationType t) : type(t), firstSeen(QDateTime::currentDateTimeUtc()) {
        lastSeen = firstSeen;
    }

    void add(const NppNotification& notification) {
        count++;
        lastSeen = QDateTime::currentDateTimeUtc();
        lastData = notification.data;
    }
};

// =============================================================================
// Main Notification Handler (Observer Pattern)
// =============================================================================

class NppNotificationHandler : public QObject {
    Q_OBJECT
public:
    static NppNotificationHandler& instance();

    // Observer management
    void registerObserver(NppNotificationObserver* observer, const QString& name = QString());
    void unregisterObserver(NppNotificationObserver* observer);
    void registerObserver(NppNotificationCallback callback, const QString& name = QString());
    void unregisterObserver(const QString& name);

    // Convenience: register slot-based observer
    template<typename T>
    void registerObserver(T* obj, bool (T::*method)(const NppNotification&),
                          const QString& name = QString());

    // Filter management
    void setGlobalFilter(const NppNotificationFilter& filter);
    void clearGlobalFilter();
    const NppNotificationFilter& globalFilter() const { QReadLocker locker(&_filterLock); return _globalFilter; }

    // Notification dispatch (thread-safe)
    void postNotification(const NppNotification& notification);
    void postNotification(NppNotificationType type, const NppNotificationData& data = NppNotificationData(),
                          NppNotificationPriority priority = NppNotificationPriority::Normal);
    void emitNotification(const NppNotification& notification);  // Synchronous

    // Buffer notifications (convenience)
    void notifyBufferOpened(int bufferId, const QString& path);
    void notifyBufferClosed(int bufferId);
    void notifyBufferActivated(int bufferId, int viewId = -1);
    void notifyBufferModified(int bufferId, bool dirty);
    void notifyBufferEncodingChanged(int bufferId, const QString& oldEnc, const QString& newEnc);
    void notifyBufferLanguageChanged(int bufferId, const QString& oldLang, const QString& newLang);
    void notifyBufferSaved(int bufferId, const QString& path);

    // File notifications
    void notifyFileDeleted(const QString& path);
    void notifyFileRenamed(const QString& oldPath, const QString& newPath);
    void notifyFileExternallyModified(const QString& path);

    // Session notifications
    void notifySessionLoaded(const QString& sessionFile);
    void notifySessionSaved(const QString& sessionFile);
    void notifySessionError(const QString& error);

    // Plugin notifications
    void notifyPluginLoaded(const QString& name);
    void notifyPluginUnloaded(const QString& name);
    void notifyPluginError(const QString& name, const QString& error);

    // UI notifications
    void notifyThemeChanged(const QString& themeName);
    void notifyZoomChanged(int zoom);
    void notifyApplicationError(const QString& error, const QString& details = QString());

    // Notification history
    QVector<NppNotification> history(int maxCount = 100) const;
    QVector<NppNotification> history(NppNotificationType type, int maxCount = 50) const;
    void clearHistory();

    // Aggregation
    QVector<NppNotificationAggregate> getAggregates(NppNotificationType type, int windowSeconds = 60) const;
    void setAggregationEnabled(bool enabled) { _aggregationEnabled = enabled; }
    bool isAggregationEnabled() const { return _aggregationEnabled; }

    // Stats
    int totalPosted() const { QReadLocker locker(&_statsLock); return _totalPosted; }
    int totalEmitted() const { QReadLocker locker(&_statsLock); return _totalEmitted; }
    int totalHandled() const { QReadLocker locker(&_statsLock); return _totalHandled; }
    int observerCount() const { QReadLocker locker(&_observersLock); return _observers.size() + _callbackObservers.size(); }

    // Enable/disable (for batching)
    void setEnabled(bool enabled);
    bool isEnabled() const { QMutexLocker locker(&_enabledMutex); return _enabled; }

    // Thread safety
    void enableThreadSafety(bool enabled);
    bool isThreadSafe() const { return _threadSafe; }

signals:
    void notificationPosted(const NppNotification& notification);
    void notificationEmitted(const NppNotification& notification);
    void bufferOpened(int bufferId);
    void bufferClosed(int bufferId);
    void bufferActivated(int bufferId, int viewId);
    void bufferModified(int bufferId, bool dirty);
    void bufferSaved(int bufferId);
    void fileDeleted(const QString& path);
    void fileRenamed(const QString& oldPath, const QString& newPath);
    void fileExternallyModified(const QString& path);
    void themeChanged(const QString& themeName);
    void zoomChanged(int zoom);
    void pluginLoaded(const QString& name);
    void pluginUnloaded(const QString& name);
    void pluginError(const QString& name, const QString& error);
    void sessionLoaded(const QString& sessionFile);
    void sessionError(const QString& error);
    void applicationError(const QString& error);
    void aggregationReady(const QVector<NppNotificationAggregate>& aggregates);

public slots:
    void flushPending();

private:
    NppNotificationHandler();
    ~NppNotificationHandler() override;

    void dispatchNotification(const NppNotification& notification);
    void dispatchToObservers(const NppNotification& notification);
    void dispatchToCallbacks(const NppNotification& notification);
    void updateAggregation(const NppNotification& notification);

    // Observer storage
    struct ObserverEntry {
        NppNotificationObserver* observer = nullptr;
        QString name;
    };
    QVector<ObserverEntry> _observers;
    struct CallbackEntry {
        NppNotificationCallback callback;
        QString name;
    };
    QVector<CallbackEntry> _callbackObservers;

    mutable QReadWriteLock _observersLock;
    mutable QReadWriteLock _filterLock;
    mutable QReadWriteLock _statsLock;
    mutable QMutex _historyMutex;
    mutable QMutex _enabledMutex;
    mutable QMutex _aggregationMutex;

    NppNotificationFilter _globalFilter;
    QVector<NppNotification> _history;
    mutable QVector<NppNotificationAggregate> _aggregates;
    bool _aggregationEnabled = false;

    // Pending notifications queue (for batch mode)
    QVector<NppNotification> _pendingQueue;
    QMutex _pendingMutex;
    QWaitCondition _pendingCondition;
    bool _processingPending = false;

    // Stats
    int _totalPosted = 0;
    int _totalEmitted = 0;
    int _totalHandled = 0;

    // State
    bool _enabled = true;
    bool _threadSafe = true;

    Q_DISABLE_COPY_MOVE(NppNotificationHandler)
};

// =============================================================================
// Scintilla Notification Bridge
// =============================================================================

// Scintilla notification codes (SCN_*)
// Guarded so they don't leak into files that don't expect them
#ifndef NPP_SCN_DEFINED
#define NPP_SCINTILLA_NOTIFY_DEFINED

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
#ifndef SCN_AUTOCSELECTION
#define SCN_AUTOCSELECTION 2021
#endif
#ifndef SCN_AUTOCCANCELLED
#define SCN_AUTOCCANCELLED 2022
#endif

#endif  // NPP_SCINTILLA_NOTIFY_DEFINED

// Sci_NotifyHeader struct (minimal)
struct Sci_NotifyHeader {
    void* hwndFrom = nullptr;
    uintptr_t idFrom = 0;
    unsigned int code = 0;
};

// SCNotification struct (minimal)
struct SCNotification {
    Sci_NotifyHeader nmhdr;
    int position = 0;
    int ch = 0;
    int modifiers = 0;
    int modificationType = 0;
    const char* text = nullptr;
    int length = 0;
    int linesAdded = 0;
    int message = 0;
    uintptr_t wParam = 0;
    intptr_t lParam = 0;
    int line = 0;
    int foldLevelNow = 0;
    int foldLevelPrev = 0;
    int margin = 0;
    int listType = 0;
    int x = 0;
    int y = 0;
};

// Scintilla notification handler interface
class NppSciNotificationBridge : public QObject {
    Q_OBJECT
public:
    explicit NppSciNotificationBridge(QObject* parent = nullptr);
    ~NppSciNotificationBridge() override;

    // Handle Scintilla notification
    bool handleScintillaNotification(const SCNotification* notification, int bufferId = -1);

    // Handle N++ native notification
    bool handleNppNotification(void* nmhdr, unsigned int code,
                              uintptr_t wParam = 0, intptr_t lParam = 0);

    // Convert to unified notification
    NppNotification toNotification(const SCNotification* sciNotification, int bufferId = -1) const;

signals:
    void scintillaNotification(const SCNotification* notification, int bufferId);
    void nppNotification(unsigned int code, uintptr_t wParam, intptr_t lParam);

    // Convenience signals mapped to NppNotificationHandler
    void fileModified(const QString& path);
    void bufferActivated(int bufferId);
    void zoomChanged(int zoom);
    void selectionChanged();
    void cursorPositionChanged(int line, int column);
    void contentChanged(int bufferId);

private:
    bool handleModified(const SCNotification* notification, int bufferId);
    bool handleSavePoint(const SCNotification* notification, int bufferId);
    bool handleMarginClick(const SCNotification* notification, int bufferId);
    bool handleCharAdded(const SCNotification* notification, int bufferId);
    bool handleDoubleClick(const SCNotification* notification, int bufferId);
    bool handleUpdateUI(const SCNotification* notification, int bufferId);
    bool handleZoom(const SCNotification* notification, int bufferId);
    bool handlePainted(const SCNotification* notification, int bufferId);
    bool handleAutoComplete(const SCNotification* notification, int bufferId);
};

// =============================================================================
// Utility Functions
// =============================================================================

inline const char* notificationTypeToString(NppNotificationType type) {
    switch (type) {
        case NppNotificationType::BufferOpened: return "BufferOpened";
        case NppNotificationType::BufferClosed: return "BufferClosed";
        case NppNotificationType::BufferActivated: return "BufferActivated";
        case NppNotificationType::BufferCreated: return "BufferCreated";
        case NppNotificationType::BufferDeleted: return "BufferDeleted";
        case NppNotificationType::BufferModified: return "BufferModified";
        case NppNotificationType::BufferReloaded: return "BufferReloaded";
        case NppNotificationType::BufferSaved: return "BufferSaved";
        case NppNotificationType::BufferDirty: return "BufferDirty";
        case NppNotificationType::BufferClean: return "BufferClean";
        case NppNotificationType::BufferEncodingChanged: return "BufferEncodingChanged";
        case NppNotificationType::BufferLanguageChanged: return "BufferLanguageChanged";
        case NppNotificationType::BufferReadOnlyChanged: return "BufferReadOnlyChanged";
        case NppNotificationType::BufferEolChanged: return "BufferEolChanged";
        case NppNotificationType::BufferTimestampChanged: return "BufferTimestampChanged";
        case NppNotificationType::FileOpened: return "FileOpened";
        case NppNotificationType::FileClosed: return "FileClosed";
        case NppNotificationType::FileSaved: return "FileSaved";
        case NppNotificationType::FileDeleted: return "FileDeleted";
        case NppNotificationType::FileRenamed: return "FileRenamed";
        case NppNotificationType::FileMoved: return "FileMoved";
        case NppNotificationType::FilePermissionsChanged: return "FilePermissionsChanged";
        case NppNotificationType::FileMonitorTriggered: return "FileMonitorTriggered";
        case NppNotificationType::FileExternallyModified: return "FileExternallyModified";
        case NppNotificationType::FileExternallyDeleted: return "FileExternallyDeleted";
        case NppNotificationType::FileConflict: return "FileConflict";
        case NppNotificationType::ThemeChanged: return "ThemeChanged";
        case NppNotificationType::ThemeLoading: return "ThemeLoading";
        case NppNotificationType::StyleChanged: return "StyleChanged";
        case NppNotificationType::LayoutChanged: return "LayoutChanged";
        case NppNotificationType::PanelVisibilityChanged: return "PanelVisibilityChanged";
        case NppNotificationType::PanelDocked: return "PanelDocked";
        case NppNotificationType::PanelUndocked: return "PanelUndocked";
        case NppNotificationType::ZoomChanged: return "ZoomChanged";
        case NppNotificationType::DpiChanged: return "DpiChanged";
        case NppNotificationType::FontChanged: return "FontChanged";
        case NppNotificationType::LanguageChanged: return "LanguageChanged";
        case NppNotificationType::SessionLoaded: return "SessionLoaded";
        case NppNotificationType::SessionSaved: return "SessionSaved";
        case NppNotificationType::SessionLoadError: return "SessionLoadError";
        case NppNotificationType::SessionSaveError: return "SessionSaveError";
        case NppNotificationType::SessionNew: return "SessionNew";
        case NppNotificationType::PluginLoaded: return "PluginLoaded";
        case NppNotificationType::PluginUnloaded: return "PluginUnloaded";
        case NppNotificationType::PluginError: return "PluginError";
        case NppNotificationType::PluginCommandInvoked: return "PluginCommandInvoked";
        case NppNotificationType::PluginMessage: return "PluginMessage";
        case NppNotificationType::ApplicationStarting: return "ApplicationStarting";
        case NppNotificationType::ApplicationStarted: return "ApplicationStarted";
        case NppNotificationType::ApplicationShutdown: return "ApplicationShutdown";
        case NppNotificationType::ApplicationError: return "ApplicationError";
        case NppNotificationType::EditorFocused: return "EditorFocused";
        case NppNotificationType::EditorLostFocus: return "EditorLostFocus";
        case NppNotificationType::SelectionChanged: return "SelectionChanged";
        case NppNotificationType::CursorPositionChanged: return "CursorPositionChanged";
        case NppNotificationType::ScrollChanged: return "ScrollChanged";
        case NppNotificationType::MacroRecordingStarted: return "MacroRecordingStarted";
        case NppNotificationType::MacroRecordingStopped: return "MacroRecordingStopped";
        case NppNotificationType::MacroPlaybackStarted: return "MacroPlaybackStarted";
        case NppNotificationType::MacroPlaybackStopped: return "MacroPlaybackStopped";
        case NppNotificationType::FindStarted: return "FindStarted";
        case NppNotificationType::FindCompleted: return "FindCompleted";
        case NppNotificationType::ReplaceCompleted: return "ReplaceCompleted";
        case NppNotificationType::Custom: return "Custom";
        default: return "Unknown";
    }
}

inline QString notificationToDebugString(const NppNotification& n) {
    return QString("[%1] %2 priority=%3 buffer=%4")
        .arg(n.data.timestamp.toString(Qt::ISODate))
        .arg(notificationTypeToString(n.type))
        .arg(static_cast<int>(n.priority))
        .arg(n.data.bufferId);
}
