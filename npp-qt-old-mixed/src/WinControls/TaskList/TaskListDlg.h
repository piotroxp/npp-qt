#pragma once

#include <QDialog>
#include <QString>

class TaskListDlg : public QDialog
{
    Q_OBJECT
public:
    explicit TaskListDlg(QWidget* parent = nullptr);
    ~TaskListDlg() override;

    void init(QWidget* parent);
    void doDialog();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QWidget* _parent = nullptr;
};

