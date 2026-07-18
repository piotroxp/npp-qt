// ShortcutAdapter.h — Linux/X11 keyboard shortcut handling for Notepad--Qt
// Adapts Win32 RegisterHotKey / Accelerator-table semantics to Qt6 QShortcut / QAction::setShortcut()
//
// Linux/X11 considerations:
//   - QShortcut lives in the widget's context — no global hotkey registration needed
//   - Some shortcuts (e.g. PrintScreen, Meta-based combos) may be captured by the
//     desktop environment (DE) or the compositor first.  Document conflicts below.
//   - QKeySequence::StandardKey is used for portable, i18n-aware bindings where possible.
//   - All shortcuts are parented to the main window so they are destroyed automatically.
//
// DE conflict notes (see README section "Keyboard Shortcuts" for full documentation):
//   - Ctrl+Alt+Backspace   — blocked by GNOME/KDE (XServer Exit).  App does not use it.
//   - Alt+Tab / Alt+Shift+Tab — DE tab-switcher.  App uses Ctrl+PageUp/PageDown for tab nav.
//   - Meta/Super key menus  — GNOME/KDE intercept Meta before Qt sees it.  Prefer Ctrl+… shortcuts.
//   - PrintScreen           — GNOME intercepts for screenshot tool.  Map to Ctrl+Alt+P instead.
//   - F10                   — KDE menu bar access.  Use F11 (fullscreen) instead of F10.
//   - Ctrl+Alt+Arrow        — blocked by GNOME (workspace switch).  App uses Ctrl+PageUp/PageDown.
//   - Alt+F2 / Alt+Space    — GNOME/KDE launcher.  App uses Ctrl+Shift+P for command palette.

#pragma once

#include <QObject>
#include <QKeySequence>
#include <QShortcut>
#include <QAction>
#include <QMap>
#include <QSet>
#include <QString>
#include <functional>

// Callback type for activated shortcuts
using ShortcutCallback = std::function<void()>;

/**
 * ShortcutAdapter — central registry for keyboard shortcuts on the main window.
 *
 * Maps Win32-style RegisterHotKey semantics to Qt6:
 *   Win32: RegisterHotKey(hwnd, id, modifiers, vk)  → global, HWND-scoped
 *   Qt6:   ShortcutAdapter::registerShortcut(seq, cb) → widget-context, callback-based
 *
 * The adapter owns QShortcut objects; they are automatically destroyed when the
 * parent widget is destroyed.  Shortcuts are keyed by their canonical QKeySequence
 * string so that duplicates can be detected and resolved.
 */
class ShortcutAdapter : public QObject {
    Q_OBJECT

public:
    explicit ShortcutAdapter(QObject* parent = nullptr);
    ~ShortcutAdapter() override;

    // ── Registration ──────────────────────────────────────────────────────────

    /// Register a shortcut with a callback.  Returns true on success.
    /// If the same QKeySequence is already registered, replaces the old entry.
    bool registerShortcut(const QKeySequence& keySeq, ShortcutCallback callback);

    /// Unregister a shortcut.  Returns true if it was found and removed.
    bool unregisterShortcut(const QKeySequence& keySeq);

    /// Register a shortcut on an existing QAction (sets its QAction::setShortcut).
    /// The action takes ownership semantics; no separate QShortcut is created.
    void registerShortcutForAction(QAction* action, const QKeySequence& keySeq);

    /// Unregister (clear) the shortcut from a QAction.
    void unregisterShortcutFromAction(QAction* action);

    // ── Query ─────────────────────────────────────────────────────────────────

    /// Returns true if the given key sequence is currently registered.
    bool isRegistered(const QKeySequence& keySeq) const;

    /// Returns all registered key sequences.
    QList<QKeySequence> registeredSequences() const;

    /// Returns a human-readable string describing any conflict with keySeq.
    /// Returns an empty QString if no conflict exists.
    QString conflictInfo(const QKeySequence& keySeq) const;

    // ── Standard shortcuts ───────────────────────────────────────────────────
    // These mirror Win32 Accelerator table entries and map directly to their
    // QKeySequence::StandardKey equivalents where defined.

    /// Register the standard Notepad-- file/edit/search/view shortcuts on the
    /// main window.  Each entry wires a QShortcut to the provided callback map.
    void registerStandardShortcuts();

    // ── Conflict detection ────────────────────────────────────────────────────

    /// Check whether keySeq conflicts with any registered shortcut or any
    /// QAction currently attached to the parent widget.
    /// Returns a conflict description or empty string.
    QString checkConflict(const QKeySequence& keySeq) const;

signals:
    /// Emitted when a shortcut conflict is detected.
    void shortcutConflict(const QKeySequence& conflict, const QString& description);

    /// Emitted whenever a shortcut is registered.
    void shortcutRegistered(const QKeySequence& keySeq);

    /// Emitted whenever a shortcut is unregistered.
    void shortcutUnregistered(const QKeySequence& keySeq);

    /// Emitted when a registered shortcut is activated — carries the command string
    /// that the slot should dispatch (e.g. "file.save", "edit.undo").
    /// MainWindow connects this to its dispatchCommand() slot.
    void shortcutDispatchRequested(const QString& command);

private:
    // Internal: build the canonical string key used in _shortcutMap
    static QString toKey(const QKeySequence& ks) {
        return ks.toString();
    }

    // Internal: resolve a StandardKey to a concrete QKeySequence
    static QKeySequence fromStandardKey(QKeySequence::StandardKey sk);

    // Helper: collect all QAction shortcuts currently attached to parent widget
    QSet<QKeySequence> collectActionShortcuts() const;

    // Registry: key sequence string → QShortcut
    QMap<QString, QShortcut*> _shortcutMap;

    // Callbacks: key sequence string → callback
    QMap<QString, ShortcutCallback> _callbackMap;

    // Actions tracked for unregisterShortcutFromAction
    QMap<QAction*, QKeySequence> _actionShortcutMap;
};
