// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <QWidget>
#include <QListWidget>
#include <QVector>
#include <QByteArray>
#include <QClipboard>
#include <QApplication>

#include "../DockingDlgInterface/DockingDlgInterface.h"

// Resource IDs
#define IDD_CLIPBOARDHISTORY_PANEL 2800
#define IDC_LIST_CLIPBOARD (IDD_CLIPBOARDHISTORY_PANEL + 1)

#define CH_PROJECTPANELTITLE "Clipboard History"

class ScintillaEditView;

// Clipboard data info structure
struct ClipboardDataInfo {
    QByteArray _data;
    bool _isBinaryContained = false;
};

// Byte array wrapper for clipboard data
class ByteArray {
public:
    ByteArray() = default;
    explicit ByteArray(const ClipboardDataInfo& cd);
    
    ~ByteArray() {
        if (_pBytes)
            delete[] _pBytes;
    }
    
    const unsigned char* getPointer() const { return _pBytes; }
    size_t getLength() const { return _length; }

protected:
    unsigned char* _pBytes = nullptr;
    size_t _length = 0;
};

// String array for display
class StringArray : public ByteArray {
public:
    StringArray(const ClipboardDataInfo& cd, size_t maxLen);
};

// ClipboardHistoryPanel - Shows clipboard history
class ClipboardHistoryPanel : public DockingDlgInterface
{
    Q_OBJECT

public:
    ClipboardHistoryPanel();
    ~ClipboardHistoryPanel() override = default;

    void init(QWidget* parent, ScintillaEditView** ppEditView);
    void setParent(QWidget* parent2set);

    ClipboardDataInfo getClipboadData();
    void addToClipboadHistory(ClipboardDataInfo cbd);
    int getClipboardDataIndex(const ClipboardDataInfo& cbd);

    void setBackgroundColor(QRgb bgColour) override;
    void setForegroundColor(QRgb fgColour) override;

    bool trackClipboardOps(bool bTrack);

signals:
    void clipboardEntrySelected(const QString& text);
    void clipboardEntryActivated(const QString& text, bool isBinary);

protected:
    intptr_t run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam) override;

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onClipboardChanged();

private:
    void drawItem(QListWidgetItem* item);
    QString getDisplayString(const ClipboardDataInfo& cbd);
    void insertSelectedItem(int index);

    ScintillaEditView** _ppEditView = nullptr;
    QVector<ClipboardDataInfo> _clipboardDataInfos;
    bool _isTrackingClipboardOps = true;
    QRgb _lbBgColor = 0xFFFFFFFF;
    QRgb _lbFgColor = 0xFF000000;
    
    QListWidget* _pClipboardList = nullptr;
};

// Static storage for clipboard change handling
static ClipboardHistoryPanel* g_clipboardHistoryPanel = nullptr;

ClipboardHistoryPanel::ClipboardHistoryPanel()
    : DockingDlgInterface(IDD_CLIPBOARDHISTORY_PANEL)
{
    setWindowTitle(CH_PROJECTPANELTITLE);
    g_clipboardHistoryPanel = this;
}

void ClipboardHistoryPanel::init(QWidget* parent, ScintillaEditView** ppEditView)
{
    DockingDlgInterface::init(parent);
    _ppEditView = ppEditView;
    
    // Connect to clipboard changes
    connect(QApplication::clipboard(), &QClipboard::dataChanged, 
            this, &ClipboardHistoryPanel::onClipboardChanged);
}

void ClipboardHistoryPanel::setParent(QWidget* parent2set)
{
    _hParent = parent2set;
}

ClipboardDataInfo ClipboardHistoryPanel::getClipboadData()
{
    ClipboardDataInfo clipboardData;
    
    const QMimeData* mimeData = QApplication::clipboard()->mimeData();
    if (!mimeData) return clipboardData;
    
    // Get text data
    if (mimeData->hasText()) {
        QString text = mimeData->text();
        QByteArray utf8 = text.toUtf8();
        clipboardData._data = utf8;
    }
    
    // Check for binary data (images etc.)
    if (mimeData->hasImage() || mimeData->hasHtml()) {
        clipboardData._isBinaryContained = true;
    }
    
    return clipboardData;
}

void ClipboardHistoryPanel::addToClipboadHistory(ClipboardDataInfo cbd)
{
    int i = getClipboardDataIndex(cbd);
    if (i == 0) return; // Already at top
    
    if (i != -1) {
        _clipboardDataInfos.remove(i);
        if (_pClipboardList) {
            delete _pClipboardList->takeItem(i);
        }
    }
    
    _clipboardDataInfos.prepend(cbd);
    
    if (_pClipboardList) {
        auto* item = new QListWidgetItem(getDisplayString(cbd), _pClipboardList);
        item->setData(Qt::UserRole, 0); // Index marker
    }
}

int ClipboardHistoryPanel::getClipboardDataIndex(const ClipboardDataInfo& cbd)
{
    for (int i = 0; i < _clipboardDataInfos.size(); ++i) {
        if (_clipboardDataInfos[i]._data == cbd._data) {
            return i;
        }
    }
    return -1;
}

