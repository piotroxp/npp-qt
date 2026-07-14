// AboutDialog.cpp - About dialog with credits, license, and build info
// Copyright (C) 2026 Agent Army
// SPDX-License-Identifier: GPL-3.0-only

#include "AboutDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QStackedWidget>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QDate>
#include <QPalette>
#include <QFont>
#include <QStyle>
#include <QDir>
#include <QFile>
#include <QRegularExpression>

// ─── GPL v3 License Text (first ~50 lines) ───────────────────────────────────
static const char* gpl3Prefix = R"(
                    GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                            Preamble

  The GNU General Public License is a free, copyleft license for
software and other kinds of works.

  The licenses for most software and other practical works are designed
to take away your freedom to share and change the works.  By contrast,
the GNU General Public License is intended to guarantee your freedom to
share and change all versions of a program--to make sure it remains free
software for all its users.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
them if you wish), that you receive source code or can get it if you
want it, that you can change the software or use pieces of it in new
free programs, and that you know you can do these things.

  To protect your rights, we need to prevent others from denying you
these rights or asking you to surrender the rights.  Therefore, you have
certain responsibilities if you distribute copies of the software, or if
you modify it.

                        TERMS AND CONDITIONS

  0. Definitions.

  "This License" refers to version 3 of the GNU General Public License.

  "Copyright" also means copyright-like laws that apply to other kinds of
works, such as semiconductor masks.

  "The Program" refers to any copyrightable work licensed under this
License.  Each licensee is addressed as "you".  "Licensees" and
"recipients" may be individuals or organizations.

  To "modify" a work means to copy from or adapt all or part of the work
in a fashion requiring copyright permission, other than the making of an
exact copy.  The resulting work is called a "modified version" of the
earlier work or a work "based on" the earlier work.

  A "covered work" means either the unmodified Program or a work based
on the Program.

  To "propagate" a work means to do anything with it that, without
permission, would make you directly or secondarily liable for
infringement under applicable copyright law, except executing it on a
computer or modifying a private copy.  Propagation includes copying,
distribution (with or without modification), making available to the
public, and in some countries other activities as well.

  To "convey" a work means any kind of propagation that enables other
parties to make or receive copies.  Mere interaction with a user
through a computer network, with no transfer of a copy, is not
conveying.

  An interactive user interface displays "Appropriate Legal Notices" to
the extent that it includes a convenient and prominently visible feature
that (1) displays an appropriate copyright notice, and (2) tells the user
that there is no warranty for the work (except to the extent that
warranties are provided), that licensees may convey the work under this
License, and how to view a copy of this License.  If the interface
presents a list of user commands or options, such as a menu, a prominent
item in the list meets this criterion.

)";

