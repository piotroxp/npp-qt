// MISC/md5/md5Dlgs.h - Qt6 port of MD5 hash dialog
#pragma once

#include <QDialog>

// Hash type enum - moved outside classes for easier access
enum HashType { hash_md5 = 16, hash_sha1 = 20, hash_sha256 = 32, hash_sha512 = 64 };

class HashFromFilesDlg : public QDialog
{
    Q_OBJECT
public:
    HashFromFilesDlg(QWidget* parent = nullptr);
    ~HashFromFilesDlg() = default;

    void accept() override;
    void reject() override;

    void setHashType(int hashType2set);

private:
    int _ht = hash_md5;
};

class HashFromTextDlg : public QDialog
{
    Q_OBJECT
public:
    HashFromTextDlg(QWidget* parent = nullptr);
    ~HashFromTextDlg() = default;

    void accept() override;
    void reject() override;
    void generateHash();
    void generateHashPerLine();
    void setHashType(int hashType2set);

private:
    int _ht = hash_md5;
};

#define HASH_MAX_LENGTH hash_sha512
#define HASH_STR_MAX_LENGTH (hash_sha512 * 2 + 1)