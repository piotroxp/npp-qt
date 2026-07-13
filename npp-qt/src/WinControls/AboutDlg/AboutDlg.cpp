// Semantic Lift: AboutDlg — Win32 dialog + RichEdit → Qt6 QDialog + QTextBrowser
// Original: PowerEditor/src/WinControls/AboutDlg/AboutDlg.cpp (1031 lines)
// Win32 → Qt6: DialogBoxParam + RichEdit20W + ShellExecute → QDialog + QTextBrowser + QDesktopServices

#include "AboutDlg.h"
#include "StaticDialog.h"
#include "NppDarkMode.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

AboutDlg::AboutDlg()
    : QDialog(nullptr)
    , _parent(nullptr)
{
    setWindowTitle(tr("About Notepad++"));
    setMinimumSize(520, 420);
    setModal(true);

    QVBoxLayout* mainLay = new QVBoxLayout(this);

    // Icon + title
    QHBoxLayout* titleLay = new QHBoxLayout();
    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(QPixmap(":/icons/about.png").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setFixedSize(64, 64);

    QVBoxLayout* titleTextLay = new QVBoxLayout();
    QLabel* nameLabel = new QLabel(QStringLiteral("Notepad++ (Qt port)"));
    QLabel* versionLabel = new QLabel(QStringLiteral("Version 8.x Qt6"));
    QLabel* descLabel = new QLabel(tr("Free (LGPL) Multi-Language Text Editor"));
    nameLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    titleTextLay->addWidget(nameLabel);
    titleTextLay->addWidget(versionLabel);
    titleTextLay->addWidget(descLabel);

    titleLay->addWidget(iconLabel);
    titleLay->addLayout(titleTextLay);
    titleLay->addStretch();
    mainLay->addLayout(titleLay);

    mainLay->addSpacing(10);

    // Rich text browser for credits/links
    QTextBrowser* browser = new QTextBrowser();
    browser->setOpenExternalLinks(true);
    browser->setHtml(buildAboutHtml());
    browser->setMinimumHeight(280);
    mainLay->addWidget(browser);

    // Buttons
    QDialogButtonBox* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    mainLay->addWidget(btnBox);

    NppDarkMode::instance().autoNppDarkMode(this);
}

AboutDlg::~AboutDlg()
{
}

void AboutDlg::setParent(QWidget* parent)
{
    _parent = parent;
    setParent(parent);
    QDialog::setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void AboutDlg::doDialog()
{
    if (!isVisible()) {
        show();
    } else {
        raise();
        activateWindow();
    }
}

void AboutDlg::display(int tabIndex)
{
    Q_UNUSED(tabIndex);
    doDialog();
}

QString AboutDlg::buildAboutHtml() const
{
    return QStringLiteral(R"(
<!DOCTYPE html>
<html>
<head>
<style>
body { font-family: sans-serif; font-size: 12px; background: #2d2d2d; color: #e0e0e0; margin: 8px; }
a { color: #4da6ff; }
b { color: #ffffff; }
</style>
</head>
<body>
<p>This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.</p>

<p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.</p>

<p>Ported from Notepad++ (Win32) to Qt6. Original Notepad++ by <b>Don Ho</b>.</p>

<p>Qt port maintainers: npp-qt team<br/>
<a href="https://notepad-plus-plus.org">https://notepad-plus-plus.org</a></p>

<p>Powered by:</p>
<ul>
<li>Qt Framework (Qt6)</li>
<li>QScintilla (Scintilla port)</li>
<li>Boost (header-only libraries)</li>
<li>pugixml (XML parsing)</li>
</ul>

<p>Icon sets from <b>Florian X.</b> and other contributors.</p>

<p>Plugins API compatibility layer included.</p>
</body>
</html>
)");
}

void AboutDlg::goToUpdateSite(int tabIndex)
{
    Q_UNUSED(tabIndex);
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://notepad-plus-plus.org/downloads/")));
}

void AboutDlg::linkSharableLink(int which)
{
    Q_UNUSED(which);
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://notepad-plus-plus.org/")));
}

void AboutDlg::linkChangelog(int which)
{
    Q_UNUSED(which);
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://notepad-plus-plus.org/downloads/#changelog")));
}

bool AboutDlg::OpenFileInNewInstance(const QString& filePath)
{
    Q_UNUSED(filePath);
    return false; // Let Qt handle this
}

bool AboutDlg::openConfigFileInNewInstance(const QString& configFilePath)
{
    Q_UNUSED(configFilePath);
    return false;
}
