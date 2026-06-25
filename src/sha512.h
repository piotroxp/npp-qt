// npp-qt: SHA-512 checksum utility
#pragma once
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

class SHA512 {
public:
    SHA512() = default;
    ~SHA512() = default;

    void update(const QString& data) { update(data.toUtf8()); }
    void update(const QByteArray& data) { _data += data; }

    QByteArray final_() {
        return QCryptographicHash::hash(_data, QCryptographicHash::Sha512);
    }

    QString finalHex() {
        return QString::fromLatin1(final_().toHex()).toUpper();
    }

    static QString hash(const QString& data) {
        return QString::fromLatin1(
            QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha512).toHex()
        ).toUpper();
    }

    static QString hash(const QByteArray& data) {
        return QString::fromLatin1(
            QCryptographicHash::hash(data, QCryptographicHash::Sha512).toHex()
        ).toUpper();
    }

private:
    QByteArray _data;
};
