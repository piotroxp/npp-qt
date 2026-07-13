#pragma once

#include <QWidget>
#include <QString>
#include <QStringList>

class CustomFileDialog : public QObject
{
    Q_OBJECT
public:
    explicit CustomFileDialog(QWidget* parent = nullptr);
    ~CustomFileDialog() override;

    // Single file open
    static QString doOpenDlg(QWidget* parent,
                              const QString& title,
                              const QString& initialDir,
                              const QString& filter,
                              QString* selectedFilter = nullptr,
                              int options = 0);

    // Single file save
    static QString doSaveDlg(QWidget* parent,
                              const QString& title,
                              const QString& initialDir,
                              const QString& filter,
                              QString* selectedFilter = nullptr,
                              int options = 0);

    // Multiple file open
    static QStringList doOpenMultiFilesDlg(QWidget* parent,
                                             const QString& initialDir,
                                             const QString& filter,
                                             QString* selectedFilter = nullptr);

    // Directory picker
    static QString doDirectoryDlg(QWidget* parent,
                                    const QString& title,
                                    const QString& initialDir,
                                    int options = 0);

    // Helpers
    void setDefExt(const QString& ext);
    void setFilter(const QString& filter);
    void setInitialDir(const QString& dir);
    QString getCurrentDir() const;
    QString getFileName() const;
    void setFileName(const QString& fileName);
    bool fileExist(const QString& fileName) const;

    static QString getUniquePath(const QString& path,
                                   bool isLeft,
                                   bool doesExist,
                                   bool* isDup);

private:
    QString _defaultExt;
    QString _filter;
    QString _initialDir;
    QString _lastSelectedFile;
};

