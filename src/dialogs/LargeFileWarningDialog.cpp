// LargeFileWarningDialog.cpp - Warning dialog for large file opening
// Copyright (C) 2026 Agent Army
// GPL v3

#include "LargeFileWarningDialog.h"
#include <QCoreApplication>
#include <QUrl>
#include <QStyle>
#include <QApplication>

// ============================================================================
// Construction
// ============================================================================

LargeFileWarningDialog::LargeFileWarningDialog(const QString& filePath,
                                                 qint64 fileSize,
                                                 QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Large File Warning"));
    setMinimumWidth(480);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUi(filePath, fileSize);
}

LargeFileWarningDialog::~LargeFileWarningDialog() = default;

// ============================================================================
// UI Setup
// ============================================================================

void LargeFileWarningDialog::setupUi(const QString& filePath, qint64 fileSize) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Warning icon and message
    QLabel* iconLabel = new QLabel(this);
    iconLabel->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxWarning)
                            .pixmap(48, 48));
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel* msgLabel = new QLabel(
        tr("<b>Opening a large file</b><br><br>"
           "The file <b>%1</b> is %2 in size.<br>"
           "Loading large files may cause the editor to become slow or unresponsive.")
            .arg(QUrl::toPercentEncoding(filePath))
            .arg(formatSize(fileSize)),
        this);
    msgLabel->setWordWrap(true);
    msgLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    mainLayout->addWidget(iconLabel);
    mainLayout->addWidget(msgLabel);
    mainLayout->addSpacing(8);

    // Options group
    QGroupBox* optionsGroup = new QGroupBox(tr("How would you like to open this file?"), this);
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroup);

    _radioNormal = new QRadioButton(
        tr("Open normally (load full content into memory)"), this);
    _radioNormal->setChecked(true);
    optionsLayout->addWidget(_radioNormal);

    _radioReadOnly = new QRadioButton(
        tr("Open in read-only mode (prevent accidental modifications)"), this);
    optionsLayout->addWidget(_radioReadOnly);

    _radioChunked = new QRadioButton(
        tr("Open with chunked/lazy loading (load visible portions on demand)"), this);
    optionsLayout->addWidget(_radioChunked);

    mainLayout->addWidget(optionsGroup);

    // Don't ask again
    _chkDontAsk = new QCheckBox(
        tr("Don't ask me again for files larger than 10 MB"), this);
    mainLayout->addWidget(_chkDontAsk);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    QPushButton* cancelBtn = new QPushButton(tr("Cancel"), this);
    cancelBtn->setAutoDefault(false);
    connect(cancelBtn, &QPushButton::clicked, this, &LargeFileWarningDialog::reject);
    btnLayout->addWidget(cancelBtn);

    QPushButton* openBtn = new QPushButton(tr("Open"), this);
    openBtn->setDefault(true);
    connect(openBtn, &QPushButton::clicked, this, [this]() {
        if (_radioNormal->isChecked()) {
            _selectedMode = OpenMode::Normal;
        } else if (_radioReadOnly->isChecked()) {
            _selectedMode = OpenMode::ReadOnly;
        } else {
            _selectedMode = OpenMode::Chunked;
        }
        accept();
    });
    btnLayout->addWidget(openBtn);

    mainLayout->addLayout(btnLayout);
}

QString LargeFileWarningDialog::formatSize(qint64 bytes) const {
    if (bytes < 1024)
        return tr("%1 B").arg(bytes);
    if (bytes < 1024 * 1024)
        return tr("%1 KB").arg(bytes / 1024);
    if (bytes < 1024 * 1024 * 1024)
        return tr("%1 MB").arg(bytes / (1024 * 1024));
    return tr("%1 GB").arg(bytes / (1024.0 * 1024 * 1024), 0, 'f', 2);
}

// ============================================================================
// Static Helper
// ============================================================================

LargeFileWarningDialog::OpenMode LargeFileWarningDialog::prompt(
        const QString& filePath,
        qint64 fileSize,
        QWidget* parent)
{
    LargeFileWarningDialog dlg(filePath, fileSize, parent);
    if (dlg.exec() == QDialog::Accepted)
        return dlg.selectedMode();
    return OpenMode::Cancel;
}


void LargeFileWarningDialog::onOpenChunked() {
    _selectedMode = OpenMode::Chunked;
    accept();
}


void LargeFileWarningDialog::onOpenNormally() {
    _selectedMode = OpenMode::Normal;
    accept();
}

void LargeFileWarningDialog::onOpenReadOnly() {
    _selectedMode = OpenMode::ReadOnly;
    accept();
}

void LargeFileWarningDialog::onCancel() {
    _selectedMode = OpenMode::Cancel;
    reject();
}
