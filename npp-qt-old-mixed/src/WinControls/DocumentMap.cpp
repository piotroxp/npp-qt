// Semantic Lift: Win32 DocumentMap → Qt6 DocumentMap
// Original: PowerEditor/src/WinControls/DocumentMap/documentMap.cpp
// Target: npp-qt/src/WinControls/DocumentMap.cpp

#include "DocumentMap.h"
#include "Window.h"
#include "Buffer.h"
#include <QApplication>
#include <QBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QStyle>
#include <QStyleOption>

// =============================================================================
// Helpers
// =============================================================================

static int dpiScale(int base) {
    return static_cast<int>(base * qApp->devicePixelRatio());
}

// =============================================================================
// ViewZoneWidget — static color storage
// Mirrors Win32 static COLORREF ViewZoneDlg::_focus / ::_frost.
// =============================================================================

QColor ViewZoneWidget::_focusColor = QColor(0xFF, 0x80, 0x00);
QColor ViewZoneWidget::_frostColor = QColor(0xFF, 0xFF, 0xFF);

void ViewZoneWidget::setFocusColorStatic(const QColor& col) {
    _focusColor = col;
}

void ViewZoneWidget::setFrostColorStatic(const QColor& col) {
    _frostColor = col;
}

// =============================================================================
// ViewZoneWidget — constructor
// Mirrors Win32 ViewZoneDlg subclassed canvas.
// =============================================================================

ViewZoneWidget::ViewZoneWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setFocusPolicy(Qt::StrongFocus);
    // Background blending
    setAutoFillBackground(false);
}

void ViewZoneWidget::setFocusColor(const QColor& col) {
    if (_focusColor != col) {
        _focusColor = col;
        update();
    }
}

void ViewZoneWidget::setFrostColor(const QColor& col) {
    if (_frostColor != col) {
        _frostColor = col;
        update();
    }
}

void ViewZoneWidget::drawZone(int higherY, int lowerY) {
    _higherY = higherY;
    _lowerY = lowerY;
    update();
}

void ViewZoneWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw frost (white) background
    painter.fillRect(rect(), _frostColor);

    // Draw focus (orange) zone
    if (_lowerY > _higherY)
    {
        QRect zoneRect(0, _higherY, width(), _lowerY - _higherY);
        painter.fillRect(zoneRect, _focusColor);
    }
}

void ViewZoneWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit mouseClicked(event->pos().y());
    }
}

void ViewZoneWidget::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        emit mouseClicked(event->pos().y());
    }
}

void ViewZoneWidget::wheelEvent(QWheelEvent* event) {
    emit mouseWheel(event);
    event->accept();
}

