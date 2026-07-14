// StatusBar.cpp - Status bar widget with comprehensive document info
// Copyright (C) 2026 Agent Army
// GPL v3

#include "StatusBar.h"
#include "DpiManager.h"
#include <QLabel>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QProgressBar>
#include <QToolButton>
#include <QStyle>

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent) {
    setSizeGripEnabled(true);
    setupLabels();
    setupConnections();
    applyDpiScaling();
    applyTheme();
}

StatusBar::~StatusBar() = default;

void StatusBar::setupLabels() {
    // Create clickable labels
    _posLabel = createClickableLabel("Ln 1, Col 1");
    _encLabel = createClickableLabel("UTF-8");
    _eolLabel = createClickableLabel("LF");
    _langLabel = createClickableLabel("Normal");
    _typeLabel = createClickableLabel("Plain Text");
    _selLabel = new QLabel(this);
    _selLabel->setMinimumWidth(60);
    _msgLabel = new QLabel(this);
    _msgLabel->setStyleSheet("QLabel { font-weight: bold; }");
    _msgLabel->setMinimumWidth(150);
    _msgLabel->setVisible(false);

    // Extended labels
    _docInfoLabel = new QLabel(this);
    _docInfoLabel->setText("Ln 1 of 1 (100%)");
    
    _tabIndentLabel = new QLabel(this);
    _tabIndentLabel->setText("Tab Size: 4");
    
    _bomLabel = new QLabel(this);
    _bomLabel->setText("BOM");
    _bomLabel->setVisible(false);
    _bomLabel->setToolTip("Byte Order Mark present");
    
    _readOnlyLabel = new QLabel(this);
    _readOnlyLabel->setText("RO");
    _readOnlyLabel->setVisible(false);
    _readOnlyLabel->setToolTip("Read-Only file");
    _readOnlyLabel->setCursor(Qt::PointingHandCursor);
    connect(_readOnlyLabel, &QLabel::linkActivated, this, &StatusBar::readOnlyClicked);
    
    _modifiedLabel = new QLabel(this);
    _modifiedLabel->setText("●");
    _modifiedLabel->setVisible(false);
    _modifiedLabel->setToolTip("Document modified");
    
    _zoomLabel = createClickableLabel("100%");
    _zoomLabel->setToolTip("Click to zoom in/out");
    
    _modeLabel = new QLabel(this);
    _modeLabel->setText("INS");
    _modeLabel->setToolTip("Insert/Overwrite mode");
    
    _eolTypeLabel = new QLabel(this);
    _eolTypeLabel->setText("Unix (LF)");
    
    _notificationLabel = new QLabel(this);
    _notificationLabel->setVisible(false);
    _notificationLabel->setStyleSheet("QLabel { color: palette(highlight); font-weight: bold; padding: 2px 8px; }");
    
    // Progress bar
    _progressBar = new QProgressBar(this);
    _progressBar->setMinimum(0);
    _progressBar->setMaximum(100);
    _progressBar->setValue(0);
    _progressBar->setTextVisible(false);
    _progressBar->setFixedHeight(12);
    _progressBar->setVisible(false);
    
    _progressCancelBtn = new QToolButton(this);
    _progressCancelBtn->setText("×");
    _progressCancelBtn->setFixedWidth(16);
    _progressCancelBtn->setVisible(false);
    connect(_progressCancelBtn, &QToolButton::clicked, this, &StatusBar::onProgressCancelClicked);
    
    // Add widgets to status bar
    // Permanent widgets (right side)
    addPermanentWidget(_modifiedLabel, 0);
    addPermanentWidget(_readOnlyLabel, 0);
    addPermanentWidget(_bomLabel, 0);
    addPermanentWidget(_modeLabel, 0);
    addPermanentWidget(_zoomLabel, 0);
    addPermanentWidget(_tabIndentLabel, 0);
    addPermanentWidget(_docInfoLabel, 0);
    addPermanentWidget(_eolTypeLabel, 0);
    addPermanentWidget(_eolLabel, 0);
    addPermanentWidget(_encLabel, 0);
    addPermanentWidget(_langLabel, 0);
    addPermanentWidget(_typeLabel, 0);
    addPermanentWidget(_selLabel, 0);
    addPermanentWidget(_posLabel, 0);
    
    // Main widget (left side) - notification/progress
    addWidget(_notificationLabel, 2);
    addWidget(_msgLabel, 1);
}

QLabel* StatusBar::createClickableLabel(const QString& text) {
    auto* label = new QLabel(text, this);
    label->setCursor(Qt::PointingHandCursor);
    label->setStyleSheet("QLabel { padding: 0px 4px; }");
    return label;
}

