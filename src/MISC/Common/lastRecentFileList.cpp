// Ported to Qt6
#include "lastRecentFileList.h"
#include <QAction>
#include <QMenu>
#include <QSettings>
#include "Common.h"
#include "menuCmdID.h"

LastRecentFileList::LastRecentFileList(QObject* parent)
    : QObject(parent), _userMax(30), _idFreeArray(30, true) {}

void LastRecentFileList::initMenu(QMenu* hMenu, int idBase, int posBase, bool doSubMenu)
{
    if (doSubMenu) {
        _hParentMenu = hMenu;
        _hMenu = new QMenu();
    } else {
        _hParentMenu = nullptr;
        _hMenu = hMenu;
    }
    _idBase = idBase;
    _posBase = posBase;
    _nativeLangEncoding = 1252;
    _idFreeArray.fill(true);
}

void LastRecentFileList::switchMode()
{
    if (_hMenu) _hMenu->clear();
    if (_hParentMenu && !_hParentMenu.isNull()) {
        _hMenu = _hParentMenu;
        _hParentMenu = nullptr;
    } else if (_hMenu && !_hMenu.isNull()) {
        _hParentMenu = _hMenu;
        _hMenu = new QMenu();
    }
    _hasSeparators = false;
}

void LastRecentFileList::updateMenu()
{
    if (!_hMenu) return;
    _hMenu->clear();
    if (_size > 0) {
        _hMenu->addAction("Restore Recent Closed File", this, [this]() {
            if (!_lrfl.isEmpty()) emit itemSelected(_lrfl.back()._name);
        });
        QAction* cleanAction = _hMenu->addAction("Empty Recent Files List");
        QObject::connect(cleanAction, &QAction::triggered, this, &LastRecentFileList::clear);
        _hMenu->addSeparator();
        for (int j = 0; j < _size; ++j) {
            const RecentItem& item = _lrfl[j];
            QString label = BuildMenuFileName(100, static_cast<unsigned>(j), item._name);
            QAction* action = _hMenu->addAction(label);
            QObject::connect(action, &QAction::triggered, this, [this, j]() {
                if (j < _lrfl.size()) emit itemSelected(_lrfl[j]._name);
            });
        }
        _hasSeparators = true;
    }
}

void LastRecentFileList::add(const QString& fn)
{
    if (_userMax == 0 || _locked) return;
    int existingIndex = find(fn);
    if (existingIndex != -1) remove(static_cast<size_t>(existingIndex));
    RecentItem itemToAdd(fn);
    if (_size == _userMax) {
        itemToAdd._id = _lrfl.front()._id;
        _lrfl.dequeue();
    } else {
        itemToAdd._id = popFirstAvailableID();
        ++_size;
    }
    _lrfl.enqueue(itemToAdd);
    updateMenu();
}

void LastRecentFileList::remove(const QString& fn)
{
    int index = find(fn);
    if (index != -1) remove(static_cast<size_t>(index));
}

void LastRecentFileList::remove(size_t index)
{
    if (_size == 0 || _locked) return;
    if (index < static_cast<size_t>(_lrfl.size())) {
        setAvailable(_lrfl[static_cast<int>(index)]._id);
        _lrfl.removeAt(static_cast<int>(index));
        --_size;
        updateMenu();
    }
}

void LastRecentFileList::clear()
{
    if (_size == 0) return;
    while (!_lrfl.isEmpty()) {
        setAvailable(_lrfl.front()._id);
        _lrfl.dequeue();
    }
    _size = 0;
    updateMenu();
}

QString LastRecentFileList::getItem(int id) const
{
    for (int i = 0; i < _size; ++i)
        if (_lrfl[i]._id == id) return _lrfl[i]._name;
    return _lrfl.isEmpty() ? QString() : _lrfl.back()._name;
}

QString LastRecentFileList::getIndex(int index) const
{
    if (index >= 0 && index < _lrfl.size()) return _lrfl[index]._name;
    return QString();
}

void LastRecentFileList::setUserMaxNbLRF(int size)
{
    _userMax = size;
    if (_size > _userMax) {
        int toPop = _size - _userMax;
        while (toPop > 0) {
            setAvailable(_lrfl.front()._id);
            _lrfl.dequeue();
            --toPop; --_size;
        }
        updateMenu();
        _size = _userMax;
    }
}

void LastRecentFileList::saveLRFL()
{
    QSettings settings("Notepad++", "npp-qt");
    settings.beginGroup("RecentFiles");
    settings.setValue("maxCount", _userMax);
    settings.beginWriteArray("files");
    for (int i = _size - 1; i >= 0; --i) {
        settings.setArrayIndex(_size - 1 - i);
        settings.setValue("path", _lrfl[i]._name);
    }
    settings.endArray();
    settings.endGroup();
}

void LastRecentFileList::loadLRFL()
{
    QSettings settings("Notepad++", "npp-qt");
    settings.beginGroup("RecentFiles");
    _userMax = settings.value("maxCount", 30).toInt();
    int count = settings.beginReadArray("files");
    for (int i = 0; i < count && _size < _userMax; ++i) {
        settings.setArrayIndex(i);
        QString path = settings.value("path").toString();
        if (!path.isEmpty()) {
            RecentItem item(path);
            item._id = popFirstAvailableID();
            _lrfl.append(item);
            ++_size;
        }
    }
    settings.endArray();
    settings.endGroup();
    updateMenu();
}

int LastRecentFileList::find(const QString& fn) const
{
    for (int i = 0; i < _size; ++i)
        if (_lrfl[i]._name.compare(fn, Qt::CaseInsensitive) == 0) return i;
    return -1;
}

int LastRecentFileList::popFirstAvailableID()
{
    for (int i = 0; i < 30; ++i)
        if (_idFreeArray[i]) { _idFreeArray[i] = false; return i + _idBase; }
    return 0;
}

void LastRecentFileList::setAvailable(int id)
{
    int index = id - _idBase;
    if (index >= 0 && index < 30) _idFreeArray[index] = true;
}
