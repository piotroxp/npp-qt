// FileReloadDialog.cpp - Prompt when file is modified externally
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FileReloadDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

FileReloadDialog::FileReloadDialog(const QString& filePath, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("File Changed"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumWidth(400);

    QVBoxLayout* main = new QVBoxLayout(this);

    QLabel* msg = new QLabel(
        tr("The file <b>%1</b> has been modified externally.")
            .arg(filePath),
        this);
    msg->setWordWrap(true);
    main->addWidget(msg);

    QGroupBox* group = new QGroupBox(tr("What would you like to do?"), this);
    QVBoxLayout* vbox = new QVBoxLayout(group);

    _radioReload = new QRadioButton(tr("Reload the file from disk"), this);
    _radioReload->setChecked(true);
    vbox->addWidget(_radioReload);

    _radioKeep = new QRadioButton(tr("Keep the current version (don't reload)"), this);
    vbox->addWidget(_radioKeep);

    _radioNothing = new QRadioButton(tr("Do nothing (silently ignore)"), this);
    vbox->addWidget(_radioNothing);

    main->addWidget(group);

    _chkDontAskAll = new QCheckBox(
        tr("Remember this choice for all files modified during this session"), this);
    main->addWidget(_chkDontAskAll);

    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addStretch();

    QPushButton* cancelBtn = new QPushButton(tr("Cancel"), this);
    cancelBtn->setAutoDefault(false);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnRow->addWidget(cancelBtn);

    QPushButton* okBtn = new QPushButton(tr("OK"), this);
    okBtn->setDefault(true);
    connect(okBtn, &QPushButton::clicked, this, [this]() {
        if (_radioReload->isChecked())
            _action = Action::Reload;
        else if (_radioKeep->isChecked())
            _action = Action::KeepDisk;
        else
            _action = Action::DoNothing;
        accept();
    });
    btnRow->addWidget(okBtn);

    main->addLayout(btnRow);
}

FileReloadDialog::~FileReloadDialog() = default;

FileReloadDialog::Action FileReloadDialog::prompt(const QString& filePath, QWidget* parent) {
    FileReloadDialog dlg(filePath, parent);
    if (dlg.exec() == QDialog::Accepted)
        return dlg.selectedAction();
    return Action::DoNothing;
}