void ViewZoneWidget::keyPressEvent(QKeyEvent* event) {
    // VK_UP / VK_DOWN / VK_PRIOR / VK_NEXT — delegate to parent
    switch (event->key())
    {
    case Qt::Key_Up:
        emit mouseClicked(currentCenterPosY() - viewerHeight());
        break;
    case Qt::Key_Down:
        emit mouseClicked(currentCenterPosY() + viewerHeight());
        break;
    case Qt::Key_PageUp:
        emit mouseClicked(currentCenterPosY() - viewerHeight() * 2);
        break;
    case Qt::Key_PageDown:
        emit mouseClicked(currentCenterPosY() + viewerHeight() * 2);
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

// =============================================================================
// ViewZoneDlg — floating overlay
// Mirrors Win32 ViewZoneDlg (a frameless Tool window).
// =============================================================================

ViewZoneDlg::ViewZoneDlg(QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    // Transparent background so the editor shows through
    setStyleSheet(QStringLiteral("background: transparent;"));

    // Overlay flags
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setWindowFlags(windowFlags() & ~Qt::WindowTitleHint);

    _vzWidget = new ViewZoneWidget(this);
    _vzWidget->setFocusColor(ViewZoneWidget::focusColorStatic());
    _vzWidget->setFrostColor(ViewZoneWidget::frostColorStatic());

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_vzWidget);
    setLayout(layout);

    connect(_vzWidget, &ViewZoneWidget::mouseClicked, this, &ViewZoneDlg::mouseClicked);
    connect(_vzWidget, &ViewZoneWidget::mouseWheel, this, &ViewZoneDlg::wheel);
}

void ViewZoneDlg::init(QWidget* parent) {
    Q_UNUSED(parent);
    // Set parent as top-level for proper window management
}

void ViewZoneDlg::doDialog() {
    show();
    raise();
}

void ViewZoneDlg::display(bool toShow) {
    if (toShow) {
        show();
        raise();
    } else {
        hide();
    }
}

void ViewZoneDlg::drawZone(int higherY, int lowerY) {
    if (_vzWidget)
        _vzWidget->drawZone(higherY, lowerY);
}

void ViewZoneDlg::setTransparency(int alpha) {
    // Qt transparency via window opacity (0.0 - 1.0)
    setWindowOpacity(alpha / 255.0);
}

// =============================================================================
// DocumentSnapshotWidget — floating preview
// Mirrors Win32 DocumentPeeker (IDD_DOCUMENTSNAPSHOT).
// =============================================================================

DocumentSnapshotWidget::DocumentSnapshotWidget(QWidget* parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

void DocumentSnapshotWidget::init(QWidget* parent) {
    Q_UNUSED(parent);
}

void DocumentSnapshotWidget::syncDisplay(Buffer* buf, const ScintillaEditView& scintSource) {
    if (!_pPeekerView)
        return;

    // Sync document pointer
    _pPeekerView->execute(SCI_SETDOCPOINTER, 0, reinterpret_cast<intptr_t>(buf->getDocument()));
    _pPeekerView->setCurrentBuffer(buf);

    // Sync folding
    const std::vector<size_t>& lineStateVector = buf->getHeaderLineState(&scintSource);
    _pPeekerView->syncFoldStateWith(lineStateVector);

    // Sync wrap/scroll
    MapPosition mp = buf->getMapPosition();
    if (mp._firstVisibleLine > 0 || mp._wrapColumn > 0) {
        scrollSnapshotWith(mp, scintSource.getTextZoneWidth());
    }

    _pPeekerView->defineDocType(buf->getLangType());
    _pPeekerView->showMargin(SC_MARGE_FOLDER, false);
    _pPeekerView->showMargin(0, false);
    _pPeekerView->showMargin(1, false);
    _pPeekerView->showMargin(2, false);
    _pPeekerView->showMargin(3, false);
    _pPeekerView->execute(2066, 0);  // SCI_SETCARETSTYLE, CARETSTYLE_INVISIBLE
}

void DocumentSnapshotWidget::scrollSnapshotWith(const MapPosition& mapPos, int textZoneWidth) {
    if (!_pPeekerView)
        return;

    bool changed = false;

    if (mapPos._height != -1 && _rc.bottom() != _rc.top() + mapPos._height) {
        _rc.setBottom(_rc.top() + mapPos._height);
        changed = true;
    }
    if (mapPos._width != -1 && _rc.right() != _rc.left() + mapPos._width) {
        _rc.setRight(_rc.left() + mapPos._width);
        changed = true;
    }

    if (changed) {
        _pPeekerView->resize(_rc.rectWidth(), _rc.rectHeight());
    }

    // Wrapping
    _pPeekerView->wrap(mapPos._isWrap);
    _pPeekerView->execute(SCI_SETWRAPINDENTMODE, mapPos._wrapIndentMode);

    // Reset scroll
    _pPeekerView->execute(SCI_HOMEDISPLAY);

    // Scroll to first visible display line
    _pPeekerView->execute(SCI_LINESCROLL, 0, mapPos._firstVisibleDisplayLine);
}

void DocumentSnapshotWidget::saveCurrentSnapshot(const ScintillaEditView& editView) {
    if (!_pPeekerView)
        return;

    Buffer* buffer = editView.getCurrentBuffer();
    MapPosition mapPos = buffer->getMapPosition();

    mapPos._firstVisibleDisplayLine = editView.execute(SCI_GETFIRSTVISIBLELINE);
    mapPos._firstVisibleDocLine = editView.execute(SCI_DOCLINEFROMVISIBLE, mapPos._firstVisibleDisplayLine);
    mapPos._nbLine = editView.execute(SCI_LINESONSCREEN, mapPos._firstVisibleDisplayLine);
    mapPos._lastVisibleDocLine = editView.execute(SCI_DOCLINEFROMVISIBLE, mapPos._firstVisibleDisplayLine + mapPos._nbLine);

    auto lineHeight = _pPeekerView->execute(SCI_TEXTHEIGHT, mapPos._firstVisibleDocLine);
    mapPos._height = static_cast<intptr_t>(mapPos._nbLine * lineHeight);

    // Width calculation
    QRect editorRect;
    editorRect = editView.getClientRect();
    intptr_t marginWidths = 0;
    for (int m = 0; m < 4; ++m) {
        marginWidths += editView.execute(npp_sci::SCI_GETMARGINWIDTHN, m);
    }
    double editViewWidth = editorRect.width() - marginWidths;
    double editViewHeight = editorRect.height();
    mapPos._width = static_cast<intptr_t>((editViewWidth / editViewHeight) * static_cast<double>(mapPos._height));

    mapPos._wrapIndentMode = editView.execute(SCI_GETWRAPINDENTMODE);
    mapPos._isWrap = editView.isWrap();
    if (editView.isWrap()) {
        mapPos._higherPos = editView.execute(SCI_POSITIONFROMPOINT, 0, 0);
    }

    mapPos._KByteInDoc = editView.getCurrentDocLen() / 1024;
    buffer->setMapPosition(mapPos);
}

intptr_t DocumentSnapshotWidget::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) {
    Q_UNUSED(wParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;
    default:
        break;
    }
    return 0;
}

// =============================================================================
// DocumentMap — constructor
// Mirrors Win32 DocumentMap::DocumentMap().
// =============================================================================

DocumentMap::DocumentMap()
    : DockingDlgInterface(0)
{
    setWindowTitle(QStringLiteral("Document Map"));

    // Create the view zone overlay container
    _vzWidget = new ViewZoneWidget();
    _vzWidget->setFixedHeight(30);

    // Connect view zone signals
    connect(_vzWidget, &ViewZoneWidget::mouseClicked, this, [this](int newPosY) {
        if (!_pMapView || !_ppEditView || !*_ppEditView)
            return;
        int currentCenterPosY = _vzWidget->currentCenterPosY();
        intptr_t pixelPerLine = _pMapView->execute(SCI_TEXTHEIGHT, 0);
        intptr_t jumpDistance = newPosY - currentCenterPosY;
        intptr_t nbLine2jump = jumpDistance / pixelPerLine;
        (*_ppEditView)->execute(SCI_LINESCROLL, 0, nbLine2jump);
        scrollMap();
    });

    connect(_vzWidget, &ViewZoneWidget::mouseWheel, this, [this](QWheelEvent* event) {
        if (_ppEditView && *_ppEditView) {
            // Forward wheel event to the editor
            QCoreApplication::sendEvent((*_ppEditView)->getHSelf(), event);
        }
    });

    // Connect view zone dialog
    connect(&_vzDlg, &ViewZoneDlg::mouseClicked, this, [this](int newPosY) {
        if (!_pMapView || !_ppEditView || !*_ppEditView)
            return;
        int currentCenterPosY = _vzDlg.currentCenterPosY();
        intptr_t pixelPerLine = _pMapView->execute(SCI_TEXTHEIGHT, 0);
        intptr_t jumpDistance = newPosY - currentCenterPosY;
        intptr_t nbLine2jump = jumpDistance / pixelPerLine;
        (*_ppEditView)->execute(SCI_LINESCROLL, 0, nbLine2jump);
        scrollMap();
    });
}

// =============================================================================
// init — panel initialization
// Mirrors Win32 WM_INITDIALOG in DocumentMap.
// =============================================================================

void DocumentMap::init(QApplication*, QWidget*, ScintillaEditView** ppEditView)
{
    _ppEditView = ppEditView;
}

// =============================================================================
// Panel display
// =============================================================================

void DocumentMap::display(bool toShow)
{
    DockingDlgInterface::display(toShow);
    if (toShow) {
        _vzDlg.display();
    } else {
        _vzDlg.display(false);
    }
}

// =============================================================================
// Document synchronization
// Mirrors Win32 DocumentMap::reloadMap() / showInMapTemporarily().
// =============================================================================

void DocumentMap::reloadMap()
{
    if (!_pMapView || !_ppEditView)
        return;

    Document currentDoc = (*_ppEditView)->execute(SCI_GETDOCPOINTER);
    _pMapView->execute(SCI_SETDOCPOINTER, 0, reinterpret_cast<intptr_t>(currentDoc));

    // Sync with current document
    Buffer* editBuf = (*_ppEditView)->getCurrentBuffer();
    _pMapView->setCurrentBuffer(editBuf);

    // Folding
    std::vector<size_t> lineStateVector;
    lineStateVector = (*_ppEditView)->getCurrentFoldStates();
    _pMapView->syncFoldStateWith(lineStateVector);

    // Wrapping
    if ((*_ppEditView)->isWrap() && needToRecomputeWith()) {
        wrapMap();
    }

    scrollMap();
}

void DocumentMap::showInMapTemporarily(Buffer* buf2show, ScintillaEditView* fromEditView)
{
    if (!_pMapView || !fromEditView)
        return;

    _pMapView->execute(SCI_SETDOCPOINTER, 0, reinterpret_cast<intptr_t>(buf2show->getDocument()));
    _pMapView->setCurrentBuffer(buf2show);

    const std::vector<size_t>& lineStateVector = buf2show->getHeaderLineState(fromEditView);
    _pMapView->syncFoldStateWith(lineStateVector);

    if (fromEditView->isWrap() && needToRecomputeWith(fromEditView)) {
        wrapMap(fromEditView);
    }

    MapPosition mp = buf2show->getMapPosition();
    if (mp.isValid())
        scrollMapWith(mp);
}

void DocumentMap::setSyntaxHiliting()
{
    if (!_pMapView)
        return;
    Buffer* buf = _pMapView->getCurrentBuffer();
    _pMapView->defineDocType(buf->getLangType());
    _pMapView->showMargin(SC_MARGE_FOLDER, false);
    _pMapView->showMargin(0, false);
    _pMapView->showMargin(1, false);
    _pMapView->showMargin(2, false);
    _pMapView->showMargin(3, false);
}

bool DocumentMap::needToRecomputeWith(const ScintillaEditView* editView) const
{
    const ScintillaEditView* pEditView = editView ? editView : (_ppEditView ? *_ppEditView : nullptr);
    if (!pEditView)
        return false;

    auto currentZoom = pEditView->execute(SCI_GETZOOM);
    if (_displayZoom != currentZoom)
        return true;

    int currentTextZoneWidth = pEditView->getTextZoneWidth();
    if (_displayWidth != currentTextZoneWidth)
        return true;

    return false;
}

void DocumentMap::initWrapMap()
{
    if (!_pMapView || !_ppEditView)
        return;

    QRect rect = this->rect();
    _pMapView->resize(rect.width(), rect.height());
    _pMapView->wrap(false);
    _pMapView->redraw(true);

    bool isRTL = (*_ppEditView)->isTextDirectionRTL();
    if (_pMapView->isTextDirectionRTL() != isRTL)
        _pMapView->changeTextDirection(isRTL);
}

void DocumentMap::changeTextDirection(bool isRTL)
{
    if (_pMapView)
        _pMapView->changeTextDirection(isRTL);
}

void DocumentMap::wrapMap(const ScintillaEditView* editView)
{
    const ScintillaEditView* pEditView = editView ? editView : (_ppEditView ? *_ppEditView : nullptr);
    if (!pEditView)
        return;

    QRect rect = this->rect();
    if (pEditView->isWrap()) {
        int editZoneWidth = pEditView->getTextZoneWidth();
        _displayWidth = editZoneWidth;
        _displayZoom = pEditView->execute(SCI_GETZOOM);

        int zrIdx = static_cast<int>(_displayZoom) + 10;
        if (zrIdx < 0) zrIdx = 0;
        if (zrIdx >= 30) zrIdx = 29;
        double zr = zoomRatio[zrIdx];

        double docMapWidth = editZoneWidth / zr;

        _pMapView->resize(static_cast<int>(docMapWidth), rect.height());
        _pMapView->wrap(true);
        _pMapView->execute(SCI_SETWRAPINDENTMODE, pEditView->execute(SCI_GETWRAPINDENTMODE));
    }

    doMove();
}

void DocumentMap::scrollMap()
{
    if (!_pMapView || !_ppEditView)
        return;

    QRect rcEditView;
    rcEditView = (*_ppEditView)->getClientRect();
    intptr_t higherPos = (*_ppEditView)->execute(SCI_POSITIONFROMPOINT, 0, 0);
    intptr_t lowerPos = (*_ppEditView)->execute(SCI_POSITIONFROMPOINT, rcEditView.width() - 1, rcEditView.height() - 1);

    _pMapView->execute(SCI_GOTOPOS, higherPos);
    _pMapView->execute(SCI_GOTOPOS, lowerPos);

    QRect rcMapView;
    rcMapView = _pMapView->getClientRect();
    intptr_t higherY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, higherPos);
    intptr_t lowerY = 0;
    intptr_t lineHeightMapView = _pMapView->execute(SCI_TEXTHEIGHT, 0);

    if (!(*_ppEditView)->isWrap()) {
        intptr_t lineHeightEditView = (*_ppEditView)->execute(SCI_TEXTHEIGHT, 0);
        lowerY = higherY + lineHeightMapView * (rcEditView.height()) / lineHeightEditView;
    } else {
        lowerY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, lowerPos) + lineHeightMapView;
    }

    _vzDlg.drawZone(static_cast<int>(higherY), static_cast<int>(lowerY));
}

