// MISC/cryptoWin32/cryptoSecurity.cpp - Qt6 port of crypto utilities (file hashing)
#include "cryptoSecurity.h"
#include <QCryptographicHash>
#include <QFile>

CryptoSecurity& CryptoSecurity::getInstance()
{
    static CryptoSecurity instance;
    return instance;
}

QByteArray CryptoSecurity::calculateFileHash(const QString& filePath, HashAlgorithm algo)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return QByteArray();
    
    QCryptographicHash::Algorithm qalgo;
    switch (algo) {
        case HashAlgorithm::MD5: qalgo = QCryptographicHash::Md5; break;
        case HashAlgorithm::SHA1: qalgo = QCryptographicHash::Sha1; break;
        case HashAlgorithm::SHA256: qalgo = QCryptographicHash::Sha256; break;
        case HashAlgorithm::SHA512: qalgo = QCryptographicHash::Sha512; break;
        default: qalgo = QCryptographicHash::Md5; break;
    }
    
    QCryptographicHash hash(qalgo);
    const size_t bufferSize = 64 * 1024; // 64KB chunks
    
    while (!file.atEnd()) {
        QByteArray buffer = file.read(bufferSize);
        hash.addData(buffer);
    }
    
    file.close();
    return hash.result();
}

QByteArray CryptoSecurity::calculateStringHash(const QString& str, HashAlgorithm algo)
{
    QCryptographicHash::Algorithm qalgo;
    switch (algo) {
        case HashAlgorithm::MD5: qalgo = QCryptographicHash::Md5; break;
        case HashAlgorithm::SHA1: qalgo = QCryptographicHash::Sha1; break;
        case HashAlgorithm::SHA256: qalgo = QCryptographicHash::Sha256; break;
        case HashAlgorithm::SHA512: qalgo = QCryptographicHash::Sha512; break;
        default: qalgo = QCryptographicHash::Md5; break;
    }
    
    QCryptographicHash hash(qalgo);
    hash.addData(str.toUtf8());
    return hash.result();
}

bool CryptoSecurity::verifyFileHash(const QString& filePath, const QByteArray& expectedHash, HashAlgorithm algo)
{
    QByteArray actualHash = calculateFileHash(filePath, algo);
    return actualHash == expectedHash;
}

QString CryptoSecurity::hashToHexString(const QByteArray& hash)
{
    return QString::fromLatin1(hash.toHex());
}