// ColourPicker.h — Color swatch button widget
// Converts: PowerEditor/src/WinControls/ColourPicker/ColourPicker.{h,cpp}
// Win32 → Qt6: HWND/Subclassing → QWidget subclass, WM_PAINT → paintEvent,
//              CreateWindowEx → constructor, WM_COMMAND → Qt signals
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QWidget>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>

class ColourPopup;  // forward

// Signal emitted when the user picks a colour
#define CPN_COLOURPICKED QColor

class ColourPicker : public QWidget
{
    Q_OBJECT

public:
    ColourPicker(QWidget* parent = nullptr);
    ~ColourPicker() override;

    QColor colour() const { return _colour; }
    void setColour(const QColor& c) { _colour = c; update(); }

    bool isColourEnabled() const { return _enabled; }
    void setColourEnabled(bool on);

    // Disable right-click toggle
    void disableRightClick() { _disableRightClick = true; }

signals:
    void colourPicked(const QColor& colour);
    void colourPickedRight(const QColor& colour);

protected:
    void paintEvent(QPaintEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseDoubleClickEvent(QMouseEvent* ev) override;

private:
    void destroyColorPopup();
    QColor _colour = QColor(0xFF, 0x00, 0x00);  // default red
    bool _enabled = true;
    bool _disableRightClick = false;
    ColourPopup* _popup = nullptr;
};