void DocumentMap::scrollMapWith(const MapPosition& mapPos)
{
    if (!_pMapView)
        return;

    auto firstVisibleDisplayLineMap = _pMapView->execute(SCI_GETFIRSTVISIBLELINE);
    auto firstVisibleDocLineMap = _pMapView->execute(SCI_DOCLINEFROMVISIBLE, firstVisibleDisplayLineMap);
    auto nbLineMap = _pMapView->execute(SCI_LINESONSCREEN, firstVisibleDocLineMap);
    auto lastVisibleDocLineMap = _pMapView->execute(SCI_DOCLINEFROMVISIBLE, firstVisibleDisplayLineMap + nbLineMap);

    intptr_t mapLineToScroll = 0;
    if (lastVisibleDocLineMap < mapPos._lastVisibleDocLine)
        mapLineToScroll = mapPos._lastVisibleDocLine;
    else
        mapLineToScroll = mapPos._firstVisibleDocLine;

    _pMapView->execute(SCI_GOTOLINE, mapLineToScroll);

    intptr_t higherY = 0;
    intptr_t lowerY = 0;
    if (!mapPos._isWrap) {
        auto higherPos = _pMapView->execute(SCI_POSITIONFROMLINE, mapPos._firstVisibleDocLine);
        auto lowerPos = _pMapView->execute(SCI_POSITIONFROMLINE, mapPos._lastVisibleDocLine);
        higherY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, higherPos);
        lowerY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, lowerPos);
        if (lowerY == 0) {
            auto lineHeight = _pMapView->execute(SCI_TEXTHEIGHT, mapPos._firstVisibleDocLine);
            lowerY = mapPos._nbLine * lineHeight + mapPos._firstVisibleDocLine;
        }
    } else {
        higherY = _pMapView->execute(SCI_POINTYFROMPOSITION, 0, mapPos._higherPos);
        auto lineHeight = _pMapView->execute(SCI_TEXTHEIGHT, mapPos._firstVisibleDocLine);
        lowerY = mapPos._nbLine * lineHeight + higherY;
    }

    _vzDlg.drawZone(static_cast<int>(higherY), static_cast<int>(lowerY));
}

