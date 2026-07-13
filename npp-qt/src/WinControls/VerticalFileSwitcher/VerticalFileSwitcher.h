#pragma once

#include <QWidget>
#include <QString>
#include <QStringList>

class VerticalFileSwitcher : public QWidget
{
    Q_OBJECT
public:
    explicit VerticalFileSwitcher(QWidget* parent = nullptr);
    ~VerticalFileSwitcher() override;

    void init(QWidget* parent);
    void addFile(const QString& filePath);
    void removeFile(const QString& filePath);
    void updateFile(const QString& oldPath, const QString& newPath);
    void setCurrentFile(const QString& filePath);
    QString getCurrentFile() const;
    QStringList getAllFiles() const;
    void clearAll();

signals:
    void fileSwitchRequested(const QString& filePath);

private slots:
    void onItemClicked(int index);
    void onItemDoubleClicked(int index);

private:
    QWidget* _parent = nullptr;
    class QListWidget* _listWidget = nullptr;
    QString _currentFile;
    QStringList _files;
};

