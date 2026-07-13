// Semantic Lift: TaskList — Win32 list → Qt6 QListWidget
// Original: PowerEditor/src/WinControls/TaskList/TaskList.cpp (291 lines)

#include "TaskList.h"
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QIcon>
#include <QColor>

TaskList::TaskList(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    _listWidget = new QListWidget(this);
    lay->addWidget(_listWidget);

    connect(_listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        emit taskClicked(item->text());
    });
    connect(_listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        emit taskDoubleClicked(item->text());
    });
}

TaskList::~TaskList() {}

void TaskList::init(QWidget* parent) { Q_UNUSED(parent); }

void TaskList::addTask(const QString& taskName, int priority, const QString& filePath)
{
    QListWidgetItem* item = new QListWidgetItem(taskName, _listWidget);
    item->setData(Qt::UserRole, filePath);

    // Color by priority
    if (priority >= 2)
        item->setBackground(Qt::red);
    else if (priority == 1)
        item->setBackground(Qt::yellow);
    else
        item->setBackground(Qt::green);

    _listWidget->addItem(item);
}

void TaskList::removeTask(const QString& taskName)
{
    for (int i = 0; i < _listWidget->count(); ++i) {
        QListWidgetItem* item = _listWidget->item(i);
        if (item->text() == taskName) {
            delete item;
            break;
        }
    }
}

void TaskList::clearTasks()
{
    _listWidget->clear();
}

void TaskList::updateTask(const QString& taskName, const QString& newText)
{
    for (int i = 0; i < _listWidget->count(); ++i) {
        QListWidgetItem* item = _listWidget->item(i);
        if (item->text() == taskName) {
            item->setText(newText);
            break;
        }
    }
}

QStringList TaskList::getTasks() const
{
    QStringList tasks;
    for (int i = 0; i < _listWidget->count(); ++i)
        tasks.append(_listWidget->item(i)->text());
    return tasks;
}

int TaskList::getSelectedTaskIndex() const
{
    return _listWidget->currentRow();
}
