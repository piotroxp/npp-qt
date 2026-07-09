// RecentFilesManager.h — Recent files tracking for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMenu>

class RecentFilesManager : public QObject {
    Q_OBJECT
public:
    static RecentFilesManager& instance();

    void addFile(const QString& fullPath);
    void removeFile(const QString& fullPath);
    void clear();
    QStringList files() const { return m_files; }
    int count() const { return m_files.size(); }
    int maxCount() const { return m_maxCount; }
    void setMaxCount(int n) { m_maxCount = n; }
    QMenu* buildMenu(QWidget* parent = nullptr);
    void populateMenu(QMenu* menu);
    void save();
    void load();

Q_SIGNALS:
    void fileAdded(const QString& path);
    void fileRemoved(const QString& path);
    void cleared();
    void menuNeedsRebuild();

private:
    RecentFilesManager();
    ~RecentFilesManager() override;
    Q_DISABLE_COPY_MOVE(RecentFilesManager)
    QString configPath() const;
    QStringList m_files;
    int m_maxCount = 50;
};
