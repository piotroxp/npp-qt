// MacroManager.h - Macro recording and playback for Notepad--Qt
#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>

class MacroManager : public QObject {
    Q_OBJECT

public:
    static MacroManager& instance();

    // Recording
    void startRecording();
    void stopRecording();
    bool isRecording() const { return m_recording; }

    // Playback
    void playback();
    void playback(int macroIndex);

    // Storage
    void saveMacro(const QString& name);
    void loadMacros();
    void deleteMacro(int index);

    // Commands
    void recordCommand(const QString& commandName, const QString& param = QString());

    // List
    QStringList macroNames() const;
    int macroCount() const;

Q_SIGNALS:
    void recordingStarted();
    void recordingStopped();
    void playbackStarted();
    void playbackFinished();

private:
    friend class Application;
    MacroManager();
    ~MacroManager() override;
    Q_DISABLE_COPY_MOVE(MacroManager)

    bool m_recording = false;
    QVector<QPair<QString, QString>> m_currentRecording; // command + param
    QVector<QPair<QString, QStringList>> m_savedMacros; // name + commands
};
