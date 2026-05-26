// MISC/cryptoWin32/cryptoSecurity.h - Qt6 port of crypto utilities (file hashing)
#pragma once

#include <QString>
#include <QByteArray>

enum class HashAlgorithm {
    MD5,
    SHA1,
    SHA256,
    SHA512
};

class CryptoSecurity
{
public:
    static CryptoSecurity& getInstance();
    
    // Calculate file hash
    QByteArray calculateFileHash(const QString& filePath, HashAlgorithm algo);
    
    // Calculate string hash
    QByteArray calculateStringHash(const QString& str, HashAlgorithm algo);
    
    // Verify file against known hash
    bool verifyFileHash(const QString& filePath, const QByteArray& expectedHash, HashAlgorithm algo);
    
    // Get hash as hex string
    static QString hashToHexString(const QByteArray& hash);
    
private:
    CryptoSecurity() = default;
    ~CryptoSecurity() = default;
    
    CryptoSecurity(const CryptoSecurity&) = delete;
    CryptoSecurity& operator=(const CryptoSecurity&) = delete;
};

// Convenience functions for common hash types
namespace CryptoHelper {
    inline QByteArray md5File(const QString& path) {
        return CryptoSecurity::getInstance().calculateFileHash(path, HashAlgorithm::MD5);
    }
    
    inline QByteArray sha1File(const QString& path) {
        return CryptoSecurity::getInstance().calculateFileHash(path, HashAlgorithm::SHA1);
    }
    
    inline QByteArray sha256File(const QString& path) {
        return CryptoSecurity::getInstance().calculateFileHash(path, HashAlgorithm::SHA256);
    }
    
    inline QString md5String(const QString& str) {
        return CryptoSecurity::hashToHexString(
            CryptoSecurity::getInstance().calculateStringHash(str, HashAlgorithm::MD5));
    }
    
    inline QString sha256String(const QString& str) {
        return CryptoSecurity::hashToHexString(
            CryptoSecurity::getInstance().calculateStringHash(str, HashAlgorithm::SHA256));
    }
}