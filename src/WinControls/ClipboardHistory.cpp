// Semantic Lift: Win32 ClipboardHistory → Qt6 QListWidget
// Original: PowerEditor/src/WinControls/ClipboardHistory/*.{h,cpp}
// Target: npp-qt/src/WinControls/ClipboardHistory.cpp

#include "ClipboardHistory.h"
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QPainter>
#include <QApplication>
#include <QClipboard>
#include <QGuiApplication>

// =============================================================================
// ByteArray
// =============================================================================

ByteArray::ByteArray(const ClipboardDataInfo& cd)
{
    _length = cd._data.size();
    if (_length == 0) {
        _pBytes = nullptr;
        return;
    }
    _pBytes = new unsigned char[_length];
    for (size_t i = 0; i < _length; ++i) {
        _pBytes[i] = cd._data[i];
    }
}

// =============================================================================
// StringArray — truncated display string (max 64 chars)
// =============================================================================

StringArray::StringArray(const ClipboardDataInfo& cd, size_t maxLen)
{
    size_t len = cd._data.size();
    if (len == 0) {
        _pBytes = nullptr;
        _length = 0;
        return;
    }

    bool isCompleted = (len <= maxLen);
    _length = isCompleted ? len : maxLen;

    _pBytes = new unsigned char[_length + (isCompleted ? 0 : 2)];
    for (size_t i = 0; i < _length; ++i) {
        if (!isCompleted && (i == _length - 5 || i == _length - 3 || i == _length - 1))
            _pBytes[i] = 0;
        else if (!isCompleted && (i == _length - 6 || i == _length - 4 || i == _length - 2))
            _pBytes[i] = static_cast<unsigned char>('.');
        else
            _pBytes[i] = cd._data[i];
    }

    if (!isCompleted) {
        _pBytes[_length++] = 0;
        _pBytes[_length++] = 0;
    }
}

// =============================================================================
// ClipboardHistoryPanel
// =============================================================================

ClipboardHistoryPanel::ClipboardHistoryPanel(QWidget* parent)
    : QWidget(parent)
    , _isTrackingClipboardOps(true)
    , _bgColor(Qt::white)
    , _fgColor(Qt::black)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    _listWidget = new QListWidget(this);
    _listWidget->setObjectName(QStringLiteral("IDC_LIST_CLIPBOARD"));
    _listWidget->setAlternatingRowColors(false);
    _listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    _listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Custom item delegate for colored text
    _listWidget->setItemDelegate(nullptr); // Use default for now

    layout->addWidget(_listWidget);
    setLayout(layout);

    // Connect clipboard monitoring
    connect(QApplication::clipboard(), &QClipboard::changed,
            this, &ClipboardHistoryPanel::onClipboardChanged);

    // Item interactions
    connect(_listWidget, &QListWidget::itemDoubleClicked,
            this, &ClipboardHistoryPanel::onItemDoubleClicked);
    connect(_listWidget, &QListWidget::itemClicked,
            this, &ClipboardHistoryPanel::onItemClicked);
}

ClipboardHistoryPanel::~ClipboardHistoryPanel()
{
}

void ClipboardHistoryPanel::init()
{
    // Any additional initialization
}

void ClipboardHistoryPanel::setEditor(QWidget* editor)
{
    _editor = editor;
}

void ClipboardHistoryPanel::setBackgroundColor(const QColor& bgColor)
{
    _bgColor = bgColor;
    QString style = QStringLiteral("QListWidget { background-color: %1; }").arg(bgColor.name());
    _listWidget->setStyleSheet(style);
}

void ClipboardHistoryPanel::setForegroundColor(const QColor& fgColor)
{
    _fgColor = fgColor;
    QString style = QStringLiteral("QListWidget { color: %1; }").arg(fgColor.name());
    _listWidget->setStyleSheet(style);
}

bool ClipboardHistoryPanel::trackClipboardOps(bool bTrack)
{
    bool prev = _isTrackingClipboardOps;
    _isTrackingClipboardOps = bTrack;
    return prev;
}

