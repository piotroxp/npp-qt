// Semantic Lift: Win32 AnsiCharPanel → Qt6 implementation
// Original: PowerEditor/src/WinControls/AnsiCharPanel/*.cpp
// Target: npp-qt/src/WinControls/AnsiCharPanel.h + .cpp

#include "AnsiCharPanel.h"
#include "NppDarkMode.h"
#include "DockingWnd.h"
#include "NppConstants.h"
#include "Notepad_plus_msgs.h"

// Win32 macro replacements for portability (LOWORD/HIWORD used in run_dlgProc)
#ifndef LOWORD
#define LOWORD(l) ((quint16)(((quintptr)(l)) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(l) ((quint16)((((quintptr)(l)) >> 16) & 0xFFFF))
#endif
#include <QApplication>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QKeyEvent>

// =============================================================================
// ListView
// =============================================================================

ListView::ListView(QWidget* parent)
    : Window(parent)
{
    _table = new QTableWidget(0, 0, this);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SingleSelection);
    _table->setShowGrid(true);
    _table->setAlternatingRowColors(false);
    _table->horizontalHeader()->setStretchLastSection(false);
    _table->verticalHeader()->setVisible(false);
    _table->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(_table, &QTableWidget::itemDoubleClicked,
            this, [this](QTableWidgetItem* item) {
                if (item)
                    emit itemDoubleClicked(item->row(), item->column());
            });

    connect(_table, &QTableWidget::itemActivated,
            this, [this](QTableWidgetItem* item) {
                if (item)
                    emit itemActivated(item->row());
            });

    connect(_table, &QTableWidget::customContextMenuRequested,
            this, [this](const QPoint& pos) {
                emit customContextMenuRequested(pos);
            });
}

void ListView::addColumn(const columnInfo& col2Add)
{
    _columnInfos.append(col2Add);
}

void ListView::setColumnText(size_t i, const QString& txt2Set)
{
    if (i < static_cast<size_t>(_table->columnCount())) {
        _table->setHorizontalHeaderItem(static_cast<int>(i),
                                        new QTableWidgetItem(txt2Set));
    }
}

size_t ListView::nbItem() const
{
    return static_cast<size_t>(_table->rowCount());
}

long ListView::getSelectedIndex() const
{
    return _table->currentRow();
}

void ListView::setSelection(int itemIndex)
{
    if (itemIndex >= 0 && itemIndex < _table->rowCount()) {
        _table->selectRow(itemIndex);
        _table->scrollTo(_table->model()->index(itemIndex, 0));
    }
}

void ListView::clear()
{
    _table->setRowCount(0);
}

bool ListView::removeFromIndex(size_t i)
{
    if (i >= static_cast<size_t>(_table->rowCount()))
        return false;
    _table->removeRow(static_cast<int>(i));
    return true;
}

void ListView::init(void* hInst, QWidget* hParent)
{
    WindowBase::init(hInst, hParent);
    _hSelf = this;

    if (!_columnInfos.isEmpty()) {
        _table->setColumnCount(_columnInfos.size());
        int i = 1; // 1-based to match Win32 ListView
        for (const auto& colInfo : _columnInfos) {
            QTableWidgetItem* headerItem = new QTableWidgetItem(colInfo._label);
            headerItem->setData(Qt::UserRole, i);
            _table->setHorizontalHeaderItem(i - 1, headerItem);
            _table->setColumnWidth(i - 1, colInfo._width);
            ++i;
        }
    }

    // Style the table
    _table->setFont(QApplication::font());
    _table->setColumnWidth(0, 45);
    _table->setColumnWidth(1, 45);
    _table->setColumnWidth(2, 70);
    _table->setColumnWidth(3, 90);
    _table->setColumnWidth(4, 100);
    _table->setColumnWidth(5, 120);

    // Set the table to fill available space
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(_table);
    setLayout(layout);
    setMinimumSize(300, 200);
}

void ListView::destroy()
{
    if (_table) {
        delete _table;
        _table = nullptr;
    }
    deleteLater();
}

// =============================================================================
// AsciiListView
// =============================================================================

AsciiListView::AsciiListView(QWidget* parent)
    : ListView(parent)
{
}

void AsciiListView::resetValues(int codepage)
{
    if (codepage == -1)
        codepage = 0;

    if (_codepage == codepage)
        return;

    clear();
    setValues(codepage);
}

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
        default:
        {
            // Show the character for printable ASCII (32-126)
            if (value >= 32 && value <= 126) {
                return QString(QChar(static_cast<QChar::SpecialCharacter>(value)));
            }
            return QStringLiteral("?");
        }
    }
}

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
        case 126: return QStringLiteral("");
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
        default:  return QString();
    }
}

