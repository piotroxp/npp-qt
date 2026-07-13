// Semantic Lift: AnsiCharPanel — Win32 ListView ASCII grid → Qt6 QTableWidget
// Original: PowerEditor/src/WinControls/AnsiCharPanel/ansiCharPanel.cpp (209 lines)

#include "ansiCharPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QApplication>

AnsiCharPanel::AnsiCharPanel(QWidget* parent)
    : QWidget(parent)
    , _selectedChar(-1)
{
    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // Info label
    QLabel* label = new QLabel(tr("Click a character:"), this);
    mainLay->addWidget(label);

    // 16x16 grid for ASCII 0-255
    QTableWidget* table = new QTableWidget(16, 16, this);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::SingleSelection);

    for (int row = 0; row < 16; ++row) {
        for (int col = 0; col < 16; ++col) {
            int code = row * 16 + col;
            QChar ch(static_cast<QChar>(code));
            QString display = ch.isPrint() ? QString(ch) : QStringLiteral("·");
            QTableWidgetItem* item = new QTableWidgetItem(display);
            item->setToolTip(QStringLiteral("Dec: %1 | Hex: 0x%2 | Oct: %3")
                .arg(code).arg(code, 2, 16, QLatin1Char('0')).arg(code, 3, 8, QLatin1Char('0')));
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(row, col, item);
        }
    }

    table->horizontalHeader()->setDefaultSectionSize(32);
    table->verticalHeader()->setDefaultSectionSize(32);
    mainLay->addWidget(table);

    connect(table, &QTableWidget::itemClicked, this, [this, table](QTableWidgetItem* item) {
        int row = table->row(item);
        int col = table->column(item);
        _selectedChar = row * 16 + col;
        emit charSelected(_selectedChar);
    });
}

AnsiCharPanel::~AnsiCharPanel() {}

void AnsiCharPanel::init(QWidget* parent) { Q_UNUSED(parent); }

void AnsiCharPanel::display(int asciiCode)
{
    if (asciiCode >= 0 && asciiCode < 256) {
        _selectedChar = asciiCode;
        emit charSelected(asciiCode);
    }
}
