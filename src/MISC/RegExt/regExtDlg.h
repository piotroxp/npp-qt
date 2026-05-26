// regExtDlg.h - Qt port
#pragma once
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
class regExtDlg : public QDialog {
    Q_OBJECT
public:
    regExtDlg() : QDialog() {}
    void init(HINSTANCE hInst, HWND parent) { Q_UNUSED(hInst); _hParent = parent; }
    void doDialog(bool toShow = true);
    bool checkTheFileAndAssignExt(const wchar_t* fileExt);
    void writeSettings();
private:
    HWND _hParent = nullptr;
    QListWidget* _extList = nullptr;
};
HeaderEOF

cat > /workspace/piotro/npp-qt/src/MISC/md5/md5Dlgs.h << 'HEADEREOF'
// md5Dlgs.h - Qt port (cross-platform MD5)
#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCryptographicHash>
class md5Dlgs : public QDialog {
    Q_OBJECT
public:
    md5Dlgs() : QDialog() {}
    void init(HINSTANCE hInst, HWND parent) { Q_UNUSED(hInst); _hParent = parent; }
    void doDialog();
    bool compute_md5_ansi(const wchar_t* text);
private:
    HWND _hParent = nullptr;
    QString _text2Calculate;
    QString _calculatedMD5;
};
HeaderEOF
echo "md5Dlgs created"