int AsciiListView::getHtmlNumber(unsigned char value)
{
    switch (value) {
        case 45:  return 8722;
        case 128: return 8364;
        case 130: return 8218;
        case 131: return 402;
        case 132: return 8222;
        case 133: return 8230;
        case 134: return 8224;
        case 135: return 8225;
        case 136: return 710;
        case 137: return 8240;
        case 138: return 352;
        case 139: return 8249;
        case 140: return 338;
        case 142: return 381;
        case 145: return 8216;
        case 146: return 8217;
        case 147: return 8220;
        case 148: return 8221;
        case 149: return 8226;
        case 150: return 8211;
        case 151: return 8212;
        case 152: return 732;
        case 153: return 8482;
        case 154: return 353;
        case 155: return 8250;
        case 156: return 339;
        case 158: return 382;
        case 159: return 376;
        default:  return -1;
    }
}

void AsciiListView::setValues(int codepage)
{
    _codepage = codepage;
    clear();

    for (int i = 0; i < 256; ++i) {
        unsigned char uc = static_cast<unsigned char>(i);

        QString dec = QString::number(i);
        QString hex = QString::number(i, 16).toUpper().rightJustified(2, '0');
        QString asciiStr = getAscii(uc);
        QString htmlName = getHtmlName(uc);

        QString htmlNumber;
        QString htmlHexNumber;

        if (codepage == 0 || codepage == 1252) {
            if ((i >= 32 && i <= 126 && i != 45) || (i >= 160 && i <= 255)) {
                int n = getHtmlNumber(uc);
                if (n > -1) {
                    htmlNumber = QStringLiteral("&#%1;").arg(n);
                    htmlHexNumber = QStringLiteral("&#x%1;").arg(n, 0, 16).toUpper();
                }
            } else {
                int n = getHtmlNumber(uc);
                if (n > -1) {
                    htmlNumber = QStringLiteral("&#%1;").arg(n);
                    htmlHexNumber = QStringLiteral("&#x%1;").arg(n, 0, 16).toUpper();
                }
            }
        }

        int row = _table->rowCount();
        _table->insertRow(row);

        QTableWidgetItem* decItem = new QTableWidgetItem(dec);
        QTableWidgetItem* hexItem = new QTableWidgetItem(hex);
        QTableWidgetItem* asciiItem = new QTableWidgetItem(asciiStr);
        QTableWidgetItem* htmlNameItem = new QTableWidgetItem(htmlName);
        QTableWidgetItem* htmlNumItem = new QTableWidgetItem(htmlNumber);
        QTableWidgetItem* htmlHexItem = new QTableWidgetItem(htmlHexNumber);

        // Set all items as non-editable and store row data
        decItem->setFlags(decItem->flags() & ~Qt::ItemIsEditable);
        hexItem->setFlags(hexItem->flags() & ~Qt::ItemIsEditable);
        asciiItem->setFlags(asciiItem->flags() & ~Qt::ItemIsEditable);
        htmlNameItem->setFlags(htmlNameItem->flags() & ~Qt::ItemIsEditable);
        htmlNumItem->setFlags(htmlNumItem->flags() & ~Qt::ItemIsEditable);
        htmlHexItem->setFlags(htmlHexItem->flags() & ~Qt::ItemIsEditable);

        _table->setItem(row, 0, decItem);
        _table->setItem(row, 1, hexItem);
        _table->setItem(row, 2, asciiItem);
        _table->setItem(row, 3, htmlNameItem);
        _table->setItem(row, 4, htmlNumItem);
        _table->setItem(row, 5, htmlHexItem);
    }
}

// =============================================================================
// AnsiCharPanel
// =============================================================================

AnsiCharPanel::AnsiCharPanel()
    : StaticDialog()
{
    setWindowTitle(QStringLiteral("ASCII Codes Insertion Panel"));
    _hSelf = this;
}

