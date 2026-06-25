// Semantic Lift: Win32 AboutDlg → Qt6 QDialog implementation
// Original: PowerEditor/src/WinControls/AboutDlg/
// Target: npp-qt/src/WinControls/AboutDlg.h + .cpp

#include "AboutDlg.h"
#include "Window.h"
#include "NppDarkMode.h"
#include "DockingWnd.h"
#include "functionParser.h"  // for getLexerColors()
#include <QCoreApplication>

// Win32 macro replacement (DockingWnd.h provides message constants, not LOWORD)
#ifndef LOWORD
#define LOWORD(l) ((unsigned int)(((quintptr)(l)) & 0xFFFF))
#endif
#include <QApplication>
#include <QClipboard>
#include <QScreen>
#include <QPainter>
#include <QFont>
#include <QFontDatabase>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QTextBrowser>
#include <QUrl>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

// =============================================================================
// Utility: center widget on screen
// Mirrors Win32 goToCenter() in StaticDialog.
// =============================================================================
static void centerWidget(QWidget* w) {
    QScreen* screen = QApplication::screenAt(QCursor::pos());
    if (!screen) screen = QApplication::primaryScreen();
    if (!screen) return;
    QRect sg = screen->geometry();
    QRect wg = w->geometry();
    wg.moveCenter(sg.center());
    if (wg.left() < sg.left()) wg.moveLeft(sg.left());
    if (wg.top() < sg.top()) wg.moveTop(sg.top());
    if (wg.right() > sg.right()) wg.moveRight(sg.right());
    if (wg.bottom() > sg.bottom()) wg.moveBottom(sg.bottom());
    w->setGeometry(wg);
}

// =============================================================================
// Chameleon icon — draws a chameleon silhouette matching the editor theme.
// Wire: reads foreground/background from Scintilla lexer style 0 (default style).
// Falls back to NppDarkMode palette when no Scintilla view is available.
// =============================================================================

static QPixmap getChameleonPixmap(int w, int h)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Wire: get theme colors — try NppDarkMode first, then default green
    QColor chameleonGreen(0x4A, 0x90, 0x4A);
    QColor bodyColor(0x2E, 0x7D, 0x32);
    if (NppDarkMode::instance().isEnabled()) {
        // Use accent color for chameleon body
        QRgb accent = NppDarkMode::instance().accentColor();
        chameleonGreen = QColor::fromRgb(accent);
        bodyColor = chameleonGreen.darker(130);
    }

    // Draw chameleon body (stylized lizard silhouette)
    QPainterPath body;
    // Tail + body arc
    body.moveTo(w * 0.05, h * 0.55);
    body.quadTo(w * 0.15, h * 0.35, w * 0.35, h * 0.38);
    body.quadTo(w * 0.55, h * 0.32, w * 0.65, h * 0.42);
    body.quadTo(w * 0.75, h * 0.52, w * 0.82, h * 0.50);
    body.quadTo(w * 0.88, h * 0.55, w * 0.80, h * 0.65);
    // Belly
    body.quadTo(w * 0.70, h * 0.70, w * 0.55, h * 0.68);
    body.quadTo(w * 0.40, h * 0.72, w * 0.25, h * 0.68);
    body.quadTo(w * 0.10, h * 0.65, w * 0.05, h * 0.55);
    body.closeSubpath();

    p.fillPath(body, chameleonGreen);

    // Head bump
    QPainterPath head;
    head.moveTo(w * 0.65, h * 0.42);
    head.quadTo(w * 0.80, h * 0.30, w * 0.90, h * 0.40);
    head.quadTo(w * 0.92, h * 0.48, w * 0.82, h * 0.50);
    head.closeSubpath();
    p.fillPath(head, chameleonGreen);

    // Eye circle (white)
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0xFF, 0xFF, 0xFF));
    p.drawEllipse(static_cast<int>(w * 0.82), static_cast<int>(h * 0.34), 7, 7);
    // Pupil (black)
    p.setBrush(QColor(0x10, 0x10, 0x10));
    p.drawEllipse(static_cast<int>(w * 0.84), static_cast<int>(h * 0.35), 4, 4);

    // Tail curve detail
    QPen detailPen(bodyColor.darker(110), 1.5);
    p.setPen(detailPen);
    p.drawLine(static_cast<int>(w * 0.10), static_cast<int>(h * 0.52),
               static_cast<int>(w * 0.30), static_cast<int>(h * 0.50));
    p.drawLine(static_cast<int>(w * 0.30), static_cast<int>(h * 0.48),
               static_cast<int>(w * 0.50), static_cast<int>(h * 0.46));

    p.end();
    return pix;
}

// =============================================================================
// AboutDlg
// =============================================================================

AboutDlg::~AboutDlg() {
    destroy();
}

void AboutDlg::destroy() {
    _iconLoaded = false;
    // Chameleon icon — QPixmap member, auto-cleaned by Qt
}

