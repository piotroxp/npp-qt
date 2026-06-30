// Semantic Lift: Win32 clipboardHistoryPanel.cpp → Qt6 clipboardHistoryPanel.cpp
// Original:  PowerEditor/src/WinControls/ClipboardHistory/clipboardHistoryPanel.cpp
// Target:    npp-qt/src/WinControls/ClipboardHistory/clipboardHistoryPanel.cpp
//
// Translates:
//   SetClipboardViewer / WM_CHANGECBCHAIN / WM_DRAWCLIPBOARD
//       → QApplication::clipboard()->changed signal
//   CreateWindowEx(WC_LISTBOX)    → new QListWidget(parent)
//   SendDlgItemMessage(LB_*)      → QListWidget API
//   WM_DRAWITEM / drawItem()      → ClipboardListDelegate::paint()
//   LBN_DBLCLK → paste            → QListWidget::itemDoubleClicked
//   HMENU context menus           → QMenu with QAction
//   WM_SIZE                       → resizeEvent()
//   WM_CTLCOLORLISTBOX           → stylesheet on _listWidget
//   WideCharToMultiByte + SCI_*   → QString conversion + Scintilla execute

#include "ClipboardHistory.h"

#include <QApplication>
#include <QClipboard>
#include <QStyleOptionViewItem>
#include <QTextOption>
#include <QPainter>
#include <QPaintDevice>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QScrollBar>
#include <QByteArray>
#include <QGuiApplication>

#include "ScintillaComponent.h"
#include "MISC/Common/clipboardFormats.h"
#include "NppDarkMode.h"
#include "Scintilla.h"

// =============================================================================
// ByteArray — copies ClipboardDataInfo bytes into a heap buffer
// Win32:  new unsigned char[GlobalAlloc] + memcpy from HGLOBAL data
// Qt6:    heap-allocated unsigned char[] + copy from QVector
// =============================================================================
ByteArray::ByteArray(const ClipboardDataInfo& cd)
{
    _length = cd._data.size();
    if (!_length) {
        _pBytes = nullptr;
        return;
    }
    _pBytes = new unsigned char[_length];
    for (size_t i = 0; i < _length; ++i) {
        _pBytes[i] = cd._data[i];
    }
}

// =============================================================================
// StringArray — display string truncated to maxLen, with "..." suffix
// Win32:  nulls last few bytes, appends 0 0 at end
// Qt6:    same logic — bytes at positions len-5, len-3, len-1 become 0;
//         bytes at len-6, len-4, len-2 become '.'
// =============================================================================
StringArray::StringArray(const ClipboardDataInfo& cd, size_t maxLen)
{
    size_t len = cd._data.size();

    if (!len) {
        _pBytes = nullptr;
        _length = 0;
        return;
    }

    bool isCompleted = (len <= maxLen);
    _length = isCompleted ? len : maxLen;

    _pBytes = new unsigned char[_length + (isCompleted ? 0 : 2)];
    size_t i = 0;
    for (; i < _length; ++i) {
        if (!isCompleted && (i == _length - 5 || i == _length - 3 || i == _length - 1)) {
            _pBytes[i] = 0;
        } else if (!isCompleted && (i == _length - 6 || i == _length - 4 || i == _length - 2)) {
            _pBytes[i] = static_cast<unsigned char>('.');
        } else {
            _pBytes[i] = cd._data[i];
        }
    }

    if (!isCompleted) {
        _pBytes[i++] = 0;
        _pBytes[i] = 0;
    }
}

