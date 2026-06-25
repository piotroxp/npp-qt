// npp-qt: Windows code signing verification (stub)
// Win32 used WinVerifyTrust API to verify Authenticode signatures.
// Qt6: This is a stub. Production would use platform APIs or a library.
#pragma once
#include <QString>
#include <QStringList>

// VerificationResult mirrors the Win32 TRUST_E_*/CERT_E_* constants
enum class VerificationResult {
    Trusted       = 0,   // Signature is valid and trusted
    NoSignature   = 1,   // File has no digital signature
    Invalid       = 2,   // Signature is invalid or corrupted
    Revoked       = 3,   // Certificate has been revoked
    Expired       = 4,   // Certificate has expired
    NotTrusted    = 5,   // Certificate not trusted by system
    Unknown       = 99,  // Unable to determine
};

class SignedFileVerifier {
public:
    SignedFileVerifier() = default;
    ~SignedFileVerifier() = default;

    // Verify the Authenticode signature of a file
    VerificationResult verify(const QString& filePath);

    // Get the signer certificate's subject name
    QString signerName() const { return _signerName; }

    // Get the certificate's issuer name
    QString issuerName() const { return _issuerName; }

    // Get the certificate's validity dates
    QString notBefore() const { return _notBefore; }
    QString notAfter() const { return _notAfter; }

private:
    QString _signerName;
    QString _issuerName;
    QString _notBefore;
    QString _notAfter;
};
