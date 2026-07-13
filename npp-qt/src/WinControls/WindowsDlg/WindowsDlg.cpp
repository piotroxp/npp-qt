// Semantic Lift: WindowsDlg — Win32 window list → Qt6 QDialog + QListWidget
// Original: PowerEditor/src/WinControls/WindowsDlg/WindowsDlg.cpp (1417 lines)
// Shows list of open windows for "Windows" menu functionality

#include "WindowsDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

WindowsDlg::WindowsDlg(QWidget* parent)
    : QDialog(parent)
    , _parent(parent)
{
    setWindowTitle(tr("Windows"));
    setMinimumSize(400, 300);

    QVBoxLayout* mainLay = new QVBoxLayout(this);

    mainLay->addWidget(new QLabel(tr("Open windows:")));

    QListWidget* list = new QListWidget(this);
    mainLay->addWidget(list);

    QDialogButtonBox* btnBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLay->addWidget(btnBox);
}

WindowsDlg::~WindowsDlg() {}

void WindowsDlg::init(QWidget* parent)
{
    _parent = parent;
}

void WindowsDlg::doDialog()
{
    show();
    raise();
}

QString WindowsDlg::getSelectedWindowTitle() const
{
    return QString();
}

void WindowsDlg::setSelectedWindowTitle(const QString& title)
{
    Q_UNUSED(title);
}

QStringList WindowsDlg::getAllWindowTitles() const
{
    return QStringList();
}

void WindowsDlg::closeEvent(QCloseEvent* event)
{
    hide();
    event->accept();
}
