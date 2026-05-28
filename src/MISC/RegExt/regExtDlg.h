// regExtDlg.h - Qt port
#pragma once
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QString>

// Qt-compatible shims (no Windows headers needed)
#ifdef _WIN32
#include "MISC/Common/WindowsCompat.h"
#else
using HINSTANCE = void*;
using HWND = void*;
#endif

class RegExtDlg : public QDialog {
    Q_OBJECT
public:
    RegExtDlg(QWidget* parent = nullptr);  // Fixed: had wrong signature
    void init(HINSTANCE hInst, HWND parent);
    void doDialog(bool toShow = true);
    bool checkTheFileAndAssignExt(const wchar_t* fileExt);
    void writeSettings();

private slots:
    void showDialog();
    void getRegisteredExts();
    void getDefSupportedExts();
    void addExt(const QString& ext);
    bool deleteExts(const QString& ext);
    void writeNppPath();

private:
    HWND _hParent = nullptr;
    QListWidget* _extList = nullptr;
};