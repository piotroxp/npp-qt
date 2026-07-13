// npp-qt: Process management — runs external programs and captures output
// Mirrors Win32 Process class in PowerEditor/src/Process
#pragma once
#include <QObject>
#include <QProcess>
#include <QString>
#include <QByteArray>
#include <QStringList>

class Process : public QObject {
    Q_OBJECT
public:
    explicit Process(QObject* parent = nullptr);
    ~Process() override;

    // Start a new process with the given command and arguments
    void run(const QString& command, const QStringList& args = QStringList());

    // Start a new process with the given working directory, command, and arguments
    void run(const QString& workDir, const QString& command, const QStringList& args);

    // Terminate the running process
    void kill();

    // Get the current process state
    bool isRunning() const;

    // Get the process exit code (valid after process finishes)
    int exitCode() const;

    // Read all available stdout
    QString readStdOut();

    // Read all available stderr
    QString readStdErr();

    // Block until process finishes (with optional timeout in ms)
    bool waitForFinished(int timeoutMs = -1);

    // Get/set the process exit status
    enum ExitStatus { NormalExit, CrashExit };
    ExitStatus exitStatus() const;

signals:
    // Emitted when process finishes
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    // Emitted when new stdout data is available
    void readyReadStdOut(const QString& data);
    // Emitted when new stderr data is available
    void readyReadStdErr(const QString& data);
    // Emitted on process error
    void errorOccurred(QProcess::ProcessError error);

protected slots:
    void onFinished(int exitCode, QProcess::ExitStatus status);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onErrorOccurred(QProcess::ProcessError error);

private:
    QProcess* _process = nullptr;
    QString _curCmd;
    QStringList _curArgs;
    QString _curWorkDir;
};
