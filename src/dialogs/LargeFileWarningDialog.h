// LargeFileWarningDialog.h - Warning dialog for large file opening
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

class LargeFileWarningDialog : public QDialog {
    Q_OBJECT
public:
    enum class OpenMode {
        Normal, ReadOnly, Chunked, Cancel
    };

    explicit LargeFileWarningDialog(const QString& filePath, qint64 fileSize,
                                    QWidget* parent = nullptr);
    ~LargeFileWarningDialog() override;

    OpenMode selectedMode() const { return _selectedMode; }
    bool dontAskAgain() const { return _chkDontAsk->isChecked(); }
    static OpenMode prompt(const QString& filePath, qint64 fileSize, QWidget* parent);

private slots:
    void onOpenChunked();

private:
    void setupUi(const QString& filePath, qint64 fileSize);
    QString formatSize(qint64 bytes) const;

    OpenMode _selectedMode = OpenMode::Cancel;
    QRadioButton* _radioNormal = nullptr;
    QRadioButton* _radioReadOnly = nullptr;
    QRadioButton* _radioChunked = nullptr;
    QCheckBox* _chkDontAsk = nullptr;
};
