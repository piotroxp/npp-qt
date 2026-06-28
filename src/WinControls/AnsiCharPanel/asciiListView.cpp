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

// Semantic Lift: asciiListView.cpp — AsciiChar panel with codepage-aware character data
// Inherits from ListView (Qt6 QTableWidget), implements ASCII table display.

#include "asciiListView.h"
#include "ListView.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextCodec>
#include <QString>
#include <QVector>

// Map ASCII value to readable name string
QString AsciiListView::getAscii(unsigned char value)
{
    switch (value) {
        case 0:   return QStringLiteral("NULL");
        case 1:   return QStringLiteral("SOH");
        case 2:   return QStringLiteral("STX");
        case 3:   return QStringLiteral("ETX");
        case 4:   return QStringLiteral("EOT");
        case 5:   return QStringLiteral("ENQ");
        case 6:   return QStringLiteral("ACK");
        case 7:   return QStringLiteral("BEL");
        case 8:   return QStringLiteral("BS");
        case 9:   return QStringLiteral("TAB");
        case 10:  return QStringLiteral("LF");
        case 11:  return QStringLiteral("VT");
        case 12:  return QStringLiteral("FF");
        case 13:  return QStringLiteral("CR");
        case 14:  return QStringLiteral("SO");
        case 15:  return QStringLiteral("SI");
        case 16:  return QStringLiteral("DLE");
        case 17:  return QStringLiteral("DC1");
        case 18:  return QStringLiteral("DC2");
        case 19:  return QStringLiteral("DC3");
        case 20:  return QStringLiteral("DC4");
        case 21:  return QStringLiteral("NAK");
        case 22:  return QStringLiteral("SYN");
        case 23:  return QStringLiteral("ETB");
        case 24:  return QStringLiteral("CAN");
        case 25:  return QStringLiteral("EM");
        case 26:  return QStringLiteral("SUB");
        case 27:  return QStringLiteral("ESC");
        case 28:  return QStringLiteral("FS");
        case 29:  return QStringLiteral("GS");
        case 30:  return QStringLiteral("RS");
        case 31:  return QStringLiteral("US");
        case 32:  return QStringLiteral("Space");
        case 127: return QStringLiteral("DEL");
        default: {
            // Use the current codepage to decode the byte
            QTextCodec* codec = QTextCodec::codecForMib(_codepage > 0 ? _codepage : 4); // 4 = Latin1
            if (codec) {
                QByteArray bytes;
                bytes.append(static_cast<char>(value));
                return codec->toUnicode(bytes);
            }
            // Fallback: try Latin1
            return QString(QChar::fromLatin1(static_cast<QChar::ucs4char>(value)));
        }
    }
}