void StatusBar::setupConnections() {
    // Position label click
    connect(_posLabel, &QLabel::linkActivated, this, &StatusBar::positionClicked);
    
    // Encoding label click
    connect(_encLabel, &QLabel::linkActivated, this, &StatusBar::encodingClicked);
    
    // EOL label click
    connect(_eolLabel, &QLabel::linkActivated, this, &StatusBar::eolClicked);
    
    // Language label click
    connect(_langLabel, &QLabel::linkActivated, this, &StatusBar::languageClicked);
    
    // Zoom label click
    connect(_zoomLabel, &QLabel::linkActivated, this, [this]() {
        emit zoomClicked(0);  // Reset zoom
    });
    
    // Notification timer
    _notificationTimer = new QTimer(this);
    _notificationTimer->setSingleShot(true);
    connect(_notificationTimer, &QTimer::timeout, this, &StatusBar::updateNotificationTimer);
}

void StatusBar::applyDpiScaling() {
    DpiManager& dpi = DpiManager::instance();
    
    // Scale font size if needed
    if (dpi.scaleFactor() > 1.0) {
        QFont font = _posLabel->font();
        font.setPointSizeF(font.pointSizeF() * dpi.scaleFactor());
        for (QLabel* label : findChildren<QLabel*>()) {
            label->setFont(font);
        }
    }
}

void StatusBar::applyTheme() {
    if (m_darkTheme) {
        setStyleSheet(R"(
            QStatusBar {
                background: palette(window);
                border-top: 1px solid palette(dark);
            }
            QLabel {
                color: palette(window-text);
            }
            QLabel[cssClass="clickable"] {
                color: palette(highlighted-text);
            }
        )");
    } else {
        setStyleSheet(QString());
    }
}

void StatusBar::setDarkTheme(bool dark) {
    m_darkTheme = dark;
    applyTheme();
}

void StatusBar::setPosition(int line, int col) {
    m_currentLine = line + 1;
    m_currentCol = col + 1;
    _posLabel->setText(QString("Ln %1, Col %2").arg(m_currentLine).arg(m_currentCol));
}

void StatusBar::setCursorPosition(int line, int col, int absoluteOffset) {
    m_currentLine = line + 1;
    m_currentCol = col + 1;
    m_absoluteOffset = absoluteOffset;
    _posLabel->setText(QString("Ln %1, Col %2").arg(m_currentLine).arg(m_currentCol));
    _posLabel->setToolTip(QString("Offset: %1").arg(m_absoluteOffset));
}

void StatusBar::setEncoding(const QString& enc) {
    _encLabel->setText(enc);
}

void StatusBar::setEol(const QString& eol) {
    _eolLabel->setText(eol);
}

void StatusBar::setLanguage(const QString& lang) {
    _langLabel->setText(lang);
}

void StatusBar::setFileType(const QString& type) {
    _typeLabel->setText(type);
}

void StatusBar::setMessage(const QString& msg) {
    _msgLabel->setText(msg);
    _msgLabel->setVisible(!msg.isEmpty());
}

void StatusBar::setSelection(int chars, int lines) {
    m_selectionChars = chars;
    m_selectionLines = lines;
    
    if (chars == 0 && lines == 0) {
        _selLabel->setText("");
    } else if (chars == 1 && lines == 1) {
        _selLabel->setText("1 sel");
    } else {
        _selLabel->setText(QString("%1 chars, %2 lines sel").arg(chars).arg(lines));
    }
}

void StatusBar::clearMessage() {
    _msgLabel->clear();
    _msgLabel->setVisible(false);
}

void StatusBar::setDocumentInfo(int currentLine, int totalLines, int percentage) {
    m_currentLine = currentLine;
    m_totalLines = totalLines;
    m_percentage = percentage;
    _docInfoLabel->setText(QString("Ln %1 of %2 (%3%)").arg(currentLine).arg(totalLines).arg(percentage));
}

void StatusBar::setTabSize(int size) {
    m_tabSize = size;
    _tabIndentLabel->setText(QString("Tab Size: %1").arg(size));
}

void StatusBar::setIndentSize(int spaces) {
    m_indentSize = spaces;
    _tabIndentLabel->setToolTip(QString("Indent: %1 spaces").arg(spaces));
}

void StatusBar::setBOM(bool hasBom) {
    m_hasBom = hasBom;
    _bomLabel->setVisible(hasBom);
}

