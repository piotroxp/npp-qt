// Semantic Lift: CustomFileDialog — Win32 GetOpenFileName/GetSaveFileName → Qt6 QFileDialog
// Original: PowerEditor/src/WinControls/OpenSaveFileDialog/CustomFileDialog.cpp (1098 lines)
// Win32 → Qt6: OPENFILENAME + OFN_EXPLORER + BIF_* → QFileDialog::getOpenFileName + QFileDialog::getSaveFileName

#include "CustomFileDialog.h"
#include "Parameters.h"
#include "NppIO.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QSettings>

CustomFileDialog::CustomFileDialog(QWidget* parent)
    : QWidget(parent)
{
}

CustomFileDialog::~CustomFileDialog()
{
}

QString CustomFileDialog::doOpenDlg(QWidget* parent, const QString& title,
                                     const QString& initialDir,
                                     const QString& filter,
                                     QString* selectedFilter,
                                     int options)
{
    Q_UNUSED(options);

    QFileDialog fd(parent, title, initialDir, filter);
    fd.setOption(QFileDialog::DontUseNativeDialog, false);
    fd.setAcceptMode(QFileDialog::AcceptOpen);

    if (selectedFilter && !selectedFilter->isEmpty())
        fd.selectNameFilter(*selectedFilter);

    if (fd.exec() == QDialog::Accepted) {
        QStringList files = fd.selectedFiles();
        if (!files.isEmpty()) {
            if (selectedFilter)
                *selectedFilter = fd.selectedNameFilter();
            return files.first();
        }
    }
    return QString();
}

QString CustomFileDialog::doSaveDlg(QWidget* parent, const QString& title,
                                     const QString& initialDir,
                                     const QString& filter,
                                     QString* selectedFilter,
                                     int options)
{
    Q_UNUSED(options);

    QFileDialog fd(parent, title, initialDir, filter);
    fd.setOption(QFileDialog::DontUseNativeDialog, false);
    fd.setAcceptMode(QFileDialog::AcceptSave);

    if (selectedFilter && !selectedFilter->isEmpty())
        fd.selectNameFilter(*selectedFilter);

    if (fd.exec() == QDialog::Accepted) {
        QStringList files = fd.selectedFiles();
        if (!files.isEmpty()) {
            if (selectedFilter)
                *selectedFilter = fd.selectedNameFilter();
            return files.first();
        }
    }
    return QString();
}

QStringList CustomFileDialog::doOpenMultiFilesDlg(QWidget* parent,
                                                    const QString& initialDir,
                                                    const QString& filter,
                                                    QString* selectedFilter)
{
    QFileDialog fd(parent, QString(), initialDir, filter);
    fd.setOption(QFileDialog::DontUseNativeDialog, false);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setFileMode(QFileDialog::ExistingFiles);

    if (selectedFilter && !selectedFilter->isEmpty())
        fd.selectNameFilter(*selectedFilter);

    if (fd.exec() == QDialog::Accepted) {
        if (selectedFilter)
            *selectedFilter = fd.selectedNameFilter();
        return fd.selectedFiles();
    }
    return QStringList();
}

QString CustomFileDialog::doDirectoryDlg(QWidget* parent,
                                          const QString& title,
                                          const QString& initialDir,
                                          int options)
{
    Q_UNUSED(options);

    // Use QFileDialog for directory selection
    return QFileDialog::getExistingDirectory(parent, title,
        initialDir, QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
}

void CustomFileDialog::setDefExt(const QString& ext)
{
    _defaultExt = ext;
}

void CustomFileDialog::setFilter(const QString& filter)
{
    _filter = filter;
}

void CustomFileDialog::setInitialDir(const QString& dir)
{
    _initialDir = dir;
}

QString CustomFileDialog::getCurrentDir() const
{
    return QDir::currentPath();
}

QString CustomFileDialog::getFileName() const
{
    return _lastSelectedFile;
}

void CustomFileDialog::setFileName(const QString& fileName)
{
    _lastSelectedFile = fileName;
}

bool CustomFileDialog::fileExist(const QString& fileName) const
{
    return QFileInfo::exists(fileName);
}

QString CustomFileDialog::getUniquePath(const QString& path, bool isLeft,
                                          bool doesExist, bool* isDup)
{
    QFileInfo fi(path);
    QString name = fi.fileName();
    QString base = fi.path();
    QString ext = fi.suffix();
    QString baseName = ext.isEmpty() ? name : name.chopped(ext.length() + 1);

    if (!doesExist) {
        *isDup = false;
        return path;
    }

    int num = 1;
    QString newName;
    do {
        if (ext.isEmpty())
            newName = QStringLiteral("%1 (%2)").arg(baseName).arg(num);
        else
            newName = QStringLiteral("%1 (%2).%3").arg(baseName).arg(num).arg(ext);
        ++num;
        *isDup = true;
    } while (QFileInfo::exists(base + QDir::separator() + newName));

    return base + QDir::separator() + newName;
}
