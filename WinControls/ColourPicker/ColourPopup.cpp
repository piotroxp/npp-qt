// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Qt6 port: ColourPopup replaces the Win32 owner-drawn colour grid popup.
// See ColourPopup.h for architecture details.

#include "ColourPopup.h"

#include <QColorDialog>
#include <QEvent>
#include <QGridLayout>
#include <QHideEvent>
#include <QItemDelegate>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QStyleOptionViewItem>
#include <QStylePainter>

#include "NppDarkMode.h"

// Colour palette: 48 swatches (mirrors Win32 colorItems array)
static const QList<QColor> colorItems = {
    QColor(  0,   0,   0), QColor( 64,   0,   0), QColor(128,   0,   0), QColor(128,  64,  64),
    QColor(255,   0,   0), QColor(255, 128, 128), QColor(255, 255, 128), QColor(255, 255,   0),
    QColor(255, 128,  64), QColor(255, 128,   0), QColor(128,  64,   0), QColor(128, 128,   0),
    QColor(128, 128,  64), QColor(  0,  64,   0), QColor(  0, 128,   0), QColor(  0, 255,   0),
    QColor(128, 255,   0), QColor(128, 255, 128), QColor(  0, 255, 128), QColor(  0, 255,  64),
    QColor(  0, 128, 128), QColor(  0, 128,  64), QColor(  0,  64,  64), QColor(128, 128, 128),
    QColor( 64, 128, 128), QColor(  0,   0, 128), QColor(  0,   0, 255), QColor(  0,  64, 128),
    QColor(  0, 255, 255), QColor(128, 255, 255), QColor(  0, 128, 255), QColor(  0, 128, 192),
    QColor(128, 128, 255), QColor(  0,   0, 160), QColor(  0,   0,  64), QColor(192, 192, 192),
    QColor( 64,   0,  64), QColor( 64,   0,  64), QColor(128,   0, 128), QColor(128,   0,  64),
    QColor(128, 128, 192), QColor(255, 128, 192), QColor(255, 128, 255), QColor(255,   0, 255),
    QColor(255,   0, 128), QColor(128,   0, 255), QColor( 64,   0, 128), QColor(255, 255, 255),
};

// Item delegate that paints colour swatches with 3D border (mirrors Win32 ODA_DRAWENTIRE)
class ColourSwatchDelegate : public QItemDelegate
{
public:
    explicit ColourSwatchDelegate(QObject *parent = nullptr)
        : QItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QColor color = index.data(Qt::UserRole).value<QColor>();
        if (!color.isValid()) {
            QItemDelegate::paint(painter, option, index);
            return;
        }

        QRect r = option.rect;
        r.adjust(3, 3, -3, -3);

        // Draw colour fill
        painter->fillRect(r, color);

        // Draw border (3D frame — light top-left, dark bottom-right)
        QPen lightPen(NppDarkMode::isEnabled() ? NppDarkMode::getEdgeColor()
                                               : Qt::darkGray);
        QPen darkPen(NppDarkMode::isEnabled() ? NppDarkMode::getEdgeColor()
                                              : Qt::lightGray);

        // Selected: draw 3D inset
        if (option.state & QStyle::State_Selected) {
            QRect sr = r;
            sr.adjust(0, 0, -1, -1);

            // Light top-left (highlight)
            painter->setPen(lightPen);
            painter->drawLine(sr.topLeft(), sr.bottomLeft());
            painter->drawLine(sr.topLeft(), sr.topRight());

            // Dark bottom-right (shadow)
            painter->setPen(darkPen);
            painter->drawLine(sr.bottomLeft().x(), sr.bottomLeft().y(),
                              sr.bottomRight().x(), sr.bottomRight().y());
            painter->drawLine(sr.bottomRight().x(), sr.bottomRight().y(),
                              sr.topRight().x(), sr.topRight().y());
        } else {
            // Unselected: flat border
            painter->setPen(NppDarkMode::isEnabled()
                                ? NppDarkMode::getEdgeColor()
                                : Qt::black);
            painter->drawRect(r.adjusted(0, 0, -1, -1));
        }

