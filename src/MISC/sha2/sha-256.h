// sha-256.h — Qt6 SHA-256 wrapper using QCryptographicHash
// Copyright (C) 2026 | GPL v3

#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace NppSHA256 {

QByteArray compute(const QByteArray& data);
QByteArray computeFile(const QString& filePath);
QString toHex(const QByteArray& hash);

} // namespace NppSHA256