void ClipboardHistoryPanel::setBackgroundColor(QRgb bgColour)
{
    _lbBgColor = bgColour;
    if (_pClipboardList) {
        QPalette pal = _pClipboardList->palette();
        pal.setColor(QPalette::Base, QColor::fromRgb(bgColour));
        _pClipboardList->setPalette(pal);
    }
}

void ClipboardHistoryPanel::setForegroundColor(QRgb fgColour)
{
    _lbFgColor = fgColour;
    if (_pClipboardList) {
        QPalette pal = _pClipboardList->palette();
        pal.setColor(QPalette::Text, QColor::fromRgb(fgColour));
        _pClipboardList->setPalette(pal);
    }
}

bool ClipboardHistoryPanel::trackClipboardOps(bool bTrack)
{
    bool bPreviousState = _isTrackingClipboardOps;
    _isTrackingClipboardOps = bTrack;
    return bPreviousState;
}

QString ClipboardHistoryPanel::getDisplayString(const ClipboardDataInfo& cbd)
{
    const int MAX_DISPLAY_LENGTH = 64;
    
    QString text = QString::fromUtf8(cbd._data);
    if (text.length() > MAX_DISPLAY_LENGTH) {
        text = text.left(MAX_DISPLAY_LENGTH - 3) + "...";
    }
    
    // Handle non-printable characters
    for (QChar& c : text) {
        if (!c.isPrint() && c != '\n' && c != '\r' && c != '\t') {
            c = '.';
        }
    }
    
    if (cbd._isBinaryContained) {
        text += " [Binary data]";
    }
    
    return text;
}

void ClipboardHistoryPanel::onItemDoubleClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);
    int index = _pClipboardList->currentRow();
    insertSelectedItem(index);
}

void ClipboardHistoryPanel::onClipboardChanged()
{
    if (_isTrackingClipboardOps) {
        ClipboardDataInfo clipboardData = getClipboadData();
        if (!clipboardData._data.isEmpty()) {
            addToClipboadHistory(clipboardData);
        }
    }
}

void ClipboardHistoryPanel::insertSelectedItem(int index)
{
    if (index < 0 || index >= _clipboardDataInfos.size()) return;
    
    const ClipboardDataInfo& cbd = _clipboardDataInfos[index];
    QString text = QString::fromUtf8(cbd._data);
    
    if (_ppEditView && *_ppEditView) {
        // Would insert into scintilla
        // (*_ppEditView)->execute(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(text.toUtf8().constData()));
    }
    
    emit clipboardEntryActivated(text, cbd._isBinaryContained);
}

intptr_t ClipboardHistoryPanel::run_dlgProc(intptr_t message, intptr_t wParam, intptr_t lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            _pClipboardList = new QListWidget(this);
            _pClipboardList->setSelectionMode(QAbstractItemView::SingleSelection);
            
            connect(_pClipboardList, &QListWidget::itemDoubleClicked, 
                    this, &ClipboardHistoryPanel::onItemDoubleClicked);
            
            return TRUE;
        }
        
        case WM_SIZE: {
            if (_pClipboardList) {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);
                _pClipboardList->resize(width, height);
            }
            return TRUE;
        }
        
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case IDC_LIST_CLIPBOARD:
                    if (HIWORD(wParam) == LBN_DBLCLK) {
                        int index = _pClipboardList->currentRow();
                        if (index >= 0) {
                            insertSelectedItem(index);
                        }
                    }
                    return TRUE;
            }
            break;
        }
        
        case WM_NOTIFY: {
            // Handle dock close notification
            return TRUE;
        }
        
        default:
            break;
    }
    
    return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
}

// ByteArray implementation
ByteArray::ByteArray(const ClipboardDataInfo& cd)
{
    _length = cd._data.size();
    if (!_length) {
        _pBytes = nullptr;
        return;
    }
    _pBytes = new unsigned char[_length];
    for (size_t i = 0; i < _length; ++i) {
        _pBytes[i] = static_cast<unsigned char>(cd._data[i]);
    }
}

// StringArray implementation
StringArray::StringArray(const ClipboardDataInfo& cd, size_t maxLen)
{
    size_t len = cd._data.size();
    if (!len) {
        _pBytes = nullptr;
        return;
    }
    
    bool isCompleted = (len <= maxLen);
    _length = isCompleted ? len : maxLen;
    
    _pBytes = new unsigned char[_length + (isCompleted ? 0 : 2)];
    size_t i = 0;
    for (; i < _length; ++i) {
        if (!isCompleted && (i == _length - 5 || i == _length - 3 || i == _length - 1))
            _pBytes[i] = 0;
        else if (!isCompleted && (i == _length - 6 || i == _length - 4 || i == _length - 2))
            _pBytes[i] = '.';
        else
            _pBytes[i] = static_cast<unsigned char>(cd._data[i]);
    }
    
    if (!isCompleted) {
        _pBytes[i++] = 0;
        _pBytes[i] = 0;
    }
}