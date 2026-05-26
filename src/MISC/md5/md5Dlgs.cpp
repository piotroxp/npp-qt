// MISC/md5/md5Dlgs.cpp - Qt6 port of MD5 hash dialog
#include "md5Dlgs.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QFileDialog>
#include <QGroupBox>

HashFromFilesDlg::HashFromFilesDlg(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Hash from Files");
    auto* mainLayout = new QVBoxLayout(this);
    
    auto* fileLayout = new QHBoxLayout;
    auto* fileLabel = new QLabel("Files:", this);
    auto* fileEdit = new QLineEdit(this);
    auto* browseBtn = new QPushButton("Browse...", this);
    fileLayout->addWidget(fileLabel);
    fileLayout->addWidget(fileEdit);
    fileLayout->addWidget(browseBtn);
    mainLayout->addLayout(fileLayout);
    
    auto* hashLayout = new QHBoxLayout;
    auto* hashLabel = new QLabel("Hash type:", this);
    auto* hashCombo = new QComboBox(this);
    hashCombo->addItem("MD5", hash_md5);
    hashCombo->addItem("SHA-1", hash_sha1);
    hashCombo->addItem("SHA-256", hash_sha256);
    hashCombo->addItem("SHA-512", hash_sha512);
    hashLayout->addWidget(hashLabel);
    hashLayout->addWidget(hashCombo);
    mainLayout->addLayout(hashLayout);
    
    auto* resultLabel = new QLabel("Result:", this);
    auto* resultEdit = new QTextEdit(this);
    resultEdit->setReadOnly(true);
    mainLayout->addWidget(resultLabel);
    mainLayout->addWidget(resultEdit);
    
    auto* btnLayout = new QHBoxLayout;
    auto* calcBtn = new QPushButton("Calculate", this);
    auto* closeBtn = new QPushButton("Close", this);
    btnLayout->addWidget(calcBtn);
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);
    
    connect(browseBtn, &QPushButton::clicked, this, [fileEdit]() {
        QStringList files = QFileDialog::getOpenFileNames(nullptr, "Select files");
        if (!files.isEmpty()) {
            fileEdit->setText(files.join(";"));
        }
    });
    
    connect(hashCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        _ht = hashCombo->itemData(idx).toInt();
    });
    
    connect(calcBtn, &QPushButton::clicked, this, [resultEdit, fileEdit, this]() {
        QString files = fileEdit->text();
        if (files.isEmpty()) return;
        
        QStringList fileList = files.split(';');
        QString result;
        
        for (const QString& file : fileList) {
            QFile f(file.trimmed());
            if (f.open(QIODevice::ReadOnly)) {
                QCryptographicHash hash(_ht == hash_md5 ? QCryptographicHash::Md5 : 
                                        _ht == hash_sha1 ? QCryptographicHash::Sha1 :
                                        _ht == hash_sha256 ? QCryptographicHash::Sha256 : QCryptographicHash::Sha512);
                hash.addData(&f);
                result += file + ": " + hash.result().toHex() + "\n";
                f.close();
            }
        }
        
        resultEdit->setText(result);
    });
    
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void HashFromFilesDlg::accept() { QDialog::accept(); }
void HashFromFilesDlg::reject() { QDialog::reject(); }
void HashFromFilesDlg::setHashType(int hashType2set) { _ht = hashType2set; }

HashFromTextDlg::HashFromTextDlg(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Hash from Text");
    auto* mainLayout = new QVBoxLayout(this);
    
    auto* textLabel = new QLabel("Text to hash:", this);
    auto* textEdit = new QTextEdit(this);
    mainLayout->addWidget(textLabel);
    mainLayout->addWidget(textEdit);
    
    auto* hashLayout = new QHBoxLayout;
    auto* hashLabel = new QLabel("Hash type:", this);
    auto* hashCombo = new QComboBox(this);
    hashCombo->addItem("MD5", hash_md5);
    hashCombo->addItem("SHA-1", hash_sha1);
    hashCombo->addItem("SHA-256", hash_sha256);
    hashCombo->addItem("SHA-512", hash_sha512);
    hashLayout->addWidget(hashLabel);
    hashLayout->addWidget(hashCombo);
    mainLayout->addLayout(hashLayout);
    
    auto* resultLabel = new QLabel("Result:", this);
    auto* resultEdit = new QTextEdit(this);
    resultEdit->setReadOnly(true);
    mainLayout->addWidget(resultLabel);
    mainLayout->addWidget(resultEdit);
    
    auto* btnLayout = new QHBoxLayout;
    auto* calcBtn = new QPushButton("Calculate", this);
    auto* calcLineBtn = new QPushButton("Calculate per Line", this);
    auto* closeBtn = new QPushButton("Close", this);
    btnLayout->addWidget(calcBtn);
    btnLayout->addWidget(calcLineBtn);
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);
    
    connect(hashCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        _ht = hashCombo->itemData(idx).toInt();
    });
    
    connect(calcBtn, &QPushButton::clicked, this, [resultEdit, textEdit, this]() {
        QString text = textEdit->toPlainText();
        QCryptographicHash::Algorithm algo = 
            _ht == hash_md5 ? QCryptographicHash::Md5 :
            _ht == hash_sha1 ? QCryptographicHash::Sha1 :
            _ht == hash_sha256 ? QCryptographicHash::Sha256 : QCryptographicHash::Sha512;
        QCryptographicHash hash(algo);
        hash.addData(text.toUtf8());
        resultEdit->setText(hash.result().toHex());
    });
    
    connect(calcLineBtn, &QPushButton::clicked, this, [resultEdit, textEdit, this]() {
        QString text = textEdit->toPlainText();
        QStringList lines = text.split('\n');
        QString result;
        
        for (const QString& line : lines) {
            QCryptographicHash::Algorithm algo = 
                _ht == hash_md5 ? QCryptographicHash::Md5 :
                _ht == hash_sha1 ? QCryptographicHash::Sha1 :
                _ht == hash_sha256 ? QCryptographicHash::Sha256 : QCryptographicHash::Sha512;
            QCryptographicHash hash(algo);
            hash.addData(line.toUtf8());
            result += line + ": " + hash.result().toHex() + "\n";
        }
        
        resultEdit->setText(result);
    });
    
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void HashFromTextDlg::accept() { QDialog::accept(); }
void HashFromTextDlg::reject() { QDialog::reject(); }
void HashFromTextDlg::setHashType(int hashType2set) { _ht = hashType2set; }
void HashFromTextDlg::generateHash() {}
void HashFromTextDlg::generateHashPerLine() {}