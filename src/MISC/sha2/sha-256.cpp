// sha-256.cpp — Qt6 SHA-256 wrapper using QCryptographicHash
// Copyright (C) 2026 | GPL v3

#include <QCryptographicHash>
#include <QFile>
#include "sha-256.h"

QByteArray NppSHA256::compute(const QByteArray& data) {
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256);
}

QByteArray NppSHA256::computeFile(const QString& filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return QCryptographicHash::hash(f.readAll(), QCryptographicHash::Sha256);
}

QString NppSHA256::toHex(const QByteArray& hash) {
    return QString::fromLatin1(hash.toHex());
}
