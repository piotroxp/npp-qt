// RunDlg.cpp — Qt6 translation of Run dialog
#include "RunDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCompleter>
#include <QDir>
#include <QProcess>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenu>
#include <QUrl>

// ─── Variable expansion ───────────────────────────────────────────────

static const QString fullCurrentPath = "$(" + QString("FULL_CURRENT_PATH") + ")";
static const QString currentDirectory = "$(" + QString("CURRENT_DIRECTORY") + ")";
static const QString onlyFileName = "$(" + QString("FILE_NAME") + ")";
static const QString fileNamePart = "$(" + QString("NAME_PART") + ")";
static const QString fileExtPart = "$(" + QString("EXT_PART") + ")";
static const QString currentWord = "$(" + QString("CURRENT_WORD") + ")";
static const QString nppDir = "$(" + QString("NPP_DIRECTORY") + ")";
static const QString nppFullFilePath = "$(" + QString("NPP_FULL_FILE_PATH") + ")";
static const QString currentLine = "$(" + QString("CURRENT_LINE") + ")";
static const QString currentColumn = "$(" + QString("CURRENT_COLUMN") + ")";
static const QString currentLineStr = "$(" + QString("CURRENT_LINESTR") + ")";

int whichVar(const QString& str)
{
    if (str == fullCurrentPath) return FULL_CURRENT_PATH;
    if (str == currentDirectory) return CURRENT_DIRECTORY;
    if (str == onlyFileName) return FILE_NAME;
    if (str == fileNamePart) return NAME_PART;
    if (str == fileExtPart) return EXT_PART;
    if (str == currentWord) return CURRENT_WORD;
    if (str == nppDir) return NPP_DIRECTORY;
    if (str == nppFullFilePath) return NPP_FULL_FILE_PATH;
    if (str == currentLine) return CURRENT_LINE;
    if (str == currentColumn) return CURRENT_COLUMN;
    if (str == currentLineStr) return CURRENT_LINESTR;
    return VAR_NOT_RECOGNIZED;
}

void expandNppEnvironmentStrs(const QString& src, QString& dest, QWidget* /*editor*/)
{
    dest = src;
    // Simplified: in full implementation, would expand $(VARIABLE) patterns
    // by querying editor for actual values
}

Command::Command(const QString& cmd)
    : _cmdLine(cmd)
{
}

bool Command::run(QWidget* parent, const QString& cwd)
{
    QStringList args;
    QString program = _cmdLine;

    // Extract program and arguments
    extractArgs(program, args, _cmdLine);

    // Expand environment variables
    QString expandedProgram = QProcess::systemEnvironment().join(" ");
    Q_UNUSED(expandedProgram);

    // Open URL/file with desktop services
    QFileInfo fi(program);
    if (fi.exists())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
        return true;
    }

    // Try to execute
    bool started = QProcess::startDetached(program, args, cwd);
    return started;
}

void Command::extractArgs(QString& cmdExec, QString& args, const QString& cmdEntier) const
{
    cmdExec = cmdEntier;
    args.clear();

    // Simple split on first space
    int spacePos = cmdExec.indexOf(' ');
    if (spacePos > 0)
    {
        args = cmdExec.mid(spacePos + 1).trimmed();
        cmdExec = cmdExec.left(spacePos).trimmed();
    }
}

// ─── RunDlg ──────────────────────────────────────────────────────────

RunDlg::RunDlg(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Run");
    setModal(true);
    resize(500, 120);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    _mainTextLabel = new QLabel("Enter command to run:", this);
    mainLayout->addWidget(_mainTextLabel);

    _cmdCombo = new QComboBox(this);
    _cmdCombo->setEditable(true);
    _cmdCombo->setMinimumWidth(400);
    mainLayout->addWidget(_cmdCombo);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addStretch();

    QPushButton* btnVariables = new QPushButton("Variables >>", this);
    connect(btnVariables, &QPushButton::clicked, this, &RunDlg::onVariablesClicked);
    btnLayout->addWidget(btnVariables);

    QPushButton* btnBrowse = new QPushButton("Browse...", this);
    connect(btnBrowse, &QPushButton::clicked, this, &RunDlg::onBrowseClicked);
    btnLayout->addWidget(btnBrowse);

    QPushButton* btnSave = new QPushButton("Save...", this);
    connect(btnSave, &QPushButton::clicked, this, &RunDlg::onSaveClicked);
    btnLayout->addWidget(btnSave);

    QPushButton* btnRun = new QPushButton("Run", this);
    connect(btnRun, &QPushButton::clicked, this, &RunDlg::onRunClicked);
    btnLayout->addWidget(btnRun);

    QPushButton* btnCancel = new QPushButton("Cancel", this);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(btnCancel);

    mainLayout->addLayout(btnLayout);
}

