// Semantic Lift: lastRecentFileList — Win32 MRU + SHAddToRecentDocs → Qt6 QSettings + QFile
// Original: PowerEditor/src/lastRecentFileList.cpp (311 lines)
// Win32 → Qt6: HWND + MRU list + SHAddToRecentDocs → QSettings + QFileSystemWatcher

#include "lastRecentFileList.h"
#include "Parameters.h"
#include "NppIO.h"
#include <QSettings>
#include <QFileInfo>
#include <QDateTime>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QMenu>
#include <QAction>
#include <QActionGroup>

static const QStringList recentFilesDefault;

lastRecentFileList::lastRecentFileList()
    : _nbRecentFile(0), _userMax(10), _pRecentFileMenu(nullptr), _pTrayMenu(nullptr)
{
}

lastRecentFileList::~lastRecentFileList()
{
    // Remove all recent file actions
    if (_pRecentFileMenu) {
        _pRecentFileMenu->clear();
    }
    if (_pTrayMenu) {
        _pTrayMenu->clear();
    }
}

void lastRecentFileList::init(QMenu* recentFileMenu, QMenu* trayMenu, int maxNb)
{
    _pRecentFileMenu = recentFileMenu;
    _pTrayMenu = trayMenu;
    _userMax = maxNb;
    load();
}

void lastRecentFileList::load()
{
    QString settingsFile = Parameters::getInstance().getSettingsPath() + QStringLiteral("config.xml");
    QSettings settings(settingsFile, QSettings::IniFormat);

    _nbRecentFile = settings.value(QStringLiteral("RecentFiles"), 0).toInt();
    _files.clear();

    for (int i = 0; i < _nbRecentFile; ++i) {
        QString key = QStringLiteral("Recent File ") + QString::number(i);
        QString filePath = settings.value(key).toString();
        if (!filePath.isEmpty() && QFileInfo::exists(filePath)) {
            _files.append(filePath);
        }
    }

    // Sync with max
    while (_files.size() > _userMax) {
        _files.removeLast();
    }
    _nbRecentFile = _files.size();
}

void lastRecentFileList::save() const
{
    QString settingsFile = Parameters::getInstance().getSettingsPath() + QStringLiteral("config.xml");
    QSettings settings(settingsFile, QSettings::IniFormat);

    settings.setValue(QStringLiteral("RecentFiles"), _nbRecentFile);

    for (int i = 0; i < _nbRecentFile; ++i) {
        QString key = QStringLiteral("Recent File ") + QString::number(i);
        settings.setValue(key, _files.at(i));
    }
}

void lastRecentFileList::add(const QString& fn)
{
    if (fn.isEmpty())
        return;

    QString filePath = QFileInfo(fn).absoluteFilePath();

    // Remove if already present (will be re-added at front)
    _files.removeAll(filePath);

    // Insert at front
    _files.prepend(filePath);

    // Trim to max
    while (_files.size() > _userMax) {
        _files.removeLast();
    }

    _nbRecentFile = _files.size();
    save();

    // Notify shell (on Linux: nothing needed, Qt tracks files natively)
}

void lastRecentFileList::remove(const QString& fn)
{
    QString filePath = QFileInfo(fn).absoluteFilePath();
    _files.removeAll(filePath);
    _nbRecentFile = _files.size();
    save();
}

void lastRecentFileList::setMaxNb(int nb)
{
    _userMax = nb;
    while (_files.size() > _userMax) {
        _files.removeLast();
    }
    _nbRecentFile = _files.size();
    save();
}

void lastRecentFileList::clear()
{
    _files.clear();
    _nbRecentFile = 0;
    save();
}

int lastRecentFileList::getMaxNb() const
{
    return _userMax;
}

void lastRecentFileList::setTrackedFiles(const QStringList& trackedFiles)
{
    _trackedFiles = trackedFiles;
}

QString lastRecentFileList::getFile(int index) const
{
    if (index < 0 || index >= _nbRecentFile)
        return QString();
    return _files.at(index);
}

void lastRecentFileList::updateTrackedFiles(const QStringList& newTrackedFiles)
{
    // Remove from recent list any files that no longer exist
    QStringList toRemove;
    for (const QString& f : qAsConst(_files)) {
        if (!newTrackedFiles.contains(f) && !QFileInfo::exists(f)) {
            toRemove.append(f);
        }
    }
    for (const QString& f : toRemove) {
        _files.removeAll(f);
    }

    // Remove files that don't exist
    QStringList::iterator it = _files.begin();
    while (it != _files.end()) {
        if (!QFileInfo::exists(*it)) {
            it = _files.erase(it);
        } else {
            ++it;
        }
    }

    _nbRecentFile = _files.size();
    save();
}

void lastRecentFileList::updateMenu()
{
    if (_pRecentFileMenu) {
        _pRecentFileMenu->clear();
        for (int i = 0; i < _nbRecentFile; ++i) {
            QString filePath = _files.at(i);
            QFileInfo fi(filePath);
            QString displayName = fi.fileName();
            QString fullPath = fi.absoluteFilePath();

            QAction* action = _pRecentFileMenu->addAction(
                QStringLiteral("&%1 ").arg(i + 1) + displayName);
            action->setToolTip(fullPath);
            action->setData(fullPath);
            connect(action, &QAction::triggered, this, [this, fullPath]() {
                emit fileSelected(fullPath);
            });
        }

        if (_nbRecentFile > 0) {
            _pRecentFileMenu->addSeparator();
            QAction* clearAction = _pRecentFileMenu->addAction(tr("&Clear Recent Files"));
            connect(clearAction, &QAction::triggered, this, &lastRecentFileList::clear);
        }
    }

    if (_pTrayMenu) {
        _pTrayMenu->clear();
        for (int i = 0; i < _nbRecentFile && i < 10; ++i) {
            QString filePath = _files.at(i);
            QFileInfo fi(filePath);
            QAction* action = _pTrayMenu->addAction(fi.fileName());
            action->setData(filePath);
            connect(action, &QAction::triggered, this, [this, filePath]() {
                emit fileSelected(filePath);
            });
        }
    }
}

void lastRecentFileList::setCurrentFile(const QString& fullPath)
{
    _currentFile = fullPath;
}

bool lastRecentFileList::getFullFilePathFromName(const QString& shortFileName, QString& fullFilePath) const
{
    // Search in recent files for matching short name
    for (const QString& fp : qAsConst(_files)) {
        QFileInfo fi(fp);
        if (fi.fileName() == shortFileName) {
            fullFilePath = fp;
            return true;
        }
    }
    return false;
}