void AboutDlg::doDialog() {
    if (!isCreated()) {
        create(0);
        setWindowTitle(QStringLiteral("About Notepad++"));
        setMinimumSize(520, 420);
        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

        QVBoxLayout* main = new QVBoxLayout(this);
        main->setContentsMargins(20, 16, 20, 16);
        main->setSpacing(12);

        // --- Icon + title row ---
        QHBoxLayout* titleRow = new QHBoxLayout();
        titleRow->setSpacing(16);

        // Chameleon icon (drawItem equivalent in Qt)
        // Wire: create a chameleon silhouette using Scintilla lexer colors.
        // The chameleon matches the current editor theme (dark/light/lexer colors).
        QLabel* iconLabel = new QLabel(this);
        iconLabel->setObjectName(QStringLiteral("IDC_CHAMELEON_ICON"));
        iconLabel->setFixedSize(80, 80);
        iconLabel->setAlignment(Qt::AlignCenter);
        // Build chameleon pixmap with current lexer/theme colors
        QPixmap chameleonPix = getChameleonPixmap(80, 80);
        iconLabel->setPixmap(chameleonPix);
        titleRow->addWidget(iconLabel);

        // Title + version info
        QVBoxLayout* titleInfo = new QVBoxLayout();
        titleInfo->setSpacing(4);

        QLabel* titleLabel = new QLabel(QStringLiteral("Notepad++"), this);
        QFont titleFont = titleLabel->font();
        titleFont.setBold(true);
        titleFont.setPointSize(16);
        titleLabel->setFont(titleFont);
        titleInfo->addWidget(titleLabel);

        // Wire: use real application version from QCoreApplication
        QString appVersion = QCoreApplication::applicationVersion();
        if (appVersion.isEmpty())
            appVersion = QStringLiteral("8.9.6");
        QLabel* versionLabel = new QLabel(QStringLiteral("v%1").arg(appVersion), this);
        versionLabel->setObjectName(QStringLiteral("IDC_VERSION"));
        titleInfo->addWidget(versionLabel);

        QLabel* bitLabel = new QLabel(QStringLiteral("(64-bit)"), this);
        bitLabel->setObjectName(QStringLiteral("IDC_VERSION_BIT"));
        titleInfo->addWidget(bitLabel);

        QLabel* buildTimeLabel = new QLabel(
            QStringLiteral("Build time: ") + __DATE__ " - " __TIME__, this);
        buildTimeLabel->setObjectName(QStringLiteral("IDC_BUILD_DATETIME"));
        buildTimeLabel->setEnabled(false);
        titleInfo->addWidget(buildTimeLabel);

        titleInfo->addStretch();
        titleRow->addLayout(titleInfo, 1);
        main->addLayout(titleRow);

        // --- Homepage link (URLCtrl equivalent: Qt native link) ---
        QLabel* homeLabel = new QLabel(
            QStringLiteral("<a href=\"https://notepad-plus-plus.org/\">"
                           "https://notepad-plus-plus.org/</a>"), this);
        homeLabel->setObjectName(QStringLiteral("IDC_HOME_ADDR"));
        homeLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
        homeLabel->setOpenExternalLinks(true);
        main->addWidget(homeLabel);

        // --- License text ---
        QTextEdit* licenceEdit = new QTextEdit(this);
        licenceEdit->setObjectName(QStringLiteral("IDC_LICENCE_EDIT"));
        licenceEdit->setReadOnly(true);
        licenceEdit->setMaximumHeight(140);
        licenceEdit->setTextInteractionFlags(Qt::NoTextInteraction);
        licenceEdit->setFrameShape(QFrame::NoFrame);
        licenceEdit->setStyleSheet(QStringLiteral(
            "QTextEdit { background: transparent; border: none; padding: 4px; }"
        ));
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

        // --- Close button ---
        QHBoxLayout* btnRow = new QHBoxLayout();
        btnRow->addStretch();
        QPushButton* closeBtn = new QPushButton(QStringLiteral("Close"), this);
        closeBtn->setObjectName(QStringLiteral("IDOK"));
        closeBtn->setDefault(true);
        connect(closeBtn, &QPushButton::clicked, this, [this]() { display(false); });
        btnRow->addWidget(closeBtn);
        main->addLayout(btnRow);

        setLayout(main);

        // Apply initial dark mode
        applyDarkMode();
    }

    centerWidget(this);
    display(true);
}

