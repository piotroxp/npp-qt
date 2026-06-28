// Semantic Lift: Win32 MFC → Qt6
// Original: PowerEditor/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.h
// Target: npp-qt/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcherListView.h
//
// NOTE: VerticalFileSwitcherListView functionality has been merged into
// VerticalFileSwitcher itself. This file is kept as a minimal stub to satisfy
// any legacy includes. The list-view operations (newItem, closeItem, activateItem,
// setItemIconStatus, setItemColor, getSelectedFiles, etc.) are implemented
// directly as methods on the VerticalFileSwitcher class using QListWidget.

#pragma once

#include "Window.h"  // forward-declared IWindow interface
#include "Buffer.h"
#include <QListWidget>
#include <QVector>

class VerticalFileSwitcherListView : public QListWidget
{
    Q_OBJECT

public:
    VerticalFileSwitcherListView(QWidget* parent = nullptr);
    ~VerticalFileSwitcherListView() override = default;

    void init(HINSTANCE hInst, HWND parent, HIMAGELIST hImaLst);
    void destroy();
    void initList();

    BufferID getBufferInfoFromIndex(int index, int& view) const;
    int newItem(BufferID bufferID, int iView);
    int closeItem(BufferID bufferID, int iView);
    void activateItem(BufferID bufferID, int iView);
    void setItemIconStatus(BufferID bufferID);
    std::wstring getFullFilePath(size_t i) const;
    void setItemColor(BufferID bufferID);

    void insertColumn(const wchar_t* name, int width, int index);
    void resizeColumns(int totalWidth);
    void deleteColumn(size_t i);

    int nbSelectedFiles() const;
    QVector<BufferViewInfo> getSelectedFiles(bool reverse = false) const;
    void reload();
    void redrawItems();
    void ensureVisibleCurrentItem() const;

    void setBackgroundColor(COLORREF bgColour);
    void setForegroundColor(COLORREF fgColour);

protected:
    int find(BufferID bufferID, int iView) const;
    int add(BufferID bufferID, int iView);
    void remove(int index, bool removeFromListview = true);
    void removeAll();
    void selectCurrentItem() const;

    HIMAGELIST _hImaLst = nullptr;
    int _currentIndex = 0;

    static const int _groupID = 1;
    static const int _group2ID = 2;
};
