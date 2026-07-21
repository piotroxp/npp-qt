// FileNameStringSplitter.h — Win32 path string splitter
// Copyright (C) 2026 | GPL v3
#pragma once
#include <QString>
#include <QStringList>
// Utility for splitting paths — use QFileInfo/QDir instead
QStringList splitPath(const QString& path);
