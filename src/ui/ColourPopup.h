// ColourPopup.h — Popup colour-grid widget
// Converts: PowerEditor/src/WinControls/ColourPicker/ColourPopup.{h,cpp}
// Win32 → Qt6: IDD_COLOUR_POPUP dialog → QDialog, WM_DRAWITEM → paintEvent,
//              ChooseColor → QColorDialog, LB_ADDSTRING → QGridLayout
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QColor>
#include <QColorDialog>
#include <QVector>
#include <QMouseEvent>
#include <QEnterEvent>

class ColourPopup : public QDialog
{
    Q_OBJECT

public:
    explicit ColourPopup(const QColor& initialColour, QWidget* parent = nullptr);
    ~ColourPopup() override;

    using ColourCallback = std::function<void(const QColor&)>;
    void setColourPickedCallback(ColourCallback cb) { _callback = std::move(cb); }

    // Shows the popup anchored at global point `pos`, with this widget as anchor
    void popupAt(const QPoint& pos, QWidget* anchor);

signals:
    void colourSelected(const QColor& colour);

protected:
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void leaveEvent(QEvent*) override;
    void closeEvent(QCloseEvent* ev) override;
    bool eventFilter(QObject* watched, QEvent* ev) override;

private:
    void buildColourGrid();
    void onColourClicked(int index);
    void onOkClicked();

    static constexpr int COLS = 8;
    static constexpr int ROWS = 6;
    static constexpr int SWATCH_SIZE = 20;

    QColor _currentColour;
    ColourCallback _callback;

    // 48-colour N++ palette
    static const QVector<QColor>& palette();

    QVector<QColor> _swatchColors;
    QVector<QWidget*> _swatches;
    QWidget* _hoveredSwatch = nullptr;

    // "Anchor" widget so we can close when user clicks outside
    QWidget* _anchorWidget = nullptr;
};
