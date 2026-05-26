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

#include <QWidget>
#include <QListWidget>
#include <QVector>
#include <QString>

#include "../DockingDlgInterface/DockingDlgInterface.h"

// Resource IDs
#define IDD_ANSIASCII_PANEL 2700
#define IDC_LIST_ANSICHAR (IDD_ANSIASCII_PANEL + 1)

// Forward declarations
class ScintillaEditView;

#define AI_PROJECTPANELTITLE "ASCII Codes Insertion Panel"

// AnsiCharPanel - ASCII/ANSI character insertion panel
// Displays a grid of characters for easy insertion into documents
class AnsiCharPanel : public DockingDlgInterface
{
    Q_OBJECT

public:
    AnsiCharPanel();
    ~AnsiCharPanel() override = default;

    void init(QWidget* parent, ScintillaEditView** ppEditView);
    void setParent(QWidget* parent2set);

    void switchEncoding();
    void insertChar(unsigned char char2insert) const;
    void insertString(const QString& string2insert) const;

    void setBackgroundColor(QRgb bgColour) override;
    void setForegroundColor(QRgb fgColour) override;

signals:
    void insertCharacter(QChar ch);
    void insertText(const QString& text);

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onItemClicked(QListWidgetItem* item);

private:
    void setupCharGrid();
    void updateCharDisplay();

    ScintillaEditView** _ppEditView = nullptr;
    
    QListWidget* _pCharList = nullptr;
    QButtonGroup* _pCharButtons = nullptr;
    QWidget* _pCharGrid = nullptr;
    
    int _currentCodepage = 0;
    QRgb _backgroundColor = 0xFFFFFFFF;
    QRgb _foregroundColor = 0xFF000000;
};