ClipboardDataInfo ClipboardHistoryPanel::getClipboadDataFromSystem()
{
    ClipboardDataInfo clipboardData;

    const QMimeData* mimeData = QApplication::clipboard()->mimeData();
    if (!mimeData) return clipboardData;

    if (mimeData->hasText()) {
        QString text = mimeData->text();
        // Convert QString to UTF-16LE bytes (like Win32 CF_UNICODETEXT)
        QByteArray utf16;
        utf16.reserve(text.length() * 2);
        const auto* utf16Ptr = reinterpret_cast<const char*>(text.utf16());
        utf16 = QByteArray(utf16Ptr, text.length() * 2);
        for (int i = 0; i < utf16.size(); ++i) {
            clipboardData._data.append(static_cast<unsigned char>(utf16[i]));
        }
    } else if (mimeData->hasFormat(QStringLiteral("application/x-qt-windows-mime;valueformat=\"cf_binary\""))) {
        QByteArray data = mimeData->data(QStringLiteral("application/x-qt-windows-mime;valueformat=\"cf_binary\""));
        for (int i = 0; i < data.size(); ++i) {
            clipboardData._data.append(static_cast<unsigned char>(data[i]));
        }
        clipboardData._isBinaryContained = true;
    }

    return clipboardData;
}

void ClipboardHistoryPanel::onClipboardChanged(QClipboard::Mode mode)
{
    if (mode != QClipboard::Clipboard) return;
    if (!_isTrackingClipboardOps) return;

    ClipboardDataInfo clipboardData = getClipboadDataFromSystem();
    if (clipboardData._data.isEmpty()) return;

    addToClipboadHistory(clipboardData);
}

void ClipboardHistoryPanel::addToClipboadHistory(const ClipboardDataInfo& cbd)
{
    int idx = getClipboardDataIndex(cbd);

    // If already at top, ignore
    if (idx == 0) return;

    // If exists elsewhere, remove old entry
    if (idx > 0) {
        delete _listWidget->takeItem(idx);
        _clipboardDataInfos.remove(idx);
    }

    // Build display string (max 64 chars)
    StringArray sa(cbd, 64);
    QString displayStr;
    if (cbd._isBinaryContained) {
        // Treat as UTF-8
        displayStr = QString::fromUtf8(reinterpret_cast<const char*>(sa.getPointer()), sa.getLength());
    } else {
        // Treat as UTF-16LE (like Win32 CF_UNICODETEXT)
        displayStr = QString::fromUtf16(
            reinterpret_cast<const char16_t*>(sa.getPointer()),
            sa.getLength() / 2
        );
    }

    // Insert at top
    _clipboardDataInfos.prepend(cbd);
    QListWidgetItem* item = new QListWidgetItem(displayStr, _listWidget);
    item->setData(Qt::UserRole, _clipboardDataInfos.size()); // Set actual index for retrieval
    _listWidget->insertItem(0, item);
}

int ClipboardHistoryPanel::getClipboardDataIndex(const ClipboardDataInfo& cbd) const
{
    for (int i = 0; i < _clipboardDataInfos.size(); ++i) {
        const ClipboardDataInfo& existing = _clipboardDataInfos[i];
        if (existing._data.size() == cbd._data.size()) {
            bool found = true;
            for (int j = 0; j < existing._data.size(); ++j) {
                if (existing._data[j] != cbd._data[j]) {
                    found = false;
                    break;
                }
            }
            if (found) return i;
        }
    }
    return -1;
}

void ClipboardHistoryPanel::onItemDoubleClicked(QListWidgetItem* item)
{
    int row = _listWidget->row(item);
    if (row < 0 || row >= _clipboardDataInfos.size()) return;

    const ClipboardDataInfo& cbd = _clipboardDataInfos[row];

    if (!_editor) return;

    // In full implementation, this would:
    // 1. Get encoding from current buffer
    // 2. Convert clipboard data to appropriate encoding
    // 3. Insert at current cursor position using Scintilla

    emit clipboardItemDoubleClicked(row);
}

void ClipboardHistoryPanel::onItemClicked(QListWidgetItem* item)
{
    int row = _listWidget->row(item);
    emit clipboardItemClicked(row);
}
