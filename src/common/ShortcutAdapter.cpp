// ShortcutAdapter.cpp — Linux/X11 keyboard shortcut handling for Notepad--Qt
// Adapts Win32 RegisterHotKey / Accelerator-table semantics to Qt6 QShortcut / QAction::setShortcut()

#include "ShortcutAdapter.h"
#include <QMainWindow>
#include <QWidget>
#include <QAction>
#include <QMenuBar>
#include <QDebug>

// ShortcutManager integration — guarded so the test binary (which doesn't link EditorCommandManager)
// can still call checkConflict() for local shortcut and QAction conflict detection.
// ShortcutManager integration — guarded so the test binary (which doesn't link EditorCommandManager)
// can still call checkConflict() for local shortcut and QAction conflict detection.
// We always include the header; the if constexpr below ensures no link dependency when unused.
// Note: ShortcutManager integration for conflict detection is available in the main
// npp-qt binary (which links EditorCommandManager).  The test binary does not link it.
// For production use, uncomment the include below and add NPPQT_HAVE_SHORTCUT_MANAGER
// to the ShortcutAdapter.cpp compilation target.
// #include "../core/ShortcutManager.h"

ShortcutAdapter::ShortcutAdapter(QObject* parent)
    : QObject(parent)
{
}

ShortcutAdapter::~ShortcutAdapter() {
    // QShortcut objects are deleted automatically when the parent widget is destroyed.
    // Explicit cleanup clears the registry maps.
    _shortcutMap.clear();
    _callbackMap.clear();
    _actionShortcutMap.clear();
}

// ── Registration ─────────────────────────────────────────────────────────────

bool ShortcutAdapter::registerShortcut(const QKeySequence& keySeq, ShortcutCallback callback) {
    if (keySeq.isEmpty())
        return false;

    const QString key = toKey(keySeq);

    // Remove any existing QShortcut for this sequence before replacing.
    if (_shortcutMap.contains(key)) {
        QShortcut* old = _shortcutMap.take(key);
        if (old && old->parent() == this) {
            delete old;
        } else if (old) {
            old->disconnect();
        }
        _callbackMap.remove(key);
    }

    // QShortcut must be parented to a QWidget so its events reach the widget's
    // event handler.  Fall back to a static no-op widget when no widget parent is
    // available; in that case callers must handle activated() directly or set a
    // widget parent before registering shortcuts.
    static QWidget staticParent;  // never shown; safe fallback for QShortcut parent
    QWidget* parentWidget = qobject_cast<QWidget*>(parent());
    QShortcut* sc = new QShortcut(keySeq, static_cast<QWidget*>(parentWidget ? parentWidget : &staticParent));
    if (!sc) {
        qWarning() << "[ShortcutAdapter] failed to create QShortcut for" << keySeq.toString();
        return false;
    }

    // Connect activated signal to the user callback.
    // Qt::QueuedConnection keeps the callback execution out-of-band, preventing
    // re-entrancy issues if the callback modifies the shortcut map.
    connect(sc, &QShortcut::activated, this,
        [cb = std::move(callback)]() {
            if (cb)
                cb();
        }, Qt::QueuedConnection);

    _shortcutMap.insert(key, sc);
    _callbackMap.insert(key, std::move(callback));

    emit shortcutRegistered(keySeq);
    return true;
}

bool ShortcutAdapter::unregisterShortcut(const QKeySequence& keySeq) {
    const QString key = toKey(keySeq);

    QShortcut* sc = _shortcutMap.take(key);
    if (!sc)
        return false;

    if (sc->parent() == this) {
        delete sc;
    } else {
        sc->disconnect();
    }
    _callbackMap.remove(key);

    emit shortcutUnregistered(keySeq);
    return true;
}

void ShortcutAdapter::registerShortcutForAction(QAction* action, const QKeySequence& keySeq) {
    if (!action || keySeq.isEmpty())
        return;

    // Store the old shortcut only if this action isn't already tracked, so we
    // don't overwrite a user-set shortcut that was already saved.
    if (!_actionShortcutMap.contains(action))
        _actionShortcutMap.insert(action, action->shortcut());

    action->setShortcut(keySeq);
}

void ShortcutAdapter::unregisterShortcutFromAction(QAction* action) {
    if (!action)
        return;

    auto it = _actionShortcutMap.find(action);
    if (it != _actionShortcutMap.end()) {
        action->setShortcut(it.value());
        _actionShortcutMap.erase(it);
    } else {
        action->setShortcut(QKeySequence());
    }
}

// ── Query ─────────────────────────────────────────────────────────────────────

