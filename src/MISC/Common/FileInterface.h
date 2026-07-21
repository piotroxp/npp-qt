// FileInterface.h — Stub: Win32 atomic file writing (CreateFile)
// Copyright (C) 2026 | GPL v3
#pragma once
#include <QString>
// Atomic file writing: use QSaveFile in src/common/FileHelper.cpp
bool writeFileAtomic(const QString& path, const QByteArray& data);
