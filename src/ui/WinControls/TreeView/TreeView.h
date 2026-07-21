// TreeView.h - Qt6 tree view for project panel and file browser
// INTENT: source uses CTreeCtrl. Target uses QTreeWidget.
#pragma once
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>

class TreeView : public QTreeWidget {
    Q_OBJECT
public:
    explicit TreeView(QWidget* parent = nullptr);
    ~TreeView() override = default;

    // Add a folder node
    QTreeWidgetItem* addFolder(const QString& name, QTreeWidgetItem* parent = nullptr);

    // Add a file node
    QTreeWidgetItem* addFile(const QString& name, const QString& fullPath,
                              QTreeWidgetItem* parent = nullptr);

    // Expand all nodes
    void expandAll();

    // Get selected file path
    QString selectedFilePath() const;

signals:
    void fileActivated(const QString& fullPath);
    void fileSelected(const QString& fullPath);

private slots:
    void onItemActivated(QTreeWidgetItem* item, int column);

private:
    bool isFolder(QTreeWidgetItem* item) const;
};
