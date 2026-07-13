// Semantic Lift: Win32 ToolTip → Qt6 QToolTip
// Original: PowerEditor/src/WinControls/ToolTip/*.{h,cpp}
// Target: npp-qt/src/WinControls/ToolTip.h

#pragma once

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QEvent>
#include <QTimer>
#include <QApplication>

// =============================================================================
// ToolTip — tooltip widget (lifted from ToolTip.h)
// Translates Win32 TOOLTIPS_CLASS → QLabel overlay with timer
// =============================================================================

class ToolTip : public QLabel
{
    Q_OBJECT

public:
    ToolTip(QWidget* parent = nullptr);
    ~ToolTip() override = default;

    void showTip(const QRect& rect, const QString& text, int xOff = 0, int yOff = 0);
    void show();
    void hideTip();
    bool isVisible() const;

private:
    QTimer* _hideTimer = nullptr;
    QPoint _trackPos;
};