// Map ASCII value to HTML entity name (HTML 4 named entities subset)
QString AsciiListView::getHtmlName(unsigned char value)
{
    switch (value) {
        case 33:  return QStringLiteral("&excl;");
        case 34:  return QStringLiteral("&quot;");
        case 35:  return QStringLiteral("&num;");
        case 36:  return QStringLiteral("&dollar;");
        case 37:  return QStringLiteral("&percnt;");
        case 38:  return QStringLiteral("&amp;");
        case 39:  return QStringLiteral("&apos;");
        case 40:  return QStringLiteral("&lpar;");
        case 41:  return QStringLiteral("&rpar;");
        case 42:  return QStringLiteral("&ast;");
        case 43:  return QStringLiteral("&plus;");
        case 44:  return QStringLiteral("&comma;");
        case 45:  return QStringLiteral("&minus;");
        case 46:  return QStringLiteral("&period;");
        case 47:  return QStringLiteral("&sol;");
        case 58:  return QStringLiteral("&colon;");
        case 59:  return QStringLiteral("&semi;");
        case 60:  return QStringLiteral("&lt;");
        case 61:  return QStringLiteral("&equals;");
        case 62:  return QStringLiteral("&gt;");
        case 63:  return QStringLiteral("&quest;");
        case 64:  return QStringLiteral("&commat;");
        case 91:  return QStringLiteral("&lbrack;");
        case 92:  return QStringLiteral("&bsol;");
        case 93:  return QStringLiteral("&rbrack;");
        case 94:  return QStringLiteral("&Hat;");
        case 95:  return QStringLiteral("&lowbar;");
        case 96:  return QStringLiteral("&grave;");
        case 123: return QStringLiteral("&lbrace;");
        case 124: return QStringLiteral("&vert;");
        case 125: return QStringLiteral("&rbrace;");
        case 126: return QString(); // ascii tilde
        case 128: return QStringLiteral("&euro;");
        case 130: return QStringLiteral("&sbquo;");
        case 131: return QStringLiteral("&fnof;");
        case 132: return QStringLiteral("&bdquo;");
        case 133: return QStringLiteral("&hellip;");
        case 134: return QStringLiteral("&dagger;");
        case 135: return QStringLiteral("&Dagger;");
        case 136: return QStringLiteral("&circ;");
        case 137: return QStringLiteral("&permil;");
        case 138: return QStringLiteral("&Scaron;");
        case 139: return QStringLiteral("&lsaquo;");
        case 140: return QStringLiteral("&OElig;");
        case 142: return QStringLiteral("&Zcaron;");
        case 145: return QStringLiteral("&lsquo;");
        case 146: return QStringLiteral("&rsquo;");
        case 147: return QStringLiteral("&ldquo;");
        case 148: return QStringLiteral("&rdquo;");
        case 149: return QStringLiteral("&bull;");
        case 150: return QStringLiteral("&ndash;");
        case 151: return QStringLiteral("&mdash;");
        case 152: return QStringLiteral("&tilde;");
        case 153: return QStringLiteral("&trade;");
        case 154: return QStringLiteral("&scaron;");
        case 155: return QStringLiteral("&rsaquo;");
        case 156: return QStringLiteral("&oelig;");
        case 158: return QStringLiteral("&zcaron;");
        case 159: return QStringLiteral("&Yuml;");
        case 160: return QStringLiteral("&nbsp;");
        case 161: return QStringLiteral("&iexcl;");
        case 162: return QStringLiteral("&cent;");
        case 163: return QStringLiteral("&pound;");
        case 164: return QStringLiteral("&curren;");
        case 165: return QStringLiteral("&yen;");
        case 166: return QStringLiteral("&brvbar;");
        case 167: return QStringLiteral("&sect;");
        case 168: return QStringLiteral("&uml;");
        case 169: return QStringLiteral("&copy;");
        case 170: return QStringLiteral("&ordf;");
        case 171: return QStringLiteral("&laquo;");
        case 172: return QStringLiteral("&not;");
        case 173: return QStringLiteral("&shy;");
        case 174: return QStringLiteral("&reg;");
        case 175: return QStringLiteral("&macr;");
        case 176: return QStringLiteral("&deg;");
        case 177: return QStringLiteral("&plusmn;");
        case 178: return QStringLiteral("&sup2;");
        case 179: return QStringLiteral("&sup3;");
        case 180: return QStringLiteral("&acute;");
        case 181: return QStringLiteral("&micro;");
        case 182: return QStringLiteral("&para;");
        case 183: return QStringLiteral("&middot;");
        case 184: return QStringLiteral("&cedil;");
        case 185: return QStringLiteral("&sup1;");
        case 186: return QStringLiteral("&ordm;");
        case 187: return QStringLiteral("&raquo;");
        case 188: return QStringLiteral("&frac14;");
        case 189: return QStringLiteral("&frac12;");
        case 190: return QStringLiteral("&frac34;");
        case 191: return QStringLiteral("&iquest;");
        case 192: return QStringLiteral("&Agrave;");
        case 193: return QStringLiteral("&Aacute;");
        case 194: return QStringLiteral("&Acirc;");
        case 195: return QStringLiteral("&Atilde;");
        case 196: return QStringLiteral("&Auml;");
        case 197: return QStringLiteral("&Aring;");
        case 198: return QStringLiteral("&AElig;");
        case 199: return QStringLiteral("&Ccedil;");
        case 200: return QStringLiteral("&Egrave;");
        case 201: return QStringLiteral("&Eacute;");
        case 202: return QStringLiteral("&Ecirc;");
        case 203: return QStringLiteral("&Euml;");
        case 204: return QStringLiteral("&Igrave;");
        case 205: return QStringLiteral("&Iacute;");
        case 206: return QStringLiteral("&Icirc;");
        case 207: return QStringLiteral("&Iuml;");
        case 208: return QStringLiteral("&ETH;");
        case 209: return QStringLiteral("&Ntilde;");
        case 210: return QStringLiteral("&Ograve;");
        case 211: return QStringLiteral("&Oacute;");
        case 212: return QStringLiteral("&Ocirc;");
        case 213: return QStringLiteral("&Otilde;");
        case 214: return QStringLiteral("&Ouml;");
        case 215: return QStringLiteral("&times;");
        case 216: return QStringLiteral("&Oslash;");
        case 217: return QStringLiteral("&Ugrave;");
        case 218: return QStringLiteral("&Uacute;");
        case 219: return QStringLiteral("&Ucirc;");
        case 220: return QStringLiteral("&Uuml;");
        case 221: return QStringLiteral("&Yacute;");
        case 222: return QStringLiteral("&THORN;");
        case 223: return QStringLiteral("&szlig;");
        case 224: return QStringLiteral("&agrave;");
        case 225: return QStringLiteral("&aacute;");
        case 226: return QStringLiteral("&acirc;");
        case 227: return QStringLiteral("&atilde;");
        case 228: return QStringLiteral("&auml;");
        case 229: return QStringLiteral("&aring;");
        case 230: return QStringLiteral("&aelig;");
        case 231: return QStringLiteral("&ccedil;");
        case 232: return QStringLiteral("&egrave;");
        case 233: return QStringLiteral("&eacute;");
        case 234: return QStringLiteral("&ecirc;");
        case 235: return QStringLiteral("&euml;");
        case 236: return QStringLiteral("&igrave;");
        case 237: return QStringLiteral("&iacute;");
        case 238: return QStringLiteral("&icirc;");
        case 239: return QStringLiteral("&iuml;");
        case 240: return QStringLiteral("&eth;");
        case 241: return QStringLiteral("&ntilde;");
        case 242: return QStringLiteral("&ograve;");
        case 243: return QStringLiteral("&oacute;");
        case 244: return QStringLiteral("&ocirc;");
        case 245: return QStringLiteral("&otilde;");
        case 246: return QStringLiteral("&ouml;");
        case 247: return QStringLiteral("&divide;");
        case 248: return QStringLiteral("&oslash;");
        case 249: return QStringLiteral("&ugrave;");
        case 250: return QStringLiteral("&uacute;");
        case 251: return QStringLiteral("&ucirc;");
        case 252: return QStringLiteral("&uuml;");
        case 253: return QStringLiteral("&yacute;");
        case 254: return QStringLiteral("&thorn;");
        case 255: return QStringLiteral("&yuml;");
        default: return QString();
    }
}

