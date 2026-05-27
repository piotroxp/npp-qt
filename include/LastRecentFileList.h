// LastRecentFileList.h - Qt6 stub
#pragma once
#include <QObject>
#include <QStringList>
#include <QMenu>

class LastRecentFileList : public QObject {
    Q_OBJECT
public:
    LastRecentFileList(QObject* parent = nullptr);
    void init(QWidget* parent, int cmdID);
    void setList(const QStringList& files);
    QStringList getList() const { return {}; }
    void updateMenu(QMenu* menu) { Q_UNUSED(menu); }
};
