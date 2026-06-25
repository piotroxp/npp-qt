// npp-qt: Document snapshot implementation
#include "documentSnapshot.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include "md5.h"

bool DocumentSnapshot::capture(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    _filePath = filePath;
    QTextStream in(&file);
    in.setCodec("UTF-8");
    _content = in.readAll();
    _snapshotTime = QFileInfo(file).lastModified();
    _hash = MD5::hash(_content.toUtf8());
    return true;
}

bool DocumentSnapshot::hasChanged() const
{
    QFile file(_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return true;  // File gone = changed

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString current = in.readAll();
    return MD5::hash(current.toUtf8()) != _hash;
}

bool DocumentSnapshot::restore() const
{
    // Restoring means the document is already loaded in the editor.
    // This method signals the parent that a restore is needed.
    Q_UNUSED(_content);
    return true;
}

void DocumentSnapshot::clear()
{
    _filePath.clear();
    _content.clear();
    _snapshotTime = QDateTime();
    _hash.clear();
}

void DocumentSnapshotManager::addSnapshot(const QString& filePath)
{
    // Remove existing snapshot for this file
    removeSnapshot(filePath);

    DocumentSnapshot snap;
    if (snap.capture(filePath))
        _snapshots.append(snap);
}

void DocumentSnapshotManager::removeSnapshot(const QString& filePath)
{
    for (int i = 0; i < _snapshots.size(); ++i) {
        if (_snapshots[i].filePath() == filePath) {
            _snapshots.removeAt(i);
            return;
        }
    }
}

QVector<QString> DocumentSnapshotManager::changedSnapshots() const
{
    QVector<QString> result;
    for (const DocumentSnapshot& snap : _snapshots) {
        if (snap.hasChanged())
            result.append(snap.filePath());
    }
    return result;
}

bool DocumentSnapshotManager::hasChanged(const QString& filePath) const
{
    for (const DocumentSnapshot& snap : _snapshots) {
        if (snap.filePath() == filePath)
            return snap.hasChanged();
    }
    return false;  // No snapshot = assume not changed
}

void DocumentSnapshotManager::clearAll()
{
    for (auto& snap : _snapshots)
        snap.clear();
    _snapshots.clear();
}
