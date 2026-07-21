// FindCharsInRangeDialog.h - Find Characters in Range dialog
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class ScintillaEditor;

// Background worker for scanning characters
class FindCharsWorker : public QThread {
    Q_OBJECT

public:
    explicit FindCharsWorker(ScintillaEditor* editor, int startHex, int endHex,
                              int maxResults, QObject* parent = nullptr);
    ~FindCharsWorker() override;

    void run() override;
    void requestCancel();

signals:
    void resultReady(int line, int col, int byteValue);
    void progress(int scanned, int found);
    void finished(int totalFound);

private:
    ScintillaEditor* _editor;
    int _startHex;
    int _endHex;
    int _maxResults;
    QAtomicInt _cancelled{0};
};

class FindCharsInRangeDialog : public QDialog {
    Q_OBJECT

public:
    explicit FindCharsInRangeDialog(ScintillaEditor* editor, QWidget* parent = nullptr);
    ~FindCharsInRangeDialog() override;

signals:
    void charactersFound(const QVector<QPair<int, QString>>& results);

private slots:
    void onFindClicked();
    void onCloseClicked();
    void onCancelClicked();
    void onWorkerResult(int line, int col, int byteValue);
    void onWorkerProgress(int scanned, int found);
    void onWorkerFinished(int totalFound);
    void onResultClicked();

private:
    void setupUi();
    void applyStyle();
    void populateResults(const QVector<QPair<int, QString>>& results);
    void setControlsEnabled(bool enabled);
    void updateCountLabel();
    QString formatCharacter(int byteValue);

    ScintillaEditor* _editor = nullptr;

    QSpinBox* _startRange = nullptr;
    QSpinBox* _endRange = nullptr;
    QSpinBox* _maxResults = nullptr;
    QTextEdit* _results = nullptr;
    QLabel* _countLabel = nullptr;
    QLabel* _statusLabel = nullptr;
    QPushButton* _findBtn = nullptr;
    QPushButton* _cancelBtn = nullptr;
    QPushButton* _closeBtn = nullptr;

    FindCharsWorker* _worker = nullptr;

    // Accumulated results
    QVector<QPair<int, QString>> _allResults;
    int _totalScanned = 0;
};