// Return HTML numeric entity number for special characters not in printable range
int AsciiListView::getHtmlNumber(unsigned char value)
{
    switch (value) {
        case 45:  return 8722;   // minus sign
        case 128: return 8364;   // euro
        case 130: return 8218;   // single low-9 quotation mark
        case 131: return 402;    // latin small letter f with hook
        case 132: return 8222;   // double low-9 quotation mark
        case 133: return 8230;   // horizontal ellipsis
        case 134: return 8224;   // dagger
        case 135: return 8225;   // double dagger
        case 136: return 710;    // modifier letter circumflex accent
        case 137: return 8240;   // per mille sign
        case 138: return 352;    // latin capital letter s with caron
        case 139: return 8249;   // single left-pointing angle quotation mark
        case 140: return 338;    // latin capital ligature oe
        case 142: return 381;    // latin capital letter z with caron
        case 145: return 8216;   // left single quotation mark
        case 146: return 8217;   // right single quotation mark
        case 147: return 8220;   // left double quotation mark
        case 148: return 8221;   // right double quotation mark
        case 149: return 8226;   // bullet
        case 150: return 8211;   // en dash
        case 151: return 8212;   // em dash
        case 152: return 732;    // small tilde
        case 153: return 8482;   // trade mark sign
        case 154: return 353;    // latin small letter s with caron
        case 155: return 8250;   // single right-pointing angle quotation mark
        case 156: return 339;    // latin small ligature oe
        case 158: return 382;    // latin small letter z with caron
        case 159: return 376;    // latin capital letter y with diaeresis
        default: return -1;
    }
}

// Populate the table with all 256 ASCII values
void AsciiListView::setValues(int codepage)
{
    _codepage = codepage;

    // Clear all rows (replaces ListView_DeleteAllItems)
    if (_tableWidget)
        _tableWidget->setRowCount(0);

    for (int i = 0; i < 256; ++i) {
        QString dec = QString::number(i);
        QString hex = QString::number(i, 16).toUpper().rightJustified(2, QChar('0'));
        QString s = getAscii(static_cast<unsigned char>(i));

        QString htmlNumber;
        QString htmlHexNumber;
        QString htmlName;

        if (codepage == 0 || codepage == 1252) {
            // CP1252: printable ASCII or high CP1252 chars get numeric entities
            if ((i >= 32 && i <= 126 && i != 45) || (i >= 160 && i <= 255)) {
                int n = getHtmlNumber(static_cast<unsigned char>(i));
                if (n > 0) {
                    htmlNumber = QStringLiteral("&#%1;").arg(n);
                    htmlHexNumber = QStringLiteral("&#x%1;").arg(n, 0, 16);
                }
            } else {
                int n = getHtmlNumber(static_cast<unsigned char>(i));
                if (n > 0) {
                    htmlNumber = QStringLiteral("&#%1;").arg(n);
                    htmlHexNumber = QStringLiteral("&#x%1;").arg(n, 0, 16);
                }
            }
            htmlName = getHtmlName(static_cast<unsigned char>(i));
        } else {
            // Other codepages: no HTML entities
            htmlNumber.clear();
            htmlHexNumber.clear();
            htmlName.clear();
        }

        std::vector<QString> values2Add;
        values2Add.reserve(6);
        values2Add.push_back(dec);
        values2Add.push_back(hex);
        values2Add.push_back(s);
        values2Add.push_back(htmlName);
        values2Add.push_back(htmlNumber);
        values2Add.push_back(htmlHexNumber);

        addLine(values2Add);
    }
}

void AsciiListView::resetValues(int codepage)
{
    if (codepage == -1)
        codepage = 0;

    if (_codepage == codepage)
        return;

    setValues(codepage);
}