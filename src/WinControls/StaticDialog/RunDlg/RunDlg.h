// RunDlg.h — Qt6 translation of Run dialog
#pragma once

#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class RunDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RunDlg(QWidget* parent = nullptr);
    ~RunDlg() override;

    void doDialog(bool isRTL = false);

    // Insert variable like $(FULL_CURRENT_PATH)
    void insertVariable(unsigned char id);

    QString getCommand() const;
    void addToHistory(const QString& cmd);
    void removeFromHistory(const QString& cmd);

signals:
    void commandRun(const QString& cmd);

protected:
    int run_dlgProc(QEvent* event);
    bool event(QEvent* event) override;

private slots:
    void onBrowseClicked();
    void onVariablesClicked();
    void onSaveClicked();
    void onRunClicked();
    void onCancelClicked();

private:
    QComboBox* _cmdCombo = nullptr;
    QLabel* _mainTextLabel = nullptr;
    QString _currentCommand;
};

// Run variable IDs
enum RunVarID {
    FULL_CURRENT_PATH = 0,
    CURRENT_DIRECTORY = 1,
    FILE_NAME = 2,
    NAME_PART = 3,
    EXT_PART = 4,
    CURRENT_WORD = 5,
    NPP_DIRECTORY = 6,
    NPP_FULL_FILE_PATH = 7,
    CURRENT_LINE = 8,
    CURRENT_COLUMN = 9,
    CURRENT_LINESTR = 10,
    VAR_NOT_RECOGNIZED = -1
};

// Find which variable a string represents
int whichVar(const QString& str);

// Expand $(VARIABLE) patterns
void expandNppEnvironmentStrs(const QString& src, QString& dest, QWidget* editor);

// Command execution
class Command
{
public:
    Command() = default;
    explicit Command(const QString& cmd);

    bool run(QWidget* parent, const QString& cwd = QString());
    QString commandLine() const { return _cmdLine; }

private:
    void extractArgs(QString& cmdExec, QString& args, const QString& cmdEntier) const;

    QString _cmdLine;
};
