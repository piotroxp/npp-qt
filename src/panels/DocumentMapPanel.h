// DocumentMapPanel.h - Document map/mini-map panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPen>
#include <QPointer>
#include <Qsci/qsciscintilla.h>

// Needed for QPointer<ScintillaEditor> and the inline editor() accessor:
// QPointer's data() casts QObject* -> T*, which requires T to be a complete
// type so the compiler can see the inheritance.
#include "editor/ScintillaEditor.h"

class QsciLexer;

// MapTextView: a read-only QsciScintilla used as the minimap canvas.
// Declared here (not in the .cpp) so Qt's MOC can process Q_OBJECT.
class MapTextView : public QsciScintilla {
    Q_OBJECT
public:
    explicit MapTextView(QWidget* parent = nullptr);

signals:
    // Emitted on left-click with the click position in viewport coordinates
    void viewportClicked(const QPoint& pos);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    QPoint _dragStart;
    bool _dragging = false;
};

class DocumentMapPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit DocumentMapPanel(QWidget* parent = nullptr);
    ~DocumentMapPanel() override;

    void setEditor(ScintillaEditor* editor);
    ScintillaEditor* editor() const { return _editor; }

    // Called when the active buffer changes externally (e.g., tab switch)
    void onBufferChanged();

public slots:
    void scheduleMinimapUpdate();

private slots:
    // Editor → minimap sync
    void onEditorScroll(int newValue);
    // Minimap → editor sync
    void onMiniMapScroll(int sliderValue);
    // Minimap click → jump
    void onMiniMapClicked(const QPoint& pos);
    // Periodic viewport overlay refresh
    void onViewportTimer();
    // Document line count changed
    void onLinesChanged();
    // Cursor moved → highlight in minimap
    void onCursorPositionChanged(int line, int col);

private:
    // Legacy slots kept for API compatibility (wired up by callers)
    void syncFromEditor();
    void syncToEditor(int sliderValue);
    void updateViewZone();

    void setupMiniMap();
    void updateViewportOverlay();
    void updateCursorMarker();
    void syncMapToEditor();
    void syncEditorToMapFirstLine(int miniFirstLine);

    // QPointer so we can null-check safely after a tab-close that deleted the
    // underlying ScintillaEditor mid-method (see SIGSEGV in
    // QsciScintilla::lineIndexFromPosition from updateCursorMarker).
    QPointer<ScintillaEditor> _editor;
    QWidget* _content = nullptr;
    MapTextView* _mapEditor = nullptr;
    QTimer* _viewportTimer = nullptr;
    QTimer* _debounceTimer = nullptr;
    bool _syncing = false;
    int _lastEditorFirstLine = -1;
    int _lastEditorLines = -1;

    // Overlay widget painted on top of the minimap showing viewport region
    QWidget* _viewportOverlay = nullptr;
    int _overlayTop = 0;
    int _overlayHeight = 0;
};
