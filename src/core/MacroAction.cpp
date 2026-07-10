// MacroAction.cpp - A single recorded action in a macro
// Copyright (C) 2026 Agent Army | GPL v3

#include "MacroAction.h"
#include "../editor/ScintillaEditor.h"
#include <Qsci/qsciscintilla.h>

// ============================================================================
// MacroAction
// ============================================================================
MacroAction::MacroAction(int sciCommand, int param)
    : _command(sciCommand), _hasStringParam(false), _intParam(param)
{}

MacroAction::MacroAction(int sciCommand, const QString& stringParam)
    : _command(sciCommand), _hasStringParam(true), _intParam(0), _stringParam(stringParam)
{}

void MacroAction::playback(ScintillaEditor* editor) const {
    if (!editor) return;
    QsciScintilla* sci = editor->qsciEditor();
    if (!sci) return;

    if (_hasStringParam) {
        QByteArray ba = _stringParam.toUtf8();
        sci->SendScintilla(_command, ba.size(), ba.constData());
    } else {
        sci->SendScintilla(_command, _intParam);
    }
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
    } else {
        _hasStringParam = false;
        _intParam = v.toInt();
    }
}

// ============================================================================
// MacroActionList
// ============================================================================
QVariantMap MacroActionList::toJson() const {
    QVariantMap m;
    m["name"] = name;
    m["shortcut"] = shortcut;
    QVariantList actionsList;
    for (const MacroAction& a : actions)
        actionsList.append(a.toJson());
    m["actions"] = actionsList;
    return m;
}

void MacroActionList::fromJson(const QVariantMap& map) {
    name = map["name"].toString();
    shortcut = map["shortcut"].toString();
    QVariantList actionsList = map["actions"].toList();
    actions.clear();
    for (const QVariant& v : actionsList) {
        MacroAction a;
        a.fromJson(v.toMap());
        actions.append(a);
    }
}
