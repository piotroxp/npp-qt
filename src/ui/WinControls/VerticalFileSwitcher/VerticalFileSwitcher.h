// VerticalFileSwitcher.h - Qt6 vertical file switcher panel
// INTENT: source uses a list control. Target uses QListWidget.
#pragma once
#include <QWidget>
#include <QListWidget>

class VerticalFileSwitcher : public QWidget {
    Q_OBJECT
public:
    explicit VerticalFileSwitcher(QWidget* parent = nullptr);
    ~VerticalFileSwitcher() override = default;

    void addFile(const QString& path);
    void removeFile(const QString& path);
    void selectFile(int index);
    int currentIndex() const;

signals:
    void fileSelected(const QString& path);
    void fileActivated(const QString& path);

private slots:
    void onItemActivated(QListWidgetItem* item);

private:
    QListWidget* _list = nullptr;
};
