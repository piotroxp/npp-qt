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
#include <QApplication>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercustom.h>

// -----------------------------------------------------------------------------
// MapTextView implementation
// -----------------------------------------------------------------------------
MapTextView::MapTextView(QWidget* parent)
    : QsciScintilla(parent)
    , _dragging(false)
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
    setMouseTracking(true);
}

void MapTextView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        _dragStart = event->position().toPoint();
        _dragging = true;
        viewport()->setCursor(Qt::PointingHandCursor);
    }
    QsciScintilla::mousePressEvent(event);
}

void MapTextView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && _dragging) {
        _dragging = false;
        viewport()->unsetCursor();
        QPoint pos = event->position().toPoint();
        if ((pos - _dragStart).manhattanLength() < QApplication::startDragDistance()) {
            emit viewportClicked(pos);
        }
    }
    QsciScintilla::mouseReleaseEvent(event);
}

void MapTextView::wheelEvent(QWheelEvent* event) {
    // Absorb wheel events so they don't scroll the map independently.
    event->accept();
}

// -----------------------------------------------------------------------------
// DocumentMapPanel
// -----------------------------------------------------------------------------
DocumentMapPanel::DocumentMapPanel(QWidget* parent)
    : QDockWidget("Document Map", parent)
    , _editor(nullptr)
    , _content(nullptr)
    , _mapEditor(nullptr)
    , _viewportTimer(nullptr)
    , _debounceTimer(nullptr)
    , _syncing(false)
    , _lastEditorFirstLine(-1)
    , _lastEditorLines(-1)
    , _viewportOverlay(nullptr)
    , _overlayTop(0)
    , _overlayHeight(0)
{
    setObjectName("DocumentMapPanel");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setMinimumWidth(80);

    _content = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(_content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // The minimap — drives its own scrollbar which syncs to the main editor
    _mapEditor = new MapTextView(_content);
    layout->addWidget(_mapEditor, 1);

    // Thin spacer to simulate scrollbar presence
    QWidget* spacer = new QWidget(_content);
    spacer->setFixedWidth(4);
    spacer->setStyleSheet("background: palette(shadow);");
    layout->addWidget(spacer);

    // Overlay widget painted on top of the map to show the viewport region
    _viewportOverlay = new QWidget(_mapEditor);
    _viewportOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    _viewportOverlay->raise();

    // Debounce minimap text refresh on text changes
    _debounceTimer = new QTimer(this);
    _debounceTimer->setInterval(80);
    _debounceTimer->setSingleShot(true);
    connect(_debounceTimer, &QTimer::timeout, this, &DocumentMapPanel::scheduleMinimapUpdate);

    // Timer-driven viewport overlay refresh (~20 fps)
    _viewportTimer = new QTimer(this);
    _viewportTimer->setInterval(50);
    connect(_viewportTimer, &QTimer::timeout, this, &DocumentMapPanel::onViewportTimer);

    setWidget(_content);
}

DocumentMapPanel::~DocumentMapPanel() = default;

// -----------------------------------------------------------------------------
// setEditor — wire up all signals between main editor and minimap
// -----------------------------------------------------------------------------
void DocumentMapPanel::setEditor(ScintillaEditor* editor) {
    // CRITICAL: setEditor() is invoked synchronously from
    // MainWindow::onTabChanged(), which fires synchronously from
    // QTabWidget::setCurrentIndex(). At that instant the newly-selected tab's
    // Scintilla widget may be partially torn down (tab-close race) or not
    // fully initialised, and any synchronous signal it emits would call our
    // callbacks BEFORE we finished wiring. That produced the
    // QsciScintilla::lineIndexFromPosition SIGSEGV inside updateCursorMarker.
    //
    // Strategy:
    //   1. Disconnect signals from the *previous* editor now (cheap, safe —
    //      _editor is a QPointer; if it was already deleted the data() call
    //      returns nullptr and we skip).
    //   2. Defer the new-editor rebind (setupMiniMap + connect + initial sync)
    //      to the next event-loop tick so the new tab's widget is fully
    //      constructed and idle by the time we touch it.
    //   3. The lambda captures the raw pointer; we re-validate via _editor
    //      before doing anything in case the panel was reassigned or the
    //      editor got destroyed in the meantime.
    ScintillaEditor* prev = _editor.data();
    if (prev) {
        QsciScintilla* qs = prev->qsciEditor();
        if (qs) {
            QScrollBar* mainScroll = qs->verticalScrollBar();
            disconnect(mainScroll, &QScrollBar::valueChanged, this, nullptr);
            disconnect(qs, &QsciScintilla::linesChanged, this, nullptr);
            disconnect(qs, &QsciScintilla::textChanged, this, nullptr);
            disconnect(qs, &QsciScintilla::cursorPositionChanged, this, nullptr);
            disconnect(qs, &QsciScintilla::modificationChanged, this, nullptr);
        }
    }
    _editor = editor;
    _lastEditorFirstLine = -1;
    _lastEditorLines = -1;

    QTimer::singleShot(0, this, [this, editor]() {
        // Bail if the panel was reassigned to a different editor meanwhile
        // or if the captured editor was destroyed between schedule and fire.
        if (_editor != editor) return;

        if (!editor) {
            if (_mapEditor) {
                _mapEditor->setText({});
                _mapEditor->SendScintilla(QsciScintillaBase::SCI_SETFIRSTVISIBLELINE, 0);
            }
            _viewportTimer->stop();
            if (_viewportOverlay) _viewportOverlay->hide();
            return;
        }

        QsciScintilla* qs = editor->qsciEditor();
        if (!qs) return;  // editor exists but its Qsci child is gone — skip cleanly

        setupMiniMap();

        QScrollBar* mainScroll = qs->verticalScrollBar();

    // ---- Editor → minimap sync ----
    // Drive minimap scrollbar from the main editor's scrollbar
    connect(mainScroll, &QScrollBar::valueChanged,
            this, &DocumentMapPanel::onEditorScroll);

    // Text changes → debounced minimap text refresh
    connect(qs, &QsciScintilla::textChanged, this, [this]() {
        if (_debounceTimer && !_debounceTimer->isActive())
            _debounceTimer->start();
    });

    // Line count changes (e.g., file loaded)
    connect(qs, &QsciScintilla::linesChanged,
            this, &DocumentMapPanel::onLinesChanged);

    // Cursor position → highlight in minimap
    connect(qs, &QsciScintilla::cursorPositionChanged,
            this, &DocumentMapPanel::onCursorPositionChanged);

    // ---- Minimap → editor sync ----
    // Map scrollbar drives the main editor
    connect(_mapEditor->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &DocumentMapPanel::onMiniMapScroll);

    // Click in minimap → jump to line
    connect(_mapEditor, &MapTextView::viewportClicked,
            this, &DocumentMapPanel::onMiniMapClicked);

    // Initial population
    _mapEditor->setText(qs->text());

    _viewportTimer->start();
    updateViewportOverlay();

    // NOTE (cherry-picked from 1801580 on semantic-lift/wip, kept during
    // 6c9ea11 resolution): do NOT call updateCursorMarker() here. That method
    // calls qs->getCursorPosition(...) which on a freshly-constructed editor
    // dereferences into a QsciScintilla whose vtable hasn't been wired yet —
    // producing a NULL dispatch in lineIndexFromPosition → SIGSEGV. The
    // cursorPositionChanged signal connection above will trigger the marker
    // update naturally once the widget is initialized enough to emit that
    // signal. If the user opens a tab and the cursor never moves (rare), the
    // cursor marker will be one event behind — acceptable cosmetic cost to
    // avoid a hard crash on every tab open.
    //
    // 6c9ea11's full QTimer::singleShot deferral wrapper is intentionally NOT
    // introduced here: that patch wraps the entire setEditor() body in a
    // lambda, but origin/master's tree lacks the matching outer singleShot
    // call, so applying it verbatim would leave an unmatched '});' and break
    // the build. The bug fix (no early updateCursorMarker()) is preserved;
    // the structural deferral would need a separate, hand-written port.
}

// -----------------------------------------------------------------------------
// onBufferChanged — call this when the active tab changes
// -----------------------------------------------------------------------------
void DocumentMapPanel::onBufferChanged() {
    if (!_editor) return;

    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;

    // Reload minimap content with the new buffer text
    _mapEditor->setText(qs->text());

    // Share the same lexer so colours match the main editor
    if (qs->lexer()) {
        _mapEditor->setLexer(qs->lexer());
    }

    _lastEditorFirstLine = -1;
    _lastEditorLines = -1;
    updateViewportOverlay();
    scheduleMinimapUpdate();
}

// -----------------------------------------------------------------------------
// scheduleMinimapUpdate — called by debounce timer; refreshes minimap text
// -----------------------------------------------------------------------------
void DocumentMapPanel::scheduleMinimapUpdate() {
    if (!_editor || !_mapEditor) return;

    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;
    if (_mapEditor->text() != qs->text()) {
        _mapEditor->setText(qs->text());
    }

    // Re-sync scroll position in case line count changed
    syncMapToEditor();
    updateViewportOverlay();
}

// -----------------------------------------------------------------------------
// onLinesChanged — react to line count changes
// -----------------------------------------------------------------------------
void DocumentMapPanel::onLinesChanged() {
    if (!_editor || !_mapEditor) return;
    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;
    _lastEditorLines = qs->lines();
    _mapEditor->setText(qs->text());
    syncMapToEditor();
    updateViewportOverlay();
}

// -----------------------------------------------------------------------------
// onCursorPositionChanged — sync cursor position highlight in the minimap
// -----------------------------------------------------------------------------
void DocumentMapPanel::onCursorPositionChanged(int line, int col) {
    Q_UNUSED(col);
    updateCursorMarker();
    Q_UNUSED(line);
}

void DocumentMapPanel::updateCursorMarker() {
    if (!_editor || !_mapEditor) return;
    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;

    // Get current cursor line from main editor
    int cursorLine = 0;
    qs->getCursorPosition(&cursorLine, nullptr);
    cursorLine = qBound(0, cursorLine, _mapEditor->lines() - 1);

    // Position the caret in the minimap to match — the caret line highlight
    // will show the current position in the map view
    _mapEditor->setCursorPosition(cursorLine, 0);
}

// -----------------------------------------------------------------------------
// setupMiniMap — configure the map editor's appearance
// -----------------------------------------------------------------------------
void DocumentMapPanel::setupMiniMap() {
    if (!_editor || !_mapEditor) return;

    QsciScintilla* mainQsci = _editor->qsciEditor();

    // Use a tiny monospace font for the minimap
    QFont tinyFont = mainQsci->font();
    tinyFont.setPointSizeF(tinyFont.pointSizeF() * 0.38f);
    tinyFont.setPointSize(qMax(3, tinyFont.pointSize()));
    _mapEditor->setFont(tinyFont);

    // Zoom out significantly so the minimap shows many lines
    _mapEditor->zoomTo(-5);

    // Share the lexer from the main editor so colours match
    if (mainQsci->lexer()) {
        _mapEditor->setLexer(mainQsci->lexer());
    }

    // Match display settings
    _mapEditor->setIndentationGuides(mainQsci->indentationGuides());
    _mapEditor->setTabWidth(mainQsci->tabWidth());
    _mapEditor->setEolMode(mainQsci->eolMode());
    _mapEditor->setMarginsFont(tinyFont);

    // Hide all margins
    _mapEditor->setMarginWidth(0, 0);
    _mapEditor->setMarginWidth(1, 0);
    _mapEditor->setMarginWidth(2, 0);
    _mapEditor->SendScintilla(QsciScintillaBase::SCI_SETMARGINS, 0);

    // Initial text
    _mapEditor->setText(mainQsci->text());
    _lastEditorLines = mainQsci->lines();
}

// -----------------------------------------------------------------------------
// onEditorScroll — editor scrolled → drive the minimap scrollbar
// -----------------------------------------------------------------------------
void DocumentMapPanel::onEditorScroll(int newValue) {
    if (!_editor || !_mapEditor || _syncing) return;

    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;
    QScrollBar* mainScroll = qs->verticalScrollBar();

    int mainMax = mainScroll->maximum();
    if (mainMax <= 0) return;

    // Proportionally map main scroll position → minimap scroll position
    int miniMax = _mapEditor->verticalScrollBar()->maximum();
    int miniVal = qBound(0, (newValue * miniMax) / mainMax, miniMax);

    _syncing = true;
    _mapEditor->verticalScrollBar()->setValue(miniVal);
    _syncing = false;

    _lastEditorFirstLine = qs->firstVisibleLine();
    updateViewportOverlay();
}

// -----------------------------------------------------------------------------
// onMiniMapScroll — minimap scrollbar dragged → drive the main editor
// -----------------------------------------------------------------------------
void DocumentMapPanel::onMiniMapScroll(int miniVal) {
    if (!_editor || _syncing) return;

    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;
    QScrollBar* mainScroll = qs->verticalScrollBar();

    int mainMax = mainScroll->maximum();
    if (mainMax <= 0) return;

    int miniMax = _mapEditor->verticalScrollBar()->maximum();
    if (miniMax <= 0) return;

    // Map minimap scroll position → main editor scroll position
    int mainVal = qBound(0, (miniVal * mainMax) / miniMax, mainMax);

    _syncing = true;
    mainScroll->setValue(mainVal);
    _syncing = false;
}

// -----------------------------------------------------------------------------
// onMiniMapClicked — minimap clicked → jump to the clicked line in the editor
// -----------------------------------------------------------------------------
void DocumentMapPanel::onMiniMapClicked(const QPoint& pos) {
    if (!_editor || !_mapEditor) return;

    int miniTotalLines = _mapEditor->lines();
    int miniVisibleLines = _mapEditor->SendScintilla(QsciScintillaBase::SCI_LINESONSCREEN);
    int miniFirstLine = _mapEditor->firstVisibleLine();

    if (miniTotalLines <= 0 || miniVisibleLines <= 0) return;

    // Map click y-coordinate → line number in minimap
    int clickLineInDoc = miniFirstLine;
    int lineHeight = _mapEditor->SendScintilla(QsciScintillaBase::SCI_TEXTHEIGHT, 0);
    if (lineHeight > 0) {
        clickLineInDoc = miniFirstLine + (pos.y() / lineHeight);
    } else {
        int viewHeight = _mapEditor->viewport()->height();
        clickLineInDoc = miniFirstLine + (pos.y() * miniVisibleLines) / qMax(1, viewHeight);
    }
    clickLineInDoc = qBound(0, clickLineInDoc, miniTotalLines - 1);

    // Jump the main editor to that line
    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;
    _syncing = true;
    qs->setFirstVisibleLine(clickLineInDoc);
    qs->setCursorPosition(clickLineInDoc, 0);
    _syncing = false;

    syncMapToEditor();
    updateViewportOverlay();
}

// -----------------------------------------------------------------------------
// syncMapToEditor — make the minimap's scrollbar match the main editor's
// -----------------------------------------------------------------------------
void DocumentMapPanel::syncMapToEditor() {
    if (!_editor || !_mapEditor || _syncing) return;

    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;
    QScrollBar* mainScroll = qs->verticalScrollBar();

    int mainMax = mainScroll->maximum();
    if (mainMax <= 0) return;

    int miniMax = _mapEditor->verticalScrollBar()->maximum();
    if (miniMax <= 0) return;

    int miniVal = _mapEditor->verticalScrollBar()->value();
    int mainVal = mainScroll->value();

    int expectedMini = qBound(0, (mainVal * miniMax) / mainMax, miniMax);
    if (miniVal != expectedMini) {
        _syncing = true;
        _mapEditor->verticalScrollBar()->setValue(expectedMini);
        _syncing = false;
    }

    _lastEditorFirstLine = qs->firstVisibleLine();
}

// -----------------------------------------------------------------------------
// syncEditorToMapFirstLine — scroll the main editor to match minimap first line
// -----------------------------------------------------------------------------
void DocumentMapPanel::syncEditorToMapFirstLine(int miniFirstLine) {
    if (!_editor) return;
    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;

    int mainMax = qs->verticalScrollBar()->maximum();
    int miniMax = _mapEditor->verticalScrollBar()->maximum();
    if (mainMax <= 0 || miniMax <= 0) return;

    int mainFirstLine = qBound(0, (miniFirstLine * mainMax) / miniMax, mainMax);
    _syncing = true;
    qs->SendScintilla(QsciScintillaBase::SCI_SETFIRSTVISIBLELINE, mainFirstLine);
    _syncing = false;
}

// -----------------------------------------------------------------------------
// onViewportTimer — periodically update the viewport overlay
// -----------------------------------------------------------------------------
void DocumentMapPanel::onViewportTimer() {
    if (!_editor || !_mapEditor) return;
    updateViewportOverlay();
}

// -----------------------------------------------------------------------------
// updateViewportOverlay — paint the translucent viewport region on the minimap
// -----------------------------------------------------------------------------
void DocumentMapPanel::updateViewportOverlay() {
    if (!_editor || !_mapEditor) return;

    QsciScintilla* qs = _editor->qsciEditor();
    if (!qs) return;
    int totalLines = qs->lines();
    if (totalLines <= 0) {
        _viewportOverlay->hide();
        return;
    }

    int miniTotalLines = _mapEditor->lines();
    int miniVisibleLines = _mapEditor->SendScintilla(QsciScintillaBase::SCI_LINESONSCREEN);
    int miniFirstLine = _mapEditor->firstVisibleLine();

    if (miniTotalLines <= 0 || miniVisibleLines <= 0) {
        _viewportOverlay->hide();
        return;
    }

    // Compute minimap line height
    int lineHeight = _mapEditor->SendScintilla(QsciScintillaBase::SCI_TEXTHEIGHT, 0);
    if (lineHeight <= 0) lineHeight = 14;

    int mapHeight = _mapEditor->viewport()->height();

    // Calculate what portion of the document the main editor shows
    int mainVisibleLines = qs->SendScintilla(QsciScintillaBase::SCI_LINESONSCREEN);
    if (mainVisibleLines <= 0) mainVisibleLines = 1;

    float docFraction = float(mainVisibleLines) / float(qMax(1, totalLines));
    int miniVisibleRegion = qMax(1, int(docFraction * miniTotalLines));

    // Position the overlay based on minimap scroll ratio
    int visibleRange = qMax(1, miniTotalLines - miniVisibleRegion);
    float scrollRatio = float(miniFirstLine) / float(visibleRange);
    int overlayY = int(scrollRatio * mapHeight);
    overlayY = qBound(0, overlayY, mapHeight - 1);

    int overlayH = qMax(4, miniVisibleRegion * lineHeight);
    overlayH = qMin(overlayH, mapHeight - overlayY);

    QRect newRect(0, overlayY, _mapEditor->viewport()->width(), overlayH);
    if (_viewportOverlay->geometry() != newRect) {
        _viewportOverlay->setGeometry(newRect);
        _viewportOverlay->show();
    }

    // Paint semi-transparent fill with border
    QPainter painter(_viewportOverlay);
    painter.fillRect(_viewportOverlay->rect(), QColor(80, 120, 255, 50));
    QPen pen(QColor(40, 80, 200, 180));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawRect(_viewportOverlay->rect().adjusted(0, 0, -1, -1));

    _overlayTop = overlayY;
    _overlayHeight = overlayH;
}
