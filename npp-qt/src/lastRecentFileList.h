#pragma once

#include <QString>
#include <QStringList>
#include <QMenu>
#include <QObject>

class lastRecentFileList : public QObject
{
    Q_OBJECT

public:
    lastRecentFileList();
    ~lastRecentFileList() override;

    void init(QMenu* recentFileMenu, QMenu* trayMenu, int maxNb = 10);
    void load();
    void save();
    void add(const QString& fn);
    void remove(const QString& fn);
    void clear();
    void setMaxNb(int nb);
    int getMaxNb() const;
    void setTrackedFiles(const QStringList& trackedFiles);
    void updateTrackedFiles(const QStringList& newTrackedFiles);
    int getNbOfFiles() const { return _nbRecentFile; }
    QString getFile(int index) const;
    void setCurrentFile(const QString& fullPath);
    void updateMenu();
    bool getFullFilePathFromName(const QString& shortFileName, QString& fullFilePath) const;

signals:
    void fileSelected(const QString& filePath);

private:
    int _nbRecentFile;
    int _userMax;
    QStringList _files;
    QStringList _trackedFiles;
    QString _currentFile;
    QMenu* _pRecentFileMenu = nullptr;
    QMenu* _pTrayMenu = nullptr;
};

