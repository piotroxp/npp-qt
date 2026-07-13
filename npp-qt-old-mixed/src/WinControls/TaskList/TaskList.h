#pragma once

#include <QWidget>
#include <QString>
#include <QStringList>

class TaskList : public QWidget
{
    Q_OBJECT
public:
    explicit TaskList(QWidget* parent = nullptr);
    ~TaskList() override;

    void init(QWidget* parent);
    void addTask(const QString& taskName, int priority, const QString& filePath);
    void removeTask(const QString& taskName);
    void clearTasks();
    void updateTask(const QString& taskName, const QString& newText);
    QStringList getTasks() const;
    int getSelectedTaskIndex() const;

signals:
    void taskClicked(const QString& taskName);
    void taskDoubleClicked(const QString& taskName);

private:
    class QListWidget* _listWidget = nullptr;
};

