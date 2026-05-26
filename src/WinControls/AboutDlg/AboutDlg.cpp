// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "AboutDlg.h"

#include <QLayout>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

AboutDlg::AboutDlg()
{
    setWindowTitle("About Notepad++");
    setMinimumSize(450, 350);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Logo area - use QLabel with QPixmap
    _pLogoLabel = new QLabel(this);
    _pLogoLabel->setAlignment(Qt::AlignCenter);
    _pLogoLabel->setMinimumSize(80, 80);
    _pLogoLabel->setMaximumSize(120, 120);
    _pLogoLabel->setStyleSheet("background-color: #333333; border: 1px solid #555555;");
    mainLayout->addWidget(_pLogoLabel, 0, Qt::AlignCenter);
    
    // Version info
    _pVersionLabel = new QLabel(this);
    _pVersionLabel->setAlignment(Qt::AlignCenter);
    _pVersionLabel->setStyleSheet("font-weight: bold; font-size: 14pt;");
    mainLayout->addWidget(_pVersionLabel, 0, Qt::AlignCenter);
    
    _pBitnessLabel = new QLabel(this);
    _pBitnessLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(_pBitnessLabel, 0, Qt::AlignCenter);
    
    _pBuildTimeLabel = new QLabel(this);
    _pBuildTimeLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(_pBuildTimeLabel, 0, Qt::AlignCenter);
    
    // Home page link
    _pHomeAddrLabel = new QLabel(this);
    _pHomeAddrLabel->setAlignment(Qt::AlignCenter);
    _pHomeAddrLabel->setOpenExternalLinks(true);
    mainLayout->addWidget(_pHomeAddrLabel, 0, Qt::AlignCenter);
    
    // Licence text
    QGroupBox* licenceGroup = new QGroupBox("License", this);
    QVBoxLayout* licenceLayout = new QVBoxLayout(licenceGroup);
    
    _pLicenceEdit = new QTextEdit(this);
    _pLicenceEdit->setReadOnly(true);
    _pLicenceEdit->setPlainText(LICENCE_TXT);
    _pLicenceEdit->setMaximumHeight(120);
    licenceLayout->addWidget(_pLicenceEdit);
    
    mainLayout->addWidget(licenceGroup);
    
    // Close button
    QPushButton* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, [this]() { display(false); });
    mainLayout->addWidget(closeBtn, 0, Qt::AlignRight);
    
    refreshLogo();
}

void AboutDlg::refreshLogo()
{
    // Load logo - would normally load from resources
    QPixmap logo(80, 80);
    logo.fill(Qt::darkBlue);
    _pLogoLabel->setPixmap(logo);
}

void AboutDlg::doDialog()
{
    if (!isCreated()) {
        create(IDD_ABOUTBOX);
    }
    
    // Set version info
    QString version = "Notepad++ v8.x.x"; // Would get from NppParameters
    _pVersionLabel->setText(version);
    _pBitnessLabel->setText("(32-bit)"); // or "(64-bit)"
    
    QString buildTime = QString("Build time: %1 - %2")
        .arg(__DATE__)
        .arg(__TIME__);
    _pBuildTimeLabel->setText(buildTime);
    
    _pHomeAddrLabel->setText("<a href='https://notepad-plus-plus.org/'>https://notepad-plus-plus.org/</a>");
    
    display(true);
}

void AboutDlg::destroy()
{
    //_emailLink.destroy();
    // Nothing to clean up - pixmap is auto-managed
}

intptr_t AboutDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);
    
    switch (message) {
        case WM_INITDIALOG:
            // Apply dark mode styling if enabled
            //NppDarkMode::autoSubclassAndThemeChildControls(_hSelf);
            return TRUE;
            
        case WM_COMMAND:
            switch (wParam) {
                case IDCANCEL:
                case IDOK:
                    display(false);
                    return TRUE;
            }
            break;
            
        case WM_DESTROY:
            destroy();
            return TRUE;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}

// =====================================================
// DebugInfoDlg implementation
// =====================================================

