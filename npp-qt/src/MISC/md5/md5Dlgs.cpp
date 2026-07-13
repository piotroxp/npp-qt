// md5Dlgs.cpp — Qt6 stub for Notepad++ Hash dialog classes
// Minimal implementation to satisfy linker

#include "MISC/md5/md5Dlgs.h"
#include "MISC/md5/md5.h"
#include "MISC/sha1/sha1.h"
#include "MISC/sha2/sha-256.h"
#include "MISC/sha512/sha512.h"
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>

// ─── HashFromFilesDlg ───────────────────────────────────────────────────────

void HashFromFilesDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
}

void HashFromFilesDlg::destroy()
{
}

void HashFromFilesDlg::setHashType(hashType hashType2set)
{
    _ht = hashType2set;
}

intptr_t HashFromFilesDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    Q_UNUSED(message);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);
    return 0;
}

// ─── HashFromTextDlg ───────────────────────────────────────────────────────

void HashFromTextDlg::doDialog(bool isRTL)
{
    Q_UNUSED(isRTL);
}

void HashFromTextDlg::destroy()
{
}

void HashFromTextDlg::setHashType(hashType hashType2set)
{
    _ht = hashType2set;
}

void HashFromTextDlg::generateHash()
{
    // Stub: Qt6 implementation via QCryptographicHash
}

void HashFromTextDlg::generateHashPerLine()
{
    // Stub: Qt6 implementation via QCryptographicHash
}

intptr_t HashFromTextDlg::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam)
{
    Q_UNUSED(message);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);
    return 0;
}
