// Semantic Lift: Win32 AboutDlg → Qt6 QDialog implementation
// Original: PowerEditor/src/WinControls/AboutDlg/
#include <QClipboard>
// Target: npp-qt/src/WinControls/AboutDlg.h + .cpp

#include "AboutDlg.h"
#include "Window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QMessageBox>
#include <QFont>
#include <QFontDatabase>
#include <QDateTime>

// =============================================================================
// Utility: center widget on screen
// =============================================================================
static void centerWidget(QWidget* w) {
    QScreen* screen = QApplication::screenAt(QCursor::pos());
    if (!screen) screen = QApplication::primaryScreen();
    if (!screen) return;
    QRect sg = screen->geometry();
    QRect wg = w->geometry();
    wg.moveCenter(sg.center());
    // Clamp to screen
    if (wg.left() < sg.left()) wg.moveLeft(sg.left());
    if (wg.top() < sg.top()) wg.moveTop(sg.top());
    if (wg.right() > sg.right()) wg.moveRight(sg.right());
    if (wg.bottom() > sg.bottom()) wg.moveBottom(sg.bottom());
    w->setGeometry(wg);
}

// =============================================================================
// AboutDlg
// =============================================================================

AboutDlg::~AboutDlg() {
    destroy();
}

void AboutDlg::destroy() {
    // Icon is QIcon — automatically cleaned up
    _iconLoaded = false;
}

void AboutDlg::doDialog() {
    if (!isCreated()) {
        create(0); // Dialog ID not needed in Qt version
        setWindowTitle(QStringLiteral("About Notepad++"));
        setMinimumSize(520, 400);

        // Main layout
        QVBoxLayout* main = new QVBoxLayout(this);
        main->setContentsMargins(16, 16, 16, 16);

        // Icon + title row
        QHBoxLayout* titleRow = new QHBoxLayout();

        // Chameleon icon placeholder
        QLabel* iconLabel = new QLabel(this);
        iconLabel->setObjectName(QStringLiteral("IDC_CHAMELEON_ICON"));
        iconLabel->setFixedSize(80, 80);
        iconLabel->setAlignment(Qt::AlignCenter);
        updateIcon();
        titleRow->addWidget(iconLabel);

        // Title + version info
        QVBoxLayout* titleInfo = new QVBoxLayout();
        titleInfo->addSpacing(16);

        QLabel* titleLabel = new QLabel(QStringLiteral("Notepad++"), this);
        QFont titleFont = titleLabel->font();
        titleFont.setBold(true);
        titleFont.setPointSize(16);
        titleLabel->setFont(titleFont);
        titleInfo->addWidget(titleLabel);

        QLabel* versionLabel = new QLabel(QStringLiteral("v8.9.6"), this);
        versionLabel->setObjectName(QStringLiteral("IDC_VERSION"));
        titleInfo->addWidget(versionLabel);

        QLabel* bitLabel = new QLabel(QStringLiteral("(64-bit)"), this);
        bitLabel->setObjectName(QStringLiteral("IDC_VERSION_BIT"));
        titleInfo->addWidget(bitLabel);

        QLabel* buildTimeLabel = new QLabel(QStringLiteral("Build time: ") + __DATE__ " - " __TIME__, this);
        buildTimeLabel->setObjectName(QStringLiteral("IDC_BUILD_DATETIME"));
        titleInfo->addWidget(buildTimeLabel);

        titleInfo->addStretch();
        titleRow->addLayout(titleInfo);
        main->addLayout(titleRow);

        // Homepage link (URLCtrl placeholder)
        QLabel* homeLabel = new QLabel(QStringLiteral("<a href=\"https://notepad-plus-plus.org/\">https://notepad-plus-plus.org/</a>"), this);
        homeLabel->setObjectName(QStringLiteral("IDC_HOME_ADDR"));
        homeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
        main->addWidget(homeLabel);

        // License text
        QTextEdit* licenceEdit = new QTextEdit(this);
        licenceEdit->setObjectName(QStringLiteral("IDC_LICENCE_EDIT"));
        licenceEdit->setReadOnly(true);
        licenceEdit->setMaximumHeight(120);
        licenceEdit->setPlainText(
            QStringLiteral("This program is free software; you can redistribute it and/or "
                          "modify it under the terms of the GNU General Public License "
                          "as published by the Free Software Foundation; either "
                          "version 3 of the License, or at your option any later version.\n\n"
                          "This program is distributed in the hope that it will be useful, "
                          "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
                          "GNU General Public License for more details.\n\n"
                          "You should have received a copy of the GNU General Public License "
                          "along with this program. If not, see <https://www.gnu.org/licenses/>.")
        );
        main->addWidget(licenceEdit);

        // Close button
        QHBoxLayout* btnRow = new QHBoxLayout();
        btnRow->addStretch();
        QPushButton* closeBtn = new QPushButton(QStringLiteral("Close"), this);
        closeBtn->setObjectName(QStringLiteral("IDOK"));
        connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
        btnRow->addWidget(closeBtn);
        main->addLayout(btnRow);

        setLayout(main);
    }

    centerWidget(this);
    display(true);
}

