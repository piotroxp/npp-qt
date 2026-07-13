// npp-qt: MD5 checksum dialogs
#include "md5Dlgs.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QClipboard>
#include <QApplication>
#include <QCryptographicHash>

// =============================================================================
// MD5 utility
// =============================================================================
MD5::MD5() = default;

void MD5::addData(const QString& text)
{
    addData(text.toUtf8());
}

void MD5::addData(const QByteArray& data)
{
    _digest = QCryptographicHash::hash(data, QCryptographicHash::Md5);
}

QString MD5::result()
{
    return QString::fromLatin1(_digest.toHex()).toUpper();
}

QByteArray MD5::resultBytes()
{
    return _digest;
}

// =============================================================================
// MD5FromTextDlg
// =============================================================================
MD5FromTextDlg::MD5FromTextDlg(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("MD5 checksum (text)"));
    setModal(false);
    resize(450, 300);

    QVBoxLayout* mainLay = new QVBoxLayout(this);

    mainLay->addWidget(new QLabel(QStringLiteral("Enter or paste text:")));
    _textEdit = new QPlainTextEdit;
    mainLay->addWidget(_textEdit, 1);

    mainLay->addWidget(new QLabel(QStringLiteral("MD5 hash:")));
    _hashEdit = new QLineEdit;
    _hashEdit->setReadOnly(true);
    mainLay->addWidget(_hashEdit);

    QHBoxLayout* btnLay = new QHBoxLayout;
    btnLay->addStretch();
    _computeButton = new QPushButton(QStringLiteral("Compute"));
    _copyButton = new QPushButton(QStringLiteral("Copy"));
    _closeButton = new QPushButton(QStringLiteral("Close"));
    btnLay->addWidget(_computeButton);
    btnLay->addWidget(_copyButton);
    btnLay->addWidget(_closeButton);
    mainLay->addLayout(btnLay);

    connect(_computeButton, &QPushButton::clicked, this, &MD5FromTextDlg::onCompute);
    connect(_copyButton, &QPushButton::clicked, this, &MD5FromTextDlg::onCopyToClipboard);
    connect(_closeButton, &QPushButton::clicked, this, &MD5FromTextDlg::onClose);
}

MD5FromTextDlg::~MD5FromTextDlg() = default;

void MD5FromTextDlg::doDialog()
{
    show();
    raise();
}

void MD5FromTextDlg::destroy() { close(); }

void MD5FromTextDlg::onCompute()
{
    QString text = _textEdit->toPlainText();
    MD5 md5;
    md5.addData(text);
    _hashEdit->setText(md5.result());
}

void MD5FromTextDlg::onCopyToClipboard()
{
    if (!_hashEdit->text().isEmpty())
        QApplication::clipboard()->setText(_hashEdit->text());
}

void MD5FromTextDlg::onClose() { hide(); }

// =============================================================================
// MD5FromFilesDlg
// =============================================================================
MD5FromFilesDlg::MD5FromFilesDlg(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("MD5 checksum (files)"));
    setModal(false);
    resize(500, 350);

    QVBoxLayout* mainLay = new QVBoxLayout(this);

    QHBoxLayout* pathLay = new QHBoxLayout;
    pathLay->addWidget(new QLabel(QStringLiteral("File:")));
    _filePathEdit = new QLineEdit;
    _browseButton = new QPushButton(QStringLiteral("Browse..."));
    pathLay->addWidget(_filePathEdit, 1);
    pathLay->addWidget(_browseButton);
    mainLay->addLayout(pathLay);

    mainLay->addWidget(new QLabel(QStringLiteral("Results:")));
    _resultEdit = new QPlainTextEdit;
    _resultEdit->setReadOnly(true);
    mainLay->addWidget(_resultEdit, 1);

    QHBoxLayout* btnLay = new QHBoxLayout;
    btnLay->addStretch();
    _computeButton = new QPushButton(QStringLiteral("Compute"));
    _copyButton = new QPushButton(QStringLiteral("Copy"));
    _closeButton = new QPushButton(QStringLiteral("Close"));
    btnLay->addWidget(_computeButton);
    btnLay->addWidget(_copyButton);
    btnLay->addWidget(_closeButton);
    mainLay->addLayout(btnLay);

    connect(_browseButton, &QPushButton::clicked, this, &MD5FromFilesDlg::onBrowse);
    connect(_computeButton, &QPushButton::clicked, this, &MD5FromFilesDlg::onCompute);
    connect(_copyButton, &QPushButton::clicked, this, &MD5FromFilesDlg::onCopyToClipboard);
    connect(_closeButton, &QPushButton::clicked, this, &MD5FromFilesDlg::onClose);
}

MD5FromFilesDlg::~MD5FromFilesDlg() = default;

void MD5FromFilesDlg::doDialog()
{
    show();
    raise();
}

void MD5FromFilesDlg::destroy() { close(); }

void MD5FromFilesDlg::onBrowse()
{
    QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Select File"));
    if (!path.isEmpty())
        _filePathEdit->setText(path);
}

void MD5FromFilesDlg::onCompute()
{
    QString path = _filePathEdit->text().trimmed();
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        _resultEdit->setPlainText(QStringLiteral("Error: Cannot open file."));
        return;
    }

    QByteArray data = file.readAll();
    MD5 md5;
    md5.addData(data);
    QString hash = md5.result();
    QString info = QStringLiteral("%1  %2").arg(hash, path);
    _resultEdit->setPlainText(info);
}

void MD5FromFilesDlg::onCopyToClipboard()
{
    QString text = _resultEdit->toPlainText();
    if (!text.isEmpty())
        QApplication::clipboard()->setText(text.trimmed());
}

void MD5FromFilesDlg::onClose() { hide(); }