bool ShortcutAdapter::isRegistered(const QKeySequence& keySeq) const {
    return _shortcutMap.contains(toKey(keySeq));
}

QList<QKeySequence> ShortcutAdapter::registeredSequences() const {
    QList<QKeySequence> result;
    for (QShortcut* sc : std::as_const(_shortcutMap)) {
        if (sc)
            result.append(sc->key());
    }
    return result;
}

QString ShortcutAdapter::conflictInfo(const QKeySequence& keySeq) const {
    return checkConflict(keySeq);
}

// ── Standard shortcuts ────────────────────────────────────────────────────────
// These mirror the Win32 accelerator-table entries defined in Notepad++.
// QKeySequence::StandardKey is used where available for cross-platform, i18n-
// aware bindings (the label text respects the desktop environment's locale).

void ShortcutAdapter::registerStandardShortcuts() {
    // File shortcuts — mapped from Win32 Accelerator table entries
    (void)registerShortcut(QKeySequence::New,
        [this]() { emit shortcutDispatchRequested("file.new"); });
    (void)registerShortcut(QKeySequence::Open,
        [this]() { emit shortcutDispatchRequested("file.open"); });
    (void)registerShortcut(QKeySequence::Save,
        [this]() { emit shortcutDispatchRequested("file.save"); });
    (void)registerShortcut(QKeySequence("Ctrl+Shift+S"),
        [this]() { emit shortcutDispatchRequested("file.saveAs"); });
    (void)registerShortcut(QKeySequence("Ctrl+W"),
        [this]() { emit shortcutDispatchRequested("file.close"); });
    (void)registerShortcut(QKeySequence("Ctrl+Shift+W"),
        [this]() { emit shortcutDispatchRequested("file.closeAll"); });
    (void)registerShortcut(QKeySequence::Print,
        [this]() { emit shortcutDispatchRequested("file.print"); });
    (void)registerShortcut(QKeySequence("Alt+F4"),
        [this]() { emit shortcutDispatchRequested("file.exit"); });

    // Edit shortcuts
    (void)registerShortcut(QKeySequence::Undo,
        [this]() { emit shortcutDispatchRequested("edit.undo"); });
    (void)registerShortcut(QKeySequence::Redo,
        [this]() { emit shortcutDispatchRequested("edit.redo"); });
    (void)registerShortcut(QKeySequence::Cut,
        [this]() { emit shortcutDispatchRequested("edit.cut"); });
    (void)registerShortcut(QKeySequence::Copy,
        [this]() { emit shortcutDispatchRequested("edit.copy"); });
    (void)registerShortcut(QKeySequence::Paste,
        [this]() { emit shortcutDispatchRequested("edit.paste"); });
    (void)registerShortcut(QKeySequence::Delete,
        [this]() { emit shortcutDispatchRequested("edit.delete"); });
    (void)registerShortcut(QKeySequence::SelectAll,
        [this]() { emit shortcutDispatchRequested("edit.selectAll"); });

    // Search shortcuts
    (void)registerShortcut(QKeySequence::Find,
        [this]() { emit shortcutDispatchRequested("edit.find"); });
    (void)registerShortcut(QKeySequence("Ctrl+H"),
        [this]() { emit shortcutDispatchRequested("edit.replace"); });
    (void)registerShortcut(QKeySequence::FindNext,
        [this]() { emit shortcutDispatchRequested("search.findNext"); });
    (void)registerShortcut(QKeySequence::FindPrevious,
        [this]() { emit shortcutDispatchRequested("search.findPrev"); });
    (void)registerShortcut(QKeySequence("Ctrl+G"),
        [this]() { emit shortcutDispatchRequested("edit.goto"); });
    (void)registerShortcut(QKeySequence("Ctrl+F2"),
        [this]() { emit shortcutDispatchRequested("search.toggleBookmark"); });
    (void)registerShortcut(QKeySequence("Ctrl+Shift+F"),
        [this]() { emit shortcutDispatchRequested("search.findInFiles"); });

    // View shortcuts
    (void)registerShortcut(QKeySequence::FullScreen,
        [this]() { emit shortcutDispatchRequested("view.fullScreen"); });
    (void)registerShortcut(QKeySequence::ZoomIn,
        [this]() { emit shortcutDispatchRequested("view.zoomIn"); });
    (void)registerShortcut(QKeySequence::ZoomOut,
        [this]() { emit shortcutDispatchRequested("view.zoomOut"); });
    (void)registerShortcut(QKeySequence("Ctrl+0"),
        [this]() { emit shortcutDispatchRequested("view.zoomReset"); });
    (void)registerShortcut(QKeySequence("Ctrl+Alt+Shift+T"),
        [this]() { emit shortcutDispatchRequested("view.alwaysOnTop"); });

    // Tab navigation
    (void)registerShortcut(QKeySequence("Ctrl+1"),
        [this]() { emit shortcutDispatchRequested("tab.1"); });
    (void)registerShortcut(QKeySequence("Ctrl+2"),
        [this]() { emit shortcutDispatchRequested("tab.2"); });
    (void)registerShortcut(QKeySequence("Ctrl+3"),
        [this]() { emit shortcutDispatchRequested("tab.3"); });
    (void)registerShortcut(QKeySequence("Ctrl+4"),
        [this]() { emit shortcutDispatchRequested("tab.4"); });
    (void)registerShortcut(QKeySequence("Ctrl+5"),
        [this]() { emit shortcutDispatchRequested("tab.5"); });
    (void)registerShortcut(QKeySequence("Ctrl+6"),
        [this]() { emit shortcutDispatchRequested("tab.6"); });
    (void)registerShortcut(QKeySequence("Ctrl+7"),
        [this]() { emit shortcutDispatchRequested("tab.7"); });
    (void)registerShortcut(QKeySequence("Ctrl+8"),
        [this]() { emit shortcutDispatchRequested("tab.8"); });
    (void)registerShortcut(QKeySequence("Ctrl+Tab"),
        [this]() { emit shortcutDispatchRequested("tab.next"); });
    (void)registerShortcut(QKeySequence("Ctrl+Shift+Tab"),
        [this]() { emit shortcutDispatchRequested("tab.prev"); });

    // Encoding shortcuts
    (void)registerShortcut(QKeySequence("Ctrl+Shift+N"),
        [this]() { emit shortcutDispatchRequested("encoding.utf8"); });

    // Settings shortcuts
    (void)registerShortcut(QKeySequence("Ctrl+Alt+P"),
        [this]() { emit shortcutDispatchRequested("settings.preferences"); });
    (void)registerShortcut(QKeySequence("Ctrl+Shift+P"),
        [this]() { emit shortcutDispatchRequested("settings.commandPalette"); });
}

