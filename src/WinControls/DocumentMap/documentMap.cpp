// documentMap.cpp — Qt6 translation
#include "documentMap.h"
#include <QVBoxLayout>
#include <QScrollBar>
#include <QPainter>
#include <QWheelEvent>

// ─── ViewZoneCanvas ──────────────────────────────────────────

ViewZoneCanvas::ViewZoneCanvas(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(20);
    setAutoFillBackground(true);
}

void ViewZoneCanvas::drawZone(int higherY, int lowerY)
{
    _higherY = higherY;
    _lowerY = lowerY;
    update();
}

void ViewZoneCanvas::setColors(QRgb focus, QRgb frost)
{
    _focusColor = focus;
    _frostColor = frost;
    update();
}

void ViewZoneCanvas::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    QRect rect = event->rect();

    // Background
    p.fillRect(rect, QColor::fromRgb(_frostColor));

    // Highlighted zone
    if (_higherY < _lowerY && _lowerY > 0)
    {
        QRect zoneRect(0, _higherY, width(), _lowerY - _higherY);
        p.fillRect(zoneRect.intersected(rect), QColor::fromRgb(_focusColor));
    }
}

// ─── DocumentMap ─────────────────────────────────────────────

DocumentMap::DocumentMap(QWidget* parent)
    : QDockWidget("Document Map", parent)
{
    setObjectName("DocumentMap");
    setAllowedAreas(Qt::AllDockWidgetAreas);

    // Create view zone canvas
    _viewZone = new ViewZoneCanvas(this);

    // Would create minimap scroll area here
    // For now, use a placeholder
    QWidget* content = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_viewZone);
    setWidget(content);
}

DocumentMap::~DocumentMap() = default;

void DocumentMap::init(ScintillaEditView** ppEditor)
{
    _ppEditor = ppEditor;
    // Would create Scintilla view for minimap
}

void DocumentMap::reloadMap()
{
    update();
}

void DocumentMap::scrollMap()
{
    // Sync with editor scroll position
    if (_ppEditor && *_ppEditor)
    {
        // Get editor scroll position and update view zone
        // Simplified: would query Scintilla for first/last visible line
        int firstLine = 0;
        int lastLine = 100;
        _viewZone->drawZone(firstLine, lastLine);
    }
}

void DocumentMap::scrollMap(bool direction, int mode)
{
    Q_UNUSED(direction);
    Q_UNUSED(mode);
    scrollMap();
}

void DocumentMap::wrapMap()
{
    // Would recreate minimap with wrapping
}

void DocumentMap::showInMapTemporarily(ScintillaEditView* editor)
{
    _isTemporarilyShowing = true;
    Q_UNUSED(editor);
}

void DocumentMap::clearTemporaryDisplay()
{
    _isTemporarilyShowing = false;
}

void DocumentMap::wheelEvent(QWheelEvent* event)
{
    QDockWidget::wheelEvent(event);
}

void DocumentMap::mousePressEvent(QMouseEvent* event)
{
    // Jump to clicked position
    int y = event->y();
    emit scrollRequested(y > _viewZone->height() / 2, 0); // PageDown/Up
}

void DocumentMap::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
        mousePressEvent(event);
}

void DocumentMap::resizeEvent(QResizeEvent* event)
{
    QDockWidget::resizeEvent(event);
    scrollMap();
}
