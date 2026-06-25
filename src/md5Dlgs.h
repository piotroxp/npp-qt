// npp-qt: MD5 checksum dialogs
#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QFileDialog>

// MD5 from text: compute MD5 of text entered in the dialog
class MD5FromTextDlg : public QDialog {
    Q_OBJECT
public:
    explicit MD5FromTextDlg(QWidget* parent = nullptr);
    ~MD5FromTextDlg() override;
    void doDialog();
    void destroy();
    const QWidget* getHSelf() const { return this; }

private slots:
    void onCompute();
    void onCopyToClipboard();
    void onClose();

private:
    QPlainTextEdit* _textEdit = nullptr;
    QLineEdit* _hashEdit = nullptr;
    QPushButton* _computeButton = nullptr;
    QPushButton* _copyButton = nullptr;
    QPushButton* _closeButton = nullptr;
};

// MD5 from files: compute MD5 of one or more files
class MD5FromFilesDlg : public QDialog {
    Q_OBJECT
public:
    explicit MD5FromFilesDlg(QWidget* parent = nullptr);
    ~MD5FromFilesDlg() override;
    void doDialog();
    void destroy();
    const QWidget* getHSelf() const { return this; }

private slots:
    void onBrowse();
    void onCompute();
    void onCopyToClipboard();
    void onClose();

private:
    QLineEdit* _filePathEdit = nullptr;
    QPushButton* _browseButton = nullptr;
    QPlainTextEdit* _resultEdit = nullptr;
    QPushButton* _computeButton = nullptr;
    QPushButton* _copyButton = nullptr;
    QPushButton* _closeButton = nullptr;
};

// Simple MD5 utility (for MD5FromTextDlg internal use)
class MD5 {
public:
    MD5();
    void addData(const QString& text);
    void addData(const QByteArray& data);
    QString result();
    QByteArray resultBytes();
private:
    QByteArray _digest;
};