// ─── Credits content ──────────────────────────────────────────────────────────
static const char* creditsHtml = R"(
<html>
<head>
<style>
  body { background:#2b2b2b; color:#e0e0e0; font-family:Segoe UI, Arial, sans-serif; font-size:13px; }
  h1,h2,h3 { color:#f5c211; margin:8px 0; }
  h2 { font-size:16px; border-bottom:1px solid #555; padding-bottom:4px; }
  ul { margin:6px 0; }
  li { margin:3px 0; }
  .section { margin-bottom:16px; }
  .highlight { color:#80c0ff; }
  .mono { font-family:Courier New, monospace; color:#b0d0b0; }
</style>
</head>
<body>
<div class="section">
<h2>Notepad-next</h2>
<p>A Qt-powered Notepad++ fork</p>
<p>Licensed under <b>GNU General Public License v3</b>.</p>
</div>

<div class="section">
<h2>Original Project</h2>
<p><b>Notepad++</b> — Created and maintained by <span class="highlight">Don Ho</span></p>
<p><span class="mono">https://notepad-plus-plus.org</span></p>
<p>We gratefully acknowledge Don Ho's foundational work, which inspired this project.</p>
</div>

<div class="section">
<h2>Core Contributors</h2>
<ul>
<li><b>Agent Army</b> — Project Lead &amp; Core Development</li>
<li><b>Don Ho</b> — Original Notepad++ Author (Notepad-next is a fork)</li>
<li><b>Scintilla Project</b> — Text Editor Component</li>
</ul>
</div>

<div class="section">
<h2>Third-Party Libraries</h2>
<ul>
<li><b>Qt Framework</b> — Cross-platform UI toolkit <span class="mono">(qt.io)</span></li>
<li><b>Scintilla</b> — Powerful text editing component <span class="mono">(scintilla.org)</span></li>
<li><b>QScintilla</b> — Qt binding for Scintilla</li>
</ul>
</div>

<div class="section">
<h2>Special Thanks</h2>
<ul>
<li>Notepad++ community for years of plugin development and feedback</li>
<li>Open-source contributors worldwide</li>
<li>Qt Company for the excellent Qt framework</li>
</ul>
</div>

<div class="section">
<h2>Acknowledgment</h2>
<p>This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License.</p>
</div>
</body>
</html>
)";

// ─── Info / Changelog placeholder ───────────────────────────────────────────
static const char* infoHtml = R"(
<html>
<head>
<style>
  body { background:#2b2b2b; color:#e0e0e0; font-family:Segoe UI, Arial, sans-serif; font-size:13px; }
  h1,h2,h3 { color:#f5c211; margin:8px 0; }
  h2 { font-size:15px; border-bottom:1px solid #555; padding-bottom:4px; }
  ul { margin:6px 0; }
  li { margin:3px 0; }
  .mono { font-family:Courier New, monospace; color:#b0d0b0; }
  .new { color:#7f7; }
  .fix { color:#ffaa44; }
  .note { color:#88aaff; font-style:italic; }
</style>
</head>
<body>
<h1>Release Notes</h1>

<div style="margin-bottom:16px;">
<h2>Version 1.0.0 (Initial Release)</h2>
<ul>
<li class="new">First release of Notepad-next</li>
<li class="new">Qt6-based modern architecture</li>
<li class="new">Scintilla-powered text editing engine</li>
<li class="new">Dark theme support with modern UI</li>
<li class="new">Tab-based document interface</li>
<li class="new">Find and Replace functionality</li>
<li class="new">Syntax highlighting for 80+ languages</li>
</ul>
</div>

<div style="margin-bottom:16px;">
<h2>Known Issues</h2>
<ul>
<li class="fix">Some plugins from Notepad++ are not yet compatible</li>
<li class="fix">Macros feature pending implementation</li>
<li class="fix">File comparison plugin not yet ported</li>
</ul>
</div>

<div>
<h2>Upcoming Features</h2>
<ul>
<li class="note">Plugin API compatibility layer</li>
<li class="note">Enhanced file explorer panel</li>
<li class="note">Markdown preview mode</li>
<li class="note">Collaborative editing support</li>
<li class="note">Cloud sync integration</li>
</ul>
</div>
</body>
</html>
)";


// ─── Implementation ─────────────────────────────────────────────────────────

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
    , tabStack(nullptr)
    , creditsBrowser(nullptr)
    , licenseBrowser(nullptr)
    , infoBrowser(nullptr)
{
    setWindowTitle(QStringLiteral("About Notepad-next"));
    setMinimumSize(550, 450);
    setModal(true);
    resize(580, 520);

    // Allow resizing
    setSizeGripEnabled(true);

    setupUi();
    applyStyle();
}

AboutDialog::~AboutDialog() = default;

void AboutDialog::setupUi() {
    // Main vertical layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 16, 20, 16);
    mainLayout->setSpacing(12);

    // ── Logo + Title area ──────────────────────────────────────────────────
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(16);

    // Logo frame with rounded corners
    QFrame* logoFrame = new QFrame(this);
    logoFrame->setObjectName("logoFrame");
    logoFrame->setFixedSize(80, 80);
    QVBoxLayout* logoFrameLayout = new QVBoxLayout(logoFrame);
    logoFrameLayout->setContentsMargins(0, 0, 0, 0);
    logoFrameLayout->setAlignment(Qt::AlignCenter);

    logoLabel = new QLabel("N++", this);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setObjectName("logoText");
    logoFrameLayout->addWidget(logoLabel);

    // Title + subtitle + version
    QVBoxLayout* titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(2);
    titleLayout->setAlignment(Qt::AlignVCenter);

    titleLabel = new QLabel("Notepad-next", this);
    titleLabel->setObjectName("titleText");

    subtitleLabel = new QLabel(QStringLiteral("A Qt-powered Notepad++ fork"), this);
    subtitleLabel->setObjectName("subtitleText");

    QDate today = QDate::currentDate();
    versionLabel = new QLabel(
        QStringLiteral("Version %1  ·  %2")
            .arg("1.0.0")
            .arg(today.toString(QStringLiteral("yyyy-MM-dd"))),
        this);
    versionLabel->setObjectName("versionText");

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);
    titleLayout->addWidget(versionLabel);

    headerLayout->addWidget(logoFrame);
    headerLayout->addLayout(titleLayout, 1);

    mainLayout->addLayout(headerLayout);

    // ── Description ─────────────────────────────────────────────────────────
    descriptionLabel = new QLabel(
        QStringLiteral("A modern, cross-platform text editor built with Qt %1.\n"
                       "Licensed under GNU General Public License v3.")
            .arg(qVersion()),
        this);
    descriptionLabel->setObjectName("descriptionText");
    descriptionLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(descriptionLabel);

    // ── Tab stack for Credits / License / Info ─────────────────────────────
    tabStack = new QStackedWidget(this);
    tabStack->setObjectName("tabStack");

    creditsBrowser = new QTextBrowser(this);
    creditsBrowser->setObjectName("creditsBrowser");
    creditsBrowser->setOpenExternalLinks(true);
    creditsBrowser->setHtml(QString::fromUtf8(creditsHtml));

    licenseBrowser = new QTextBrowser(this);
    licenseBrowser->setObjectName("licenseBrowser");
    licenseBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
    licenseBrowser->setText(QString::fromUtf8(gpl3Prefix));

    infoBrowser = new QTextBrowser(this);
    infoBrowser->setObjectName("infoBrowser");
    infoBrowser->setOpenExternalLinks(true);
    infoBrowser->setHtml(QString::fromUtf8(infoHtml));

    tabStack->addWidget(creditsBrowser);
    tabStack->addWidget(licenseBrowser);
    tabStack->addWidget(infoBrowser);

    mainLayout->addWidget(tabStack, 1);

    // ── Button bar ─────────────────────────────────────────────────────────
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(8);

    creditsBtn = new QPushButton(QStringLiteral("Credits"), this);
    creditsBtn->setObjectName("tabBtn");
    creditsBtn->setCheckable(true);
    creditsBtn->setChecked(true);
    creditsBtn->setFixedHeight(32);

    licenseBtn = new QPushButton(QStringLiteral("License"), this);
    licenseBtn->setObjectName("tabBtn");
    licenseBtn->setCheckable(true);
    licenseBtn->setFixedHeight(32);

    infoBtn = new QPushButton(QStringLiteral("Release Notes"), this);
    infoBtn->setObjectName("tabBtn");
    infoBtn->setCheckable(true);
    infoBtn->setFixedHeight(32);

    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    websiteBtn = new QPushButton(QStringLiteral("Website"), this);
    websiteBtn->setObjectName("actionBtn");
    websiteBtn->setFixedHeight(32);

    closeBtn = new QPushButton(QStringLiteral("Close"), this);
    closeBtn->setObjectName("closeBtn");
    closeBtn->setFixedHeight(32);
    closeBtn->setDefault(true);

    btnLayout->addWidget(creditsBtn);
    btnLayout->addWidget(licenseBtn);
    btnLayout->addWidget(infoBtn);
    btnLayout->addWidget(spacer);
    btnLayout->addWidget(websiteBtn);
    btnLayout->addWidget(closeBtn);

    mainLayout->addLayout(btnLayout);

    // ── Build info footer ──────────────────────────────────────────────────
    // Detect build info from available macros
    QString compilerInfo;
#ifdef _MSC_VER
    compilerInfo = QStringLiteral("MSVC %1").arg(_MSC_VER);
#elif defined(__GNUC__)
    compilerInfo = QStringLiteral("GCC %1.%2.%3")
                       .arg(__GNUC__).arg(__GNUC_MINOR__).arg(__GNUC_PATCHLEVEL__);
#elif defined(__clang__)
    compilerInfo = QStringLiteral("Clang %1.%2.%3")
                       .arg(__clang_major__).arg(__clang_minor__).arg(__clang_patchlevel__);
#else
    compilerInfo = QStringLiteral("Unknown compiler");
#endif

    QString archInfo;
#ifdef __x86_64__
    archInfo = QStringLiteral("x86_64");
#elif defined(__i386__)
    archInfo = QStringLiteral("x86");
#elif defined(__aarch64__)
    archInfo = QStringLiteral("ARM64");
#elif defined(__arm__)
    archInfo = QStringLiteral("ARM");
#else
    archInfo = QStringLiteral("Unknown");
#endif

#if defined(NDEBUG) || !defined(DEBUG)
    QString buildType = QStringLiteral("Release");
#else
    QString buildType = QStringLiteral("Debug");
#endif

    QLabel* buildInfoLabel = new QLabel(
        QStringLiteral("%1  ·  %2  ·  %3")
            .arg(compilerInfo)
            .arg(archInfo)
            .arg(buildType),
        this);
    buildInfoLabel->setObjectName("buildInfoText");
    buildInfoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(buildInfoLabel);

    // ── Connections ────────────────────────────────────────────────────────
    connect(creditsBtn, &QPushButton::clicked, this, &AboutDialog::onShowCredits);
    connect(licenseBtn, &QPushButton::clicked, this, &AboutDialog::onShowLicense);
    connect(infoBtn, &QPushButton::clicked, this, &AboutDialog::onShowInfo);
    connect(websiteBtn, &QPushButton::clicked, this, &AboutDialog::onOpenWebsite);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void AboutDialog::applyStyle() {
    // Dark palette
    QPalette dark;
    dark.setColor(QPalette::Window, QColor(43, 43, 43));
    dark.setColor(QPalette::WindowText, QColor(224, 224, 224));
    dark.setColor(QPalette::Base, QColor(30, 30, 30));
    dark.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    dark.setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
    dark.setColor(QPalette::ToolTipText, QColor(224, 224, 224));
    dark.setColor(QPalette::Text, QColor(224, 224, 224));
    dark.setColor(QPalette::Button, QColor(53, 53, 53));
    dark.setColor(QPalette::ButtonText, QColor(224, 224, 224));
    dark.setColor(QPalette::BrightText, QColor(255, 255, 255));
    dark.setColor(QPalette::Highlight, QColor(245, 194, 17));
    dark.setColor(QPalette::HighlightedText, QColor(25, 25, 25));
    setPalette(dark);

    // Main stylesheet
    setStyleSheet(R"(
        QDialog {
            background: #2b2b2b;
        }
        #logoFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #3a3a3a, stop:1 #2b2b2b);
            border: 2px solid #555;
            border-radius: 12px;
        }
        #logoText {
            font-size: 36px;
            font-weight: 900;
            color: #f5c211;
            background: transparent;
        }
        #titleText {
            font-size: 24px;
            font-weight: 700;
            color: #f5c211;
            background: transparent;
        }
        #subtitleText {
            font-size: 13px;
            color: #b0b0b0;
            background: transparent;
        }
        #versionText {
            font-size: 12px;
            color: #888;
            background: transparent;
            margin-top: 4px;
        }
        #descriptionText {
            font-size: 12px;
            color: #c0c0c0;
            background: transparent;
            padding: 4px 12px;
        }
        #tabStack {
            border: 1px solid #404040;
            border-radius: 6px;
            background: #1e1e1e;
        }
        QTextBrowser {
            background: #1e1e1e;
            color: #e0e0e0;
            border: none;
            padding: 8px;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 13px;
        }
        QTextBrowser QScrollBar:vertical {
            background: #2b2b2b;
            width: 10px;
            border-radius: 5px;
        }
        QTextBrowser QScrollBar::handle:vertical {
            background: #555;
            border-radius: 5px;
            min-height: 20px;
        }
        QTextBrowser QScrollBar::handle:vertical:hover {
            background: #666;
        }
        QTextBrowser QScrollBar::add-line:vertical,
        QTextBrowser QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QTextBrowser QScrollBar:horizontal {
            background: #2b2b2b;
            height: 10px;
            border-radius: 5px;
        }
        QTextBrowser QScrollBar::handle:horizontal {
            background: #555;
            border-radius: 5px;
            min-width: 20px;
        }
        QTextBrowser QScrollBar::handle:horizontal:hover {
            background: #666;
        }
        #tabBtn {
            background: #3a3a3a;
            color: #c0c0c0;
            border: 1px solid #505050;
            border-radius: 6px;
            padding: 4px 14px;
            font-size: 13px;
            min-width: 80px;
        }
        #tabBtn:hover {
            background: #484848;
            color: #e0e0e0;
        }
        #tabBtn:checked {
            background: #f5c211;
            color: #1e1e1e;
            border-color: #d4a800;
            font-weight: 600;
        }
        #actionBtn {
            background: #3a3a3a;
            color: #c0c0c0;
            border: 1px solid #505050;
            border-radius: 6px;
            padding: 4px 14px;
            font-size: 13px;
        }
        #actionBtn:hover {
            background: #484848;
            color: #e0e0e0;
        }
        #closeBtn {
            background: #f5c211;
            color: #1e1e1e;
            border: 1px solid #d4a800;
            border-radius: 6px;
            padding: 4px 20px;
            font-size: 13px;
            font-weight: 600;
        }
        #closeBtn:hover {
            background: #ffcf33;
        }
        #closeBtn:pressed {
            background: #d4a800;
        }
        #buildInfoText {
            font-size: 11px;
            color: #666;
            background: transparent;
        }
    )");
}

