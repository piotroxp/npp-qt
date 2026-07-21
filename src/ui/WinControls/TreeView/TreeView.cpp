// TreeView.cpp
#include "TreeView.h"
#include <QHeaderView>

TreeView::TreeView(QWidget* parent)
    : QTreeWidget(parent) {
    setHeaderLabels({tr("Name")});
    header()->hide();
    setAnimated(true);
    setIndentation(16);
    connect(this, &QTreeWidget::itemActivated, this, &TreeView::onItemActivated);
}

QTreeWidgetItem* TreeView::addFolder(const QString& name, QTreeWidgetItem* parent) {
    auto* item = new QTreeWidgetItem(parent);
    item->setText(0, name);
    item->setIcon(0, QIcon::fromTheme("folder"));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    if (parent) {
        parent->addChild(item);
    } else {
        addTopLevelItem(item);
    }
    return item;
}

QTreeWidgetItem* TreeView::addFile(const QString& name, const QString& fullPath,
                                   QTreeWidgetItem* parent) {
    auto* item = new QTreeWidgetItem(parent);
    item->setText(0, name);
    item->setData(0, Qt::UserRole, fullPath);
    item->setIcon(0, QIcon::fromTheme("text-x-generic"));
    if (parent) {
        parent->addChild(item);
    } else {
        addTopLevelItem(item);
    }
    return item;
}

void TreeView::expandAll() {
    QTreeWidget::expandAll();
}

QString TreeView::selectedFilePath() const {
    auto* item = currentItem();
    if (!item) return QString();
    return item->data(0, Qt::UserRole).toString();
}

void TreeView::onItemActivated(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    if (!isFolder(item)) {
        QString path = item->data(0, Qt::UserRole).toString();
        emit fileActivated(path);
        emit fileSelected(path);
    }
}

bool TreeView::isFolder(QTreeWidgetItem* item) const {
    return !item->data(0, Qt::UserRole).isValid();
}