void DocumentMap::doMove()
{
    if (!_hwndScintilla || !_vzDlg.isVisible())
        return;

    // Map scintilla coordinates to global
    QRect rc = _hwndScintilla->rect();
    QPoint topLeft = _hwndScintilla->mapToGlobal(rc.topLeft());
    QPoint bottomRight = _hwndScintilla->mapToGlobal(rc.bottomRight());

    // Set view zone to cover the scintilla
    _vzDlg.setGeometry(QRect(topLeft, QSize(bottomRight.x() - topLeft.x(), bottomRight.y() - topLeft.y())));
}

void DocumentMap::fold(size_t line, bool foldOrNot)
{
    if (_pMapView)
        _pMapView->fold(line, foldOrNot, false);
}

void DocumentMap::foldAll(bool mode)
{
    if (_pMapView)
        _pMapView->foldAll(mode);
}

void DocumentMap::scrollMap(bool direction, MoveMode whichMode)
{
    if (!_ppEditView || !*_ppEditView)
        return;

    auto firstVisibleDisplayLine = (*_ppEditView)->execute(SCI_GETFIRSTVISIBLELINE);
    auto nbLine = (*_ppEditView)->execute(SCI_LINESONSCREEN, firstVisibleDisplayLine);
    int nbLine2go = (whichMode == MoveMode::perLine) ? 1 : static_cast<int>(nbLine);
    (*_ppEditView)->execute(SCI_LINESCROLL, 0, direction ? nbLine2go : -nbLine2go);
    scrollMap();
}

