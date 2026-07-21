// AboutDlg.cpp
#include "AboutDlg.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QApplication>

AboutDlg::AboutDlg(QWidget* parent)
    : StaticDialog(parent) {
    setWindowTitle(tr("About Notepad--"));
    setMinimumWidth(400);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel(tr("<h2>Notepad--</h2>"), this));
    layout->addWidget(new QLabel(tr("Version 1.0.0"), this));
    layout->addWidget(new QLabel(tr("A Qt-based text editor"), this));
    layout->addSpacing(10);

    auto* gplLabel = new QLabel(
        tr("This program is free software: you can redistribute it and/or modify "
           "it under the terms of the GNU General Public License as published by "
           "the Free Software Foundation, either version 3 of the License."),
        this);
    gplLabel->setWordWrap(true);
    layout->addWidget(gplLabel);

    layout->addSpacing(10);
    auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(btnBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(btnBox);
}

void AboutDlg::showAbout(QWidget* parent) {
    AboutDlg dlg(parent);
    dlg.exec();
}
