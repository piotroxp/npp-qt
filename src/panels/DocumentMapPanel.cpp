// DocumentMapPanel.cpp - Document map/mini-map panel implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "DocumentMapPanel.h"
#include "../editor/ScintillaEditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QScrollBar>
#include <QFont>
#include <QCursor>
#include <QDebug>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>

DocumentMapPanel::DocumentMapPanel(QWidget* parent)
    : QDockWidget("Document Map", parent)
    , _editor(nullptr)
    , _content(nullptr)
    , _mapEditor(nullptr)
    , _viewZone(nullptr)
    , _scrollBar(nullptr)
    , _syncing(false)
{
    setObjectName("DocumentMapPanel");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    _content = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(_content);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Create scrollbar for the minimap
    _scrollBar = new QScrollBar(Qt::Vertical, _content);
    _scrollBar->setMinimum(0);
    _scrollBar->setMaximum(100);
    _scrollBar->setPageStep(10);
    layout->addWidget(_scrollBar);

    setWidget(_content);
}

DocumentMapPanel::~DocumentMapPanel() = default;

void DocumentMapPanel::setEditor(ScintillaEditor* editor) {
    // Disconnect old editor signals
    if (_editor) {
        disconnect(_editor->qsciEditor(), &QsciScintilla::linesChanged, this, &DocumentMapPanel::updateViewZone);
    }

    _editor = editor;

    if (!_editor) {
        // Clear minimap if no editor
        if (_mapEditor) {
            _mapEditor->deleteLater();
            _mapEditor = nullptr;
        }
        if (_viewZone) {
            _viewZone->deleteLater();
            _viewZone = nullptr;
        }
        return;
    }

    setupMiniMap();

    // Connect scroll synchronization
    QsciScintilla* mainQsci = _editor->qsciEditor();
    QScrollBar* mainScroll = mainQsci->verticalScrollBar();

    // When main editor scrolls, update minimap view zone
    connect(mainScroll, &QScrollBar::valueChanged, this, &DocumentMapPanel::onMainEditorScroll);
    connect(mainQsci, &QsciScintilla::linesChanged, this, &DocumentMapPanel::updateViewZone);

    // When minimap scrollbar changes, scroll main editor
    connect(_mapEditor->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &DocumentMapPanel::syncScrollFromMiniMap);

    // When main editor text changes, update minimap
    connect(mainQsci, &QsciScintilla::textChanged, this, [this]() {
        if (_mapEditor && _editor) {
            _mapEditor->setText(_editor->toPlainText());
            updateViewZone();
        }
    });

    // When cursor position changes, update view zone
    connect(mainQsci, &QsciScintilla::cursorPositionChanged, this, &DocumentMapPanel::updateViewZone);

    // Initial update
    updateViewZone();
}

void DocumentMapPanel::setupMiniMap() {
    if (!_editor) return;

    // Get main editor reference
    QsciScintilla* mainQsci = _editor->qsciEditor();

    // Create a new QsciScintilla as the minimap
    _mapEditor = new QsciScintilla(_content);

    // Use the same text as the main editor
    // Note: We keep them synchronized rather than sharing a document
    // This is simpler and avoids API compatibility issues
    _mapEditor->setText(mainQsci->text());

    // Configure minimap for read-only viewing
    _mapEditor->setReadOnly(true);
    _mapEditor->setCaretLineVisible(false);
    _mapEditor->setMarginWidth(0, 0);
    _mapEditor->setMarginWidth(1, 0);
    _mapEditor->setMarginWidth(2, 0);
    _mapEditor->setFolding(QsciScintilla::NoFoldStyle, -1);
    _mapEditor->setWrapMode(QsciScintilla::WrapNone);
    _mapEditor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _mapEditor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _mapEditor->setAutoIndent(false);
    _mapEditor->setBraceMatching(QsciScintilla::NoBraceMatch);
    _mapEditor->setWhitespaceVisibility(QsciScintilla::WsInvisible);
    _mapEditor->setEdgeMode(QsciScintilla::EdgeNone);
    _mapEditor->setContextMenuPolicy(Qt::NoContextMenu);

    // Set a very small font for the minimap
    QFont smallFont = _editor->font();
    smallFont.setPointSizeF(smallFont.pointSizeF() * 0.4);
    smallFont.setPointSize(3);  // Minimum readable size
    if (smallFont.pointSize() < 3) smallFont.setPointSize(3);
    _mapEditor->setFont(smallFont);

    // Copy the same lexer to maintain syntax highlighting colors
    if (mainQsci->lexer()) {
        _mapEditor->setLexer(mainQsci->lexer());
    }

    // Add minimap to layout
    QLayout* layout = _content->layout();
    if (layout) {
        // Insert at the beginning (left of scrollbar)
        layout->addWidget(_mapEditor);
    }

    // Create view zone rubber band
    _viewZone = new QRubberBand(QRubberBand::Rectangle, _mapEditor);
    _viewZone->setStyleSheet("QRubberBand { background-color: rgba(100, 150, 255, 80); border: 1px solid rgba(50, 100, 200, 150); }");

    // Connect minimap scrollbar interaction to scroll main editor
    connect(_mapEditor->verticalScrollBar(), &QSlider::sliderMoved,
            this, &DocumentMapPanel::onMiniMapClicked);
}

