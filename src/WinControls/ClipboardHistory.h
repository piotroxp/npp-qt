// Semantic Lift: Win32 ClipboardHistory → Qt6 QListWidget
// Original: PowerEditor/src/WinControls/ClipboardHistory/*.{h,cpp}
// Target: npp-qt/src/WinControls/ClipboardHistory.h

#pragma once

#include <QWidget>
#include <QListWidget>
#include <QVector>
#include <QString>
#include <QColor>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>

// =============================================================================
// ClipboardDataInfo — clipboard data storage (lifted from clipboardHistoryPanel.h)
// =============================================================================

struct ClipboardDataInfo
{
    QVector<unsigned char> _data;
    bool _isBinaryContained = false;
};

// =============================================================================
// ByteArray — raw byte buffer (lifted from clipboardHistoryPanel.h)
// =============================================================================

class ByteArray
{
public:
    ByteArray() = default;
    explicit ByteArray(const ClipboardDataInfo& cd);

    const unsigned char* getPointer() const { return _pBytes; }
    size_t getLength() const { return _length; }

protected:
    unsigned char* _pBytes = nullptr;
    size_t _length = 0;
};

// =============================================================================
// StringArray — byte buffer that may contain string data (lifted from clipboardHistoryPanel.h)
// =============================================================================

class StringArray : public ByteArray
{
public:
    StringArray(const ClipboardDataInfo& cd, size_t maxLen);
};

// =============================================================================
// ClipboardHistoryPanel — docking clipboard history panel (lifted from clipboardHistoryPanel.h)
// Translates: ClipboardViewer chain, WM_DRAWCLIPBOARD, ListBox custom draw → QClipboard + QListWidget
// =============================================================================

class ClipboardHistoryPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ClipboardHistoryPanel(QWidget* parent = nullptr);
    ~ClipboardHistoryPanel() override;

    void init();
    void setEditor(QWidget* editor);

    // Colors
    void setBackgroundColor(const QColor& bgColor);
    void setForegroundColor(const QColor& fgColor);

    // Track clipboard operations
    bool trackClipboardOps(bool bTrack);
    bool isTrackingClipboardOps() const { return _isTrackingClipboardOps; }

    // Add clipboard data to history
    void addToClipboadHistory(const ClipboardDataInfo& cbd);

signals:
    void clipboardItemClicked(int index);
    void clipboardItemDoubleClicked(int index);

private slots:
    void onClipboardChanged(QClipboard::Mode mode);
    void onItemDoubleClicked(QListWidgetItem* item);
    void onItemClicked(QListWidgetItem* item);

private:
    ClipboardDataInfo getClipboadDataFromSystem();
    int getClipboardDataIndex(const ClipboardDataInfo& cbd) const;

    QListWidget* _listWidget = nullptr;
    QVector<ClipboardDataInfo> _clipboardDataInfos;
    QWidget* _editor = nullptr;
    bool _isTrackingClipboardOps = true;
    QColor _bgColor;
    QColor _fgColor;
};
