// TaskList.cpp
#include "TaskList.h"
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QFileInfo>

TaskList::TaskList(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    _list = new QListWidget(this);
    layout->addWidget(_list);

    connect(_list, &QListWidget::itemDoubleClicked, this, &TaskList::onItemDoubleClicked);
    _list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_list, &QListWidget::customContextMenuRequested, this, &TaskList::onContextMenu);
}

void TaskList::addDocument(const QString& path) {
    if (findDocument(path) >= 0) return;
    auto* item = new QListWidgetItem(QFileInfo(path).fileName(), _list);
    item->setData(Qt::UserRole, path);
    item->setToolTip(path);
}

void TaskList::removeDocument(const QString& path) {
    int row = findDocument(path);
    if (row >= 0) {
        delete _list->takeItem(row);
    }
}

void TaskList::selectDocument(int index) {
    if (index >= 0 && index < _list->count()) {
        _list->setCurrentRow(index);
        auto* item = _list->item(index);
        if (item) {
            emit documentSelected(item->data(Qt::UserRole).toString());
        }
    }
}

void TaskList::selectDocument(const QString& path) {
    selectDocument(findDocument(path));
}

int TaskList::documentCount() const { return _list->count(); }

QString TaskList::documentAt(int index) const {
    if (index < 0 || index >= _list->count()) return QString();
    auto* item = _list->item(index);
    return item ? item->data(Qt::UserRole).toString() : QString();
}

int TaskList::findDocument(const QString& path) const {
    for (int i = 0; i < _list->count(); ++i) {
        if (_list->item(i)->data(Qt::UserRole).toString() == path)
            return i;
    }
    return -1;
}

void TaskList::onItemDoubleClicked(QListWidgetItem* item) {
    if (item) {
        int row = _list->row(item);
        emit documentActivated(row);
        emit documentSelected(item->data(Qt::UserRole).toString());
    }
}

void TaskList::onContextMenu(const QPoint& pos) {
    auto* item = _list->itemAt(pos);
    if (!item) return;
    QString path = item->data(Qt::UserRole).toString();
    QMenu menu;
    QAction* closeAct = menu.addAction(tr("Close"));
    connect(closeAct, &QAction::triggered, this, [this, path]() {
        removeDocument(path);
        emit documentClosed(path);
    });
    menu.exec(_list->mapToGlobal(pos));
}