// =============================================================================
// ClipboardListDelegate::paint — custom item rendering
// Win32:  SetTextColor + SetBkColor + DrawText in WM_DRAWITEM handler
// Qt6:    QPalette-based background + drawText for foreground
// =============================================================================
void ClipboardListDelegate::paint(QPainter* painter,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const
{
    if (!index.isValid())
        return;

    painter->save();

    // Background — use highlight for selected items
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else {
        painter->fillRect(option.rect, _bgColor);
    }

    QString text = index.data(Qt::DisplayRole).toString();
    QColor textColor = (option.state & QStyle::State_Selected)
        ? option.palette.color(QPalette::HighlightedText)
        : _fgColor;

    QTextOption to;
    to.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    painter->setPen(textColor);

    QRect textRect = option.rect;
    textRect.setLeft(textRect.left() + 4);
    painter->drawText(textRect, text, to);

    painter->restore();
}

// =============================================================================
// ClipboardHistoryPanel — constructor
// Win32:  ClipboardHistoryPanel() — zero-init fields
// Qt6:    initialise _listWidget, _delegate, _contextMenu, connections
// =============================================================================
ClipboardHistoryPanel::ClipboardHistoryPanel()
    : DockingDlgInterface(0)  // 0 = no dialog resource ID in Qt6
    , _listWidget(nullptr)
    , _delegate(nullptr)
    , _ppEditView(nullptr)
    , _lbBgColor(-1)
    , _lbFgColor(-1)
    , _isTrackingClipboardOps(true)
    , _contextMenu(nullptr)
    , _actPaste(nullptr)
    , _actClear(nullptr)
    , _actFind(nullptr)
{
    // Win32: CreateWindowEx(WC_LISTBOX, ...) with LBS_OWNERDRAWFIXED style
    _listWidget = new QListWidget(this);
    _listWidget->setObjectName(QStringLiteral("IDC_LIST_CLIPBOARD"));
    _listWidget->setAlternatingRowColors(false);
    _listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    _listWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    _listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _listWidget->setAcceptDrops(false);
    _listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Remove default frame — Win32 listbox has no WS_BORDER (NOT WS_BORDER in .rc)
    _listWidget->setFrameShape(QFrame::NoFrame);

    // Win32: WM_DRAWITEM + drawItem() — custom list-box item painting
    _delegate = new ClipboardListDelegate(_listWidget);
    _listWidget->setItemDelegate(_delegate);

    // Win32: TRACKPOPUPMENU + HMENU context menu
    _contextMenu = new QMenu(_listWidget);
    _actPaste = _contextMenu->addAction(tr("&Paste"), this, [this]() {
        QListWidgetItem* item = _listWidget->currentItem();
        if (item)
            onItemActivated(item);
    });
    _actFind = _contextMenu->addAction(tr("&Find in files"), this, [this]() {
        emit itemActivated(_listWidget->currentRow());
    });
    _contextMenu->addSeparator();
    _actClear = _contextMenu->addAction(tr("&Clear Clipboard History"), this, [this]() {
        _clipboardDataInfos.clear();
        _listWidget->clear();
    });

    // Win32: SetClipboardViewer(_hSelf) — clipboard viewer chain
    // Qt6: QClipboard::changed signal is always active; Qt manages the chain
    connect(QApplication::clipboard(), &QClipboard::changed,
            this, &ClipboardHistoryPanel::onClipboardChanged);

    // Win32: IDC_LIST_CLIPBOARD + LBN_DBLCLK — double-click to paste
    connect(_listWidget, &QListWidget::itemDoubleClicked,
            this, &ClipboardHistoryPanel::onItemDoubleClicked);

    // Win32: single-click could be used for preview
    connect(_listWidget, &QListWidget::itemClicked,
            this, &ClipboardHistoryPanel::onItemClicked);

    // Win32: Enter key triggers paste (same as double-click)
    connect(_listWidget, &QListWidget::itemActivated,
            this, &ClipboardHistoryPanel::onItemActivated);

    // Default size — will be overridden by resizeEvent
    resize(300, 400);
}

ClipboardHistoryPanel::~ClipboardHistoryPanel()
{
    // Win32: ChangeClipboardChain(_hSelf, _hwndNextCbViewer) on WM_DESTROY
    // Qt6: QClipboard auto-detaches — no action needed
    delete _delegate;
}

// =============================================================================
// init — Win32: WM_INITDIALOG handler
// =============================================================================
void ClipboardHistoryPanel::init(HINSTANCE, HWND hPere, ScintillaEditView** ppEditView)
{
    Q_UNUSED(hPere);
    _ppEditView = ppEditView;

    // Win32: SetClipboardViewer(_hSelf) — begins watching clipboard changes
    // Qt6: QClipboard::changed signal is always active; no registration needed

    // Win32: NppDarkMode::setDarkScrollBar(GetDlgItem(_hSelf, IDC_LIST_CLIPBOARD))
    NppDarkMode::setDarkScrollBar(_listWidget);
}

// =============================================================================
// setBackgroundColor — Win32 COLORREF override
// Qt6:    apply via delegate + stylesheet; store as RGB int
// =============================================================================
void ClipboardHistoryPanel::setBackgroundColor(const QColor& bgColor)
{
    // Store as RGB int — mirrors Win32 COLORREF (B|G|R byte order in Win32 is 0x00BBGGRR,
    // but we store as Qt RGB for simplicity; used only for identity checks)
    _lbBgColor = bgColor.red() | (bgColor.green() << 8) | (bgColor.blue() << 16);
    _delegate->setBackgroundColor(bgColor);
    _listWidget->viewport()->update();

    // Win32: WM_CTLCOLORLISTBOX → return CreateSolidBrush(bgColor)
    // Qt6: apply via stylesheet on the viewport
    QString ss = QStringLiteral("QListWidget { background-color: %1; }").arg(bgColor.name());
    _listWidget->setStyleSheet(ss);
}

// =============================================================================
// setForegroundColor — Win32 COLORREF override
// =============================================================================
void ClipboardHistoryPanel::setForegroundColor(const QColor& fgColor)
{
    _lbFgColor = fgColor.red() | (fgColor.green() << 8) | (fgColor.blue() << 16);
    _delegate->setForegroundColor(fgColor);
    _listWidget->viewport()->update();

    QString ss = QStringLiteral("QListWidget { color: %1; }").arg(fgColor.name());
    _listWidget->setStyleSheet(ss);
}

// =============================================================================
// display — show/hide the panel
// =============================================================================
void ClipboardHistoryPanel::display(bool toShow)
{
    if (toShow)
        show();
    else
        hide();
}

// =============================================================================
// trackClipboardOps — enable/disable clipboard monitoring
// Returns previous state
// =============================================================================
bool ClipboardHistoryPanel::trackClipboardOps(bool bTrack)
{
    bool prev = _isTrackingClipboardOps;
    _isTrackingClipboardOps = bTrack;
    return prev;
}

// =============================================================================
// getClipboadData — read the current system clipboard into ClipboardDataInfo
// Win32:
//   OpenClipboard → check CF_UNICODETEXT → check CF_NPPTEXTLEN (binary)
//   → CF_TEXT for binary bytes, else copy UTF-16LE from CF_UNICODETEXT
// Qt6:
//   QMimeData → check hasText() → check hasFormat(CF_NPPTEXTLEN)
//   → QMimeData for binary bytes, else store UTF-16LE from QString.utf16()
// =============================================================================
ClipboardDataInfo ClipboardHistoryPanel::getClipboadData()
{
    ClipboardDataInfo clipboardData;

    const QMimeData* mimeData = QApplication::clipboard()->mimeData();
    if (!mimeData)
        return clipboardData;

    // Win32: if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) return;
    if (!mimeData->hasText())
        return clipboardData;

    QString text = mimeData->text();

    // Win32: RegisterClipboardFormat(CF_NPPTEXTLEN) + IsClipboardFormatAvailable(cf_nppTextLen)
    if (mimeData->hasFormat(clipboardFormatNppTextLen())) {
        // Win32: internal binary clipboard data
        //   GetClipboardData(cf_nppTextLen) → length (unsigned long)
        //   GetClipboardData(CF_TEXT)       → raw bytes
        QByteArray lenData = mimeData->data(clipboardFormatNppTextLen());
        // The length format in Win32 is a raw unsigned long (4 bytes, native endian)
        if (lenData.size() >= 4) {
            unsigned long nbBytes = 0;
            // Detect endianness safely
            nbBytes = static_cast<unsigned long>(
                static_cast<unsigned char>(lenData[0]) |
                (static_cast<unsigned char>(lenData[1]) << 8) |
                (static_cast<unsigned char>(lenData[2]) << 16) |
                (static_cast<unsigned char>(lenData[3]) << 24));
            Q_UNUSED(nbBytes);
        }
        // In Qt6 the binary data is stored as the raw CF_NPPTEXTLEN payload
        // which contains the length prefix + binary bytes; retrieve as-is
        QByteArray binPayload = mimeData->data(clipboardFormatNppTextLen());
        if (!binPayload.isEmpty()) {
            // Payload: first 4 bytes = length (unsigned long LE), followed by bytes
            const unsigned char* raw = reinterpret_cast<const unsigned char*>(binPayload.constData());
            size_t payloadLen = static_cast<size_t>(binPayload.size());
            if (payloadLen > 4) {
                unsigned long dataLen = raw[0] | (raw[1] << 8) | (raw[2] << 16) | (raw[3] << 24);
                size_t copyLen = qMin(static_cast<size_t>(dataLen),
                                      payloadLen - 4);
                for (size_t i = 0; i < copyLen; ++i) {
                    clipboardData._data.append(raw[4 + i]);
                }
                clipboardData._isBinaryContained = true;
            }
        }
    } else {
        // Win32: standard CF_UNICODETEXT copy
        //   wchar_t* lpwchar = (wchar_t*)GlobalLock(hglb);
        //   size_t nbBytes = (lstrlenW(lpwchar) + 1) * sizeof(wchar_t);
        //   copy nbBytes into _data
        const ushort* utf16 = text.utf16();
        int len = text.length();  // number of QChar (≡ wchar_t on Win)
        // Include null terminator (mirrors Win32 lstrlenW + 1)
        for (int i = 0; i <= len; ++i) {
            unsigned char lo = utf16[i] & 0xFF;
            unsigned char hi = (utf16[i] >> 8) & 0xFF;
            clipboardData._data.append(lo);
            clipboardData._data.append(hi);
        }
    }

    return clipboardData;
}

// =============================================================================
// getClipboardDataIndex — find matching clipboard data in internal storage
// return -1 if not found, else return the index
// =============================================================================
int ClipboardHistoryPanel::getClipboardDataIndex(const ClipboardDataInfo& cbd)
{
    for (int i = 0, n = _clipboardDataInfos.size(); i < n; ++i) {
        if (_clipboardDataInfos[i]._data.size() != cbd._data.size())
            continue;
        bool found = true;
        for (int j = 0, m = cbd._data.size(); j < m; ++j) {
            if (_clipboardDataInfos[i]._data[j] != cbd._data[j]) {
                found = false;
                break;
            }
        }
        if (found)
            return i;
    }
    return -1;
}

// =============================================================================
// addToClipboadHistory — add clipboard entry to top of history list
// Win32:
//   i = getClipboardDataIndex(cbd);
//   if (i == 0) return;              // already at top
//   if (i != -1) { erase; LB_DELETESTRING }
//   insert at top + LB_INSERTSTRING 0 with empty string (drawItem paints it)
// =============================================================================
void ClipboardHistoryPanel::addToClipboadHistory(const ClipboardDataInfo& cbd)
{
    int idx = getClipboardDataIndex(cbd);

    // Already at top — Win32: "if (i == 0) return"
    if (idx == 0)
        return;

    // If exists elsewhere, remove old entry
    // Win32: SendDlgItemMessage(_hSelf, IDC_LIST_CLIPBOARD, LB_DELETESTRING, i, 0)
    if (idx > 0) {
        _listWidget->takeItem(idx);
        _clipboardDataInfos.remove(idx);
    }

    // Insert at top
    // Win32: SendDlgItemMessage(_hSelf, IDC_LIST_CLIPBOARD, LB_INSERTSTRING, 0, L"")
    //       (empty string; display painted by drawItem())
    // Qt6: insert item with display string painted by delegate
    _clipboardDataInfos.prepend(cbd);
    QString displayStr = buildDisplayString(cbd);
    QListWidgetItem* item = new QListWidgetItem(displayStr, _listWidget);
    item->setData(Qt::UserRole, 0);
    _listWidget->insertItem(0, item);
}

// =============================================================================
// buildDisplayString — truncate clipboard data to MAX_DISPLAY_LENGTH chars
// Win32:  StringArray + WcharMbcsConvertor::char2wchar → wchar_t* → DrawText
// Qt6:    StringArray → decode bytes (UTF-8 or UTF-16LE) → QString
// =============================================================================
QString ClipboardHistoryPanel::buildDisplayString(const ClipboardDataInfo& cbd)
{
    StringArray sa(cbd, MAX_DISPLAY_LENGTH);
    const unsigned char* ptr = sa.getPointer();
    size_t len = sa.getLength();

    if (!ptr || len == 0)
        return QString();

    if (cbd._isBinaryContained) {
        // Win32: treated as UTF-8 via WcharMbcsConvertor::char2wchar(cp=SC_CP_UTF8)
        // Qt6: QString::fromUtf8
        return QString::fromUtf8(reinterpret_cast<const char*>(ptr),
                                 static_cast<int>(len));
    } else {
        // Win32: treated as UTF-16LE (CF_UNICODETEXT)
        // Qt6: decode UTF-16LE byte pairs
        //       (StringArray already nulled every other byte near the end)
        QString result;
        result.reserve(static_cast<int>(len / 2));
        for (size_t i = 0; i + 1 < len; i += 2) {
            ushort ch = static_cast<ushort>(ptr[i] | (ptr[i + 1] << 8));
            if (ch != 0)
                result.append(QChar(ch));
            else if (result.isEmpty())
                continue;  // skip leading nulls
            else
                break;     // embedded/terminating null
        }
        return result;
    }
}

// =============================================================================
// removeItemAt — remove clipboard entry at index
// =============================================================================
void ClipboardHistoryPanel::removeItemAt(int index)
{
    if (index < 0 || index >= _clipboardDataInfos.size())
        return;
    _clipboardDataInfos.remove(index);
    delete _listWidget->takeItem(index);
}

// =============================================================================
// insertTopItem — insert clipboard data as item at top of list
// Win32: LB_INSERTSTRING 0 with empty string (painting deferred to drawItem)
// Qt6:  delegates to addToClipboadHistory
// =============================================================================
void ClipboardHistoryPanel::insertTopItem(const ClipboardDataInfo&)
{
    // Drawing is handled entirely by ClipboardListDelegate; no separate insert needed
}

// =============================================================================
// pasteItemAt — paste clipboard entry at row into the Scintilla editor
// Win32:
//   LB_GETCURSEL → i
//   get codepage from buffer (or SCI_GETCODEPAGE fallback)
//   if binary: SCI_REPLACESEL("") + SCI_ADDTEXT(raw bytes)
//   else: ByteArray → WideCharToMultiByte → SCI_REPLACESEL + SCI_ADDTEXT
// Qt6:
//   Convert UTF-16LE bytes to QString → toUtf8/local8Bit → SCI_*
// =============================================================================
void ClipboardHistoryPanel::pasteItemAt(int row)
{
    if (!_ppEditView || !*_ppEditView || row < 0
        || row >= _clipboardDataInfos.size())
        return;

    const ClipboardDataInfo& cbd = _clipboardDataInfos[row];
    ScintillaEditView* editView = *_ppEditView;

    if (cbd._isBinaryContained) {
        // Win32: SCI_REPLACESEL(0, L"") + SCI_ADDTEXT(data.size(), &data[0])
        editView->execute(SCI_REPLACESEL, 0,
                          reinterpret_cast<intptr_t>(""));
        editView->execute(SCI_ADDTEXT,
                          static_cast<intptr_t>(cbd._data.size()),
                          reinterpret_cast<intptr_t>(cbd._data.constData()));
    } else {
        // Win32: get codepage
        //   Buffer* buf = _ppEditView[i]->getCurrentBuffer();
        //   int codepage = (buf->getEncoding() == -1)
        //       ? (SCI_GETCODEPAGE == SC_CP_UTF8 ? SC_CP_UTF8 : 0)
        //       : SC_CP_UTF8;
        int codepage = SC_CP_UTF8;
        Buffer* buf = editView->getCurrentBuffer();
        if (buf) {
            int enc = buf->getEncoding();
            if (enc == -1) {
                int cp = static_cast<int>(editView->execute(SCI_GETCODEPAGE));
                codepage = (cp == SC_CP_UTF8) ? SC_CP_UTF8 : 0;
            } else {
                codepage = SC_CP_UTF8;
            }
        }

        // Win32: ByteArray ba(cbd); WideCharToMultiByte(cp, 0, (wchar_t*)ba, ba.len, NULL, 0, ...)
        ByteArray ba(cbd);
        const unsigned char* ptr = ba.getPointer();
        size_t len = ba.getLength();

        // Convert UTF-16LE bytes → QString
        QString text;
        text.reserve(static_cast<int>(len / 2));
        for (size_t i = 0; i + 1 < len; i += 2) {
            ushort ch = static_cast<ushort>(ptr[i] | (ptr[i + 1] << 8));
            text.append(QChar(ch));
        }

        // Convert to target encoding
        QByteArray encoded = (codepage == SC_CP_UTF8)
            ? text.toUtf8()
            : text.toLocal8Bit();  // codepage 0 = ANSI

        editView->execute(SCI_REPLACESEL, 0,
                          reinterpret_cast<intptr_t>(""));
        editView->execute(SCI_ADDTEXT,
                          static_cast<intptr_t>(encoded.size()),
                          reinterpret_cast<intptr_t>(encoded.constData()));
        editView->grabFocus();
    }
}

// =============================================================================
// onClipboardChanged — Qt slot: clipboard content changed
// Win32: WM_DRAWCLIPBOARD handler
//   if (_isTrackingClipboardOps) { addToClipboadHistory(getClipboadData()); }
//   SendMessage(_hwndNextCbViewer, WM_DRAWCLIPBOARD, ...);
// Qt6:  triggered by QClipboard::changed
// =============================================================================
void ClipboardHistoryPanel::onClipboardChanged(QClipboard::Mode mode)
{
    if (mode != QClipboard::Clipboard)
        return;
    if (!_isTrackingClipboardOps)
        return;

    ClipboardDataInfo clipboardData = getClipboadData();
    if (clipboardData._data.isEmpty())
        return;

    addToClipboadHistory(clipboardData);
}

// =============================================================================
// onItemDoubleClicked — Qt slot: user double-clicked a list item
// Win32: IDC_LIST_CLIPBOARD, HIWORD(wParam) == LBN_DBLCLK
// =============================================================================
void ClipboardHistoryPanel::onItemDoubleClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);
    int row = _listWidget->row(item);
    pasteItemAt(row);
}

