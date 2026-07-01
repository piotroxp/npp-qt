// ColourPopup stub implementation
// Full implementation lives here; header declares the interface.

#include "ColourPopup.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>

ColourPopup::ColourPopup(const QColor& defaultColor, QWidget* parent)
    : QDialog(parent), _colour(defaultColor)
{
    setWindowTitle(tr("Pick a Color"));
    setModal(false);
    resize(320, 280);

    _mapper = new QSignalMapper(this);
    connect(_mapper, &QSignalMapper::mappedInt, this, &ColourPopup::onColorChosen);

    auto* layout = new QVBoxLayout(this);

    // Colour grid — 8 columns × 6 rows (48 preset colours)
    static const QColor presetColors[] = {
        Qt::black, Qt::darkGray, Qt::gray, Qt::lightGray, Qt::white, Qt::red, Qt::green, Qt::blue,
        Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta,
        Qt::darkYellow, Qt::transparent, Qt::color0, Qt::color1, QColor(255, 127, 127), QColor(127, 255, 127), QColor(127, 127, 255), QColor(127, 255, 255),
        QColor(255, 127, 255), QColor(255, 255, 127), Qt::darkGray, Qt::gray, Qt::darkBlue, Qt::darkGreen, Qt::darkMagenta, Qt::darkCyan,
        Qt::darkYellow, Qt::darkGray, Qt::gray, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta,
        Qt::darkYellow, Qt::darkGray, Qt::gray, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta
    };

    auto* grid = new QGridLayout;
    for (int i = 0; i < 48; ++i) {
        auto* btn = new QPushButton;
        btn->setFixedSize(32, 24);
        btn->setStyleSheet(QString("background-color: %1; border: 1px solid black;")
                               .arg(presetColors[i].name()));
        btn->setProperty("colorIndex", i);
        connect(btn, &QPushButton::clicked, this, [=]() { onColorChosen(i); });
        grid->addWidget(btn, i / 8, i % 8);
    }
    layout->addLayout(grid);

    auto* customBtn = new QPushButton(tr("Custom Color..."));
    connect(customBtn, &QPushButton::clicked, this, [this]() {
        QColor c = QColorDialog::getColor(_colour, this, tr("Custom Color"));
        if (c.isValid()) {
            _colour = c;
            emit colorSelected(c);
            accept();
        }
    });
    layout->addWidget(customBtn);

    auto* cancelBtn = new QPushButton(tr("Cancel"));
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    layout->addWidget(cancelBtn);
}

void ColourPopup::onColorChosen(int colorIndex)
{
    static const QColor presetColors[] = {
        Qt::black, Qt::darkGray, Qt::gray, Qt::lightGray, Qt::white, Qt::red, Qt::green, Qt::blue,
        Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta,
        Qt::darkYellow, Qt::transparent, Qt::color0, Qt::color1, QColor(255, 127, 127), QColor(127, 255, 127), QColor(127, 127, 255), QColor(127, 255, 255),
        QColor(255, 127, 255), QColor(255, 255, 127), Qt::darkGray, Qt::gray, Qt::darkBlue, Qt::darkGreen, Qt::darkMagenta, Qt::darkCyan,
        Qt::darkYellow, Qt::darkGray, Qt::gray, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta,
        Qt::darkYellow, Qt::darkGray, Qt::gray, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta
    };
    if (colorIndex >= 0 && colorIndex < 48) {
        _colour = presetColors[colorIndex];
        emit colorSelected(_colour);
        accept();
    }
}
