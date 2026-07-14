// AboutDialog.h - About dialog with credits, license, and build info
// Copyright (C) 2026 Agent Army
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QPalette>
#include <QStackedWidget>

class AboutDialog : public QDialog {
    Q_OBJECT
public:
    explicit AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog() override;

    void setVersion(const QString& version);
    void setBuildInfo(const QString& info);

private slots:
    void onShowCredits();
    void onShowLicense();
    void onShowInfo();
    void onOpenWebsite();

private:
    void setupUi();
    void loadCredits();
    void loadLicense();
    void loadInfo();
    void applyStyle();

    QLabel* logoLabel = nullptr;
    QLabel* titleLabel = nullptr;
    QLabel* subtitleLabel = nullptr;
    QLabel* versionLabel = nullptr;
    QLabel* descriptionLabel = nullptr;
    QPushButton* creditsBtn = nullptr;
    QPushButton* licenseBtn = nullptr;
    QPushButton* websiteBtn = nullptr;
    QPushButton* closeBtn = nullptr;
    QPushButton* infoBtn = nullptr;
    QStackedWidget* tabStack = nullptr;
    QTextBrowser* creditsBrowser = nullptr;
    QTextBrowser* licenseBrowser = nullptr;
    QTextBrowser* infoBrowser = nullptr;

    QString _currentVersion;
    QString _buildInfo;
};
