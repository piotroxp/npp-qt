// ColourPicker.cpp — Color swatch button widget implementation
// Converts: PowerEditor/src/WinControls/ColourPicker/ColourPicker.{h,cpp}
// Win32 → Qt6: HWND/Subclassing → QWidget subclass, WM_PAINT → paintEvent,
//              CreateWindowEx → constructor, WM_COMMAND → Qt signals
// Copyright (C) 2026 Agent Army
// GPL v3

#include "ColourPicker.h"
#include "ColourPopup.h"

ColourPicker::ColourPicker(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(25, 25);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMouseTracking(false);
}

ColourPicker::~ColourPicker()
{
    destroyColorPopup();
}

void ColourPicker::setColourEnabled(bool on)
{
    if (_enabled == on)
        return;
    _enabled = on;
    if (!on) {
        // When disabling, use the window background so the diagonal-strike
        // overlay is clearly visible.
        setBackgroundRole(QPalette::Window);
    } else {
        setBackgroundRole(QPalette::NoRole);
    }
    update();
    emit colourPicked(_colour);
}

void ColourPicker::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // ── Background swatch ────────────────────────────────────────────
    if (_enabled) {
        // Draw the colour swatch with a 1px border
        p.fillRect(rect(), _colour);
        // Use a border that contrasts with the swatch colour
        QColor borderCol = (_colour.red() + _colour.green() + _colour.blue() < 200)
                               ? Qt::darkGray
                               : Qt::black;
        QPen border(borderCol);
        border.setWidth(1);
        p.setPen(border);
        p.drawRect(rect().adjusted(0, 0, -1, -1));
    } else {
        // Disabled: draw hatch overlay so the colour is still vaguely
        // visible but clearly marked as disabled.
        p.fillRect(rect(), _colour);
        p.setPen(Qt::NoPen);
        QColor hatch = (_colour.red() + _colour.green() + _colour.blue() < 200)
                           ? Qt::white
                           : Qt::black;
        p.setBrush(QBrush(hatch, Qt::FDiagPattern));
        p.drawRect(rect());
        // Border
        p.setPen(Qt::darkGray);
        p.drawRect(rect().adjusted(0, 0, -1, -1));
    }
}

void ColourPicker::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton) {
        destroyColorPopup();
        QPoint global = mapToGlobal(QPoint(0, height()));
        // Position the popup directly below the button
        global.ry() -= 2;
        _popup = new ColourPopup(_colour);
        _popup->setColourPickedCallback([this](const QColor& c) {
            if (_colour != c) {
                _colour = c;
                update();
            }
            emit colourPicked(c);
        });
        _popup->popupAt(global, this);
        ev->accept();
    } else if (ev->button() == Qt::RightButton) {
        if (_disableRightClick)
            return;
        _enabled = !_enabled;
        setColourEnabled(_enabled);
        emit colourPickedRight(_colour);
        ev->accept();
    }
}

void ColourPicker::mouseDoubleClickEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton) {
        // Double-click opens the custom-colour dialog directly
        QColor picked = QColorDialog::getColor(_colour, this, "Choose Colour",
                                               QColorDialog::ShowAlphaChannel);
        if (picked.isValid() && picked != _colour) {
            _colour = picked;
            update();
            emit colourPicked(_colour);
        }
        ev->accept();
    }
}

void ColourPicker::destroyColorPopup()
{
    if (_popup) {
        _popup->deleteLater();
        _popup = nullptr;
    }
}
