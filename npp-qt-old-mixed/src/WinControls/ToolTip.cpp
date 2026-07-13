// Semantic Lift: Win32 ToolTip → Qt6
// Original: PowerEditor/src/WinControls/ToolTip/*.{h,cpp}
// Target: npp-qt/src/WinControls/ToolTip.cpp

#include "ToolTip.h"
#include <QPainter>
#include <QScreen>
#include <QApplication>

ToolTip::ToolTip(QWidget* parent)
    : QLabel(parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    setMargin(3);

    // Style the tooltip
    setStyleSheet(R"(
        QLabel {
            background-color: #ffffe1;
            color: #000000;
            border: 1px solid #808080;
            padding: 2px 4px;
            font-family: "Segoe UI", "MS Shell Dlg", sans-serif;
            font-size: 8pt;
        }
    )");

    _hideTimer = new QTimer(this);
    _hideTimer->setSingleShot(true);
    connect(_hideTimer, &QTimer::timeout, this, &ToolTip::hideTip);
}

void ToolTip::showTip(const QRect& rect, const QString& text, int xOff, int yOff)
{
    hideTip();
    setText(text);

    // Size to content
    adjustSize();
    int tw = width();
    int th = height();

    QScreen* screen = QApplication::screenAt(rect.topLeft());
    if (!screen) screen = QApplication::primaryScreen();
    QRect screenGeom = screen ? screen->geometry() : QRect(0, 0, 1920, 1080);

    // Position below rect with offset
    int x = rect.left() + xOff;
    int y = rect.bottom() + 2 + yOff;

    // Keep on screen
    if (x + tw > screenGeom.right()) x = screenGeom.right() - tw;
    if (x < screenGeom.left()) x = screenGeom.left();
    if (y + th > screenGeom.bottom()) y = rect.top() - th - 2;

    _trackPos = QPoint(x, y);
    move(x, y);

    show();
    raise();

    // Auto-hide after 5 seconds
    _hideTimer->start(5000);
}

void ToolTip::show()
{
    QLabel::show();
}

void ToolTip::hideTip()
{
    _hideTimer->stop();
    hide();
}

bool ToolTip::isVisible() const
{
    return QLabel::isVisible();
}