void AboutDlg::updateIcon() {
    QLabel* iconLabel = findChild<QLabel*>(QStringLiteral("IDC_CHAMELEON_ICON"));
    if (!iconLabel) return;

    if (!_iconLoaded) {
        // Try to load chameleon icon — in Qt version this would come from resources
        // For now, use a placeholder colored rectangle
        _iconLoaded = true;
    }

    // Set a placeholder icon (colored square as chameleon placeholder)
    QPixmap pix(80, 80);
    pix.fill(QColor(0x4A, 0x90, 0x4A)); // Chameleon green
    iconLabel->setPixmap(pix);
}

intptr_t AboutDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) {
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    switch (message) {
        case 0x0110: // WM_INITDIALOG
            // Initialized in doDialog
            return 1; // TRUE

        case 0x0111: // WM_COMMAND
            switch (wParam & 0xFFFF) {
                case 1:    // IDOK
                case 4096: // IDCANCEL (Win32 dialog IDs vary)
                    display(false);
                    return 1;
            }
            break;

        case 0x0010: // WM_CLOSE
            display(false);
            return 1;

        default:
            break;
    }
    return 0; // FALSE — not handled
}

// =============================================================================
// DebugInfoDlg
// =============================================================================

void DebugInfoDlg::init(bool isAdmin, const QString& loadedPlugins) {
    _isAdmin = isAdmin;
    _loadedPlugins = loadedPlugins;
}

void DebugInfoDlg::doDialog() {
    if (!isCreated()) {
        create(0);
        setWindowTitle(QStringLiteral("Debug Info"));
        setMinimumSize(700, 500);

        QVBoxLayout* main = new QVBoxLayout(this);

        // Debug info text edit
        QTextEdit* debugEdit = new QTextEdit(this);
        debugEdit->setObjectName(QStringLiteral("IDC_DEBUGINFO_EDIT"));
        debugEdit->setReadOnly(true);
        debugEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        main->addWidget(debugEdit);

        // Build the debug info string
        buildDebugInfoString();
        debugEdit->setPlainText(_debugInfoDisplay);

        // Copy link button
        QHBoxLayout* btnRow = new QHBoxLayout();
        btnRow->addStretch();

        QPushButton* copyBtn = new QPushButton(QStringLiteral("Copy"), this);
        copyBtn->setObjectName(QStringLiteral("IDC_DEBUGINFO_COPYLINK"));
        connect(copyBtn, &QPushButton::clicked, this, [debugEdit]() {
            QApplication::clipboard()->setText(debugEdit->toPlainText());
        });
        btnRow->addWidget(copyBtn);

        QPushButton* okBtn = new QPushButton(QStringLiteral("OK"), this);
        okBtn->setObjectName(QStringLiteral("IDOK"));
        connect(okBtn, &QPushButton::clicked, this, &QWidget::close);
        btnRow->addWidget(okBtn);

        main->addLayout(btnRow);
        setLayout(main);
    }

    refreshDebugInfo();
    centerWidget(this);
    display(true);
}