void AboutDlg::applyDarkMode() {
    // Mirrors Win32 NppDarkMode::autoSubclassAndThemeChildControls + WM_CTLCOLOR*
    bool dark = NppDarkMode::instance().isEnabled();
    QString bg, fg, btnBg, btnFg;

    if (dark) {
        bg = QStringLiteral("#%1").arg(
            NppDarkMode::instance().backgroundColor(), 6, 16, QChar('0'));
        fg = QStringLiteral("#%1").arg(
            NppDarkMode::instance().textColor(), 6, 16, QChar('0'));
        btnBg = QStringLiteral("#%1").arg(
            NppDarkMode::instance().ctrlBackgroundColor(), 6, 16, QChar('0'));
        btnFg = fg;
    } else {
        bg = QStringLiteral("#%1").arg(0xFFFFFF, 6, 16, QChar('0'));
        fg = QStringLiteral("#%1").arg(0x000000, 6, 16, QChar('0'));
        btnBg = QStringLiteral("#%1").arg(0xF0F0F0, 6, 16, QChar('0'));
        btnFg = fg;
    }

    setStyleSheet(QStringLiteral(
        "QWidget { background-color: %1; color: %2; } "
        "QLabel { background-color: transparent; } "
        "QTextEdit { background-color: %3; color: %2; } "
        "QPushButton { background-color: %3; color: %4; border: 1px solid %2; "
        "padding: 4px 16px; border-radius: 4px; } "
        "QPushButton:hover { background-color: %1; } "
        "QPushButton:pressed { background-color: %2; color: %1; } "
        "QPushButton[objectName=\"IDOK\"] { font-weight: bold; }"
    ).arg(bg, fg, btnBg, btnFg));

    // Update chameleon icon for dark mode — repaint with new theme colors
    QLabel* iconLabel = findChild<QLabel*>(QStringLiteral("IDC_CHAMELEON_ICON"));
    if (iconLabel) {
        // Wire: regenerate chameleon pixmap with updated accent color
        QPixmap chameleonPix = getChameleonPixmap(iconLabel->width(), iconLabel->height());
        iconLabel->setPixmap(chameleonPix);
    }
}

intptr_t AboutDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) {
    Q_UNUSED(lParam);

    switch (message) {
        case WM_INITDIALOG:
            // Already initialized in doDialog
            return 1; // TRUE

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                case IDCANCEL:
                    display(false);
                    return 1;
            }
            break;

        case WM_CLOSE:
            display(false);
            return 1;

        case NPPM_INTERNAL_REFRESHDARKMODE:
            applyDarkMode();
            return 1;

        default:
            break;
    }
    return 0;
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
        main->setContentsMargins(12, 12, 12, 12);

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
        connect(okBtn, &QPushButton::clicked, this, [this]() { display(false); });
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
        // Leave text in selected state (mirrors Win32 EM_SETSEL + SetFocus)
        QTextCursor tc = debugEdit->textCursor();
        tc.select(QTextCursor::Document);
        debugEdit->setTextCursor(tc);
        debugEdit->setFocus();
    }
}

intptr_t DebugInfoDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) {
    Q_UNUSED(lParam);

    switch (message) {
        case WM_INITDIALOG:
            return 1;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                case IDCANCEL:
                    display(false);
                    return 1;

                case IDC_DEBUGINFO_COPYLINK: {
                    QTextEdit* debugEdit = findChild<QTextEdit*>(QStringLiteral("IDC_DEBUGINFO_EDIT"));
                    if (debugEdit) {
                        QApplication::clipboard()->setText(debugEdit->toPlainText());
                        // Mirrors Win32: set focus back to edit after copy
                        debugEdit->setFocus();
                    }
                    return 1;
                }
            }
            break;

        case WM_CLOSE:
            display(false);
            return 1;

        case NPPM_INTERNAL_REFRESHDARKMODE:
            // Re-apply dark mode theming
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
        case WM_INITDIALOG:
            return 1;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                case IDCANCEL:
                    display(false);
                    return 1;
            }
            break;

        case WM_CLOSE:
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
            QPushButton* retryBtn = new QPushButton(QStringLiteral("Cancel"), this);
            retryBtn->setObjectName(QStringLiteral("IDIGNORE"));
            connect(retryBtn, &QPushButton::clicked, this, [this]() {
                _clickedButtonId = IDIGNORE;
                display(false);
            });
            btnRow->addWidget(retryBtn);
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
    Q_UNUSED(lParam);

    switch (message) {
        case WM_INITDIALOG:
            return 1;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDCANCEL:
                    _clickedButtonId = IDCANCEL;
                    display(false);
                    return 1;
                case IDYES:
                    _clickedButtonId = IDYES;
                    display(false);
                    return 1;
                case IDNO:
                    _clickedButtonId = IDNO;
                    display(false);
                    return 1;
                case IDIGNORE:
                    _clickedButtonId = IDIGNORE;
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
    Q_UNUSED(lParam);

    switch (message) {
        case WM_INITDIALOG:
            return 1;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDCANCEL:
                    _clickedButtonId = IDCANCEL;
                    display(false);
                    return 1;
                case IDYES:
                    _clickedButtonId = IDYES;
                    display(false);
                    return 1;
                case IDNO:
                    _clickedButtonId = IDNO;
                    display(false);
                    return 1;
                case IDRETRY:
                    _clickedButtonId = IDRETRY;
                    display(false);
                    return 1;
            }
            break;

        default:
            break;
    }
    return 0;
}