void AnsiCharPanel::init(QApplication* app, QWidget* parent, void* ppEditView)
{
    StaticDialog::create(IDD_ANSIASCII_PANEL);
    _hInst = app;
    _hParent = parent;
    _ppEditView = ppEditView;

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    _listView = new AsciiListView(this);
    _listView->setObjectName(QString::number(IDC_LIST_ANSICHAR));
    _listView->init(app, this);

    // Add columns (matching the Win32 DialogTemplate)
    _listView->addColumn(columnInfo(QStringLiteral("Value"), 45));
    _listView->addColumn(columnInfo(QStringLiteral("Hex"), 45));
    _listView->addColumn(columnInfo(QStringLiteral("Character"), 70));
    _listView->addColumn(columnInfo(QStringLiteral("HTML Name"), 90));
    _listView->addColumn(columnInfo(QStringLiteral("HTML Decimal"), 100));
    _listView->addColumn(columnInfo(QStringLiteral("HTML Hexadecimal"), 120));

    // Default codepage = 0 (system default / ASCII)
    _listView->setValues(0);
    _listView->table()->setColumnWidth(0, 45);
    _listView->table()->setColumnWidth(1, 45);
    _listView->table()->setColumnWidth(2, 70);
    _listView->table()->setColumnWidth(3, 90);
    _listView->table()->setColumnWidth(4, 100);
    _listView->table()->setColumnWidth(5, 120);

    // Connect double-click → insert char/string
    connect(_listView, &AsciiListView::itemDoubleClicked,
            this, &AnsiCharPanel::onItemDoubleClicked);

    // Connect Return/Enter key → insert char
    connect(_listView->table(), &QTableWidget::activated,
            this, [this](const QModelIndex& idx) {
                onItemActivated(idx.row());
            });

    layout->addWidget(_listView);
    setLayout(layout);

    _isCreated = true;
}

void AnsiCharPanel::setParent(QWidget* parent2set)
{
    _hParent = parent2set;
}

void AnsiCharPanel::switchEncoding()
{
    // When the editor's encoding changes, update the table.
    // Mirrors Win32 AnsiCharPanel::switchEncoding().
    // Full implementation: query current buffer codepage from ScintillaEditView.
    // For now, reset to default (system ANSI codepage).
    int codepage = 0; // 0 = default/system codepage
    _listView->resetValues(codepage);
}

void AnsiCharPanel::setBackgroundColor(const QColor& bgColour)
{
    _bgColor = bgColour;
    QPalette pal = _listView->palette();
    pal.setColor(QPalette::Base, bgColour);
    pal.setColor(QPalette::AlternateBase, bgColour.lighter(105));
    _listView->setPalette(pal);
    _listView->table()->setPalette(pal);
    _listView->table()->setAlternatingRowColors(true);
    update();
}

void AnsiCharPanel::setForegroundColor(const QColor& fgColour)
{
    _fgColor = fgColour;
    QPalette pal = _listView->palette();
    pal.setColor(QPalette::Text, fgColour);
    _listView->setPalette(pal);
    _listView->table()->setPalette(pal);
    update();
}

void AnsiCharPanel::insertChar(unsigned char char2insert)
{
    // Mirrors Win32: insert the character into the current Scintilla buffer.
    // In the Win32 version, this uses MultiByteToWideChar + SCI_ADDTEXT.
    // Here we emit a signal that MainWindow connects to ScintillaEditView.
    QByteArray data;
    if (char2insert < 128) {
        data = QByteArray(1, static_cast<char>(char2insert));
    } else {
        // For non-ASCII (128-255), emit the raw byte.
        // MainWindow/ScintillaEditView will handle encoding conversion
        // based on the current buffer's codepage.
        data = QByteArray(1, static_cast<char>(char2insert));
    }

    // Real signal — MainWindow connects charInsertRequested to ScintillaEditView
    emit charInsertRequested(data);
}

void AnsiCharPanel::insertString(const QString& string2insert)
{
    // Mirrors Win32: insert a decoded string (HTML entity, etc.)
    emit charInsertRequested(string2insert.toUtf8());
}

void AnsiCharPanel::onItemDoubleClicked(int row, int col)
{
    // Mirrors Win32 NM_DBLCLK handler.
    // col == 2: insert the character at that row (ASCII value == row index).
    // other cols: insert the text from the cell.
    if (row < 0) return;
    if (col == 2) {
        insertChar(static_cast<unsigned char>(row));
    } else {
        QTableWidgetItem* item = _listView->table()->item(row, col);
        if (item)
            insertString(item->text());
    }
}

void AnsiCharPanel::onItemActivated(int row)
{
    // Mirrors Win32 VK_RETURN handler in TaskList.
    if (row < 0) return;
    insertChar(static_cast<unsigned char>(row));
}

intptr_t AnsiCharPanel::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            // Mirrors Win32 WM_INITDIALOG — dark mode theming applied in doDialog
            return TRUE;
        }

        case WM_SIZE: {
            // Mirrors Win32 WM_SIZE: resize list view to fill panel
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            Q_UNUSED(width);
            Q_UNUSED(height);
            if (_listView)
                _listView->resize(size());
            return TRUE;
        }

        case NPPM_INTERNAL_REFRESHDARKMODE: {
            // Re-apply dark mode to child controls
            // MainWindow connects NppDarkMode signals to this
            return TRUE;
        }

        default:
            break;
    }
    return StaticDialog::run_dlgProc(message, wParam, lParam);
}
