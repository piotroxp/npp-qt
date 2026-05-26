// documentMap.h — Qt6 translation: document minimap
#pragma once

#include <QDockWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QWidget>

class ScintillaEditView;
class QLabel;

// View zone (highlighted area in minimap)
class ViewZoneCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit ViewZoneCanvas(QWidget* parent = nullptr);
    void drawZone(int higherY, int lowerY);

    QColor focusColor() const { return _focusColor; }
    QColor frostColor() const { return _frostColor; }
    void setColors(QRgb focus, QRgb frost);

    static QRgb defaultFocusColor() { return qRgb(0xFF, 0x80, 0x00); }
    static QRgb defaultFrostColor() { return qRgb(0xFF, 0xFF, 0xFF); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int _higherY = 0;
    int _lowerY = 100;
    QRgb _focusColor = defaultFocusColor();
    QRgb _frostColor = defaultFrostColor();
};

// Document map panel
class DocumentMap : public QDockWidget
{
    Q_OBJECT

public:
    explicit DocumentMap(QWidget* parent = nullptr);
    ~DocumentMap() override;

    // Init with scintilla view
    void init(ScintillaEditView** ppEditView);

    // Sync with editor
    void reloadMap();
    void scrollMap();
    void scrollMap(bool direction, int mode); // perLine or perPage

    // Wrapping
    void wrapMap();
    void wrapMap(const ScintillaEditView* editView);

    // Temporary display (e.g., other file)
    void showInMapTemporarily(ScintillaEditView* editor);
    void clearTemporaryDisplay();

    // Zoom
    void zoomIn();
    void zoomOut();
    void setZoom(int zoomLevel);

signals:
    void scrollRequested(int direction, int mode);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onScrollBarAction(int action);
    void onEditorScroll();

private:
    ScintillaEditView** _ppEditor = nullptr;
    ScintillaEditView* _mapView = nullptr;
    ViewZoneCanvas* _viewZone = nullptr;
    QScrollBar* _vScroll = nullptr;
    int _zoomLevel = -10;
    bool _isTemporarilyShowing = false;
};
