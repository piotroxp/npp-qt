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
#include <QSizeGrip>

#include "../StaticDialog/StaticDialog.h"

// SizeableDlg - Sizeable dialog base with resize grip
class SizeableDlg : public StaticDialog
{
    Q_OBJECT

public:
    SizeableDlg();
    ~SizeableDlg() override = default;

    void setInfo(int idd, const QString& info) {
        _dlgID = idd;
        _info = info;
    }

    bool isLargeMode() const { return _isLargeMode; }

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private:
    int _dlgID = 0;
    QString _info;
    bool _isLargeMode = false;
    
    QSizeGrip* _pSizeGrip = nullptr;
};