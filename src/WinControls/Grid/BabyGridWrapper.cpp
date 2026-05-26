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

#include "BabyGridWrapper.h"

void BabyGridWrapper::init(QWidget* parent, int16_t id)
{
    Window::init(parent);
    
    // Create BabyGrid as child widget
    _babyGrid = new BabyGrid(parent);
    _babyGrid->setObjectName(QString("BabyGrid_%1").arg(id));
}

void BabyGridWrapper::destroy()
{
    if (_babyGrid) {
        _babyGrid->deleteLater();
        _babyGrid = nullptr;
    }
}

void BabyGridWrapper::setLineColNumber(size_t nbRow, size_t nbCol)
{
    if (_babyGrid) {
        _babyGrid->setGridDimensions(static_cast<int>(nbRow), static_cast<int>(nbCol));
    }
}

void BabyGridWrapper::setText(size_t row, size_t col, const QString& text)
{
    if (_babyGrid) {
        _babyGrid->setCellText(static_cast<int>(row), static_cast<int>(col), text);
    }
}

void BabyGridWrapper::setCursorColour(QRgb colour)
{
    if (_babyGrid) {
        _babyGrid->setCursorColor(colour);
    }
}

void BabyGridWrapper::setTextColor(const QRgb color) const
{
    if (_babyGrid) {
        _babyGrid->setTextColor(color);
    }
}

void BabyGridWrapper::setBackgroundColor(const QRgb color) const
{
    if (_babyGrid) {
        _babyGrid->setBackgroundColor(color);
    }
}

void BabyGridWrapper::setTitleColor(const QRgb color) const
{
    if (_babyGrid) {
        _babyGrid->setTitleColor(color);
    }
}

void BabyGridWrapper::setTitleTextColor(const QRgb color) const
{
    if (_babyGrid) {
        _babyGrid->setTitleTextColor(color);
    }
}

void BabyGridWrapper::setHighlightColor(const QRgb color) const
{
    if (_babyGrid) {
        _babyGrid->setHighlightColor(color);
    }
}

void BabyGridWrapper::setHighlightTextColor(const QRgb color) const
{
    if (_babyGrid) {
        _babyGrid->setHighlightTextColor(color);
    }
}

void BabyGridWrapper::setGridlinesColor(const QRgb color) const
{
    if (_babyGrid) {
        _babyGrid->setGridLineColor(color);
    }
}

int BabyGridWrapper::getSelectedRow()
{
    if (_babyGrid) {
        return _babyGrid->getSelectedRow();
    }
    return -1;
}

void BabyGridWrapper::ensureVisible(int row) const
{
    if (_babyGrid) {
        _babyGrid->scrollToItem(_babyGrid->item(row, 0));
    }
}

void BabyGridWrapper::setSelection(int row) const
{
    if (_babyGrid) {
        _babyGrid->setCurrentCell(row, 0);
    }
}

void BabyGridWrapper::setColWidth(unsigned int col, unsigned int width)
{
    if (_babyGrid) {
        _babyGrid->setColWidth(static_cast<int>(col), static_cast<int>(width));
    }
}

void BabyGridWrapper::makeColAutoWidth(bool autoWidth)
{
    if (_babyGrid) {
        _babyGrid->makeColAutoWidth(autoWidth);
    }
}

void BabyGridWrapper::clear()
{
    if (_babyGrid) {
        _babyGrid->clear();
    }
}

void BabyGridWrapper::updateView() const
{
    if (_babyGrid) {
        _babyGrid->viewport()->update();
    }
}

int BabyGridWrapper::getNumberRows() const
{
    if (_babyGrid) {
        return _babyGrid->getRows();
    }
    return 0;
}

int BabyGridWrapper::getHomeRow() const
{
    if (_babyGrid) {
        return _babyGrid->getHomeRow();
    }
    return 1;
}

void BabyGridWrapper::setLastView(size_t homeRow, size_t cursorRow) const
{
    if (_babyGrid) {
        _babyGrid->setLastView(homeRow, cursorRow);
    }
}