void DebugInfoDlg::buildDebugInfoString() {
    QString info;

    // Notepad++ version
    info += QStringLiteral("Notepad++ v8.9.6 (64-bit)\n");
    info += QStringLiteral("Build time: ") + __DATE__ " " __TIME__ + QStringLiteral("\n");

    // Scintilla/Lexilla version
    info += QStringLiteral("Scintilla/Lexilla included: 5.3.7/5.4.5\n");

    // Boost Regex
    info += QStringLiteral("Boost Regex included: 1.86.0\n");

    // pugixml
    info += QStringLiteral("pugixml included: 1.14\n");

    // JSON
    info += QStringLiteral("nlohmann JSON included: 3.11.3\n");

    // Admin mode
    info += QStringLiteral("Admin mode: ") + (_isAdmin ? QStringLiteral("ON") : QStringLiteral("OFF")) + QStringLiteral("\n");

    // Local conf
    info += QStringLiteral("Local Conf mode: OFF\n");

    // Plugins
    info += QStringLiteral("Plugins: ") + (_loadedPlugins.isEmpty() ? QStringLiteral("none") : _loadedPlugins) + QStringLiteral("\n");

    _debugInfoStr = info;
    _debugInfoDisplay = info;
}

void DebugInfoDlg::refreshDebugInfo() {
    buildDebugInfoString();
    QTextEdit* debugEdit = findChild<QTextEdit*>(QStringLiteral("IDC_DEBUGINFO_EDIT"));
    if (debugEdit) {
        debugEdit->setPlainText(_debugInfoDisplay);
    }
}

intptr_t DebugInfoDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) {
    Q_UNUSED(lParam);

    switch (message) {
        case 0x0110: // WM_INITDIALOG
            return 1;

        case 0x0111: // WM_COMMAND
            switch (wParam & 0xFFFF) {
                case 1:    // IDOK
                case 4096: // IDCANCEL
                    display(false);
                    return 1;

                case 2: { // IDC_DEBUGINFO_COPYLINK
                    QTextEdit* debugEdit = findChild<QTextEdit*>(QStringLiteral("IDC_DEBUGINFO_EDIT"));
                    if (debugEdit) {
                        QApplication::clipboard()->setText(debugEdit->toPlainText());
                    }
                    return 1;
                }
            }
            break;

        case 0x0010: // WM_CLOSE
            display(false);
            return 1;

        default:
            break;
    }
    return 0;
}

// =============================================================================
// CmdLineArgsDlg
// =============================================================================

namespace {
const QString COMMAND_ARG_HELP = QStringLiteral(
    "Usage:\n\n"
    "notepad++ [--help] [-multiInst] [-noPlugin] [-lLanguage] [-udl=\"My UDL Name\"]\n"
    "[-LlangCode] [-nLineNumber] [-cColumnNumber] [-pPosition] [-xLeftPos] [-yTopPos]\n"
    "[-monitor] [-nosession] [-notabbar] [-systemtray] [-loadingTime] [-alwaysOnTop]\n"
    "[-ro] [-fullReadOnly] [-fullReadOnlySavingForbidden] [-openSession] [-r]\n"
    "[-qn=\"Easter egg name\" | -qt=\"a text to display.\" | -qf=\"D:\\my quote.txt\"]\n"
    "[-qSpeed1|2|3] [-quickPrint] [-settingsDir=\"d:\\your settings dir\\\"]\n"
    "[-openFoldersAsWorkspace] [-titleAdd=\"additional title bar text\"]\n"
    "[filePath]\n\n"
    "--help: This help message\n"
    "-multiInst: Launch another Notepad++ instance\n"
    "-noPlugin: Launch Notepad++ without loading any plugin\n"
    "-l: Open file or Ghost type with syntax highlighting of choice\n"
    "-udl=\"My UDL Name\": Open file by applying User Defined Language\n"
    "-L: Apply indicated localization, langCode is browser language code\n"
    "-n: Scroll to indicated line on filePath\n"
    "-c: Scroll to indicated column on filePath\n"
    "-p: Scroll to indicated position on filePath\n"
    "-x: Move Notepad++ to indicated left side position on the screen\n"
    "-y: Move Notepad++ to indicated top position on the screen\n"
    "-monitor: Open file with file monitoring enabled\n"
    "-nosession: Launch Notepad++ without previous session\n"
    "-notabbar: Launch Notepad++ without tab bar\n"
    "-ro: Make the filePath read-only\n"
    "-fullReadOnly: Open all files read-only by default, toggling the R/O off and saving is allowed\n"
    "-fullReadOnlySavingForbidden: Open all files read-only by default, toggling the R/O off and saving is disabled\n"
    "-systemtray: Launch Notepad++ directly in system tray\n"
    "-loadingTime: Display Notepad++ loading time\n"
    "-alwaysOnTop: Make Notepad++ always on top\n"
    "-openSession: Open a session. filePath must be a session file\n"
    "-r: Open files recursively. This argument will be ignored if filePath contains no wildcard character\n"
    "-qn=\"Easter egg name\": Ghost type easter egg via its name\n"
    "-qt=\"text to display.\": Ghost type the given text\n"
    "-qf=\"D:\\my quote.txt\": Ghost type a file content via the file path\n"
    "-qSpeed: Ghost typing speed. Value from 1 to 3 for slow, fast and fastest\n"
    "-quickPrint: Print the file given as argument then quit Notepad++\n"
    "-settingsDir=\"d:\\your settings dir\\\": Override the default settings dir\n"
    "-openFoldersAsWorkspace: open filePath of folder(s) as workspace\n"
    "-titleAdd=\"string\": add string to Notepad++ title bar\n"
    "filePath: file or folder name to open (absolute or relative path name)\n"
);
} // namespace

