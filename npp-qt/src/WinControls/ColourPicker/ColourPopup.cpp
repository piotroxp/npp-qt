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

// Semantic Lift: Win32 ColourPopup → Qt6 ColourPopup implementation
// Original: PowerEditor/src/WinControls/ColourPicker/ColourPopup.cpp
// Target:   npp-qt/src/WinControls/ColourPicker/ColourPopup.cpp
//
// ColourPopup is a modeless dialog that shows the 48-colour NPP palette grid
// and a "Custom Colour..." button that opens QColorDialog.

#include "ColourPopup.h"

// ---------------------------------------------------------------------
// ColourPopup
// ---------------------------------------------------------------------

ColourPopup::ColourPopup(const QColor& defaultColor, QWidget* parent)
    : QDialog(parent), _colour(defaultColor)
{
    setWindowTitle(tr("Pick a Colour"));
    setModal(false);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(280, 310);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(4);
    mainLayout->setContentsMargins(6, 6, 6, 6);

    // --- Preview bar ---------------------------------------------------
    auto* previewLayout = new QHBoxLayout();
    previewLayout->setSpacing(4);

    _previewLabel = new QLabel(this);
    _previewLabel->setFixedSize(60, 22);
    _previewLabel->setFrameShape(QFrame::Box);
    _previewLabel->setStyleSheet(
        QString("background-color: %1; border: 1px solid #888888;").arg(_colour.name()));
    previewLayout->addWidget(_previewLabel);

    _previewTextLabel = new QLabel(this);
    _previewTextLabel->setText(QString("RGB(%1, %2, %3)")
                                   .arg(_colour.red())
                                   .arg(_colour.green())
                                   .arg(_colour.blue()));
    _previewTextLabel->setStyleSheet("color: palette(window-text);");
    previewLayout->addWidget(_previewTextLabel);
    previewLayout->addStretch();

    mainLayout->addLayout(previewLayout);

    // --- Colour grid (6 rows × 8 columns = 48 cells) ------------------
    auto* gridLayout = new QGridLayout();
    gridLayout->setSpacing(2);

    for (int i = 0; i < 48; ++i) {
        auto* cell = new QLabel(this);
        cell->setFixedSize(28, 22);
        cell->setCursor(Qt::PointingHandCursor);
        cell->setToolTip(QString("RGB(%1, %2, %3)")
                             .arg(nppColourItems[i].red())
                             .arg(nppColourItems[i].green())
                             .arg(nppColourItems[i].blue()));
        cell->installEventFilter(this);
        _cells.append(cell);
        gridLayout->addWidget(cell, i / 8, i % 8);
    }

    // Draw all cells initially (none selected)
    for (int i = 0; i < 48; ++i) {
        redrawCell(i, false);
    }

    mainLayout->addLayout(gridLayout);

    // --- Buttons ------------------------------------------------------
    auto* btnLayout = new QHBoxLayout();

    auto* customBtn = new QPushButton(tr("Custom Colour..."), this);
    connect(customBtn, &QPushButton::clicked, this, &ColourPopup::onCustomColorClicked);
    btnLayout->addWidget(customBtn);

    btnLayout->addStretch();

    auto* cancelBtn = new QPushButton(tr("Cancel"), this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addLayout(btnLayout);
}

bool ColourPopup::eventFilter(QObject* watched, QEvent* event)
{
    // Find which cell triggered the event
    int idx = cellIndex(qobject_cast<QLabel*>(watched));
    if (idx < 0)
        return QDialog::eventFilter(watched, event);

    if (event->type() == QEvent::MouseButtonPress) {
        selectColourAtIndex(idx);
        return true;
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        // Single click selection — close after selecting (mirrors Win32 LBN_SELCHANGE)
        return true;
    }
    return QDialog::eventFilter(watched, event);
}

void ColourPopup::selectColourAtIndex(int index)
{
    if (index < 0 || index >= 48)
        return;

    // Deselect old
    if (_selectedIndex >= 0 && _selectedIndex < 48) {
        redrawCell(_selectedIndex, false);
    }

    _selectedIndex = index;
    _colour = nppColourItems[index];

    // Select new
    redrawCell(_selectedIndex, true);
    updatePreview(_colour);

    emit colorSelected(_colour);
    accept();  // close popup after selection (mirrors Win32 LBN_SELCHANGE → display(false))
}

void ColourPopup::onCustomColorClicked()
{
    QColor c = QColorDialog::getColor(_colour, this, tr("Custom Colour"),
                                      QColorDialog::ShowAlphaChannel);
    if (!c.isValid())
        return;

    _colour = c;
    _selectedIndex = -1;  // custom

    // Deselect any previously selected grid cell
    if (_selectedIndex >= 0 && _selectedIndex < 48) {
        redrawCell(_selectedIndex, false);
    }

    updatePreview(_colour);
    emit colorSelected(_colour);
    accept();
}

void ColourPopup::updatePreview(const QColor& colour)
{
    _previewLabel->setStyleSheet(
        QString("background-color: %1; border: 1px solid #888888;").arg(colour.name()));
    _previewTextLabel->setText(QString("RGB(%1, %2, %3)")
                                   .arg(colour.red())
                                   .arg(colour.green())
                                   .arg(colour.blue()));
}

void ColourPopup::redrawCell(int index, bool selected)
{
    if (index < 0 || index >= _cells.size())
        return;

    QLabel* cell = _cells[index];
    const QColor& colour = nppColourItems[index];

    QPixmap pm(cell->size());
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    QRect r(0, 0, cell->width(), cell->height());

    // Fill with colour
    p.fillRect(r, colour);

    // Border: highlight if selected (mirrors Win32 ODS_SELECTED + draw)
    QPen borderPen(selected ? QColor(0, 170, 255) : QColor(136, 136, 136),
                   selected ? 2 : 1);
    p.setPen(borderPen);
    p.drawRect(r.adjusted(0, 0, -1, -1));
    p.end();

    cell->setPixmap(pm);
}

int ColourPopup::cellIndex(QLabel* cell) const
{
    if (!cell)
        return -1;
    for (int i = 0; i < _cells.size(); ++i) {
        if (_cells[i] == cell)
            return i;
    }
    return -1;
}
