// sha512.cpp — Qt6 SHA-512 wrapper using QCryptographicHash
// Copyright (C) 2026 | GPL v3

#include <QCryptographicHash>
#include <QFile>
#include "sha512.h"

QByteArray NppSHA512::compute(const QByteArray& data) {
    return QCryptographicHash::hash(data, QCryptographicHash::Sha512);
}

QByteArray NppSHA512::computeFile(const QString& filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return QCryptographicHash::hash(f.readAll(), QCryptographicHash::Sha512);
}

QString NppSHA512::toHex(const QByteArray& hash) {
    return QString::fromLatin1(hash.toHex());
}
