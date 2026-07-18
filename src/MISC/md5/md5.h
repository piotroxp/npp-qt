// md5.h — Qt6 MD5 wrapper using QCryptographicHash
// Copyright (C) 2026 | GPL v3

#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace NppMD5 {

/// Compute MD5 hash of raw bytes
QByteArray compute(const QByteArray& data);

/// Compute MD5 hash of a file
QByteArray computeFile(const QString& filePath);

/// Compute MD5 hash of a string (UTF-8)
QByteArray computeString(const QString& str);

/// Format hash as lowercase hex string (32 chars)
QString toHex(const QByteArray& hash);

} // namespace NppMD5
