// Semantic lift: lesDlgs.cpp — Win32 encoding/minimal dialogs → Qt6
// Original: PowerEditor/src/lesDlgs.cpp (103 lines)
// Target: npp-qt/src/lesDlgs.cpp
// Copyright (C) 2021 Don HO — GPLv3

#include "lesDlgs.h"

#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>

#include "menuCmdID.h"
#include "NppDarkMode.h"

// =============================================================================
// Local resource ID constants (from Win32 resource.h)
// =============================================================================
constexpr int IDD_BUTTON_DLG    = 2410;
constexpr int IDC_RESTORE_BUTTON = 2411;

// =============================================================================
// ButtonDlg — floating "Distraction-free / Full-screen / Post-it" button
// =============================================================================

// ---------------------------------------------------------------------------
// doDialog
// ---------------------------------------------------------------------------
void ButtonDlg::doDialog(bool isRTL)
{
    if (!isCreated()) {
        create(IDD_BUTTON_DLG, isRTL);

        // Build the minimal floating UI
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground, false);
        setFixedSize(64, 40);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(4, 4, 4, 4);

        QPushButton* restoreBtn = new QPushButton(this);
        restoreBtn->setObjectName(QString::number(IDC_RESTORE_BUTTON));
        restoreBtn->setFixedSize(56, 32);
        restoreBtn->setText(QStringLiteral("⏹"));   // generic restore icon
        restoreBtn->setToolTip(QStringLiteral("Restore normal view"));
        restoreBtn->setCursor(Qt::PointingHandCursor);

        layout->addWidget(restoreBtn);
        setLayout(layout);

        // Route button click → run_dlgProc via direct call
        connect(restoreBtn, &QPushButton::clicked, this, [this]() {
            // Translate button ID to WM_COMMAND and process
            this->run_dlgProc(0x0111, IDC_RESTORE_BUTTON, 0);
        });

        if (isRTL) {
            setLayoutDirection(Qt::RightToLeft);
        }
    }

    // Apply current dark mode state
    if (NppDarkMode::instance().isEnabled()) {
        NppDarkMode::instance().applyToWidget(this);
        NppDarkMode::instance().subclassButton(findChild<QPushButton*>());
    }

    display();
}

// ---------------------------------------------------------------------------
// display — show/hide the floating button
// ---------------------------------------------------------------------------
void ButtonDlg::display(bool toShow)
{
    if (toShow) {
        show();
        raise();
        activateWindow();
    } else {
        // Win32 behaviour: show if status is not "nada", hide otherwise
        hide();
    }
}

// ---------------------------------------------------------------------------
// run_dlgProc — process WM_INITDIALOG, dark mode events, and button click
// ---------------------------------------------------------------------------
intptr_t ButtonDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t /*lParam*/)
{
    switch (message)
    {
        // WM_INITDIALOG
        case 0x1100 + 1:
        {
            if (NppDarkMode::instance().isEnabled()) {
                NppDarkMode::instance().applyToWidget(this);
                auto* btn = findChild<QPushButton*>();
                if (btn) {
                    NppDarkMode::instance().subclassButton(btn);
                }
            }
            return true;
        }

        // WM_COMMAND
        case 0x0111:
        {
            switch (wParam & 0xFFFF)   // low word = control/command ID
            {
                case IDC_RESTORE_BUTTON:
                {
                    int bs = getButtonStatus();

                    bool isDistractionFree = (bs & buttonStatus_distractionFree) != 0;
                    bool isFullScreen      = (bs & buttonStatus_fullscreen) != 0;
                    bool isPostIt          = (bs & buttonStatus_postit) != 0;

                    int cmd = 0;
                    if (isDistractionFree) {
                        cmd = IDM_VIEW_DISTRACTIONFREE;
                    } else if (isFullScreen && isPostIt) {
                        // Full-screen + post-it: remove post-it first
                        cmd = IDM_VIEW_POSTIT;
                    } else if (isFullScreen) {
                        cmd = IDM_VIEW_FULLSCREENTOGGLE;
                    } else if (isPostIt) {
                        cmd = IDM_VIEW_POSTIT;
                    }

                    if (cmd != 0) {
                        emit viewCommand(cmd);
                    }

                    display(false);
                    return true;
                }

                default:
                    return false;
            }
        }

        // WM_PRINTCLIENT — dark mode render acknowledgement
        case 0x0318:
        {
            if (NppDarkMode::instance().isEnabled()) {
                return true;
            }
            break;
        }

        // NPPM_INTERNAL_REFRESHDARKMODE — re-apply theming
        case 0x0444:
        {
            if (NppDarkMode::instance().isEnabled()) {
                NppDarkMode::instance().applyToWidget(this);
                auto* btn = findChild<QPushButton*>();
                if (btn) {
                    NppDarkMode::instance().subclassButton(btn);
                }
            }
            return true;
        }

        default:
            break;
    }

    return false;
}

// =============================================================================
// EncodingDlg — minimal encoding info display dialog
// =============================================================================

// ---------------------------------------------------------------------------
// doDialog
// ---------------------------------------------------------------------------
void EncodingDlg::doDialog(bool isRTL)
{
    if (!isCreated()) {
        create(0, isRTL);   // no Win32 resource ID — built programmatically

        setWindowTitle(QStringLiteral("Encoding"));
        setMinimumWidth(300);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(12, 12, 12, 12);

        // Encoding name label
        QLabel* encLabel = new QLabel(this);
        encLabel->setObjectName(QStringLiteral("IDC_ENCODING_LABEL"));
        encLabel->setWordWrap(true);
        mainLayout->addWidget(encLabel);

        // BOM indicator
        QCheckBox* bomCheck = new QCheckBox(QStringLiteral("BOM (Byte Order Mark)"), this);
        bomCheck->setObjectName(QStringLiteral("IDC_BOM_CHECK"));
        bomCheck->setEnabled(false);   // read-only info
        mainLayout->addWidget(bomCheck);

        mainLayout->addStretch();

        // Apply current dark mode
        if (NppDarkMode::instance().isEnabled()) {
            NppDarkMode::instance().applyToWidget(this);
        }

        if (isRTL) {
            setLayoutDirection(Qt::RightToLeft);
        }
    }

    display();
}

// ---------------------------------------------------------------------------
// setEncodingInfo — update the displayed encoding details
// ---------------------------------------------------------------------------
void EncodingDlg::setEncodingInfo(const QString& encodingName, bool hasBOM)
{
    _currentEncoding = encodingName;
    _hasBOM = hasBOM;

    if (isCreated()) {
        auto* label = findChild<QLabel*>(QStringLiteral("IDC_ENCODING_LABEL"));
        if (label) {
            label->setText(encodingName);
        }
        auto* bomCheck = findChild<QCheckBox*>(QStringLiteral("IDC_BOM_CHECK"));
        if (bomCheck) {
            bomCheck->setChecked(hasBOM);
        }
    }
}

// ---------------------------------------------------------------------------
// run_dlgProc — handle dark mode init/refresh
// ---------------------------------------------------------------------------
intptr_t EncodingDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t /*lParam*/)
{
    switch (message)
    {
        // WM_INITDIALOG
        case 0x1100 + 1:
        {
            if (NppDarkMode::instance().isEnabled()) {
                NppDarkMode::instance().applyToWidget(this);
            }
            return true;
        }

        // NPPM_INTERNAL_REFRESHDARKMODE
        case 0x0444:
        {
            if (NppDarkMode::instance().isEnabled()) {
                NppDarkMode::instance().applyToWidget(this);
            }
            return true;
        }

        default:
            break;
    }

    return false;
}
