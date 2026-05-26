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

#include "ListView.h"

#include <QHeaderView>
#include <QTableWidgetItem>

void ListView::addLine(const QStringList& values2Add, intptr_t lParam, int pos2insert)
{
    if (values2Add.isEmpty())
        return;

    int row = (pos2insert == -1) ? rowCount() : pos2insert;
    insertRow(row);

    // Set first column with lParam
    auto* firstItem = new QTableWidgetItem(values2Add.value(0));
    firstItem->setData(Qt::UserRole, QVariant(static_cast<qlonglong>(lParam)));
    setItem(row, 0, firstItem);

    // Set additional columns
    for (int col = 1; col < values2Add.size(); ++col) {
        setItem(row, col, new QTableWidgetItem(values2Add.value(col)));
    }
}

size_t ListView::findAlphabeticalOrderPos(const QString& string2Cmp, SortDirection sortDir)
{
    const int itemCount = rowCount();
    if (!itemCount)
        return 0;

    for (int i = 0; i < itemCount; ++i) {
        auto* item = this->item(i, 0);
        if (!item) continue;

        const QString& str = item->text();
        int res = QString::compare(string2Cmp, str, Qt::CaseInsensitive);

        if (res < 0) { // string2Cmp < str
            if (sortDir == SortIncrease) {
                return static_cast<size_t>(i);
            }
        } else { // str2Cmp >= str
            if (sortDir == SortDecrease) {
                return static_cast<size_t>(i);
            }
        }
    }
    return static_cast<size_t>(itemCount);
}

// =====================================================
// AsciiListView implementation
// =====================================================

void AsciiListView::setValues(int codepage)
{
    _codepage = codepage;
    resetValues(codepage);
}

void AsciiListView::resetValues(int codepage)
{
    Q_UNUSED(codepage);
    setRowCount(0);

    // Add all 256 ASCII characters
    for (unsigned char c = 0; c < 255; ++c) {
        QStringList values;
        values << getAscii(c)                           // Value (decimal)
               << QString("0x%1").arg(c, 2, 16, QChar('0')).toUpper()  // Hex
               << getAscii(c)                           // Character
               << getHtmlName(c)                        // HTML Name
               << QString::number(getHtmlNumber(c))    // HTML Decimal
               << QString("&#x%1;").arg(c, 2, 16, QChar('0')).toUpper(); // HTML Hex

        addLine(values, c);
    }
}

QString AsciiListView::getAscii(unsigned char value)
{
    if (value < 32 || value == 127) {
        return ".";
    }
    return QString(QChar(value));
}

QString AsciiListView::getHtmlName(unsigned char value)
{
    // Common HTML entity names
    static const QMap<int, QString> htmlNames = {
        {32, "SP"}, {34, "QUOT"}, {38, "AMP"}, {39, "APOS"},
        {60, "LT"}, {62, "GT"}, {160, "NBSP"}, {161, "IEXCL"},
        {162, "CENT"}, {163, "POUND"}, {164, "CURREN"}, {165, "YEN"},
        {166, "BRVBAR"}, {167, "SECT"}, {168, "DIACR"}, {169, "COPY"},
        {170, "ORDF"}, {171, "LAQUO"}, {172, "NOT"}, {173, "SHY"},
        {174, "REG"}, {175, "MACR"}, {176, "DEG"}, {177, "PLUSMN"},
        {178, "SUP2"}, {179, "SUP3"}, {180, "ACUTE"}, {181, "MICRO"},
        {182, "PARA"}, {183, "MIDDOT"}, {184, "CEDIL"}, {185, "SUP1"},
        {186, "ORDM"}, {187, "RAQUO"}, {188, "FRAC14"}, {189, "FRAC12"},
        {190, "FRAC34"}, {191, "IQUEST"}, {192, "AGRAVE"}, {193, "AACUTE"},
        {194, "ACIRC"}, {195, "ATILDE"}, {196, "AUML"}, {197, "ARING"},
        {198, "AELIG"}, {199, "CCEDIL"}, {200, "EGRAVE"}, {201, "EACUTE"},
        {202, "ECIRC"}, {203, "EUML"}, {204, "IGRAVE"}, {205, "IACUTE"},
        {206, "ICIRC"}, {207, "IUML"}, {208, "ETH"}, {209, "NTILDE"},
        {210, "OGRAVE"}, {211, "OACUTE"}, {212, "OCIRC"}, {213, "OTILDE"},
        {214, "OUML"}, {215, "TIMES"}, {216, "OSLASH"}, {217, "UGRAVE"},
        {218, "UACUTE"}, {219, "UCIRC"}, {220, "UUML"}, {221, "YACUTE"},
        {222, "THORN"}, {223, "SZLIG"}, {224, "AGRAVE"}, {225, "AACUTE"},
        {226, "ACIRC"}, {227, "ATILDE"}, {228, "AUML"}, {229, "ARING"},
        {230, "AELIG"}, {231, "CCEDIL"}, {232, "EGRAVE"}, {233, "EACUTE"},
        {234, "ECIRC"}, {235, "EUML"}, {236, "IGRAVE"}, {237, "IACUTE"},
        {238, "ICIRC"}, {239, "IUML"}, {240, "ETH"}, {241, "NTILDE"},
        {242, "OGRAVE"}, {243, "OACUTE"}, {244, "OCIRC"}, {245, "OTILDE"},
        {246, "OUML"}, {247, "DIVIDE"}, {248, "OSLASH"}, {249, "UGRAVE"},
        {250, "UACUTE"}, {251, "UCIRC"}, {252, "UUML"}, {253, "YACUTE"},
        {254, "THORN"}, {255, "YUML"}
    };

    if (htmlNames.contains(value)) {
        return "&" + htmlNames[value] + ";";
    }
    return "";
}

int AsciiListView::getHtmlNumber(unsigned char value)
{
    return static_cast<int>(value);
}