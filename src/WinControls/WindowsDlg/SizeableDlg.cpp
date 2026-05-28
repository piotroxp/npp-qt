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

#include "SizeableDlg.h"

// Win32 macro stubs for Linux Qt6 port
#ifndef WM_INITDIALOG
#define WM_INITDIALOG 0x0110
#endif
#ifndef WM_SIZE
#define WM_SIZE 0x0005
#endif
#ifndef WM_COMMAND
#define WM_COMMAND 0x0111
#endif
#ifndef WM_NOTIFY
#define WM_NOTIFY 0x004E
#endif
#ifndef WM_DESTROY
#define WM_DESTROY 0x0002
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef IDOK
#define IDOK 1
#endif
#ifndef IDCANCEL
#define IDCANCEL 2
#endif
#ifndef LOWORD
#define LOWORD(l) ((uint16_t)((uintptr_t)(l) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(l) ((uint16_t)(((uintptr_t)(l) >> 16) & 0xFFFF))
#endif

#include <QVBoxLayout>

SizeableDlg::SizeableDlg()
    : StaticDialog()
{
    // Allow resizing
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(200, 150);
}

intptr_t SizeableDlg::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            // Add size grip in bottom-right corner
            _pSizeGrip = new QSizeGrip(this);
            
            // Get minimum size for initial sizing
            resize(400, 300);
            return TRUE;
        }
        
        case WM_COMMAND:
            switch (wParam) {
                case IDOK:
                    accept();
                    return TRUE;
                    
                case IDCANCEL:
                    reject();
                    return TRUE;
            }
            break;
        
        default:
            break;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}