// StatusBar.h - Status bar widget with comprehensive document info
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QStatusBar>
#include <QLabel>
#include <QToolButton>
#include <QProgressBar>
#include <QTimer>
#include <QHash>

class StatusBar : public QStatusBar {
    Q_OBJECT
public:
    explicit StatusBar(QWidget* parent = nullptr);
    ~StatusBar() override;

    // Basic setters
    void setPosition(int line, int col);
    void setEncoding(const QString& enc);
    void setEol(const QString& eol);
    void setLanguage(const QString& lang);
    void setFileType(const QString& type);
    void setMessage(const QString& msg);
    void setSelection(int chars, int lines);
    void clearMessage();

    // Extended setters
    void setDocumentInfo(int currentLine, int totalLines, int percentage);
    void setTabSize(int size);
    void setIndentSize(int spaces);
    void setBOM(bool hasBom);
    void setReadOnly(bool readOnly);
    void setModified(bool modified);
    void setZoomLevel(int level);
    void setInsertMode(bool insert);
    void setEolMode(const QString& mode);
    void setLineEnding(const QString& ending);
    void setCursorPosition(int line, int col, int absoluteOffset);

    // Progress operations
    void showProgress(int value, int maximum = 100, const QString& text = QString());
    void clearProgress();
    bool isShowingProgress() const { return m_showingProgress; }

    // Notification messages (temporary)
    void showNotification(const QString& msg, int durationMs = 3000);
    void clearNotification();

    // Style/Theme
    void setDarkTheme(bool dark);
    void applyTheme();

Q_SIGNALS:
    void positionClicked();
    void encodingClicked();
    void eolClicked();
    void languageClicked();
    void zoomClicked(int direction);  // +1 for zoom in, -1 for zoom out
    void readOnlyClicked();
    void progressCancelled();

public slots:
    void onAutoRefreshTimeout();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void onProgressCancelClicked();
    void updateNotificationTimer();

private:
    void setupLabels();
    void setupConnections();
    void applyDpiScaling();
    void updateNotificationDisplay();
    QLabel* createClickableLabel(const QString& text);
    QString formatEolMode(const QString& mode) const;
    QString formatZoomLevel(int level) const;

    // Labels
    QLabel* _posLabel = nullptr;
    QLabel* _encLabel = nullptr;
    QLabel* _eolLabel = nullptr;
    QLabel* _langLabel = nullptr;
    QLabel* _typeLabel = nullptr;
    QLabel* _selLabel = nullptr;
    QLabel* _msgLabel = nullptr;
    QLabel* _docInfoLabel = nullptr;
    QLabel* _tabIndentLabel = nullptr;
    QLabel* _bomLabel = nullptr;
    QLabel* _readOnlyLabel = nullptr;
    QLabel* _modifiedLabel = nullptr;
    QLabel* _zoomLabel = nullptr;
    QLabel* _modeLabel = nullptr;
    QLabel* _eolTypeLabel = nullptr;
    QLabel* _notificationLabel = nullptr;

    // Progress
    QProgressBar* _progressBar = nullptr;
    QToolButton* _progressCancelBtn = nullptr;
    bool m_showingProgress = false;
    QTimer* _progressTimer = nullptr;

    // Notification
    QTimer* _notificationTimer = nullptr;
    QString _currentNotification;
    int _notificationDuration = 3000;

    // State tracking
    bool m_darkTheme = true;
    int m_currentLine = 1;
    int m_totalLines = 1;
    int m_percentage = 100;
    int m_currentCol = 1;
    int m_absoluteOffset = 0;
    int m_tabSize = 4;
    int m_indentSize = 4;
    bool m_hasBom = false;
    bool m_readOnly = false;
    bool m_modified = false;
    int m_zoomLevel = 0;
    bool m_insertMode = true;
    QString m_eolMode;
    QString m_lineEnding;
    int m_selectionChars = 0;
    int m_selectionLines = 0;
};
