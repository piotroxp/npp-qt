// TaskList.h - Qt6 task list panel (recent files + open documents)
// INTENT: source is a list of open documents with MRU ordering. Target uses QListWidget.
#pragma once
#include <QWidget>
#include <QListWidget>
#include <QString>
#include <QVector>

class TaskList : public QWidget {
    Q_OBJECT
public:
    explicit TaskList(QWidget* parent = nullptr);
    ~TaskList() override = default;

    void addDocument(const QString& path);
    void removeDocument(const QString& path);
    void selectDocument(int index);
    void selectDocument(const QString& path);
    int documentCount() const;
    QString documentAt(int index) const;

signals:
    void documentSelected(const QString& path);
    void documentClosed(const QString& path);
    void documentActivated(int index);

private slots:
    void onItemDoubleClicked(QListWidgetItem* item);
    void onContextMenu(const QPoint& pos);

private:
    QListWidget* _list = nullptr;
    int findDocument(const QString& path) const;
};
