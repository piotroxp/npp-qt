// md5.cpp — Qt6 MD5 wrapper using QCryptographicHash
// Copyright (C) 2026 | GPL v3

#include <QCryptographicHash>
#include <QFile>
#include <QByteArray>
#include "md5.h"

QByteArray NppMD5::compute(const QByteArray& data) {
    return QCryptographicHash::hash(data, QCryptographicHash::Md5);
}

QByteArray NppMD5::computeFile(const QString& filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
}

QByteArray NppMD5::computeString(const QString& str) {
    return compute(str.toUtf8());
}

QString NppMD5::toHex(const QByteArray& hash) {
    return QString::fromLatin1(hash.toHex());
}
