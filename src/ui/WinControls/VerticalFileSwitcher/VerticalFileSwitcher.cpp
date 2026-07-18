// VerticalFileSwitcher.cpp
#include "VerticalFileSwitcher.h"
#include <QVBoxLayout>
#include <QFileInfo>

VerticalFileSwitcher::VerticalFileSwitcher(QWidget* parent)
    : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    _list = new QListWidget(this);
    layout->addWidget(_list);
    connect(_list, &QListWidget::itemActivated, this, &VerticalFileSwitcher::onItemActivated);
}

void VerticalFileSwitcher::addFile(const QString& path) {
    auto* item = new QListWidgetItem(QFileInfo(path).fileName(), _list);
    item->setData(Qt::UserRole, path);
    item->setToolTip(path);
}

void VerticalFileSwitcher::removeFile(const QString& path) {
    for (int i = 0; i < _list->count(); ++i) {
        if (_list->item(i)->data(Qt::UserRole).toString() == path) {
            delete _list->takeItem(i);
            break;
        }
    }
}

void VerticalFileSwitcher::selectFile(int index) {
    if (index >= 0 && index < _list->count()) {
        _list->setCurrentRow(index);
    }
}

int VerticalFileSwitcher::currentIndex() const {
    return _list->currentRow();
}

void VerticalFileSwitcher::onItemActivated(QListWidgetItem* item) {
    if (item) {
        QString path = item->data(Qt::UserRole).toString();
        emit fileActivated(path);
        emit fileSelected(path);
    }
}
