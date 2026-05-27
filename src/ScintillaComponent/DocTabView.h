// DocTabView.h - Qt port
#pragma once
#include <QTabWidget>
#include <QMenu>
#include "ScintillaEditView.h"
#include "Buffer.h"
class DocTabView : public QTabWidget {
    Q_OBJECT
public:
    DocTabView() : QTabWidget() {
        setTabsClosable(true);
        tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    }
    void init(HINSTANCE hInst, HWND hwndApp) {
        Q_UNUSED(hInst);
        Q_UNUSED(hwndApp);
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
    void setUpdateLnThread(UT) {  }
private:
    _updatingLn = nullptr;
    std::map<int, BufferID> _id2Buffer;
};
