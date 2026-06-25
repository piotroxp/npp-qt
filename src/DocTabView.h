// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#pragma once
#include "TabBar.h"
#include "Buffer.h"
#include <QVector>
#include "WinControls/ImageListSet.h"

class ScintillaComponent;

class DocTabView : public TabBarPlus
{
    Q_OBJECT
public:
    DocTabView() : TabBarPlus() {}
    ~DocTabView() override {}

    void init(QWidget* parent, ScintillaComponent* pView, int iconChoice, int buttonsStatus);
    void addBuffer(BufferID buffer);
    void closeBuffer(BufferID buffer);
    void bufferUpdated(const Buffer* buffer, int mask);
    bool activateBuffer(BufferID buffer);
    BufferID activeBuffer();
    BufferID findBufferByName(const wchar_t* fullfilename);
    int getIndexByBuffer(BufferID id);
    BufferID getBufferByIndex(size_t index);
    void setBuffer(size_t index, BufferID id);

signals:
    void bufferActivatedSignal(BufferID buffer);
    void bufferClosedSignal(BufferID buffer);

private:
    ScintillaComponent* _pView = nullptr;
    QVector<IconList*> _pIconListVector;
    int _iconListIndexChoice = -1;
};
