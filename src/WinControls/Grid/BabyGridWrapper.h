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
#include "../Window/Window.h"
#include "BabyGrid.h"

// BabyGridWrapper - Wrapper for BabyGrid control
class BabyGridWrapper : public Window
{
    Q_OBJECT

public:
    BabyGridWrapper() = default;
    ~BabyGridWrapper() override = default;

    void init(QWidget* parent, int16_t id);
    void destroy() override;

    // Grid dimension control
    void setLineColNumber(size_t nbRow, size_t nbCol);
    
    // Cell text
    void setText(size_t row, size_t col, const QString& text);
    
    // Colors
    void setCursorColour(QRgb colour);
    void setTextColor(const QRgb color) const;
    void setBackgroundColor(const QRgb color) const;
    void setTitleColor(const QRgb color) const;
    void setTitleTextColor(const QRgb color) const;
    void setHighlightColor(const QRgb color) const;
    void setHighlightTextColor(const QRgb color) const;
    void setGridlinesColor(const QRgb color) const;
    
    // Selection and navigation
    int getSelectedRow();
    void ensureVisible(int row) const;
    void setSelection(int row) const;
    
    // Layout
    void setColWidth(unsigned int col, unsigned int width);
    void makeColAutoWidth(bool autoWidth = true);
    
    // Grid content
    void clear();
    void updateView() const;
    
    // State
    int getNumberRows() const;
    int getHomeRow() const;
    void setLastView(size_t homeRow, size_t cursorRow) const;

private:
    BabyGrid* _babyGrid = nullptr;
};