void DocumentMap::redraw(bool)
{
    if (_pMapView)
        _pMapView->execute(SCI_COLOURISE, 0, -1);
    update();
}

// =============================================================================
// run_dlgProc — message dispatcher
// Mirrors Win32 DocumentMap::run_dlgProc.
// =============================================================================

intptr_t DocumentMap::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        // Create the minimap scintilla view
        if (_hParent) {
            // NPPM_CREATESCINTILLAHANDLE would be called here when integrated
            // For now: the map scintilla view is created externally
        }

        // Initialize the view zone overlay
        _vzDlg.doDialog();
        _vzDlg.setTransparency(128); // ~50% transparency

        reloadMap();
        setSyntaxHiliting();

        return TRUE;
    }

    case WM_SIZE:
    {
        int width = static_cast<int>(wParam & 0xFFFF);
        int height = static_cast<int>((wParam >> 16) & 0xFFFF);
        Q_UNUSED(width);
        Q_UNUSED(height);

        if (_pMapView)
        {
            if (!_pMapView->isWrap())
                _pMapView->resize(width, height);
            doMove();
        }
        return TRUE;
    }

    case WM_NOTIFY:
    {
        // Dock notifications: DMN_CLOSE, DMN_SWITCHIN, DMN_SWITCHOFF, DMN_FLOATDROPPED
        // These come from the docking manager
        Q_UNUSED(wParam);
        Q_UNUSED(lParam);
        return TRUE;
    }

    case DOCUMENTMAP_SCROLL:
    {
        bool dir = (wParam != 0);
        MoveMode mode = (lParam == 0) ? MoveMode::perLine : MoveMode::perPage;
        scrollMap(dir, mode);
        return TRUE;
    }

    case DOCUMENTMAP_MOUSECLICKED:
    {
        int newPosY = static_cast<int>((lParam >> 16) & 0xFFFF);
        int currentCenterPosY = _vzDlg.currentCenterPosY();
        if (!_pMapView || !_ppEditView || !*_ppEditView)
            return TRUE;
        intptr_t pixelPerLine = _pMapView->execute(SCI_TEXTHEIGHT, 0);
        intptr_t jumpDistance = newPosY - currentCenterPosY;
        intptr_t nbLine2jump = jumpDistance / pixelPerLine;
        (*_ppEditView)->execute(SCI_LINESCROLL, 0, nbLine2jump);
        scrollMap();
        return TRUE;
    }

    case DOCUMENTMAP_MOUSEWHEEL:
    {
        if (_ppEditView && *_ppEditView)
        {
            QWheelEvent event(QPointF(), QPointF(), QPoint(), QPoint(0, static_cast<int>(wParam)),
                Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false, Qt::MouseEventNotSynthesized);
            QCoreApplication::sendEvent((*_ppEditView)->getHSelf(), &event);
        }
        return TRUE;
    }

    default:
        break;
    }
    return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}
