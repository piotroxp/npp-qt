// DocumentMapPanel.cpp - Document map/mini-map panel implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "DocumentMapPanel.h"
#include "../editor/ScintillaEditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QScrollBar>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercustom.h>

// -----------------------------------------------------------------------------
// MapTextView implementation
// -----------------------------------------------------------------------------
MapTextView::MapTextView(QWidget* parent)
    : QsciScintilla(parent)
{
    setReadOnly(true);
    setCaretLineVisible(false);
    setMarginWidth(0, 0);
    setMarginWidth(1, 0);
    setMarginWidth(2, 0);
    setFolding(NoFoldStyle, -1);
    setWrapMode(WrapNone);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAutoIndent(false);
    setBraceMatching(NoBraceMatch);
    setWhitespaceVisibility(WsInvisible);
    setEdgeMode(EdgeNone);
    setContextMenuPolicy(Qt::NoContextMenu);
    setFrameShape(QFrame::NoFrame);
    viewport()->setAutoFillBackground(false);
}

void MapTextView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(event->position().toPoint());
    }
    QsciScintilla::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
// DocumentMapPanel
// -----------------------------------------------------------------------------
DocumentMapPanel::DocumentMapPanel(QWidget* parent)
    : QDockWidget("Document Map", parent)
    , _editor(nullptr)
    , _content(nullptr)
    , _mapEditor(nullptr)
    , _viewZone(nullptr)
    , _scrollBar(nullptr)
    , _updateTimer(nullptr)
    , _syncing(false)
    , _lastKnownFirstLine(-1)
{
    setObjectName("DocumentMapPanel");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setMinimumWidth(80);

    _content = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(_content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Minimap on the left, scrollbar on the right
    _mapEditor = new MapTextView(_content);
    layout->addWidget(_mapEditor, 1);

    _scrollBar = new QScrollBar(Qt::Vertical, _content);
    _scrollBar->setMinimum(0);
    _scrollBar->setMaximum(100);
    _scrollBar->setPageStep(10);
    layout->addWidget(_scrollBar);

    // View-zone overlay (highlight current viewport)
    _viewZone = new QRubberBand(QRubberBand::Rectangle, _mapEditor);
    _viewZone->setStyleSheet(
        "QRubberBand { background-color: rgba(80, 120, 255, 70); "
        "border: 1px solid rgba(40, 80, 200, 180); }");

    // Debounce minimap repaints (SCN_PAINTED-equivalent)
    _updateTimer = new QTimer(this);
    _updateTimer->setInterval(50);
    _updateTimer->setSingleShot(true);
    connect(_updateTimer, &QTimer::timeout, this, &DocumentMapPanel::renderMinimap);

    setWidget(_content);
}

DocumentMapPanel::~DocumentMapPanel() = default;

// -----------------------------------------------------------------------------
// setEditor — wire up all signals between main editor and minimap
// -----------------------------------------------------------------------------
void DocumentMapPanel::setEditor(ScintillaEditor* editor) {
    if (_editor) {
        QsciScintilla* qs = _editor->qsciEditor();
        QScrollBar* mainScroll = qs->verticalScrollBar();
        disconnect(mainScroll, &QScrollBar::valueChanged, this, nullptr);
        disconnect(qs, &QsciScintilla::linesChanged, this, nullptr);
        disconnect(qs, &QsciScintilla::textChanged, this, nullptr);
        disconnect(qs, &QsciScintilla::cursorPositionChanged, this, nullptr);
        disconnect(qs, &QsciScintilla::modificationChanged, this, nullptr);
    }

    _editor = editor;
    _lastKnownFirstLine = -1;

    if (!_editor) {
        if (_mapEditor) {
            _mapEditor->setText({});
            _scrollBar->setValue(0);
            _scrollBar->setMaximum(0);
        }
        _viewZone->hide();
        return;
    }

    setupMiniMap();

    QsciScintilla* qs = _editor->qsciEditor();
    QScrollBar* mainScroll = qs->verticalScrollBar();

    // ---- Editor → minimap sync ----
    // Drive minimap from the main editor's scrollbar (SCN_UPDATEUI equivalent)
    connect(mainScroll, &QScrollBar::valueChanged,
            this, &DocumentMapPanel::syncFromEditor);

    // Text changes (including on buffer switch)
    connect(qs, &QsciScintilla::textChanged,
            this, &DocumentMapPanel::scheduleMinimapUpdate);

    // ---- Minimap → editor sync ----
    // Dragging the scrollbar thumb
    connect(_scrollBar, &QScrollBar::valueChanged,
            this, &DocumentMapPanel::syncToEditor);

    // Clicking in the minimap view
    connect(_mapEditor, &MapTextView::clicked,
            this, [this](const QPoint&) { syncToEditor(_scrollBar->value()); });

    // Initial populate
    _mapEditor->setText(_editor->toPlainText());
    updateViewZone();
}

// -----------------------------------------------------------------------------
// onBufferChanged — call this when the active tab changes
// -----------------------------------------------------------------------------
void DocumentMapPanel::onBufferChanged() {
    if (!_editor) return;
    // Reload minimap content with the new buffer text
    _mapEditor->setText(_editor->toPlainText());

    // Copy lexer from main editor so colours match
    QsciScintilla* qs = _editor->qsciEditor();
    if (qs->lexer()) {
        _mapEditor->setLexer(qs->lexer());
    }

    // Reset scroll state
    _lastKnownFirstLine = -1;
    updateViewZone();
    scheduleMinimapUpdate();
}

// -----------------------------------------------------------------------------
// scheduleMinimapUpdate — debounced minimap text refresh
// -----------------------------------------------------------------------------
void DocumentMapPanel::scheduleMinimapUpdate() {
    if (_updateTimer && !_updateTimer->isActive()) {
        _updateTimer->start();
    }
}

// -----------------------------------------------------------------------------
// renderMinimap — repaint the minimap with a tiny font
// -----------------------------------------------------------------------------
void DocumentMapPanel::renderMinimap() {
    if (!_editor || !_mapEditor) return;

    // Sync text (lightweight — avoids full re-highlight on every keystroke)
    if (_mapEditor->text() != _editor->toPlainText()) {
        _mapEditor->setText(_editor->toPlainText());
    }
}

// -----------------------------------------------------------------------------
// setupMiniMap — configure the map editor's appearance
// -----------------------------------------------------------------------------
void DocumentMapPanel::setupMiniMap() {
    if (!_editor || !_mapEditor) return;

    QsciScintilla* mainQsci = _editor->qsciEditor();

    // Use a tiny font for the minimap
    QFont tinyFont = mainQsci->font();
    tinyFont.setPointSizeF(tinyFont.pointSizeF() * 0.35f);
    tinyFont.setPointSize(qMax(3, tinyFont.pointSize()));
    _mapEditor->setFont(tinyFont);

    // Share the same lexer family so colours match the main editor
    if (mainQsci->lexer()) {
        // Note: we share the same lexer instance. This is safe for colour-only
        // sharing; the minimap text is read-only so there are no conflicts.
        _mapEditor->setLexer(mainQsci->lexer());
    }

    // Copy editor settings
    _mapEditor->setIndentationGuides(mainQsci->indentationGuides());
    _mapEditor->setTabWidth(mainQsci->tabWidth());
    _mapEditor->setEolMode(mainQsci->eolMode());
    _mapEditor->setMarginsFont(tinyFont);

    // Set initial text
    _mapEditor->setText(mainQsci->text());
}

// -----------------------------------------------------------------------------
// syncFromEditor — called when the main editor scrolls or the cursor moves
// -----------------------------------------------------------------------------
void DocumentMapPanel::syncFromEditor() {
    if (!_editor || _syncing) return;

    QsciScintilla* qs = _editor->qsciEditor();
    QScrollBar* mainScroll = qs->verticalScrollBar();

    int mainMax = mainScroll->maximum();
    int mainVal = mainScroll->value();
    int mainPage = mainScroll->pageStep();

    // Proportionally drive the minimap's standalone scrollbar
    if (mainMax > 0) {
        int miniMax = _scrollBar->maximum();
        int miniVal = qBound(0, (mainVal * miniMax) / mainMax, miniMax);
        _syncing = true;
        _scrollBar->setValue(miniVal);
        _syncing = false;
    }

    updateViewZone();
}

// -----------------------------------------------------------------------------
// syncToEditor — called when the minimap scrollbar thumb is dragged
// -----------------------------------------------------------------------------
void DocumentMapPanel::syncToEditor(int sliderValue) {
    if (!_editor || _syncing) return;

    QsciScintilla* qs = _editor->qsciEditor();
    QScrollBar* mainScroll = qs->verticalScrollBar();
    int mainMax = mainScroll->maximum();

    if (mainMax <= 0) return;

    int miniMax = _scrollBar->maximum();
    if (miniMax <= 0) return;

    _syncing = true;
    int mainVal = qBound(0, (sliderValue * mainMax) / miniMax, mainMax);
    mainScroll->setValue(mainVal);
    _syncing = false;

    updateViewZone();
}

// -----------------------------------------------------------------------------
// updateViewZone — position the translucent viewport overlay on the minimap
// -----------------------------------------------------------------------------
void DocumentMapPanel::updateViewZone() {
    if (!_editor || !_mapEditor || !_viewZone) return;

    QsciScintilla* qs = _editor->qsciEditor();
    QScrollBar* mainScroll = qs->verticalScrollBar();

    int totalLines = qs->lines();
    int mainMax = mainScroll->maximum();
    int mainVal = mainScroll->value();
    int mainPage = mainScroll->pageStep();

    if (totalLines <= 0 || mainMax <= 0) {
        _viewZone->hide();
        return;
    }

    // Map editor height → proportional slices
    int viewHeight = _mapEditor->viewport()->height();
    int miniMax = _scrollBar->maximum();
    if (miniMax <= 0) {
        _viewZone->hide();
        return;
    }

    // Convert main editor scroll position to minimap pixel position
    // The minimap scrollbar represents totalLines worth of "virtual scroll units"
    int scrollRange = mainMax + mainPage;
    int visibleStart = (mainVal * viewHeight) / qMax(1, scrollRange);
    int visibleEnd   = ((mainVal + mainPage) * viewHeight) / qMax(1, scrollRange);

    int zoneTop    = qMax(0, visibleStart);
    int zoneHeight = qMin(viewHeight - zoneTop, visibleEnd - visibleStart);
    zoneHeight = qMax(4, zoneHeight);

    QRect zoneRect(0, zoneTop, _mapEditor->viewport()->width(), zoneHeight);
    _viewZone->setGeometry(zoneRect);
    _viewZone->show();
}

// -----------------------------------------------------------------------------
// computeEditorFirstVisibleLine — translate scrollbar → first visible line
// -----------------------------------------------------------------------------
int DocumentMapPanel::computeEditorFirstVisibleLine() const {
    if (!_editor) return 0;
    QsciScintilla* qs = _editor->qsciEditor();
    return qs->firstVisibleLine();
}

// -----------------------------------------------------------------------------
// applyEditorFirstVisibleLine — scroll the main editor to a given first line
// -----------------------------------------------------------------------------
void DocumentMapPanel::applyEditorFirstVisibleLine(int line) {
    if (!_editor) return;
    QsciScintilla* qs = _editor->qsciEditor();
    qs->setFirstVisibleLine(line);
}