void CmdLineArgsDlg::doDialog() {
    if (!isCreated()) {
        create(0);
        setWindowTitle(QStringLiteral("Command Line Arguments"));
        setMinimumSize(600, 500);

        QVBoxLayout* main = new QVBoxLayout(this);

        // Monospace font for command line help
        _cmdLineEditFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

        QTextEdit* cmdHelpEdit = new QTextEdit(this);
        cmdHelpEdit->setObjectName(QStringLiteral("IDC_COMMANDLINEARGS_EDIT"));
        cmdHelpEdit->setReadOnly(true);
        cmdHelpEdit->setFont(_cmdLineEditFont);
        cmdHelpEdit->setPlainText(COMMAND_ARG_HELP);
        main->addWidget(cmdHelpEdit);

        // Close button
        QHBoxLayout* btnRow = new QHBoxLayout();
        btnRow->addStretch();
        QPushButton* closeBtn = new QPushButton(QStringLiteral("Close"), this);
        closeBtn->setObjectName(QStringLiteral("IDOK"));
        connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
        btnRow->addWidget(closeBtn);
        main->addLayout(btnRow);

        setLayout(main);
    }

    centerWidget(this);
    display(true);
}

intptr_t CmdLineArgsDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) {
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    switch (message) {
        case 0x0110: // WM_INITDIALOG
            return 1;

        case 0x0111: // WM_COMMAND
            switch (wParam & 0xFFFF) {
                case 1:    // IDOK
                case 4096: // IDCANCEL
                    display(false);
                    return 1;
            }
            break;

        case 0x0010: // WM_CLOSE
            display(false);
            return 1;

        default:
            break;
    }
    return 0;
}

// =============================================================================
// DoSaveOrNotBox
// =============================================================================

void DoSaveOrNotBox::init(const QString& filename, bool isMulti) {
    _filename = filename;
    _isMulti = isMulti;
}

void DoSaveOrNotBox::doDialog(bool isRTL) {
    if (!isCreated()) {
        create(0);
        setWindowTitle(QStringLiteral("Save?"));
        setMinimumSize(400, 150);

        if (isRTL) {
            setLayoutDirection(Qt::RightToLeft);
        }

        QVBoxLayout* main = new QVBoxLayout(this);

        // Message text
        QString msg = QStringLiteral("Save file \"%1\"?").arg(_filename);
        QLabel* msgLabel = new QLabel(msg, this);
        msgLabel->setObjectName(QStringLiteral("IDC_DOSAVEORNOTTEXT"));
        main->addWidget(msgLabel);

        // Buttons
        QHBoxLayout* btnRow = new QHBoxLayout();
        btnRow->addStretch();

        QPushButton* yesBtn = new QPushButton(QStringLiteral("Yes"), this);
        yesBtn->setObjectName(QStringLiteral("IDYES"));
        connect(yesBtn, &QPushButton::clicked, this, [this]() {
            _clickedButtonId = 6; // IDYES
            display(false);
        });
        btnRow->addWidget(yesBtn);

        QPushButton* noBtn = new QPushButton(QStringLiteral("No"), this);
        noBtn->setObjectName(QStringLiteral("IDNO"));
        connect(noBtn, &QPushButton::clicked, this, [this]() {
            _clickedButtonId = 7; // IDNO
            display(false);
        });
        btnRow->addWidget(noBtn);

        if (_isMulti) {
            QPushButton* cancelBtn = new QPushButton(QStringLiteral("Cancel"), this);
            cancelBtn->setObjectName(QStringLiteral("IDCANCEL"));
            connect(cancelBtn, &QPushButton::clicked, this, [this]() {
                _clickedButtonId = 2; // IDCANCEL
                display(false);
            });
            btnRow->addWidget(cancelBtn);
        }

        main->addLayout(btnRow);
        setLayout(main);
    }

    centerWidget(this);
    display(true);
}

