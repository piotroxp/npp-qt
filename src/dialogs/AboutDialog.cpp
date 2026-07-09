// AboutDialog.cpp
// Copyright (C) 2026 Agent Army
// GPL v3

#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QDialogButtonBox>

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("About Notepad--Qt");
    setMinimumSize(450, 350);
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    _titleLabel = new QLabel("<h2>Notepad--Qt</h2>", this);
    _titleLabel->setAlignment(Qt::AlignCenter);

    _versionLabel = new QLabel("Version 1.0.0", this);
    _versionLabel->setAlignment(Qt::AlignCenter);

    _descriptionBrowser = new QTextBrowser(this);
    _descriptionBrowser->setOpenExternalLinks(true);
    _descriptionBrowser->setHtml(
        "<p>A Qt-based text editor, written by Agent Army.</p>"
        "<p>Licensed under the <b>GNU General Public License v3</b>.</p>"
        "<p>Built with Qt6 and QScintilla.</p>"
        "<hr><p>Copyright (C) 2026 Agent Army. All rights reserved.</p>"
    );

    QDialogButtonBox* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);

    mainLayout->addWidget(_titleLabel);
    mainLayout->addWidget(_versionLabel);
    mainLayout->addWidget(_descriptionBrowser, 1);
    mainLayout->addWidget(btnBox);

    connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

AboutDialog::~AboutDialog() = default;

void AboutDialog::setVersion(const QString& version) {
    _versionLabel->setText(version);
}

void AboutDialog::setDescription(const QString& desc) {
    _descriptionBrowser->setPlainText(desc);
}
