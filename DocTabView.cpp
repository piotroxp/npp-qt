// DocTabView.cpp - Qt port
#include "DocTabView.h"
#include <QTabBar>
DocTabView::DocTabView() : QTabWidget() {
    setTabsClosable(true);
    tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
}

bool DocTabView::init(QWidget* parent, ScintillaEditView* pEditView, unsigned char buttonsStatus) {
    Q_UNUSED(buttonsStatus);
    if (pEditView) {
        addTab(pEditView, QString());
    }
    return true;
}
bool DocTabView::addView(BufferID buffer, const wchar_t* utf8FileName, int posIndex, bool isTop) {
    auto* sci = new ScintillaEditView();
    addTab(sci, QString::fromUtf16(reinterpret_cast<const ushort*>(utf8FileName)));
    return true;
}
bool DocTabView::removeViewAt(int index) {
    if (index >= 0 && index < count()) {
        removeTab(index);
        return true;
    }
    return false;
}
int DocTabView::findBufferIndex(BufferID buffer) {
    for (int i = 0; i < count(); ++i) {
        if (_id2Buffer[i] == buffer) return i;
    }
    return -1;
}
BufferID DocTabView::getBufferByIndex(int index) const {
    auto it = _id2Buffer.find(index);
    return (it != _id2Buffer.end()) ? it->second : BUFFER_INVALID;
}
bool DocTabView::setBuffer(BufferID buffer) {
    int idx = findBufferIndex(buffer);
    if (idx >= 0) {
        setCurrentIndex(idx);
        return true;
    }
    return false;
}
bool DocTabView::setBuffer(int index) {
    if (index >= 0 && index < count()) {
        setCurrentIndex(index);
        return true;
    }
    return false;
}
bool DocTabView::getCurrentBufferId(BufferID* bufferId) const {
    if (!bufferId) return false;
    *bufferId = getBufferByIndex(currentIndex());
    return *bufferId != BUFFER_INVALID;
}
HWND DocTabView::getViewByIndex(int index) const {
    if (index >= 0 && index < count()) {
        auto* w = widget(index);
        return w ? w->winId() : nullptr;
    }
    return nullptr;
}
HWND DocTabView::getCurrentViewHandle() const {
    return getViewByIndex(currentIndex());
}
