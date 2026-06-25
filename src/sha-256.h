// npp-qt: SHA-256 checksum utility
#pragma once
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

class SHA256 {
public:
    SHA256() = default;
    ~SHA256() = default;

    void update(const QString& data) { update(data.toUtf8()); }
    void update(const QByteArray& data) { _data += data; }

    QByteArray final_() {
        return QCryptographicHash::hash(_data, QCryptographicHash::Sha256);
    }

    QString finalHex() {
        return QString::fromLatin1(final_().toHex()).toUpper();
    }

    static QString hash(const QString& data) {
        return QString::fromLatin1(
            QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha256).toHex()
        ).toUpper();
    }

    static QString hash(const QByteArray& data) {
        return QString::fromLatin1(
            QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex()
        ).toUpper();
    }

private:
    QByteArray _data;
};
