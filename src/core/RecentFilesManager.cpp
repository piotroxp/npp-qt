// RecentFilesManager.cpp — Recent files tracking for Notepad--Qt
// Copyright (C) 2026 Agent Army | GPL v3

#include "RecentFilesManager.h"
#include <QSettings>
#include <QAction>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>

RecentFilesManager& RecentFilesManager::instance() {
    static RecentFilesManager inst;
    return inst;
}

RecentFilesManager::RecentFilesManager() {}

RecentFilesManager::~RecentFilesManager() {
    save();
}

QString RecentFilesManager::configPath() const {
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
           + "/recent.ini";
}

void RecentFilesManager::addFile(const QString& fullPath) {
    if (fullPath.isEmpty()) return;
    QString path = QFileInfo(fullPath).absoluteFilePath();
    m_files.removeAll(path);           // remove if already present (move to front)
    m_files.prepend(path);
    while (m_files.size() > m_maxCount)
        m_files.removeLast();
    save();
    Q_EMIT fileAdded(path);
    Q_EMIT menuNeedsRebuild();
}

void RecentFilesManager::removeFile(const QString& fullPath) {
    QString path = QFileInfo(fullPath).absoluteFilePath();
    if (m_files.removeAll(path) > 0) {
        save();
        Q_EMIT fileRemoved(path);
        Q_EMIT menuNeedsRebuild();
    }
}

void RecentFilesManager::clear() {
    m_files.clear();
    save();
    Q_EMIT cleared();
    Q_EMIT menuNeedsRebuild();
}

void RecentFilesManager::save() {
    QSettings s(configPath(), QSettings::IniFormat);
    s.beginGroup("RecentFiles");
    s.setValue("Count", m_files.size());
    for (int i = 0; i < m_files.size(); ++i)
        s.setValue(QStringLiteral("File%1").arg(i), m_files[i]);
    s.endGroup();
    s.sync();
}

void RecentFilesManager::load() {
    QSettings s(configPath(), QSettings::IniFormat);
    s.beginGroup("RecentFiles");
    int count = s.value("Count", 0).toInt();
    m_files.clear();
    for (int i = 0; i < count; ++i) {
        QString path = s.value(QStringLiteral("File%1").arg(i), "").toString();
        if (!path.isEmpty() && QFileInfo::exists(path))
            m_files.append(path);
    }
    s.endGroup();
    // Trim to max
    while (m_files.size() > m_maxCount)
        m_files.removeLast();
}

QMenu* RecentFilesManager::buildMenu(QWidget* parent) {
    auto* menu = new QMenu("Recent Files", parent);
    populateMenu(menu);
    return menu;
}

void RecentFilesManager::populateMenu(QMenu* menu) {
    menu->clear();

    // Add each recent file as an action
    for (const QString& path : m_files) {
        QString name = QFileInfo(path).fileName();
        QString dir  = QFileInfo(path).absolutePath();
        auto* action = menu->addAction(name);
        action->setToolTip(path);
        action->setData(path);
        // Lambda captures 'path' safely
        QObject::connect(action, &QAction::triggered, this, [this, path]() {
            Q_EMIT fileAdded(path); // re-open signal
        });
    }

    if (!m_files.isEmpty()) {
        menu->addSeparator();
    }

    // "Clear Recent Files" action
    auto* clearAction = menu->addAction("Clear Recent Files");
    QObject::connect(clearAction, &QAction::triggered, this, &RecentFilesManager::clear);
}
