// npp-qt: Win32 Qt6 semantic lift  see SEMANTIC_LIFT_MAP.md
#include "DocTabView.h"
#include "ScintillaComponent.h"
#include "Parameters.h"

void DocTabView::init(QWidget* parent, ScintillaComponent* pView, int iconChoice, int buttonsStatus)
{
    _pView = pView;
    TabBarPlus::init(parent, false, false, buttonsStatus);
    if (iconChoice >= 0 && iconChoice < _pIconListVector.size())
        _iconListIndexChoice = iconChoice;
}

void DocTabView::addBuffer(BufferID buffer)
{
    if (buffer == BUFFER_INVALID) return;
    if (getIndexByBuffer(buffer) != -1) return;
    const Buffer* buf = MainFileManager.getBufferByID(buffer);
    if (!buf) return;
    QString name = QString::fromWCharArray(buf->getCompactFileName());
    auto* page = new QWidget(this);
    addTab(page, name);
    setTabData(count() - 1, QVariant::fromValue(static_cast<intptr_t>(buf)));
    bufferUpdated(buf, BufferChangeMask);
}

void DocTabView::closeBuffer(BufferID buffer)
{
    int idx = getIndexByBuffer(buffer);
    if (idx >= 0) removeTab(idx);
}

void DocTabView::bufferUpdated(const Buffer* buffer, int mask)
{
    int index = getIndexByBuffer(buffer->getID());
    if (index == -1) return;
    if (mask & BufferChangeReadonly || mask & BufferChangeDirty) {
        int iconIdx = 0;
        if (buffer->isDirty()) iconIdx = 1;
        else if (buffer->isFromNetwork() || buffer->isMonitoringOn()) iconIdx = 4;
        else if (buffer->getFileReadOnly()) iconIdx = 3;
        else if (buffer->getUserReadOnly()) iconIdx = 2;
        Q_UNUSED(iconIdx);
    }
    if (mask & BufferChangeFilename) {
        QString name = QString::fromWCharArray(buffer->getCompactFileName());
        setTabText(index, name);
    }
}

bool DocTabView::activateBuffer(BufferID buffer)
{
    int idx = getIndexByBuffer(buffer);
    if (idx == -1) return false;
    setCurrentIndex(idx);
    return true;
}

BufferID DocTabView::activeBuffer()
{
    return getBufferByIndex(currentIndex());
}

BufferID DocTabView::findBufferByName(const wchar_t* fullfilename)
{
    for (int i = 0; i < count(); ++i) {
        BufferID bid = static_cast<BufferID>(tabData(i).toLongLong());
        Buffer* buf = MainFileManager.getBufferByID(bid);
        if (buf && QString::fromWCharArray(fullfilename).compare(
                QString::fromWCharArray(buf->getFullPathName()), Qt::CaseInsensitive) == 0)
            return bid;
    }
    return BUFFER_INVALID;
}

int DocTabView::getIndexByBuffer(BufferID id)
{
    for (int i = 0; i < count(); ++i) {
        BufferID bid = static_cast<BufferID>(tabData(i).toLongLong());
        if (bid == id) return i;
    }
    return -1;
}

BufferID DocTabView::getBufferByIndex(size_t index)
{
    if (index >= static_cast<size_t>(count())) return BUFFER_INVALID;
    return static_cast<BufferID>(tabData(index).toLongLong());
}

void DocTabView::setBuffer(size_t index, BufferID id)
{
    if (index >= static_cast<size_t>(count())) return;
    setTabData(index, QVariant::fromValue(static_cast<intptr_t>(id)));
    bufferUpdated(MainFileManager.getBufferByID(id), BufferChangeMask);
}
