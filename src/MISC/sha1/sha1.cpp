// sha1.cpp — Qt6 SHA-1 wrapper using QCryptographicHash
// Copyright (C) 2026 | GPL v3

#include <QCryptographicHash>
#include <QFile>
#include "sha1.h"

QByteArray NppSHA1::compute(const QByteArray& data) {
    return QCryptographicHash::hash(data, QCryptographicHash::Sha1);
}

QByteArray NppSHA1::computeFile(const QString& filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return QCryptographicHash::hash(f.readAll(), QCryptographicHash::Sha1);
}

QString NppSHA1::toHex(const QByteArray& hash) {
    return QString::fromLatin1(hash.toHex());
}