void AboutDialog::onShowCredits() {
    tabStack->setCurrentWidget(creditsBrowser);
    creditsBtn->setChecked(true);
    licenseBtn->setChecked(false);
    infoBtn->setChecked(false);
}

void AboutDialog::onShowLicense() {
    tabStack->setCurrentWidget(licenseBrowser);
    creditsBtn->setChecked(false);
    licenseBtn->setChecked(true);
    infoBtn->setChecked(false);
}

void AboutDialog::onShowInfo() {
    tabStack->setCurrentWidget(infoBrowser);
    creditsBtn->setChecked(false);
    licenseBtn->setChecked(false);
    infoBtn->setChecked(true);
}

void AboutDialog::onOpenWebsite() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/notepad-next/notepad-next")));
}

void AboutDialog::setVersion(const QString& version) {
    _currentVersion = version;
    QDate today = QDate::currentDate();
    versionLabel->setText(
        QStringLiteral("Version %1  ·  %2")
            .arg(version)
            .arg(today.toString(QStringLiteral("yyyy-MM-dd"))));
}

void AboutDialog::setBuildInfo(const QString& info) {
    _buildInfo = info;
    // Append custom build info to the existing footer if provided
    if (!info.isEmpty()) {
        // The footer label already shows compiler/arch/buildtype
        // Custom info could be shown in the description or a tooltip
        descriptionLabel->setToolTip(info);
    }
}
