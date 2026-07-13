// Semantic Lift: VerticalFileSwitcher — Win32 ListView → Qt6 QListWidget
// Original: PowerEditor/src/WinControls/VerticalFileSwitcher/VerticalFileSwitcher.cpp (694 lines)

#include "VerticalFileSwitcher.h"
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QIcon>

VerticalFileSwitcher::VerticalFileSwitcher(QWidget* parent)
    : QWidget(parent)
    , _parent(parent)
{
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    _listWidget = new QListWidget(this);
    lay->addWidget(_listWidget);

    connect(_listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        int idx = _listWidget->row(item);
        onItemClicked(idx);
    });
    connect(_listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        int idx = _listWidget->row(item);
        onItemDoubleClicked(idx);
    });
}

VerticalFileSwitcher::~VerticalFileSwitcher() {}

void VerticalFileSwitcher::init(QWidget* parent)
{
    _parent = parent;
}

void VerticalFileSwitcher::addFile(const QString& filePath)
{
    if (_files.contains(filePath))
        return;

    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem(fi.fileName(), _listWidget);
    item->setData(Qt::UserRole, filePath);
    item->setToolTip(filePath);
    item->setIcon(QIcon::fromTheme(QStringLiteral("text-x-generic")));
    _files.append(filePath);
    _listWidget->addItem(item);
}

void VerticalFileSwitcher::removeFile(const QString& filePath)
{
    int idx = _files.indexOf(filePath);
    if (idx >= 0) {
        delete _listWidget->takeItem(idx);
        _files.removeAt(idx);
    }
}

void VerticalFileSwitcher::updateFile(const QString& oldPath, const QString& newPath)
{
    int idx = _files.indexOf(oldPath);
    if (idx >= 0) {
        QFileInfo fi(newPath);
        QListWidgetItem* item = _listWidget->item(idx);
        item->setText(fi.fileName());
        item->setData(Qt::UserRole, newPath);
        item->setToolTip(newPath);
        _files[idx] = newPath;
    }
}

void VerticalFileSwitcher::setCurrentFile(const QString& filePath)
{
    _currentFile = filePath;
    int idx = _files.indexOf(filePath);
    if (idx >= 0) {
        _listWidget->setCurrentRow(idx);
    }
}

QString VerticalFileSwitcher::getCurrentFile() const
{
    int row = _listWidget->currentRow();
    if (row >= 0 && row < _files.size())
        return _files.at(row);
    return QString();
}

QStringList VerticalFileSwitcher::getAllFiles() const
{
    return _files;
}

void VerticalFileSwitcher::clearAll()
{
    _listWidget->clear();
    _files.clear();
}

void VerticalFileSwitcher::onItemClicked(int index)
{
    if (index >= 0 && index < _files.size())
        emit fileSwitchRequested(_files.at(index));
}

void VerticalFileSwitcher::onItemDoubleClicked(int index)
{
    if (index >= 0 && index < _files.size())
        emit fileSwitchRequested(_files.at(index));
}
