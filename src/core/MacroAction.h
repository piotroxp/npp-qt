// MacroAction.h - A single recorded action in a macro
// Copyright (C) 2026 Agent Army | GPL v3
#pragma once

#include <QString>
#include <QVariant>

class ScintillaEditor;

// ============================================================================
// MacroAction — stores one editor command for playback
// ============================================================================
class MacroAction {
public:
    // Integer-parameter action (most commands)
    MacroAction(int sciCommand, int param = 0);

    // String-parameter action (e.g., addText)
    MacroAction(int sciCommand, const QString& stringParam);

    // Default constructor for JSON deserialization
    MacroAction() = default;

    int command() const { return _command; }
    bool hasStringParam() const { return _hasStringParam; }
    int intParam() const { return _intParam; }
    const QString& stringParam() const { return _stringParam; }

    // Replay this action on the given editor
    void playback(ScintillaEditor* editor) const;

    // Serialization
    QVariantMap toJson() const;
    void fromJson(const QVariantMap& map);

private:
    int _command = 0;          // SCI message ID
    bool _hasStringParam = false;
    int  _intParam = 0;
    QString _stringParam;
};

// ============================================================================
// MacroActionList — a complete macro (name + actions)
// ============================================================================
struct MacroActionList {
    QString name;
    QString shortcut;          // e.g. "Ctrl+Shift+1"
    QVector<MacroAction> actions;

    QVariantMap toJson() const;
    void fromJson(const QVariantMap& map);
};