void DocumentMapPanel::onMainEditorScroll() {
    if (!_editor || _syncing) return;

    _syncing = true;

    QsciScintilla* mainQsci = _editor->qsciEditor();
    QScrollBar* mainScroll = mainQsci->verticalScrollBar();

    int mainMax = mainScroll->maximum();
    int mainVal = mainScroll->value();

    if (mainMax > 0) {
        // Calculate minimap scrollbar position proportionally
        QScrollBar* miniScroll = _mapEditor->verticalScrollBar();
        int miniMax = miniScroll->maximum();
        int miniVal = (mainVal * miniMax) / mainMax;
        miniScroll->setValue(miniVal);
    }

    updateViewZone();
    _syncing = false;
}

void DocumentMapPanel::syncScrollFromMiniMap() {
    if (!_editor || _syncing) return;

    _syncing = true;

    QsciScintilla* mainQsci = _editor->qsciEditor();
    QScrollBar* mainScroll = mainQsci->verticalScrollBar();
    QScrollBar* miniScroll = _mapEditor->verticalScrollBar();

    int mainMax = mainScroll->maximum();
    int miniVal = miniScroll->value();
    int miniMax = miniScroll->maximum();

    if (miniMax > 0) {
        int mainVal = (miniVal * mainMax) / miniMax;
        mainScroll->setValue(mainVal);
    }

    updateViewZone();
    _syncing = false;
}

void DocumentMapPanel::updateViewZone() {
    if (!_editor || !_mapEditor || !_viewZone) return;

    QsciScintilla* mainQsci = _editor->qsciEditor();
    QScrollBar* mainScroll = mainQsci->verticalScrollBar();

    int mainLines = mainQsci->lines();
    int mainMax = mainScroll->maximum();
    int mainVal = mainScroll->value();
    int mainPage = mainScroll->pageStep();

    if (mainLines <= 0 || mainMax <= 0) {
        _viewZone->hide();
        return;
    }

    // Calculate view zone rectangle
    int viewHeight = _mapEditor->viewport()->height();
    int visibleStart = (mainVal * viewHeight) / (mainMax + mainPage);
    int visibleEnd = ((mainVal + mainPage) * viewHeight) / (mainMax + mainPage);

    int zoneTop = qMax(0, visibleStart);
    int zoneHeight = qMin(viewHeight - zoneTop, visibleEnd - zoneTop);

    // Position view zone
    QRect zoneRect(0, zoneTop, _mapEditor->viewport()->width(), qMax(4, zoneHeight));
    _viewZone->setGeometry(zoneRect);
    _viewZone->show();

    // Highlight current line in minimap
    int currentLine, currentCol;
    mainQsci->getCursorPosition(&currentLine, &currentCol);
    
    // Clear previous line markers
    _mapEditor->setMarkerBackgroundColor(QColor(Qt::transparent), -1);
    
    // Show current line as a marker if visible
    int lineHeight = viewHeight / qMax(1, mainLines);
    int lineTop = (currentLine * viewHeight) / qMax(1, mainLines);
    
    // Create a temporary highlight using indicator
    _mapEditor->setCaretLineVisible(true);
    _mapEditor->setCaretLineBackgroundColor(QColor(255, 255, 0, 60));
}

void DocumentMapPanel::onMiniMapClicked(int sliderValue) {
    Q_UNUSED(sliderValue);
    if (!_editor) return;

    QsciScintilla* mainQsci = _editor->qsciEditor();
    QScrollBar* mainScroll = mainQsci->verticalScrollBar();
    QScrollBar* miniScroll = _mapEditor->verticalScrollBar();

    int miniMax = miniScroll->maximum();
    if (miniMax <= 0) return;

    // Map minimap slider position to main editor position
    int mainMax = mainScroll->maximum();
    int mainVal = (sliderValue * mainMax) / miniMax;

    mainScroll->setValue(qMax(0, qMin(mainVal, mainMax)));
}

void DocumentMapPanel::syncScrollToMiniMap() {
    // Alias for syncScrollFromMiniMap
    syncScrollFromMiniMap();
}
