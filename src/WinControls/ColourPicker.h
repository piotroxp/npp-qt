// Semantic Lift: Win32 ColourPicker → Qt6 implementation
// Original: PowerEditor/src/WinControls/ColourPicker/*.h/*.cpp
// Target: npp-qt/src/WinControls/ColourPicker.h + .cpp

#pragma once

#include "WinControls/Window.h"
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QColorDialog>
#include <QStandardPaths>

#include "Window.h"

// =============================================================================
// Constants
// =============================================================================

constexpr int IDD_COLOUR_POPUP = 2100;
constexpr int IDC_COLOUR_LIST  = IDD_COLOUR_POPUP + 1;
#define CPN_COLOURPICKED 1

// The 48 preset colors (lifted from ColourPopup.cpp)
static const QColor colourItemsData[48] = {
    QColor(  0,   0,   0), QColor( 64,   0,   0), QColor(128,   0,   0), QColor(128,  64,  64),
    QColor(255,   0,   0), QColor(255, 128, 128), QColor(255, 255, 128), QColor(255, 255,   0),
    QColor(255, 128,  64), QColor(255, 128,   0), QColor(128,  64,   0), QColor(128, 128,   0),
    QColor(128, 128,  64), QColor(  0,  64,   0), QColor(  0, 128,   0), QColor(  0, 255,   0),
    QColor(128, 255,   0), QColor(128, 255, 128), QColor(  0, 255, 128), QColor(  0, 255,  64),
    QColor(  0, 128, 128), QColor(  0, 128,  64), QColor(  0,  64,  64), QColor(128, 128, 128),
    QColor( 64, 128, 128), QColor(  0,   0, 128), QColor(  0,   0, 255), QColor(  0,  64, 128),
    QColor(  0, 255, 255), QColor(128, 255, 255), QColor(  0, 128, 255), QColor(  0, 128, 192),
    QColor(128, 128, 255), QColor(  0,   0, 160), QColor(  0,   0,  64), QColor(192, 192, 192),
    QColor( 64,   0,  64), QColor(128,   0, 128), QColor(128,   0,  64), QColor(255, 255, 255),
    QColor(128, 128, 192), QColor(255, 128, 192), QColor(255, 128, 255), QColor(255,   0, 255),
    QColor(255,   0, 128), QColor(128,   0, 255), QColor( 64,   0, 128), QColor(200, 200, 200),
};

// =============================================================================
// ColourPopup — color grid popup dialog
// =============================================================================

class ColourPopup : public QDialog
{
    Q_OBJECT

public:
    explicit ColourPopup(const QColor& defaultColor, QWidget* parent = nullptr);
    ~ColourPopup() override = default;

    QColor selectedColor() const { return _colour; }

signals:
    void colorSelected(const QColor& color);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void selectColor(const QColor& color);
    QColor colorAt(const QPoint& pt) const;
    QRect colorRect(int index) const;
    void showMoreColors();

    QColor _colour;
    int _selectedIndex = -1;
    int _hoverIndex = -1;
    bool _isMoreColorsShown = false;
};

// =============================================================================
// ColourPicker — button-like color swatch control
// =============================================================================

class ColourPicker : public Window
{
    Q_OBJECT

public:
    ColourPicker(QWidget* parent = nullptr);
    ~ColourPicker() override = default;

    void init(void* hInst, QWidget* hParent) override;
    void destroy() override;

    void setColour(const QColor& c) { _currentColour = c; update(); }
    QColor getColour() const { return _currentColour; }

    bool isEnabled() const { return _isEnabled; }
    void setEnabledPicker(bool enabled) { _isEnabled = enabled; update(); }
    void disableRightClick() { _disableRightClick = true; }

signals:
    void colorPicked(const QColor& color);
    void rightClicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void showColorPopup();
    void destroyColorPopup();

    QColor _currentColour = QColor(255, 0, 0); // red default
    bool _isEnabled = true;
    bool _disableRightClick = false;
    ColourPopup* _pColourPopup = nullptr;
};
