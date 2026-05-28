// Macro.h - Qt port stub
#pragma once
#include <QString>
#include <vector>

struct recordedMacroStep {
    QString _message;
    QString _parameter;
};

class Macro {
public:
    Macro() = default;
    virtual ~Macro() = default;
    void startRecording() {}
    void stopRecording() {}
    void playback() {}
    void save(const QString& path) const {}
    void load(const QString& path) {}
    bool isEmpty() const { return true; }
    int getSize() const { return 0; }
    void addStep(const recordedMacroStep&) {}
    recordedMacroStep* getStepAt(int) { return nullptr; }
};
