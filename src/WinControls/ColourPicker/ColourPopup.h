// Semantic Lift: Win32 ColourPopup → Qt6 stub
// Original: PowerEditor/src/WinControls/ColourPicker/ColourPopup.h
// Target:   npp-qt/src/WinControls/ColourPicker/ColourPopup.h
//
// Minimal stub — ColourPopup is a dialog that shows a 6×8 color grid.
// The full implementation lives in ColourPicker.cpp.

#pragma once

#include <QDialog>
#include <QColor>
#include <QSignalMapper>

class ColourPopup : public QDialog
{
    Q_OBJECT

public:
    explicit ColourPopup(const QColor& defaultColor, QWidget* parent = nullptr);
    ~ColourPopup() override = default;

    QColor selectedColor() const { return _colour; }

signals:
    // Signal emitted when user picks a colour
    void colorSelected(const QColor& color);

public slots:
    // Called when a colour is chosen from the popup
    void onColorChosen(int colorIndex);

private:
    QColor _colour;
    QSignalMapper* _mapper = nullptr;
};
