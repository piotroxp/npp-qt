// sha1.h — Qt6 SHA-1 wrapper using QCryptographicHash
// Copyright (C) 2026 | GPL v3

#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace NppSHA1 {

/// Compute SHA-1 hash of raw bytes
QByteArray compute(const QByteArray& data);

/// Compute SHA-1 hash of a file
QByteArray computeFile(const QString& filePath);

/// Format hash as lowercase hex string (40 chars)
QString toHex(const QByteArray& hash);

} // namespace NppSHA1