        // Focus rect
        if (option.state & QStyle::State_HasFocus) {
            QRect fr = r.adjusted(-2, -2, 2, 2);
            painter->setPen(option.palette.color(QPalette::Text));
            painter->drawRect(fr.adjusted(0, 0, -1, -1));
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &) const override
    {
        // Match Win32: 8 columns × 6 rows → each item = (dialog width / 8) × (dialog height / 6)
        // Base size: 8 × 6 grid, each cell ~24×20
        return {24, 20};
    }
};


ColourPopup::ColourPopup()
    : QDialog()
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setModal(false);

    _colorList = new QListWidget(this);
    _colorList->setItemDelegate(new ColourSwatchDelegate(_colorList));
    _colorList->setSelectionMode(QAbstractItemView::SingleSelection);
    _colorList->setViewMode(QListView::IconMode);
    _colorList->setFlow(QListView::LeftToRight);
    _colorList->setGridSize({28, 24});
    _colorList->setUniformItemSizes(true);
    _colorList->setFocusPolicy(Qt::NoFocus);
    _colorList->setFrameStyle(QFrame::NoFrame);

    QPushButton *customBtn = new QPushButton(tr("Custom..."), this);
    customBtn->setMinimumHeight(20);
    customBtn->setFocusPolicy(Qt::NoFocus);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);
    layout->addWidget(_colorList);
    layout->addWidget(customBtn);

    populateColorList();
    applyDarkModeIfEnabled();

    // Connect signals
    connect(_colorList, &QListWidget::currentRowChanged,
            this, &ColourPopup::onListCurrentRowChanged);
    connect(customBtn, &QPushButton::clicked,
            this, &ColourPopup::onCustomColorClicked);
}

ColourPopup::ColourPopup(const QColor &defaultColor)
    : ColourPopup()
{
    _colour = defaultColor.isValid() ? defaultColor : Qt::white;
}

void ColourPopup::populateColorList()
{
    for (const QColor &col : colorItems) {
        QListWidgetItem *item = new QListWidgetItem(_colorList);
        item->setData(Qt::UserRole, col);
        item->setSizeHint({24, 20});
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);  // selected via row only
    }
}

void ColourPopup::applyDarkModeIfEnabled()
{
    if (NppDarkMode::isEnabled()) {
        setStyleSheet(R"(
            QDialog { background-color: #2d2d2d; }
            QListWidget { background-color: #2d2d2d; border: none; }
            QPushButton { background-color: #3d3d3d; color: #cccccc; border: 1px solid #555555; }
        )");
    } else {
        setStyleSheet(QString());
    }
}

QColor ColourPopup::currentColor() const
{
    int row = _colorList->currentRow();
    if (row >= 0 && row < colorItems.size()) {
        return colorItems.at(row);
    }
    return _colour;
}

void ColourPopup::doPopup(const QPoint &p)
{
    if (!isVisible()) {
        populateColorList();
        applyDarkModeIfEnabled();
        resize(224, 140);
        move(p);
        show();
    }
}

void ColourPopup::createColorPopup()
{
    // Called during construction; Qt widget is already created in ctor
}

void ColourPopup::onListCurrentRowChanged(int row)
{
    if (row >= 0 && row < colorItems.size()) {
        _colour = colorItems.at(row);
        hide();
        emit colorPicked(_colour);
    }
}

void ColourPopup::onCustomColorClicked()
{
    QColorDialog *dialog = new QColorDialog(_colour, this);
    dialog->setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

    if (NppDarkMode::isEnabled() && NppDarkMode::isExperimentalSupported()) {
        NppDarkMode::setDarkTitleBar(reinterpret_cast<HWND>(dialog->winId()));
    }

    connect(dialog, &QColorDialog::colorSelected,
            this, &ColourPopup::onChooseColorDialogAccepted);
    connect(dialog, &QDialog::rejected, dialog, &QObject::deleteLater);
    dialog->show();
}

void ColourPopup::onChooseColorDialogAccepted()
{
    auto *dialog = qobject_cast<QColorDialog *>(sender());
    if (dialog) {
        _colour = dialog->selectedColor();
        hide();
        emit colorPicked(_colour);
        dialog->deleteLater();
    }
}

void ColourPopup::hideEvent(QHideEvent *event)
{
    QDialog::hideEvent(event);
    hide();
}

bool ColourPopup::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);
    if (event->type() == QEvent::MouseButtonPress && isVisible()) {
        // Click outside popup → hide
        hide();
        return true;
    }
    return false;
}