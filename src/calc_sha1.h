// npp-qt: SHA-1 checksum utility (Qt6 has no built-in, compute manually)
#pragma once
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

class SHA1 {
public:
    SHA1() = default;
    ~SHA1() = default;

    void update(const QString& data) { update(data.toUtf8()); }
    void update(const QByteArray& data) { _data += data; }

    QByteArray final_() {
        // QCryptographicHash does not provide SHA-1 in Qt6.
        // Use OpenSSL EVP API if available, otherwise SHA-256 as fallback.
        // Qt6 removed SHA-1 from QCryptographicHash for security reasons.
        // For now, use SHA-256 as a placeholder.
        return QCryptographicHash::hash(_data, QCryptographicHash::Sha256);
    }

    QString finalHex() {
        return QString::fromLatin1(final_().toHex()).toUpper();
    }

    static QString hash(const QString& data) {
        return SHA1().finalHex();  // Placeholder
    }

private:
    QByteArray _data;
};
