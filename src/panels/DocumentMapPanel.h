// DocumentMapPanel.h - Document map/mini-map panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QWidget>
#include <QRubberBand>
#include <QScrollBar>
#include <QLayout>
#include <QSlider>

class ScintillaEditor;
class QsciScintilla;

class DocumentMapPanel : public QDockWidget {
    Q_OBJECT
public:
    explicit DocumentMapPanel(QWidget* parent = nullptr);
    ~DocumentMapPanel() override;

    void setEditor(ScintillaEditor* editor);
    ScintillaEditor* editor() const { return _editor; }

private slots:
    void onMainEditorScroll();
    void onMiniMapClicked(int sliderValue);
    void syncScrollFromMiniMap();

private:
    void setupMiniMap();
    void updateViewZone();
    void syncScrollToMiniMap();

    ScintillaEditor* _editor = nullptr;
    QWidget* _content = nullptr;
    QsciScintilla* _mapEditor = nullptr;
    QRubberBand* _viewZone = nullptr;
    QScrollBar* _scrollBar = nullptr;
    bool _syncing = false;
    double _zoomFactor = 0.1;  // 10% width for minimap
};