void DoSaveOrNotBox::changeLang() {
    // In full implementation, would use NativeLangSpeaker for i18n
    // Just keep the default English message
}

intptr_t DoSaveOrNotBox::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) {
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    switch (message) {
        case 0x0110: // WM_INITDIALOG
            return 1;

        case 0x0111: // WM_COMMAND
            switch (wParam & 0xFFFF) {
                case 2:    // IDCANCEL
                    _clickedButtonId = 2;
                    display(false);
                    return 1;
                case 6:    // IDYES
                    _clickedButtonId = 6;
                    display(false);
                    return 1;
                case 7:    // IDNO
                    _clickedButtonId = 7;
                    display(false);
                    return 1;
                case 10:   // IDIGNORE (Retry in multi-file case)
                    _clickedButtonId = 10;
                    display(false);
                    return 1;
            }
            break;

        default:
            break;
    }
    return 0;
}

// =============================================================================
// DoSaveAllBox
// =============================================================================

void DoSaveAllBox::doDialog(bool isRTL) {
    if (!isCreated()) {
        create(0);
        setWindowTitle(QStringLiteral("Save All?"));
        setMinimumSize(400, 150);

        if (isRTL) {
            setLayoutDirection(Qt::RightToLeft);
        }

        QVBoxLayout* main = new QVBoxLayout(this);

        // Message
        QLabel* msgLabel = new QLabel(
            QStringLiteral("Are you sure you want to save all modified documents?\n"
                          "Choose \"Always Yes\" if you don't want to see this dialog again.\n"
                          "You can re-activate this dialog in Preferences later."),
            this
        );
        msgLabel->setObjectName(QStringLiteral("IDC_DOSAVEALLTEXT"));
        main->addWidget(msgLabel);

        // Buttons
        QHBoxLayout* btnRow = new QHBoxLayout();
        btnRow->addStretch();

        QPushButton* yesBtn = new QPushButton(QStringLiteral("Yes"), this);
        yesBtn->setObjectName(QStringLiteral("IDYES"));
        connect(yesBtn, &QPushButton::clicked, this, [this]() {
            _clickedButtonId = 6;
            display(false);
        });
        btnRow->addWidget(yesBtn);

        QPushButton* noBtn = new QPushButton(QStringLiteral("No"), this);
        noBtn->setObjectName(QStringLiteral("IDNO"));
        connect(noBtn, &QPushButton::clicked, this, [this]() {
            _clickedButtonId = 7;
            display(false);
        });
        btnRow->addWidget(noBtn);

        QPushButton* cancelBtn = new QPushButton(QStringLiteral("Cancel"), this);
        cancelBtn->setObjectName(QStringLiteral("IDCANCEL"));
        connect(cancelBtn, &QPushButton::clicked, this, [this]() {
            _clickedButtonId = 2;
            display(false);
        });
        btnRow->addWidget(cancelBtn);

        main->addLayout(btnRow);
        setLayout(main);
    }

    centerWidget(this);
    display(true);
}

void DoSaveAllBox::changeLang() {
    // Full i18n via NativeLangSpeaker would go here
}

intptr_t DoSaveAllBox::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) {
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    switch (message) {
        case 0x0110: // WM_INITDIALOG
            return 1;

        case 0x0111: // WM_COMMAND
            switch (wParam & 0xFFFF) {
                case 2:    // IDCANCEL
                    _clickedButtonId = 2;
                    display(false);
                    return 1;
                case 6:    // IDYES
                    _clickedButtonId = 6;
                    display(false);
                    return 1;
                case 7:    // IDNO
                    _clickedButtonId = 7;
                    display(false);
                    return 1;
            }
            break;

        default:
            break;
    }
    return 0;
}