void DebugInfoDlg::init(QWidget* parent, bool isAdmin, const QString& loadedPlugins)
{
    StaticDialog::init(parent);
    _isAdmin = isAdmin;
    _loadedPlugins = loadedPlugins;
}

void DebugInfoDlg::doDialog()
{
    if (!isCreated()) {
        create(0); // No specific dialog ID needed
        setWindowTitle("Debug Info");
    }
    refreshDebugInfo();
    display(true);
}

void DebugInfoDlg::refreshDebugInfo()
{
    QString info;
    
    // Notepad++ version
    info += "Notepad++ version: " + QString(NOTEPAD_PLUS_VERSION) + "\n";
    info += "Build time: " + QString(__DATE__) + " - " + QString(__TIME__) + "\n\n";
    
    // Scintilla version
    info += "Scintilla/Lexilla included: " + QString(NPP_SCINTILLA_VERSION) + "/" + QString(NPP_LEXILLA_VERSION) + "\n\n";
    
    // Plugin info
    if (!_loadedPlugins.isEmpty()) {
        info += "Loaded plugins:\n" + _loadedPlugins + "\n\n";
    }
    
    // Admin mode
    info += QString("Administrator mode: %1\n").arg(_isAdmin ? "Yes" : "No");
    
    _debugInfoStr = info;
    _debugInfoDisplay = info;
    
    if (_pDebugInfoEdit) {
        _pDebugInfoEdit->setPlainText(_debugInfoDisplay);
    }
}

intptr_t DebugInfoDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            QVBoxLayout* layout = new QVBoxLayout(this);
            
            _pDebugInfoEdit = new QTextEdit(this);
            _pDebugInfoEdit->setReadOnly(true);
            _pDebugInfoEdit->setFont(QFont("Courier New", 8));
            layout->addWidget(_pDebugInfoEdit);
            
            QPushButton* copyBtn = new QPushButton("Copy to Clipboard", this);
            connect(copyBtn, &QPushButton::clicked, this, [this]() {
                QApplication::clipboard()->setText(_debugInfoDisplay);
                QMessageBox::information(this, "Copied", "Debug info copied to clipboard.");
            });
            layout->addWidget(copyBtn);
            
            QPushButton* closeBtn = new QPushButton("Close", this);
            connect(closeBtn, &QPushButton::clicked, this, [this]() { display(false); });
            layout->addWidget(closeBtn);
            
            refreshDebugInfo();
            return TRUE;
        }
        
        case WM_COMMAND:
            if (wParam == IDOK || wParam == IDCANCEL) {
                display(false);
                return TRUE;
            }
            break;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}

// =====================================================
// CmdLineArgsDlg implementation
// =====================================================

void CmdLineArgsDlg::doDialog()
{
    if (!isCreated()) {
        create(0);
        setWindowTitle("Command Line Arguments");
    }
    display(true);
}

intptr_t CmdLineArgsDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            QVBoxLayout* layout = new QVBoxLayout(this);
            
            QLabel* label = new QLabel("Enter command line arguments:", this);
            layout->addWidget(label);
            
            _pArgsEdit = new QTextEdit(this);
            _pArgsEdit->setFont(QFont(_fontFamily, _fontSize));
            _pArgsEdit->setAcceptDrops(false);
            layout->addWidget(_pArgsEdit);
            
            QHBoxLayout* btnLayout = new QHBoxLayout();
            btnLayout->addStretch();
            
            QPushButton* okBtn = new QPushButton("OK", this);
            connect(okBtn, &QPushButton::clicked, this, [this]() { display(false); });
            btnLayout->addWidget(okBtn);
            
            QPushButton* cancelBtn = new QPushButton("Cancel", this);
            connect(cancelBtn, &QPushButton::clicked, this, [this]() { display(false); });
            btnLayout->addWidget(cancelBtn);
            
            layout->addLayout(btnLayout);
            return TRUE;
        }
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}

// =====================================================
// DoSaveOrNotBox implementation
// =====================================================

