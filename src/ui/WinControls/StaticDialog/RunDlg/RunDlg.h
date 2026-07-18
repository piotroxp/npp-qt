// RunDlg.h - Qt6 "Run" dialog equivalent
// INTENT: source runs a shell command via ShellExecuteEx. Target uses QProcess.
#pragma once
#include "../StaticDialog.h"
#include <QLineEdit>
#include <QProcess>
#include <QPlainTextEdit>

class RunDlg : public StaticDialog {
    Q_OBJECT
public:
    explicit RunDlg(QWidget* parent = nullptr);
    ~RunDlg() override = default;

public slots:
    void onRun();
    void onBrowse();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    void setupUi();
    void readSettings();
    void saveSettings();

    QLineEdit* _cmdEdit = nullptr;
    QProcess* _process = nullptr;
    QString _lastWorkingDir;
};
