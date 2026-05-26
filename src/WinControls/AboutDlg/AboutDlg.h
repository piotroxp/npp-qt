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

#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QVersionNumber>

#include "../StaticDialog/StaticDialog.h"
#include "URLCtrl.h"

// About dialog - shows version info and logo
class AboutDlg : public StaticDialog
{
    Q_OBJECT

public:
    AboutDlg() = default;

    void doDialog();
    void refreshLogo();
    void destroy() override;

    static constexpr const char* LICENCE_TXT = R"(
This program is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License 
as published by the Free Software Foundation; either 
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with this program. If not, see <https://www.gnu.org/licenses/>.
)";

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private:
    //URLCtrl _emailLink;
    URLCtrl _pageLink;
    QPixmap _logoPixmap;
    
    QLabel* _pVersionLabel = nullptr;
    QLabel* _pBitnessLabel = nullptr;
    QLabel* _pBuildTimeLabel = nullptr;
    QLabel* _pLogoLabel = nullptr;
    QTextEdit* _pLicenceEdit = nullptr;
    QLabel* _pHomeAddrLabel = nullptr;
};

// Debug info dialog - shows system and plugin information
class DebugInfoDlg : public StaticDialog
{
    Q_OBJECT

public:
    DebugInfoDlg() = default;

    void init(QWidget* parent, bool isAdmin, const QString& loadedPlugins);
    void doDialog();
    void refreshDebugInfo();
    void destroy() override {}

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private:
    QString _debugInfoStr;
    QString _debugInfoDisplay;
    bool _isAdmin = false;
    QString _loadedPlugins;
    
    QTextEdit* _pDebugInfoEdit = nullptr;
    QPushButton* _pCopyButton = nullptr;
};

// Command line arguments dialog
class CmdLineArgsDlg : public StaticDialog
{
    Q_OBJECT
    Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily)
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize)

public:
    CmdLineArgsDlg() = default;
    void doDialog();
    void destroy() override {}

    QString fontFamily() const { return _fontFamily; }
    void setFontFamily(const QString& f) { _fontFamily = f; }
    int fontSize() const { return _fontSize; }
    void setFontSize(int s) { _fontSize = s; }

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private:
    QString _fontFamily = "Courier New";
    int _fontSize = 8;
    QFont _cmdLineEditFont;
    
    QTextEdit* _pArgsEdit = nullptr;
};

// "Do you want to save?" dialog for single file
class DoSaveOrNotBox : public StaticDialog
{
    Q_OBJECT

public:
    DoSaveOrNotBox() = default;

    void init(QWidget* parent, const QString& filename, bool isMulti);
    void doDialog(bool isRTL = false);
    void destroy() override {}

    int getClickedButtonId() const { return _clickedButtonId; }
    void changeLang();

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private:
    int _clickedButtonId = -1;
    QString _filename;
    bool _isMulti = false;
    
    QLabel* _pMessageLabel = nullptr;
    QPushButton* _pYesButton = nullptr;
    QPushButton* _pNoButton = nullptr;
    QPushButton* _pCancelButton = nullptr;
};

// "Do you want to save?" dialog for multiple files
class DoSaveAllBox : public StaticDialog
{
    Q_OBJECT

public:
    DoSaveAllBox() = default;
    void doDialog(bool isRTL = false);
    void destroy() override {}

    int getClickedButtonId() const { return _clickedButtonId; }
    void changeLang();

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private:
    int _clickedButtonId = -1;
    int _unsavedCount = 0;
    
    QLabel* _pMessageLabel = nullptr;
    QPushButton* _pYesButton = nullptr;
    QPushButton* _pNoButton = nullptr;
    QPushButton* _pCancelButton = nullptr;
};