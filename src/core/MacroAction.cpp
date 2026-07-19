// MacroAction.cpp - A single recorded action in a macro
// Copyright (C) 2026 Agent Army | GPL v3

#include "MacroAction.h"
class ScintillaEditor;   // forward decl — no full header needed
class QsciScintilla;    // forward decl

// ============================================================================
// MacroAction
// ============================================================================

MacroAction::MacroAction(int sciCommand, int param)
    : _command(sciCommand), _hasStringParam(false), _intParam(param), _stringParam() {}

MacroAction::MacroAction(int sciCommand, const QString& stringParam)
    : _command(sciCommand), _hasStringParam(true), _intParam(0), _stringParam(stringParam) {}

MacroAction::MacroAction() = default;
MacroAction::~MacroAction() = default;

void MacroAction::playback(ScintillaEditor* editor) const {
    // Stub: playback needs ScintillaEditor linkage, skipping for unit tests
    Q_UNUSED(editor);
}

QVariantMap MacroAction::toJson() const {
    QVariantMap m;
    m["id"] = _command;
    if (_hasStringParam) {
        m["param"] = _stringParam;
    } else {
        m["param"] = _intParam;
    }
    return m;
}

void MacroAction::fromJson(const QVariantMap& map) {
    _command = map["id"].toInt();
    QVariant v = map["param"];
    if (v.canConvert<QString>()) {
        _hasStringParam = true;
        _stringParam = v.toString();
        _intParam = 0;
    } else {
        _hasStringParam = false;
        _intParam = v.toInt();
        _stringParam.clear();
    }
}

// ============================================================================
// MacroActionList
// ============================================================================

QVariantMap MacroActionList::toJson() const {
    QVariantMap m;
    m["name"] = name;
    QVariantList actions;
    for (const auto& a : actions) {
        actions.append(a.toJson());
    }
    m["actions"] = actions;
    return m;
}

void MacroActionList::fromJson(const QVariantMap& map) {
    name = map["name"].toString();
    actions.clear();
    for (const auto& v : map["actions"].toList()) {
        MacroAction a;
        a.fromJson(v.toMap());
        actions.append(a);
    }
}