RunDlg::~RunDlg() = default;

void RunDlg::doDialog(bool isRTL)
{
    if (isRTL)
        setLayoutDirection(Qt::RightToLeft);

    // Load history from settings
    show();
    _cmdCombo->setFocus();
    _cmdCombo->lineEdit()->selectAll();
}

QString RunDlg::getCommand() const
{
    return _cmdCombo->currentText();
}

void RunDlg::addToHistory(const QString& cmd)
{
    int idx = _cmdCombo->findText(cmd);
    if (idx >= 0)
        _cmdCombo->removeItem(idx);
    _cmdCombo->insertItem(0, cmd);
    if (_cmdCombo->count() > 20) // Keep max 20 items
        _cmdCombo->removeItem(_cmdCombo->count() - 1);
}

void RunDlg::removeFromHistory(const QString& cmd)
{
    int idx = _cmdCombo->findText(cmd);
    if (idx >= 0)
        _cmdCombo->removeItem(idx);
}

void RunDlg::insertVariable(unsigned char id)
{
    QString var;
    switch (id)
    {
        case FULL_CURRENT_PATH: var = fullCurrentPath; break;
        case CURRENT_DIRECTORY: var = currentDirectory; break;
        case FILE_NAME: var = onlyFileName; break;
        case NAME_PART: var = fileNamePart; break;
        case EXT_PART: var = fileExtPart; break;
        case CURRENT_WORD: var = currentWord; break;
        case NPP_DIRECTORY: var = nppDir; break;
        case NPP_FULL_FILE_PATH: var = nppFullFilePath; break;
        case CURRENT_LINE: var = currentLine; break;
        case CURRENT_COLUMN: var = currentColumn; break;
        case CURRENT_LINESTR: var = currentLineStr; break;
        default: return;
    }

    QString txt = _cmdCombo->currentText();
    txt += var;
    _cmdCombo->setEditText(txt);
}

void RunDlg::onBrowseClicked()
{
    // Open file browser for executable
    QString file = QFileDialog::getOpenFileName(
        this, "Select Executable", QString(),
        "Executables (*.exe *.com *.cmd *.bat);;All Files (*.*)"
    );
    if (!file.isEmpty())
    {
        if (file.contains(' '))
            file = "\"" + file + "\"";
        _cmdCombo->setEditText(file);
    }
}

void RunDlg::onVariablesClicked()
{
    QMenu menu(this);
    menu.addAction(fullCurrentPath, [this] { insertVariable(FULL_CURRENT_PATH); });
    menu.addAction(currentDirectory, [this] { insertVariable(CURRENT_DIRECTORY); });
    menu.addAction(onlyFileName, [this] { insertVariable(FILE_NAME); });
    menu.addAction(fileNamePart, [this] { insertVariable(NAME_PART); });
    menu.addAction(fileExtPart, [this] { insertVariable(EXT_PART); });
    menu.addAction(currentWord, [this] { insertVariable(CURRENT_WORD); });
    menu.addAction(nppDir, [this] { insertVariable(NPP_DIRECTORY); });
    menu.addAction(nppFullFilePath, [this] { insertVariable(NPP_FULL_FILE_PATH); });
    menu.addAction(currentLine, [this] { insertVariable(CURRENT_LINE); });
    menu.addAction(currentColumn, [this] { insertVariable(CURRENT_COLUMN); });
    menu.addAction(currentLineStr, [this] { insertVariable(CURRENT_LINESTR); });

    QPoint pos = mapToGlobal(QPoint(0, 0));
    menu.exec(pos);
}

void RunDlg::onSaveClicked()
{
    // Save command to user commands list
    // In real impl would emit signal to register command
    addToHistory(getCommand());
}

void RunDlg::onRunClicked()
{
    QString cmd = getCommand();
    if (cmd.isEmpty())
        return;

    addToHistory(cmd);
    Command command(cmd);
    bool ok = command.run(this);

    if (ok)
        accept();
    else
        reject();
}

void RunDlg::onCancelClicked()
{
    reject();
}

int RunDlg::run_dlgProc(QEvent* event)
{
    Q_UNUSED(event);
    return 0;
}

bool RunDlg::event(QEvent* event)
{
    return QDialog::event(event);
}
