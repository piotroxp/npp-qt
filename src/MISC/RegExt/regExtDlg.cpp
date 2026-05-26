// MISC/RegExt/regExtDlg.cpp - Qt6 port of Registry Extension dialog
#include "regExtDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QGroupBox>

RegExtDlg::RegExtDlg(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Registration");
    resize(600, 400);
    
    auto* mainLayout = new QVBoxLayout(this);
    
    // Registered extensions group
    auto* regGroup = new QGroupBox("Registered Extensions", this);
    auto* regLayout = new QVBoxLayout(regGroup);
    auto* regList = new QListWidget(this);
    regLayout->addWidget(regList);
    
    auto* regBtnLayout = new QHBoxLayout;
    auto* refreshBtn = new QPushButton("Refresh", this);
    auto* deleteBtn = new QPushButton("Delete", this);
    regBtnLayout->addWidget(refreshBtn);
    regBtnLayout->addWidget(deleteBtn);
    regBtnLayout->addStretch();
    regLayout->addLayout(regBtnLayout);
    
    mainLayout->addWidget(regGroup);
    
    // Buttons
    auto* btnLayout = new QHBoxLayout;
    auto* registerBtn = new QPushButton("Register", this);
    auto* unregisterBtn = new QPushButton("Unregister", this);
    auto* closeBtn = new QPushButton("Close", this);
    btnLayout->addWidget(registerBtn);
    btnLayout->addWidget(unregisterBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);
    
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void RegExtDlg::showDialog()
{
    exec();
}

void RegExtDlg::getRegisteredExts()
{
    // On Qt/Linux, we don't have Windows registry
    // Extensions are managed through .desktop files and MIME types
}

void RegExtDlg::getDefSupportedExts()
{
    // Get default supported extensions
}

void RegExtDlg::addExt(const QString& ext)
{
    // Add extension to registered list
    Q_UNUSED(ext);
}

bool RegExtDlg::deleteExts(const QString& ext2Delete)
{
    Q_UNUSED(ext2Delete);
    return true;
}

void RegExtDlg::writeNppPath()
{
    // Write Notepad++ path to configuration
}