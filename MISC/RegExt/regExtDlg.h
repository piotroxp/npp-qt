// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "regExtDlgRc.h"
#include "WinControls/StaticDialog.h"

#include <QString>

constexpr int extNameLen = 32;

class RegExtDlg : public StaticDialog
{
    Q_OBJECT

public:
    RegExtDlg() = default;
    ~RegExtDlg() override = default;

    void doDialog(bool isRTL = false);

public slots:
    void onAddFromLangExtClicked();
    void onRemoveExtClicked();
    void onLangListCurrentRowChanged(int row);
    void onLangExtListCurrentRowChanged(int row);
    void onRegisteredExtsListCurrentRowChanged(int row);
    void onCustomExtTextChanged(const QString& text);
    void onAccepted();

signals:
    void extensionAdded(const QString& ext);
    void extensionRemoved(const QString& ext);

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private:
    void populateRegisteredExts();
    void populateDefSupportedExts();
    void addExt(const QString& ext);
    bool deleteExts(const QString& ext2Delete);
    void writeNppPath();

    // On Unix, file associations are managed via .desktop files in
    // ~/.local/share/applications/ or via the MIME-type database.
    // These functions are stubs on non-Windows — the Win32 registry calls
    // are replaced with QSettings-backed storage.
    int getNbSubKey(const QString& keyPath) const;
    int getNbSubValue(const QString& keyPath) const;

    bool _isCustomize = false;

    // Language extension data (identical to Win32 defExtArray)
    static const int nbSupportedLang = 10;
    static const int nbExtMax = 28;
    static const int extNameMax = 18;

    static const QString defExtArray[nbSupportedLang][nbExtMax];
};
