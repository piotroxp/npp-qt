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

// Semantic Lift: Win32 asciiListView → Qt6 QTableWidget subclass for ASCII table panel

#pragma once

#include "ListView.h"
#include <QString>

// AsciiListView extends ListView with ASCII character data population.
// Displays a 256-row table with columns: Decimal, Hex, Char, HTML Name,
// HTML Decimal, HTML Hex.
class AsciiListView : public ListView
{
    Q_OBJECT

public:
    // Constructor with optional parent for Qt parent-child model
    explicit AsciiListView(QWidget* parent = nullptr);

    // Get underlying table widget for direct access
    QTableWidget* table() const { return _tableWidget; }

    // Populate table with all 256 values for the given codepage
    // codepage: Windows codepage number (0 = default, 1252 = CP1252)
    void setValues(int codepage = 0);

    // Reset table when codepage changes
    void resetValues(int codepage);

    // Map byte value 0-255 to readable name (e.g. "NUL", "TAB", "A")
    static QString getAscii(unsigned char value);

    // Map byte value to HTML named entity (e.g. "&lt;", "&euro;")
    static QString getHtmlName(unsigned char value);

    // Get HTML numeric entity code for special high chars
    static int getHtmlNumber(unsigned char value);

private:
    int _codepage = -1;
};