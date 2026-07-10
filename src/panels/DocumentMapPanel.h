// DocumentMapPanel.h - Document map/mini-map panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QWidget>
#include <QRubberBand>
#include <QScrollBar>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <Qsci/qsciscintilla.h>

class ScintillaEditor;
class QsciLexer;

// MapTextView: a read-only QsciScintilla used as the minimap canvas.
// Declared here (not in the .cpp) so Qt's MOC can process Q_OBJECT.
class MapTextView : public QsciScintilla {
    Q_OBJECT
public:
    explicit MapTextView(QWidget* parent = nullptr);

signals:
    void clicked(const QPoint& pos);

protected:
    void mousePressEvent(QMouseEvent* event) override;
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
    void syncFromEditor();
    void syncToEditor(int sliderValue);
    void scheduleMinimapUpdate();

private:
    void setupMiniMap();
    void updateViewZone();
    void renderMinimap();
    int computeEditorFirstVisibleLine() const;
    void applyEditorFirstVisibleLine(int line);

    ScintillaEditor* _editor = nullptr;
    QWidget* _content = nullptr;
    MapTextView* _mapEditor = nullptr;
    QRubberBand* _viewZone = nullptr;
    QScrollBar* _scrollBar = nullptr;
    QTimer* _updateTimer = nullptr;
    bool _syncing = false;
    int _lastKnownFirstLine = -1;
};
