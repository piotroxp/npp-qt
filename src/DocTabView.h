// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
#include "TabBar.h"
#include "Buffer.h"
#include <QVector>
#include "WinControls/ImageListSet.h"

class ScintillaComponent;

class DocTabView : public TabBarPlus, virtual public Window
{
    Q_OBJECT
public:
    // Window interface implementation
    QWidget* getHSelf() override { return this; }
    void display(bool show = true) override { show ? QWidget::show() : QWidget::hide(); }
    void show() override { QWidget::show(); }
    void hide() override { QWidget::hide(); }
    int getHeight() const override { return rect().height(); }
    QRect getClientRect() const override { return rect(); }
    void destroy() override { deleteLater(); }
    void init(void* hInst, QWidget* hParent) override { (void)hInst; (void)hParent; }
    void redraw(bool forceUpdate = false) override { update(); if (forceUpdate) repaint(); }

    DocTabView() : TabBarPlus() {}
    ~DocTabView() override {}

    void init(QWidget* parent, ScintillaComponent* pView, int iconChoice, int buttonsStatus);
    void init(void* /*HINSTANCE*/, QWidget* parent, ScintillaComponent* pView, int iconIdx, int buttonStatus) { Q_UNUSED(iconIdx); Q_UNUSED(buttonStatus); init(parent, pView, 0, 0); }
    void destroy(bool withRebuild = false, bool force = false);
    void addBuffer(BufferID buffer);
    void closeBuffer(BufferID buffer);
    void bufferUpdated(const Buffer* buffer, int mask);
    bool activateBuffer(BufferID buffer);
    BufferID activeBuffer();
    BufferID findBufferByName(const wchar_t* fullfilename);
    int getIndexByBuffer(BufferID id);
    BufferID getBufferByIndex(size_t index);
    void setBuffer(size_t index, BufferID id);
    void* getImgLst() { return nullptr; }
    void deletItemAt(int) {}  // Note: typo kept to match call site

signals:
    void bufferActivatedSignal(BufferID buffer);
    void bufferClosedSignal(BufferID buffer);

private:
    ScintillaComponent* _pView = nullptr;
    QVector<IconList*> _pIconListVector;
    int _iconListIndexChoice = -1;
};
