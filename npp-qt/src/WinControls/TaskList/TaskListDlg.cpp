// Semantic Lift: TaskListDlg — Win32 dialog → Qt6 QDialog
// Original: PowerEditor/src/WinControls/TaskList/TaskListDlg.cpp (229 lines)

#include "TaskListDlg.h"
#include "TaskList.h"
#include <QVBoxLayout>

TaskListDlg::TaskListDlg(QWidget* parent)
    : QDialog(parent)
    , _parent(parent)
{
    setWindowTitle(tr("Task List"));
    setModal(false);
    setMinimumSize(400, 300);
}

TaskListDlg::~TaskListDlg() {}

void TaskListDlg::init(QWidget* parent)
{
    _parent = parent;
    QVBoxLayout* lay = new QVBoxLayout(this);
    TaskList* taskList = new TaskList(this);
    lay->addWidget(taskList);
}

void TaskListDlg::doDialog()
{
    show();
    raise();
}

void TaskListDlg::closeEvent(QCloseEvent* event)
{
    hide();
    event->accept();
}
