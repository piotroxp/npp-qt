// ShortcutManager.h — Full key binding management for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3
#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <functional>
#include "../common/Types.h"

class ScintillaEditor;
class Buffer;

// ============================================================================
// KeyBinding — represents a single keyboard shortcut
// ============================================================================
struct KeyBinding {
    int commandId = -1;
    QString commandName;
    int keyCode = 0;         // Qt::Key_* value
    int modifiers = 0;       // Qt::KeyboardModifier flags OR'd together
    QString shortcutText;     // Human-readable: "Ctrl+Shift+P"

    QString toString() const;
    static KeyBinding fromString(const QString& str, int cmdId = -1, const QString& name = QString());
    bool isValid() const { return commandId >= 0 && keyCode > 0; }
    bool matches(int kc, int mods) const {
        return keyCode == kc && modifiers == mods;
    }
};

// ============================================================================
// ShortcutManager — manages global and buffer-local keyboard bindings
// ============================================================================
class ShortcutManager : public QObject {
    Q_OBJECT

public:
    static ShortcutManager& instance();

    // Bind a shortcut to a command globally
    void bindKey(int commandId, const QString& shortcut);
    void unbindKey(const QString& shortcut);

    // Buffer-local bindings (override global)
    void bindKeyLocal(BufferID buffer, int commandId, const QString& shortcut);
    void unbindKeyLocal(BufferID buffer, const QString& shortcut);

    // Resolve: check buffer-local first, then global
    int resolveBinding(const QString& shortcut, BufferID buffer = nullptr) const;
    int resolveBinding(int keyCode, int modifiers, BufferID buffer = nullptr) const;

    // Conflict detection
    struct Conflict { QString shortcut; int cmd1, cmd2; };
    QVector<Conflict> getConflicts() const;
    bool hasConflict(const QString& shortcut) const;
    int getBoundCommand(const QString& shortcut) const;

    // Storage
    void loadFromJson(const QString& path);
    void saveToJson(const QString& path) const;
    QString defaultConfigPath() const;

    // Accessors
    QVector<KeyBinding> allBindings() const { return _globalBindings; }
    void clear();

    // Notification
    void notifyConflict(const QString& shortcut, int existingCmd, int newCmd);

Q_SIGNALS:
    void shortcutChanged(const QString& shortcut, int commandId);
    void conflictDetected(const QString& shortcut, int cmd1, int cmd2);
    void allShortcutsChanged();

private:
    friend class Application;
    ShortcutManager();
    ~ShortcutManager() override;
    Q_DISABLE_COPY_MOVE(ShortcutManager)

    int _makeKey(const QString& shortcut) const;
    QString _makeShortcutText(int keyCode, int modifiers) const;

    QVector<KeyBinding> _globalBindings;
    QMap<int, int> _globalShortcutMap;  // key → commandId
    QMap<QString, KeyBinding> _localBindings;  // "bufferId:shortcut" → binding
};
