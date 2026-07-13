// Semantic Lift: Win32 BabyGrid → Qt6 BabyGrid
// Original: PowerEditor/src/WinControls/Grid/BabyGrid.cpp
// Target: npp-qt/src/WinControls/Grid/BabyGrid.cpp

#include "BabyGrid.h"
#include <QHeaderView>
#include <QApplication>
#include <QItemSelectionModel>
#include <QItemDelegate>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QKeyEvent>
#include <QDebug>

// =============================================================================
// BabyGridDelegate
// =============================================================================

BabyGridDelegate::BabyGridDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
    _titleFont = QApplication::font();
    _headerFont = QApplication::font();
    _bodyFont = QApplication::font();
    _titleColor = QApplication::palette().color(QPalette::Button);
    _titleTextColor = QApplication::palette().color(QPalette::ButtonText);
    _titleGridlineColor = QApplication::palette().color(QPalette::Mid);
}

void BabyGridDelegate::drawTitle(QPainter* p, const QRect& rect) const
{
    p->save();
    p->fillRect(rect, _titleColor);

    // Draw raised border (Win32 DrawEdge equivalent)
    QPen borderPen(_titleGridlineColor, 1);
    p->setPen(borderPen);
    p->drawRect(rect.adjusted(0, 0, -1, -1));

    // Draw title text
    if (!_titleText.isEmpty()) {
        QRect textRect = rect.adjusted(4, 2, -4, -2);
        p->setPen(_titleTextColor);
        p->setFont(_titleFont);
        if (_useEllipsis) {
            p->drawText(textRect, Qt::AlignCenter | Qt::TextSingleLine | Qt::AlignVCenter,
                       _titleText);
        } else {
            p->drawText(textRect, Qt::AlignCenter | Qt::AlignVCenter, _titleText);
        }
    }
    p->restore();
}

void BabyGridDelegate::drawCell(QPainter* p, const QRect& rect,
                                const QModelIndex& index,
                                bool isSelected, bool hasFocus) const
{
    int row = index.row();
    int col = index.column();
    QString key = QStringLiteral("%1,%2").arg(row).arg(col);

    // Get cell data
    CellData cd;
    QVariant cellVar = index.data(Qt::UserRole);
    if (cellVar.canConvert<CellData>()) {
        cd = cellVar.value<CellData>();
    }

    // Background color
    QBrush bgBrush = index.data(Qt::BackgroundRole).value<QBrush>();
    if (!bgBrush.color().isValid()) {
        if (isSelected && hasFocus) {
            bgBrush = QBrush(_highlightColor);
        } else if (isSelected && !hasFocus) {
            bgBrush = QBrush(QColor(200, 200, 200));
        } else {
            bgBrush = QBrush(_bgColor);
        }
    }

    // Draw cell background
    p->fillRect(rect, bgBrush);

    // Grid lines
    p->save();
    QPen gridPen(_gridColor, 1);
    p->setPen(gridPen);
    // Right edge
    p->drawLine(rect.topRight(), rect.bottomRight());
    // Bottom edge
    p->drawLine(rect.bottomLeft(), rect.bottomRight());
    p->restore();

    // Text color
    QColor txtColor = _textColor;
    if (isSelected && hasFocus) {
        txtColor = _highlightTextColor;
    }

    // Text
    QString text = index.data(Qt::DisplayRole).toString();
    if (!text.isEmpty()) {
        QRect textRect = rect.adjusted(2, 1, -3, -1);
        p->setPen(txtColor);
        p->setFont(_bodyFont);

        Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter;
        if (cd.iType == 2) { // NUMERIC
            align = Qt::AlignRight | Qt::AlignVCenter;
        }

        if (_useEllipsis) {
            p->drawText(textRect, align | Qt::TextSingleLine, text);
        } else {
            p->drawText(textRect, align, text);
        }
    }

    // Cursor highlight (current cell border)
    if (isSelected && hasFocus) {
        p->save();
        p->setPen(QPen(txtColor, 2));
        p->drawRect(rect.adjusted(1, 1, -2, -2));
        p->restore();
    }
}

