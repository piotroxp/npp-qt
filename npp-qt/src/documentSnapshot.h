// npp-qt: Document snapshot — tracks document content at save points
// for comparison and "reload modified" notifications.
#pragma once
#include <QString>
#include <QVector>
#include <QDateTime>

// DocumentSnapshot tracks a saved snapshot of a file's content
// and MD5 hash for detecting external modifications.
class DocumentSnapshot {
public:
    DocumentSnapshot() = default;

    // Capture current content of file as snapshot
    bool capture(const QString& filePath);

    // Check if current file content differs from snapshot
    bool hasChanged() const;

    // Get stored content (for "reload" dialog)
    QString content() const { return _content; }

    // Get stored file path
    QString filePath() const { return _filePath; }

    // Get snapshot timestamp
    QDateTime snapshotTime() const { return _snapshotTime; }

    // Restore snapshot (replace current editor content)
    bool restore() const;

    void clear();

private:
    QString _filePath;
    QString _content;
    QDateTime _snapshotTime;
    QString _hash;  // MD5 or similar hash of content
};

// DocumentSnapshotManager manages all open document snapshots.
class DocumentSnapshotManager {
public:
    DocumentSnapshotManager() = default;

    // Take snapshot of given file (called on file save)
    void addSnapshot(const QString& filePath);

    // Remove snapshot (called when file is closed)
    void removeSnapshot(const QString& filePath);

    // Check which snapshots have changed
    QVector<QString> changedSnapshots() const;

    // Check single file
    bool hasChanged(const QString& filePath) const;

    // Invalidate all snapshots (e.g., on session load)
    void clearAll();

private:
    QVector<DocumentSnapshot> _snapshots;
};
