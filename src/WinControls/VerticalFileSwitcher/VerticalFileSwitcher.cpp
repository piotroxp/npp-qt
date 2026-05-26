// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warrant...
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "VerticalFileSwitcher.h"

#include <QInputDialog>
#include <QKeyEvent>

// Note: Comment was truncated, proceeding with implementation

VerticalFileSwitcher::VerticalFileSwitcher()
    : StaticDialog()
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setObjectName("VerticalFileSwitcher");
}

void VerticalFileSwitcher::init(QWidget* parent)
{
    StaticDialog::init(parent);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
    
    // Label
    QLabel* titleLabel = new QLabel("Opened Files", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // File list with vertical layout
    _pFileList = new QListWidget(this);
    _pFileList->setSelectionMode(QAbstractItemView::SingleSelection);
    _pFileList->setSortingEnabled(false);
    mainLayout->addWidget(_pFileList);
    
    // Warning: this may take some memory
    connect(_pFileList, &QListWidget::itemDoubleClicked, this, [this]() {
        _currentIndex = _pFileList->currentRow();
        selectEntry();
    });
}

void VerticalFileSwitcher::destroy()
{
    _entryStrings.clear();
    if (_pFileList) {
        _pFileList->clear();
    }
}

void VerticalFileSwitcher::addEntry(const QString& entryText)
{
    _entryStrings.append(entryText);
    if (_pFileList) {
        _pFileList->addItem(entryText);
    }
}

void VerticalFileSwitcher::removeCurrentEntry()
{
    if (_currentIndex < _entryStrings.size()) {
        _entryStrings.remove(_currentIndex);
        if (_pFileList) {
            delete _pFileList->takeItem(_currentIndex);
        }
        
        if (_currentIndex >= _entryStrings.size() && _currentIndex > 0) {
            _currentIndex = _entryStrings.size() - 1;
        }
    }
}

void VerticalFileSwitcher::switchDown()
{
    if (_currentIndex + 1 >= _entryStrings.size()) {
        // Wrap around - go to beginning
        _previousIndex = _currentIndex;
        _currentIndex = 0;
        
        if (_pFileList) {
            _pFileList->setCurrentRow(0);
        }
        
        emit verticalFileSwitcherWantsToChange(SwitcherDirection_down);
    } else {
        ++_currentIndex;
        
        if (_pFileList) {
            _pFileList->setCurrentRow(_currentIndex);
        }
        
        emit verticalFileSwitcherWantsToChange(-1);
    }
    
    selectEntry();
}

void VerticalFileSwitcher::switchUp()
{
    if (_currentIndex == 0) {
        // Wrap around going to end
        _previousIndex = _currentIndex;
        _currentIndex = _entryStrings.size() - 1;
        
        if (_pFileList) {
            _pFileList->setCurrentRow(_currentIndex);
        }
        
        emit verticalFileSwitcherWantsToChange(SwitcherDirection_up);
    } else {
        --_currentIndex;
        
        if (_pFileList) {
            _pFileList->setCurrentRow(_currentIndex);
        }
        
        emit verticalFileSwitcherWantsToChange(-1);
    }
    
    selectEntry();
}

void VerticalFileSwitcher::removeAllEntries()
{
    _entryStrings.clear();
    _currentIndex = 0;
    
    if (_pFileList) {
        _pFileList->clear();
    }
}

void VerticalFileSwitcher::selectEntry()
{
    emit closing(_currentIndex);
    hide();
}

void VerticalFileSwitcher::updateList()
{
    if (_pFileList) {
        _pFileList->blockSignals(true);
        _pFileList->clear();
        
        for (const QString& entry : _entryStrings) {
            _pFileList->addItem(entry);
        }
        
        _pFileList->setCurrentRow(_currentIndex);
        _pFileList->blockSignals(false);
    }
}

intptr_t VerticalFileSwitcher::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            updateList();
            return TRUE;
            
        case WM_COMMAND:
            switch (wParam) {
                case IDOK: {
                    close();
                    selectEntry();
                    return TRUE;
                }
                case IDCANCEL: {
                    // Cancel goes back to previous file
                    emit closing(_previousIndex);
                    hide();
                    return TRUE;
                }
            }
            break;
            
        case WM_ACTIVATE:
            if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) {
                selectEntry();
            }
            break;
    }
    
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}