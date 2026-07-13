// Semantic Lift: FindCharsInRange — Win32 dialog + grid → Qt6 QDialog + QTableWidget
// Original: PowerEditor/src/WinControls/FindCharsInRange/FindCharsInRange.cpp (253 lines)
// Win32 → Qt6: CharGrid + WM_PAINT → QTableWidget

#include "FindCharsInRange.h"
#include "NppDarkMode.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QChar>
#include <QApplication>

FindCharsInRangeDlg::FindCharsInRangeDlg(QWidget* parent)
    : QDialog(parent)
    , _selectedChar(-1)
{
    setWindowTitle(tr("Find Characters in Range"));
    setModal(true);
    setMinimumSize(600, 400);

    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // Info label
    mainLay->addWidget(new QLabel(tr("Click a character to insert its code:")));

    // Grid of characters
    QTableWidget* table = new QTableWidget(16, 16, this);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setOnItemActivated([this, table](QTableWidgetItem* item) {
        if (item) {
            int row = table->row(item);
            int col = table->column(item);
            _selectedChar = row * 16 + col;
            accept();
        }
    });

    // Populate grid with ASCII characters 0-255
    for (int row = 0; row < 16; ++row) {
        for (int col = 0; col < 16; ++col) {
            int code = row * 16 + col;
            QChar ch(code);
            QString display = ch.isPrint() ? QString(ch) : QStringLiteral("·");
            QTableWidgetItem* item = new QTableWidgetItem(display);
            item->setToolTip(QStringLiteral("U+%1 (%2)").arg(code, 4, 16, QLatin1Char('0')).arg(code));
            table->setItem(row, col, item);
        }
    }

    table->horizontalHeader()->setDefaultSectionSize(36);
    table->verticalHeader()->setDefaultSectionSize(36);
    mainLay->addWidget(table);

    // Buttons
    QDialogButtonBox* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLay->addWidget(btnBox);

    NppDarkMode::instance().autoNppDarkMode(this);
}

FindCharsInRangeDlg::~FindCharsInRangeDlg()
{
}

void FindCharsInRangeDlg::init()
{
}

void FindCharsInRangeDlg::doDialog()
{
    show();
    raise();
}

void FindCharsInRangeDlg::display()
{
    show();
    raise();
    activateWindow();
}

void FindCharsInRangeDlg::goToChar(int charCode)
{
    if (charCode < 0 || charCode > 255)
        return;
    _selectedChar = charCode;
}

int FindCharsInRangeDlg::getSelectedChar() const
{
    return _selectedChar;
}

void FindCharsInRangeDlg::closeEvent(QCloseEvent* event)
{
    accept();
    event->accept();
}