// =============================================================================
// onItemClicked — Qt slot: user single-clicked a list item
// Win32: could be used for preview; no-op in Win32
// =============================================================================
void ClipboardHistoryPanel::onItemClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);
}

// =============================================================================
// onItemActivated — Qt slot: Enter key or programmatic activation
// Win32: paste on Enter keydown in dialog proc
// =============================================================================
void ClipboardHistoryPanel::onItemActivated(QListWidgetItem* item)
{
    int row = _listWidget->row(item);
    pasteItemAt(row);
    emit itemActivated(row);
}

// =============================================================================
// contextMenuEvent — right-click context menu
// Win32: WM_CONTEXTMENU or TRACKPOPUPMENU(hMenu, ...)
// =============================================================================
void ClipboardHistoryPanel::contextMenuEvent(QContextMenuEvent* event)
{
    // Map from panel coordinates to list widget viewport coordinates
    QPoint vpPos = _listWidget->viewport()->mapFrom(this, event->pos());
    QListWidgetItem* item = _listWidget->itemAt(vpPos);
    if (!item) {
        event->ignore();
        return;
    }

    _actPaste->setEnabled(true);
    _contextMenu->popup(event->globalPos());
    event->accept();
}

// =============================================================================
// keyPressEvent — handle Delete (remove item) and Enter (paste)
// Win32: WM_KEYDOWN handler in dialog proc
// =============================================================================
void ClipboardHistoryPanel::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        int row = _listWidget->currentRow();
        if (row >= 0)
            removeItemAt(row);
        event->accept();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QListWidgetItem* item = _listWidget->currentItem();
        if (item)
            pasteItemAt(_listWidget->row(item));
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

