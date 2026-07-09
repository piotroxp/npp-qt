// MacroManager.cpp - Macro recording and playback for Notepad--Qt
#include "MacroManager.h"
#include "Application.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>

// ============================================================================
// Singleton
// ============================================================================
MacroManager& MacroManager::instance() {
    static MacroManager instance;
    return instance;
}

MacroManager::MacroManager() {
    loadMacros();
}

MacroManager::~MacroManager() = default;

// ============================================================================
// Recording
// ============================================================================
void MacroManager::startRecording() {
    if (m_recording)
        return;
    m_recording = true;
    m_currentRecording.clear();
    emit recordingStarted();
}

void MacroManager::stopRecording() {
    if (!m_recording)
        return;
    m_recording = false;
    emit recordingStopped();
}

void MacroManager::recordCommand(const QString& commandName, const QString& param) {
    if (!m_recording)
        return;
    m_currentRecording.append(qMakePair(commandName, param));
}

// ============================================================================
// Playback
// ============================================================================
void MacroManager::playback() {
    if (m_savedMacros.isEmpty())
        return;
    playback(0);
}

void MacroManager::playback(int macroIndex) {
    if (macroIndex < 0 || macroIndex >= m_savedMacros.size())
        return;

    emit playbackStarted();

    const auto& [name, commands] = m_savedMacros[macroIndex];
    for (const QString& cmd : commands) {
        app().onMenuCommand(cmd);
    }

    emit playbackFinished();
}

// ============================================================================
// Storage
// ============================================================================
static QString macrosFilePath() {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty())
        configDir = QDir::home().filePath(".config/notepad--qt");
    return QDir(configDir).filePath("macros.json");
}

void MacroManager::loadMacros() {
    QFile file(macrosFilePath());
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject())
        return;

    QJsonObject root = doc.object();
    QJsonArray macros = root["macros"].toArray();

    m_savedMacros.clear();
    for (const QJsonValue& v : macros) {
        QJsonObject m = v.toObject();
        QString name = m["name"].toString();
        QJsonArray cmds = m["commands"].toArray();
        QStringList cmdList;
        for (const QJsonValue& c : cmds)
            cmdList.append(c.toString());
        m_savedMacros.append(qMakePair(name, cmdList));
    }
}

void MacroManager::saveMacro(const QString& name) {
    if (m_currentRecording.isEmpty())
        return;

    QStringList commands;
    for (const auto& [cmd, param] : m_currentRecording)
        commands.append(param.isEmpty() ? cmd : QString("%1:%2").arg(cmd, param));

    m_savedMacros.append(qMakePair(name, commands));
    m_currentRecording.clear();

    // Persist
    QJsonArray macros;
    for (const auto& [n, cmds] : m_savedMacros) {
        QJsonObject m;
        m["name"] = n;
        QJsonArray cmdsArr;
        for (const QString& c : cmds)
            cmdsArr.append(c);
        m["commands"] = cmdsArr;
        macros.append(m);
    }

    QJsonObject root;
    root["macros"] = macros;

    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty())
        configDir = QDir::home().filePath(".config/notepad--qt");
    QDir().mkpath(configDir);

    QFile file(QDir(configDir).filePath("macros.json"));
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}

void MacroManager::deleteMacro(int index) {
    if (index < 0 || index >= m_savedMacros.size())
        return;

    m_savedMacros.removeAt(index);

    // Re-persist
    QJsonArray macros;
    for (const auto& [n, cmds] : m_savedMacros) {
        QJsonObject m;
        m["name"] = n;
        QJsonArray cmdsArr;
        for (const QString& c : cmds)
            cmdsArr.append(c);
        macros.append(m);
    }

    QJsonObject root;
    root["macros"] = macros;

    QFile file(macrosFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}

// ============================================================================
// List
// ============================================================================
QStringList MacroManager::macroNames() const {
    QStringList names;
    for (const auto& [name, cmds] : m_savedMacros)
        names.append(name);
    return names;
}

int MacroManager::macroCount() const {
    return m_savedMacros.size();
}
