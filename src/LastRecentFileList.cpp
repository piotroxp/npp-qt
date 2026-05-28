#include "LastRecentFileList.h"

LastRecentFileList::LastRecentFileList(QObject* parent) : QObject(parent) {}
void LastRecentFileList::init(QWidget* parent, int cmdID) { Q_UNUSED(parent); Q_UNUSED(cmdID); }
void LastRecentFileList::setList(const QStringList& files) { Q_UNUSED(files); }