// =============================================================================
// resizeEvent — fill panel with list widget
// Win32: WM_SIZE → MoveWindow(GetDlgItem(_hSelf, IDC_LIST_CLIPBOARD), 0, 0, w, h, TRUE)
// =============================================================================
void ClipboardHistoryPanel::resizeEvent(QResizeEvent* event)
{
    if (_listWidget)
        _listWidget->resize(event->size());
    QWidget::resizeEvent(event);
}

// =============================================================================
// run_dlgProc — Win32 dialog procedure dispatcher
// Translates each Win32 message case to the Qt equivalent slot/override
// =============================================================================
intptr_t ClipboardHistoryPanel::run_dlgProc(unsigned int message,
                                             intptr_t wParam, intptr_t lParam)
{
    switch (message) {

    case WM_INITDIALOG: {
        // Win32: SetClipboardViewer(_hSelf) + NppDarkMode::setDarkScrollBar
        NppDarkMode::setDarkScrollBar(_listWidget);
        return 1;  // TRUE (message handled)
    }

    case NPPM_INTERNAL_REFRESHDARKMODE: {
        // Win32: refresh dark scrollbar on theme change
        NppDarkMode::setDarkScrollBar(_listWidget);
        return 1;
    }

    case WM_CHANGECBCHAIN: {
        // Win32: another clipboard viewer was removed/changed
        //   if (_hwndNextCbViewer == (HWND)wParam) _hwndNextCbViewer = (HWND)lParam;
        //   else if (_hwndNextCbViewer) SendMessage(_hwndNextCbViewer, ...);
        // Qt6: QClipboard manages the chain internally — stub for compat
        Q_UNUSED(wParam);
        Q_UNUSED(lParam);
        return 1;
    }

    case WM_DRAWCLIPBOARD: {
        // Win32: clipboard content changed
        //   if (_isTrackingClipboardOps) addToClipboadHistory(getClipboadData());
        //   SendMessage(_hwndNextCbViewer, WM_DRAWCLIPBOARD, ...);
        // Qt6: handled by onClipboardChanged slot via QClipboard::changed
        // Stub exists for message-routing compatibility
        Q_UNUSED(wParam);
        Q_UNUSED(lParam);
        return 1;
    }

    case WM_DESTROY: {
        // Win32: ChangeClipboardChain(_hSelf, _hwndNextCbViewer)
        // Qt6: QClipboard auto-cleans; no action needed
        return 0;
    }

    case WM_COMMAND: {
        // Win32: IDC_LIST_CLIPBOARD notification (LBN_*)
        switch (LOWORD(wParam)) {
        case IDC_LIST_CLIPBOARD: {
            if (HIWORD(wParam) == LBN_DBLCLK) {
                int sel = _listWidget->currentRow();
                if (sel >= 0)
                    pasteItemAt(sel);
            }
            return 1;
        }
        default:
            break;
        }
        break;
    }

    case WM_NOTIFY: {
        // Win32: NMHDR notification — DMN_CLOSE when docking container closes
        // Qt6: docking manager uses signals; stub for compat
        Q_UNUSED(wParam);
        Q_UNUSED(lParam);
        return 0;
    }

    case WM_SIZE: {
        // Win32: MoveWindow(GetDlgItem(_hSelf, IDC_LIST_CLIPBOARD), 0, 0, w, h, TRUE)
        int width  = LOWORD(lParam);
        int height = HIWORD(lParam);
        if (_listWidget)
            _listWidget->resize(width, height);
        return 0;
    }

    case WM_CTLCOLORLISTBOX: {
        // Win32: return CreateSolidBrush(_lbBgColor) if background is custom
        if (_lbBgColor != -1) {
            Q_UNUSED(wParam);
            // Cannot return a GDI HBRUSH in Qt6; colour applied via stylesheet
            // return reinterpret_cast<intptr_t>(CreateSolidBrush(_lbBgColor));
            return 0;
        }
        break;
    }

    case WM_DRAWITEM: {
        // Win32: drawItem((DRAWITEMSTRUCT*)lParam)
        // Qt6: handled by ClipboardListDelegate; no-op here
        Q_UNUSED(wParam);
        Q_UNUSED(lParam);
        return 0;
    }

    default:
        break;
    }

    return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}