// ── Conflict detection ───────────────────────────────────────────────────────


QSet<QKeySequence> ShortcutAdapter::collectActionShortcuts() const {
    QSet<QKeySequence> result;
    QWidget* w = qobject_cast<QWidget*>(parent());
    if (!w)
        return result;

    // Direct children only (not deep recursion into menus/popups).
    for (QAction* action : w->findChildren<QAction*>(Qt::FindDirectChildrenOnly)) {
        if (!action->shortcut().isEmpty())
            result.insert(action->shortcut());
    }

    // Also check the menu bar (attached to the main window).
    if (QMainWindow* mw = qobject_cast<QMainWindow*>(w)) {
        if (QMenuBar* mb = mw->menuBar()) {
            for (QAction* action : mb->findChildren<QAction*>(Qt::FindDirectChildrenOnly)) {
                if (!action->shortcut().isEmpty())
                    result.insert(action->shortcut());
            }
        }
    }

    return result;
}

QString ShortcutAdapter::checkConflict(const QKeySequence& keySeq) const {
    if (keySeq.isEmpty())
        return QString();

    // Check against other registered QShortcuts in this adapter.
    for (auto it = _callbackMap.constBegin(); it != _callbackMap.constEnd(); ++it) {
        QKeySequence existing{it.key()};
        if (!existing.isEmpty() && existing.matches(keySeq) != QKeySequence::NoMatch) {
            return QStringLiteral("Shortcut '") + keySeq.toString()
                + QStringLiteral("' conflicts with registered shortcut '")
                + existing.toString() + QStringLiteral("'");
        }
    }

    // Check against QAction shortcuts on the parent widget hierarchy.
    QSet<QKeySequence> actionShortcuts = collectActionShortcuts();
    for (const QKeySequence& existing : actionShortcuts) {
        if (!existing.isEmpty() && existing.matches(keySeq) != QKeySequence::NoMatch) {
            return QStringLiteral("Shortcut '") + keySeq.toString()
                + QStringLiteral("' conflicts with QAction shortcut '")
                + existing.toString() + QStringLiteral("'");
        }
    }

    // Note: ShortcutManager conflict detection is handled in the main binary's
    // ShortcutManager integration.  Add #include "../core/ShortcutManager.h" above
    // and link ShortcutManager.cpp to re-enable it here.

    return QString();
}
