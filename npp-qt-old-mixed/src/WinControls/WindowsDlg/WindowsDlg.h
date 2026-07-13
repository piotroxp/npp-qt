#pragma once

#include <QDialog>
#include <QString>
#include <QStringList>

class WindowsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit WindowsDlg(QWidget* parent = nullptr);
    ~WindowsDlg() override;

    void init(QWidget* parent);
    void doDialog();

    QString getSelectedWindowTitle() const;
    void setSelectedWindowTitle(const QString& title);
    QStringList getAllWindowTitles() const;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QWidget* _parent = nullptr;
};

