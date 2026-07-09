// AboutDialog.h - About dialog
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>

class QLabel;
class QPushButton;
class QTextBrowser;

class AboutDialog : public QDialog {
    Q_OBJECT
public:
    explicit AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog() override;

    void setVersion(const QString& version);
    void setDescription(const QString& desc);

private:
    QLabel* _titleLabel = nullptr;
    QLabel* _versionLabel = nullptr;
    QTextBrowser* _descriptionBrowser = nullptr;
};
