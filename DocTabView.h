// DocTabView.h - Qt port
#pragma once
#include <QTabWidget>
#include <QMenu>
#include "ScintillaEditView.h"
#include "Buffer.h"
// UT is a thread handle (void* on Linux)
using UT = void*;

class DocTabView : public QTabWidget {
    Q_OBJECT
public:
    DocTabView() : QTabWidget(), _updatingLn(nullptr) {
        setTabsClosable(true);
        tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    }
    void init(QWidget* parent, ScintillaEditView* pEditView, unsigned char buttonsStatus) {
        Q_UNUSED(pEditView);
        Q_UNUSED(buttonsStatus);
    }
    bool addView(BufferID buffer, const wchar_t* utf8FileName, int posIndex, bool isTop);
    bool removeViewAt(int index);
    int findBufferIndex(BufferID buffer);
    BufferID getBufferByIndex(int index) const;
    bool setBuffer(BufferID buffer);
    bool setBuffer(int index);
    bool getCurrentBufferId(BufferID* bufferId) const;
    HWND getViewByIndex(int index) const;
    HWND getCurrentViewHandle() const;
    void setUpdateLnThread(UT) { }
private:
    UT _updatingLn;
    std::map<int, BufferID> _id2Buffer;
};
