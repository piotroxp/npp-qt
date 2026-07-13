// npp-qt: MD5 checksum utility
#pragma once
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

// MD5 digest — wrapper around QCryptographicHash
class MD5 {
public:
    MD5() = default;
    ~MD5() = default;

    void update(const QString& data) { update(data.toUtf8()); }
    void update(const QByteArray& data) { _data += data; }

    QByteArray final_() {
        return QCryptographicHash::hash(_data, QCryptographicHash::Md5);
    }

    QString finalHex() {
        return QString::fromLatin1(final_().toHex()).toUpper();
    }

    static QString hash(const QString& data) {
        return QString::fromLatin1(
            QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Md5).toHex()
        ).toUpper();
    }

    static QString hash(const QByteArray& data) {
        return QString::fromLatin1(
            QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex()
        ).toUpper();
    }

private:
    QByteArray _data;
};