void StatusBar::setReadOnly(bool readOnly) {
    m_readOnly = readOnly;
    _readOnlyLabel->setVisible(readOnly);
    if (readOnly) {
        _readOnlyLabel->setStyleSheet("QLabel { color: palette(highlight); font-weight: bold; }");
    } else {
        _readOnlyLabel->setStyleSheet(QString());
    }
}

void StatusBar::setModified(bool modified) {
    m_modified = modified;
    _modifiedLabel->setVisible(modified);
    if (modified) {
        _modifiedLabel->setStyleSheet("QLabel { color: #ffa500; font-weight: bold; }");
    } else {
        _modifiedLabel->setStyleSheet(QString());
    }
}

void StatusBar::setZoomLevel(int level) {
    m_zoomLevel = level;
    _zoomLabel->setText(formatZoomLevel(level));
    _zoomLabel->setToolTip("Click to reset zoom");
}

void StatusBar::setInsertMode(bool insert) {
    m_insertMode = insert;
    _modeLabel->setText(insert ? "INS" : "OVR");
    _modeLabel->setToolTip(insert ? "Insert mode" : "Overwrite mode");
}

void StatusBar::setEolMode(const QString& mode) {
    m_eolMode = mode;
    _eolTypeLabel->setText(formatEolMode(mode));
}

void StatusBar::setLineEnding(const QString& ending) {
    m_lineEnding = ending;
    _eolLabel->setText(ending);
}

QString StatusBar::formatEolMode(const QString& mode) const {
    if (mode.contains("CRLF", Qt::CaseInsensitive) || mode == "windows" || mode == "dos") {
        return "CRLF (Windows)";
    } else if (mode.contains("LF", Qt::CaseInsensitive) && !mode.contains("CR")) {
        return "LF (Unix)";
    } else if (mode.contains("CR", Qt::CaseInsensitive)) {
        return "CR (Mac)";
    }
    return mode;
}

QString StatusBar::formatZoomLevel(int level) const {
    if (level == 0) {
        return "100%";
    }
    return QString("%1%").arg(100 + (level * 10));
}

void StatusBar::showProgress(int value, int maximum, const QString& text) {
    m_showingProgress = true;
    _progressBar->setMaximum(maximum);
    _progressBar->setValue(value);
    _progressBar->setVisible(true);
    _progressCancelBtn->setVisible(true);
    
    if (!text.isEmpty()) {
        _notificationLabel->setText(text);
        _notificationLabel->setVisible(true);
    }
}

void StatusBar::clearProgress() {
    m_showingProgress = false;
    _progressBar->setVisible(false);
    _progressBar->setValue(0);
    _progressCancelBtn->setVisible(false);
}

void StatusBar::onProgressCancelClicked() {
    emit progressCancelled();
    clearProgress();
}

void StatusBar::showNotification(const QString& msg, int durationMs) {
    _currentNotification = msg;
    _notificationDuration = durationMs;
    _notificationLabel->setText(msg);
    _notificationLabel->setVisible(true);
    
    _notificationTimer->stop();
    _notificationTimer->start(durationMs);
}

void StatusBar::updateNotificationTimer() {
    clearNotification();
}

void StatusBar::clearNotification() {
    _notificationLabel->clear();
    _notificationLabel->setVisible(false);
}

void StatusBar::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
    
    // Add context menu actions
    QAction* copyAction = menu.addAction("Copy Status Info");
    QAction* resetAction = menu.addAction("Reset to Defaults");
    
    menu.addSeparator();
    
    // Encoding submenu
    QMenu* encMenu = menu.addMenu("Set Encoding");
    QStringList encodings = {"UTF-8", "UTF-8 BOM", "UTF-16 LE", "UTF-16 BE", "ANSI"};
    for (const QString& enc : encodings) {
        encMenu->addAction(enc);
    }
    
    // EOL submenu
    QMenu* eolMenu = menu.addMenu("Set Line Ending");
    eolMenu->addAction("Windows (CRLF)");
    eolMenu->addAction("Unix (LF)");
    eolMenu->addAction("Mac (CR)");
    
    menu.addSeparator();
    
    // Zoom submenu
    QMenu* zoomMenu = menu.addMenu("Zoom");
    zoomMenu->addAction("Zoom In")->setData("zoom.in");
    zoomMenu->addAction("Zoom Out")->setData("zoom.out");
    zoomMenu->addAction("Reset Zoom")->setData("zoom.reset");
    
    menu.addSeparator();
    
    QAction* prefsAction = menu.addAction("Preferences...");
    
    menu.exec(event->globalPos());
}

void StatusBar::onAutoRefreshTimeout() {
    // Auto-refresh any dynamic content
    update();
}
