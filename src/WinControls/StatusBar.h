// Semantic Lift: Win32 StatusBar → Qt6 QStatusBar
// Original: PowerEditor/src/WinControls/StatusBar/StatusBar.h
// Target: npp-qt/src/WinControls/StatusBar.h

#pragma once

#include <QStatusBar>
#include <QLabel>
#include <QTimer>
#include <QHash>
#include <QString>

enum class StatusBarPart {
    Ready = 0,
    Position = 1,
    Length = 2,
    Encoding = 3,
    EolFormat = 4,
    Language = 5,
    FileSize = 6,
    PollInterval = 7,
};

class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    StatusBar(QWidget* parent = nullptr);
    ~StatusBar() override;

    void init(QApplication* app, QWidget* parent);
    void destroy();

    void setText(const QString& text, int partIndex = 0);
    QString text(int partIndex = 0) const;

    void setInt(int value, int partIndex);
    void flash(const QString& message, int durationMs = 3000);

    void showAnimate(bool show = true);
    void hideAnimate();

    void setEncoding(const QString& encoding);
    void setEol(const QString& eol);
    void setLanguage(const QString& language);

    void setFileName(const QString& fileName);
    void setDocType(const QString& docType);

    void setProgress(int percent);
    void clearProgress();

signals:
    void statusClicked(int part);
    void statusDoubleClicked(int part);

public slots:
    void onDocModified();
    void updateStatus();

private slots:
    void onFlashTimeout();

private:
    struct PartInfo {
        QLabel* label = nullptr;
        int fixedWidth = -1;
        int minWidth = 0;
    };

    QLabel* getOrCreatePart(int partIndex);

    QHash<int, PartInfo> _parts;
    QLabel* _pAnimateLabel = nullptr;
    QTimer* _pFlashTimer = nullptr;
    QString _lastText;
    QString _lastLanguage;
    QString _lastEncoding;
    QString _lastEol;
};
