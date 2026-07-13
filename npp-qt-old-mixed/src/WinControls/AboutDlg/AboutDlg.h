#pragma once

#include <QDialog>
#include <QString>

class AboutDlg : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDlg();
    ~AboutDlg() override;

    void setParent(QWidget* parent);
    void doDialog();
    void display(int tabIndex = 0);

    static void goToUpdateSite(int tabIndex = 0);
    static void linkSharableLink(int which = 0);
    static void linkChangelog(int which = 0);
    static bool OpenFileInNewInstance(const QString& filePath);
    static bool openConfigFileInNewInstance(const QString& configFilePath);

private:
    QString buildAboutHtml() const;
    QWidget* _parent = nullptr;
};

