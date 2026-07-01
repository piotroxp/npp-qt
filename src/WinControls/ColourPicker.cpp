// Semantic Lift: Win32 ColourPicker → Qt6 implementation
// Original: PowerEditor/src/WinControls/ColourPicker/*.h/*.cpp
// Target:   npp-qt/src/WinControls/ColourPicker.cpp
//
// ColourPopup is defined in ColourPicker/ColourPopup.h (lifted from
// PowerEditor/src/WinControls/ColourPicker/ColourPopup.h/.cpp)

#include "ColourPicker.h"
#include <QDebug>

// =============================================================================
// ColourPicker
// =============================================================================

ColourPicker::ColourPicker(QWidget* parent)
    : Window(parent)
{
    setFixedSize(25, 25);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_TintedBackground, false);
}

void ColourPicker::init(void* hInst, QWidget* hParent)
{
    WindowBase::init(hInst, hParent);
}

void ColourPicker::destroy()
{
    destroyColorPopup();
    deleteLater();
}

void ColourPicker::destroyColorPopup()
{
    if (_pColourPopup) {
        _pColourPopup->hide();
        delete _pColourPopup;
        _pColourPopup = nullptr;
    }
}

void ColourPicker::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    QRect r = rect().adjusted(1, 1, -2, -2);

    // Background — the color swatch
    if (_isEnabled) {
        p.fillRect(r, _currentColour);
    } else {
        // Disabled: show hatching
        p.fillRect(r, Qt::lightGray);
        p.save();
        p.setPen(Qt::darkGray);
        // Diagonal hatch pattern
        for (int i = -r.height(); i < r.width(); i += 4) {
            p.drawLine(r.left() + i, r.top(), r.left(), r.top() + i);
        }
        p.restore();
    }

    // Border
    p.setPen(Qt::darkGray);
    p.drawRect(r.adjusted(0, 0, -1, -1));
}

void ColourPicker::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Single click: show popup on left click down (like Win32 BN_CLICKED)
        showColorPopup();
    } else if (event->button() == Qt::RightButton) {
        if (!_disableRightClick) {
            _isEnabled = !_isEnabled;
            emit rightClicked();
            update();
        }
    }
}

void ColourPicker::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        showColorPopup();
    }
}

void ColourPicker::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}

void ColourPicker::showColorPopup()
{
    destroyColorPopup();

    _pColourPopup = new ColourPopup(_currentColour, this);
    connect(_pColourPopup, &ColourPopup::colorSelected,
            this, [this](const QColor& color) {
                if (_currentColour != color) {
                    _currentColour = color;
                    emit colorPicked(color);
                    update();
                }
            });
    _pColourPopup->show();
}
