// npp-qt: Run dialog — execute shell command and show output
#pragma once
#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>

class RunDlg : public QDialog {
    Q_OBJECT
public:
    explicit RunDlg(QWidget* parent = nullptr);
    ~RunDlg() override;

    void init(void* hInst, QWidget*& parent);
    void doDialog();
    void display(int cmdID, bool isRTL = false);

private slots:
    void onRun();
    void onCancel();
    void onBrowse();

private:
    QLineEdit* _commandEdit = nullptr;
    QPlainTextEdit* _outputEdit = nullptr;
    QPushButton* _runButton = nullptr;
    QPushButton* _cancelButton = nullptr;
    QPushButton* _browseButton = nullptr;
};
