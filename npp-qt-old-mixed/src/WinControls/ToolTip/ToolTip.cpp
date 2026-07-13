// Semantic Lift: ToolTip — Win32 TTM_ADDTOOL + WM_NOTIFY → Qt6 QLabel overlay
// Original: PowerEditor/src/WinControls/ToolTip/ToolTip.cpp (69 lines)
// Win32 → Qt6: TOOLINFO + TTM_RELOPENV → QLabel with QTimer auto-hide

#include "ToolTip.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QStyleOptionFrame>
#include <QStyle>

ToolTip* ToolTip::_pSelf = nullptr;

ToolTip::ToolTip(QWidget* parent)
    : QLabel(parent, Qt::FramelessWindowHint | Qt::ToolTip)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setTextFormat(Qt::PlainText);
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    _pSelf = this;
    _hideTimer = new QTimer(this);
    _hideTimer->setSingleShot(true);
    connect(_hideTimer, &QTimer::timeout, this, &ToolTip::hideTooltip);
}

ToolTip::~ToolTip()
{
    _pSelf = nullptr;
}

void ToolTip::init(QWidget* parent)
{
    _parentWidget = parent;
    if (parent)
        parent->installEventFilter(this);
}

ToolTip* ToolTip::getInstance()
{
    if (!_pSelf) {
        _pSelf = new ToolTip(nullptr);
    }
    return _pSelf;
}

void ToolTip::addToToolTip(void* hwnd, const QString& text)
{
    Q_UNUSED(hwnd);
    Q_UNUSED(text);
    // In Qt6, tooltips are set via QWidget::setToolTip()
    // This is a custom tooltip for special cases
}

void ToolTip::delFromToolTip(void* hwnd)
{
    Q_UNUSED(hwnd);
}

void ToolTip::showTooltip(const QString& text, int x, int y)
{
    setText(text);

    // Size to content
    adjustSize();
    int tw = width();
    int th = height();

    // Keep on screen
    QDesktopWidget* desktop = QApplication::desktop();
    if (x + tw > desktop->width())
        x = desktop->width() - tw - 4;
    if (y + th > desktop->height())
        y = desktop->height() - th - 4;

    move(x, y);
    show();
    raise();

    // Auto-hide after 5 seconds
    _hideTimer->start(5000);
}

void ToolTip::hideTooltip()
{
    hide();
}

bool ToolTip::eventFilter(QObject* obj, QEvent* event)
{
    Q_UNUSED(obj);
    if (event->type() == QEvent::Leave) {
        hideTooltip();
    }
    return false;
}

void ToolTip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);

    // Draw styled frame (like native tooltips)
    QStyleOptionFrame opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_PanelTipLabel, &opt, &p, this);

    // Draw text
    QRect textRect = this->rect().adjusted(4, 2, -4, -2);
    p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextExpandTabs, text());
}