void DoSaveOrNotBox::init(QWidget* parent, const QString& filename, bool isMulti)
{
    StaticDialog::init(parent);
    _filename = filename;
    _isMulti = isMulti;
}

void DoSaveOrNotBox::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    
    if (!isCreated()) {
        create(0);
    }
    
    QString msg;
    if (_isMulti) {
        msg = "Do you want to save changes to multiple files?";
    } else {
        msg = QString("Do you want to save changes to \"%1\"?").arg(_filename);
    }
    
    if (_pMessageLabel) {
        _pMessageLabel->setText(msg);
    }
    
    display(true);
}

void DoSaveOrNotBox::changeLang()
{
    // Would update button text based on localization
}

intptr_t DoSaveOrNotBox::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            QVBoxLayout* layout = new QVBoxLayout(this);
            layout->setContentsMargins(20, 20, 20, 20);
            
            _pMessageLabel = new QLabel(this);
            _pMessageLabel->setWordWrap(true);
            layout->addWidget(_pMessageLabel);
            
            QHBoxLayout* btnLayout = new QHBoxLayout();
            btnLayout->addStretch();
            
            _pYesButton = new QPushButton("&Yes", this);
            connect(_pYesButton, &QPushButton::clicked, this, [this]() {
                _clickedButtonId = IDYES;
                display(false);
            });
            btnLayout->addWidget(_pYesButton);
            
            _pNoButton = new QPushButton("&No", this);
            connect(_pNoButton, &QPushButton::clicked, this, [this]() {
                _clickedButtonId = IDNO;
                display(false);
            });
            btnLayout->addWidget(_pNoButton);
            
            QPushButton* cancelBtn = new QPushButton("Cancel", this);
            connect(cancelBtn, &QPushButton::clicked, this, [this]() {
                _clickedButtonId = IDCANCEL;
                display(false);
            });
            btnLayout->addWidget(cancelBtn);
            
            layout->addLayout(btnLayout);
            
            changeLang();
            return TRUE;
        }
        
        case WM_COMMAND:
            if (wParam == IDYES || wParam == IDNO || wParam == IDCANCEL) {
                _clickedButtonId = static_cast<int>(wParam);
                display(false);
                return TRUE;
            }
            break;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}

// =====================================================
// DoSaveAllBox implementation
// =====================================================

void DoSaveAllBox::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
    
    if (!isCreated()) {
        create(0);
    }
    
    if (_pMessageLabel) {
        _pMessageLabel->setText(QString("You have %1 files with unsaved changes.\nDo you want to save them?").arg(_unsavedCount));
    }
    
    display(true);
}

void DoSaveAllBox::changeLang()
{
    // Would update button text based on localization
}

intptr_t DoSaveAllBox::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            QVBoxLayout* layout = new QVBoxLayout(this);
            layout->setContentsMargins(20, 20, 20, 20);
            
            _pMessageLabel = new QLabel(this);
            _pMessageLabel->setWordWrap(true);
            layout->addWidget(_pMessageLabel);
            
            QHBoxLayout* btnLayout = new QHBoxLayout();
            btnLayout->addStretch();
            
            _pYesButton = new QPushButton("Save All", this);
            connect(_pYesButton, &QPushButton::clicked, this, [this]() {
                _clickedButtonId = IDYES;
                display(false);
            });
            btnLayout->addWidget(_pYesButton);
            
            _pNoButton = new QPushButton("Don't Save", this);
            connect(_pNoButton, &QPushButton::clicked, this, [this]() {
                _clickedButtonId = IDNO;
                display(false);
            });
            btnLayout->addWidget(_pNoButton);
            
            QPushButton* cancelBtn = new QPushButton("Cancel", this);
            connect(cancelBtn, &QPushButton::clicked, this, [this]() {
                _clickedButtonId = IDCANCEL;
                display(false);
            });
            btnLayout->addWidget(cancelBtn);
            
            layout->addLayout(btnLayout);
            
            changeLang();
            return TRUE;
        }
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}