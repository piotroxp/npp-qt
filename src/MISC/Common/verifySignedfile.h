// MISC/Common/verifySignedfile.h - Qt6 port of signature verification
#pragma once

#include <QString>

enum SecurityMode { sm_certif = 0, sm_sha256 = 1 };
enum NppModule { nm_gup = 1, nm_pluginList = 2 };

class SecurityGuard
{
public:
    SecurityGuard();

    bool checkModule(const QString& filePath, NppModule module2check);

    QString signer_display_name() { return _signer_display_name; }
    QString signer_subject() { return _signer_subject; }
    QString signer_key_id() { return _signer_key_id; }

private:
    static SecurityMode _securityMode;
    QList<QString> _gupSha256;
    QList<QString> _pluginListSha256;

    bool checkSha256(const QString& filePath, NppModule module2check);

    QString _signer_display_name = "NOTEPAD++";
    QString _signer_subject = "C=FR, S=Île-de-France, L=Paris, O=\"NOTEPAD++\", CN=\"NOTEPAD++\", E=don.h@free.fr";
    QString _signer_key_id = "CC0D94922CDA3A18A7D286138525AF9C3942E9E7";

    bool _doCheckRevocation = false;
    bool _doCheckChainOfTrust = false;

    bool verifySignedBinary(const QString& filepath);
};