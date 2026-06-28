#pragma once

// md5Dlgs.h — Qt6 port of Notepad++ Hash dialog classes
// Ported from PowerEditor/src/MISC/md5/md5Dlgs.h

#pragma once

#include "StaticDialog.h"

// hashType enum (mirrors original N++)
enum hashType {hash_md5 = 16, hash_sha1 = 20, hash_sha256 = 32, hash_sha512 = 64};

#define HASH_MAX_LENGTH hash_sha512
#define HASH_STR_MAX_LENGTH (hash_sha512 * 2 + 1)

class HashFromFilesDlg : public StaticDialog
{
public:
    HashFromFilesDlg() = default;
    void init(void* /*hInst*/, QWidget* /*parent*/) { /* no-op stub */ }
    void init(QWidget* parent) { /* single-arg stub for Notepad_plus.cpp compat */ }
    void doDialog(bool isRTL = false);
    void destroy() override;
    void setHashType(hashType hashType2set);
    QWidget* getHSelf() { return this; }
    const QWidget* getHSelf() const { return this; }
    // isCreated() inherited from StaticDialog

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;
    hashType _ht = hash_md5;
};

class HashFromTextDlg : public StaticDialog
{
public:
    HashFromTextDlg() = default;
    void init(void* /*hInst*/, QWidget* /*parent*/) { /* no-op stub */ }
    void init(QWidget* parent) { /* single-arg stub for Notepad_plus.cpp compat */ }
    void doDialog(bool isRTL = false);
    void destroy() override;
    void generateHash();
    void generateHashPerLine();
    void setHashType(hashType hashType2set);
    QWidget* getHSelf() { return this; }
    const QWidget* getHSelf() const { return this; }
    // isCreated() inherited from StaticDialog

protected:
    intptr_t run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) override;
    hashType _ht = hash_md5;
};
