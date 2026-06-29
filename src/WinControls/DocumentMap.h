// Semantic Lift: Win32 DocumentMap → Qt6 DocumentMap
// Original: PowerEditor/src/WinControls/DocumentMap/documentMap.h
// Target: npp-qt/src/WinControls/DocumentMap.h

#pragma once

#include "DockingWnd.h"
#include "StaticDialog.h"
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QApplication>
#include <QPaintEvent>
#include <QDebug>
#include <QPoint>

#include "ScintillaComponent.h"

class Buffer;

// Custom messages for view zone communication (mirrors Win32 DOCUMENTMAP_*)
const unsigned int DOCUMENTMAP_SCROLL       = 0x1000 + 4;
const unsigned int DOCUMENTMAP_MOUSECLICKED = 0x1000 + 5;
const unsigned int DOCUMENTMAP_MOUSEWHEEL    = 0x1000 + 6;

enum class MoveMode {
    perLine,
    perPage
};

// =============================================================================
// ViewZoneWidget — the orange/frost "view zone" overlay widget
// Mirrors Win32 ViewZoneDlg (IDD_VIEWZONE / IDC_VIEWZONECANVAS).
// =============================================================================

class ViewZoneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewZoneWidget(QWidget* parent = nullptr);
    ~ViewZoneWidget() override = default;

    // Colors (mirrors Win32 ViewZoneDlg::setColour)
    void setFocusColor(const QColor& col);
    void setFrostColor(const QColor& col);
    static void setFocusColorStatic(const QColor& col);
    static void setFrostColorStatic(const QColor& col);
    static QColor focusColorStatic() { return _focusColor; }
    static QColor frostColorStatic() { return _frostColor; }

    // Zone drawing (mirrors ViewZoneDlg::drawZone)
    void drawZone(int higherY, int lowerY);

    int viewerHeight() const { return _lowerY - _higherY; }
    int currentCenterPosY() const { return (_lowerY - _higherY) / 2 + _higherY; }

signals:
    void mouseClicked(int posY);
    void mouseWheel(QWheelEvent* event);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    int _higherY = 0;
    int _lowerY = 0;

    // Static color holders (mirrors Win32 static COLORREF)
    static QColor _focusColor;
    static QColor _frostColor;
};

// =============================================================================
// ViewZoneDlg — floating overlay dialog for the view zone
// Mirrors Win32 ViewZoneDlg (a frameless floating dialog).
// =============================================================================

class ViewZoneDlg : public QWidget
{
    Q_OBJECT

public:
    explicit ViewZoneDlg(QWidget* parent = nullptr);
    ~ViewZoneDlg() override = default;

    void init(QWidget* parent);
    void doDialog();

    void display(bool toShow = true);
    bool isVisible() const { return QWidget::isVisible(); }

    void drawZone(int higherY, int lowerY);
    int viewerHeight() const { return _vzWidget ? _vzWidget->viewerHeight() : 0; }
    int currentCenterPosY() const { return _vzWidget ? _vzWidget->currentCenterPosY() : 0; }

    void setFocusColor(const QColor& col) { if (_vzWidget) _vzWidget->setFocusColor(col); }
    void setFrostColor(const QColor& col) { if (_vzWidget) _vzWidget->setFrostColor(col); }

    // Win32 compat: setColour with enum
    enum class ViewZoneColorIndex { focus, frost };
    void setColour(const QColor& col, ViewZoneColorIndex idx) {
        if (idx == ViewZoneColorIndex::focus) setFocusColor(col);
        else setFrostColor(col);
    }

    // Set transparency (mirrors NppParameters::setTransparent)
    void setTransparency(int alpha); // 0-255

signals:
    void mouseClicked(int posY);
    void scroll(int direction, bool perPage);
    void wheel(QWheelEvent* event);

private:
    ViewZoneWidget* _vzWidget = nullptr;
};

// =============================================================================
// DocumentSnapshotWidget — a tiny floating preview pane
// Mirrors Win32 DocumentPeeker (IDD_DOCUMENTSNAPSHOT).
// =============================================================================

class DocumentSnapshotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentSnapshotWidget(QWidget* parent = nullptr);
    ~DocumentSnapshotWidget() override = default;

    void init(QWidget* parent);
    void doDialog(const QPoint& pos, Buffer* buf, const ScintillaEditView& scintSource);
    void syncDisplay(Buffer* buf, const ScintillaEditView& scintSource);
    void scrollSnapshotWith(const MapPosition& mapPos, int textZoneWidth);
    void saveCurrentSnapshot(const ScintillaEditView& editView);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam);

private:
    ScintillaEditView* _pPeekerView = nullptr;
    QRect _rc;
};

// =============================================================================
// DocumentMap — the main Document Map docking panel
// Mirrors Win32 DocumentMap (IDD_DOCUMENTMAP).
// =============================================================================

class DocumentMap : public DockingDlgInterface
{
    Q_OBJECT

public:
    explicit DocumentMap();
    ~DocumentMap() override = default;

    void init(QApplication* app, QWidget* parent, ScintillaEditView** ppEditView);

    // Panel display
    void display(bool toShow = true) override;
    void setParentWindow(QWidget* parent) { _hParent = parent; }

    // Synchronization with editor
    void reloadMap();
    void showInMapTemporarily(Buffer* buf2show, ScintillaEditView* fromEditView);
    void wrapMap(const ScintillaEditView* editView = nullptr);
    void initWrapMap();
    void scrollMap();
    void scrollMap(bool direction, MoveMode whichMode);
    void scrollMapWith(const MapPosition& mapPos);
    void fold(size_t line, bool foldOrNot);
    void foldAll(bool mode);
    void setSyntaxHiliting();
    void changeTextDirection(bool isRTL);
    bool isTemporarilyShowing() const { return _isTemporarilyShowing; }
    void setTemporarilyShowing(bool v) { _isTemporarilyShowing = v; }

    ScintillaEditView** scintillaViewPtr() { return _ppEditView; }

signals:
    void closeRequested();
    void switchIn();
    void switchOff();
    void floatDropped();

public:
    // Win32 compat: isClosed() — checks if panel is hidden/minimized
    bool isClosed() const { return !isVisible(); }
    // Win32 compat: setText(bool) — show/hide panel
    void setText(bool show = true) { display(show); }
    void setText(const QString& /*title*/) { /* stub */ }
    // Win32 compat: ViewZoneDlg color setting (mirrors ViewZoneDlg::setColour)
    void setViewZoneColour(QRgb color, ViewZoneDlg::ViewZoneColorIndex idx) { _vzDlg.setColour(QColor(color), idx); }

public slots:
    void redraw(bool forceUpdate = false);

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;

private:
    bool needToRecomputeWith(const ScintillaEditView* editView = nullptr) const;
    void doMove();

    ScintillaEditView** _ppEditView = nullptr;
    ScintillaEditView* _pMapView = nullptr;
    QWidget* _hwndScintilla = nullptr;
    QWidget* _hParent = nullptr;

    // View zone overlay (mirrors Win32 ViewZoneDlg _vzDlg)
    ViewZoneDlg _vzDlg;
    ViewZoneWidget* _vzWidget = nullptr;

    bool _isTemporarilyShowing = false;

    // Scroll state
    intptr_t _displayZoom = -1;
    intptr_t _displayWidth = 0;

    // DPI-aware defaults
    int _defaultWidth = 150;
    int _defaultHeight = 400;

    // Dock ID string
    QString _dockID = QStringLiteral("DocMap");

    static constexpr double zoomRatio[30] = {
        1, 1, 1, 1, 1.5, 2, 2.5, 2.5, 3.5, 3.5,
        4, 4.5, 5, 5, 5.5, 6, 6.5, 7, 7, 7.5,
        8, 8.5, 8.5, 9.5, 9.5, 10, 10.5, 11, 11, 11.5
    };
};