void BabyGridDelegate::paint(QPainter* painter,
                             const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize BabyGridDelegate::sizeHint(const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const
{
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    sz.setHeight(_rowHeight);
    return sz;
}

bool BabyGridDelegate::editorEvent(QEvent* event,
                                    QAbstractItemModel* model,
                                    const QStyleOptionViewItem& option,
                                    const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        emit cellClicked(index.row(), index.column());
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        emit cellDoubleClicked(index.row(), index.column());
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void BabyGridDelegate::setTitleText(const QString& text) { _titleText = text; }
void BabyGridDelegate::setTitleHeight(int h) { _titleHeight = h; }
void BabyGridDelegate::setHeaderHeight(int h) { _headerHeight = h; }
void BabyGridDelegate::setRowHeight(int h) { _rowHeight = h; }
void BabyGridDelegate::setTitleFont(const QFont& font) { _titleFont = font; }
void BabyGridDelegate::setHeaderFont(const QFont& font) { _headerFont = font; }
void BabyGridDelegate::setBodyFont(const QFont& font) { _bodyFont = font; }

// =============================================================================
// BabyGrid
// =============================================================================

BabyGrid::BabyGrid(QWidget* parent)
    : QTableWidget(parent)
{
    _delegate = new BabyGridDelegate(this);
    setItemDelegate(_delegate);

    // Configure as grid-like table
    setColumnCount(255);
    setRowCount(100);
    setAlternatingRowColors(false);

    // Column 0: row numbers
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(false);

    // Set default sizes
    verticalHeader()->setDefaultSectionSize(_rowHeight);
    for (int c = 0; c < 255; ++c) {
        setColumnWidth(c, 50);
    }

    // Selection behavior: full row
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // Cell editing disabled (read-only grid)
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Connect signals
    connect(this, &QTableWidget::cellClicked,
            this, &BabyGrid::cellClicked);
    connect(this, &QTableWidget::cellDoubleClicked,
            this, &BabyGrid::cellDoubleClicked);
}

void BabyGrid::setGridDimensions(int rows, int cols)
{
    if (rows > 0) setRowCount(rows);
    if (cols > 0) setColumnCount(cols);
}

void BabyGrid::setColumnWidth(unsigned int col, unsigned int width)
{
    if (col < static_cast<unsigned>(columnCount())) {
        QTableWidget::setColumnWidth(col, static_cast<int>(width));
    }
}

int BabyGrid::columnWidth(unsigned int col) const
{
    if (col < static_cast<unsigned>(columnCount()))
        return QTableWidget::columnWidth(col);
    return 0;
}

void BabyGrid::setRowHeight(unsigned int row, unsigned int height)
{
    if (row < static_cast<unsigned>(rowCount())) {
        QTableWidget::setRowHeight(row, static_cast<int>(height));
        _rowHeight = static_cast<int>(height);
    }
}

int BabyGrid::rowHeight(unsigned int row) const
{
    if (row < static_cast<unsigned>(rowCount()))
        return QTableWidget::rowHeight(row);
    return 0;
}

void BabyGrid::setTitleHeight(unsigned int h)
{
    _titleHeight = static_cast<int>(h);
    update();
}

void BabyGrid::setTitleFont(const QFont& font)
{
    _delegate->setTitleFont(font);
    update();
}

void BabyGrid::setHeaderFont(const QFont& font)
{
    _delegate->setHeaderFont(font);
    update();
}

void BabyGrid::setBodyFont(const QFont& font)
{
    QTableWidget::setFont(font);
    _delegate->setBodyFont(font);
    update();
}

void BabyGrid::setTitle(const QString& text)
{
    _delegate->setTitleText(text);
    update();
}

void BabyGrid::setTextColor(const QColor& c)
{
    _delegate->setTextColor(c);
    update();
}

void BabyGrid::setBackgroundColor(const QColor& c)
{
    _delegate->setBackgroundColor(c);
    update();
}

void BabyGrid::setGridlineColor(const QColor& c)
{
    setShowGrid(true);
    _delegate->setGridlineColor(c);
    update();
}

void BabyGrid::setHighlightColor(const QColor& c)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Highlight, c);
    setPalette(pal);
    _delegate->setHighlightColor(c);
    update();
}

void BabyGrid::setHighlightTextColor(const QColor& c)
{
    QPalette pal = palette();
    pal.setColor(QPalette::HighlightedText, c);
    setPalette(pal);
    _delegate->setHighlightTextColor(c);
    update();
}

void BabyGrid::setTitleColor(const QColor& c)
{
    _delegate->setTitleColor(c);
    update();
}

void BabyGrid::setTitleTextColor(const QColor& c)
{
    _delegate->setTitleTextColor(c);
    update();
}

void BabyGrid::setTitleGridlineColor(const QColor& c)
{
    _delegate->setTitleGridlineColor(c);
    update();
}

void BabyGrid::setCellText(int row, int col, const QString& text)
{
    if (row < 0 || col < 0 || row >= rowCount() || col >= columnCount())
        return;

    QTableWidgetItem* item = this->item(row, col);
    if (!item) {
        item = new QTableWidgetItem(text);
        setItem(row, col, item);
    } else {
        item->setText(text);
    }
}

QString BabyGrid::cellText(int row, int col) const
{
    if (row < 0 || col < 0 || row >= rowCount() || col >= columnCount())
        return QString();
    QTableWidgetItem* item = this->item(row, col);
    return item ? item->text() : QString();
}

void BabyGrid::clearGrid()
{
    clear();
    setRowCount(100);
    setColumnCount(255);
    _cellData.clear();
    _cellProtection.clear();
}

void BabyGrid::setRowsNumbered(bool on)
{
    _rowsNumbered = on;
    _delegate->setRowsNumbered(on);
    update();
}

void BabyGrid::setColsNumbered(bool on)
{
    _colsNumbered = on;
    _delegate->setColsNumbered(on);
    update();
}

void BabyGrid::setEllipsis(bool on)
{
    _ellipsis = on;
    _delegate->setEllipsis(on);
    update();
}

void BabyGrid::setAutoRow(bool on)
{
    _autoRow = on;
}

void BabyGrid::setColAutoWidth(bool on)
{
    _colAutoWidth = on;
}

void BabyGrid::setShowIntegralRows(bool on)
{
    _showIntegralRows = on;
}

void BabyGrid::setExtendLastColumn(bool on)
{
    _extendLastColumn = on;
    if (on) {
        horizontalHeader()->setStretchLastSection(true);
    }
}

void BabyGrid::setCursorPos(int row, int col)
{
    _cursorRow = row;
    _cursorCol = col;
    if (row >= 0 && row < rowCount() && col >= 0 && col < columnCount()) {
        setCurrentCell(row, col, QItemSelectionModel::ClearAndSelect);
    }
    drawCursor();
}

void BabyGrid::protectCell(int row, int col, bool protect)
{
    QString key = makeKey(row, col);
    _cellProtection[key] = protect;
    if (protect) {
        QTableWidgetItem* item = this->item(row, col);
        if (!item) {
            item = new QTableWidgetItem();
            setItem(row, col, item);
        }
        QColor bg = palette().color(QPalette::Window);
        item->setBackground(bg);
    }
}

bool BabyGrid::isCellProtected(int row, int col) const
{
    QString key = makeKey(row, col);
    return _cellProtection.value(key, false);
}

void BabyGrid::drawCursor()
{
    // XOR draw cursor — handled via selection highlight
    update();
}

void BabyGrid::mousePressEvent(QMouseEvent* event)
{
    QTableWidget::mousePressEvent(event);
    emit cellClicked(currentRow(), currentColumn());
}

void BabyGrid::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTableWidget::mouseDoubleClickEvent(event);
    emit cellDoubleClicked(currentRow(), currentColumn());
}

void BabyGrid::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
        case Qt::Key_Home:
        case Qt::Key_End:
            QTableWidget::keyPressEvent(event);
            _cursorRow = currentRow();
            _cursorCol = currentColumn();
            drawCursor();
            break;
        default:
            QTableWidget::keyPressEvent(event);
    }
}

void BabyGrid::focusInEvent(QFocusEvent* event)
{
    _gridHasFocus = true;
    QTableWidget::focusInEvent(event);
}

void BabyGrid::focusOutEvent(QFocusEvent* event)
{
    _gridHasFocus = false;
    QTableWidget::focusOutEvent(event);
}

bool BabyGrid::event(QEvent* event)
{
    return QTableWidget::event(event);
}

void registerBabyGridClass()
{
    // No-op in Qt6 — BabyGrid is a regular QWidget subclass.
    // The Win32 RegisterGridClass() call is replaced by the normal
    // Qt widget construction (new BabyGrid).
}
