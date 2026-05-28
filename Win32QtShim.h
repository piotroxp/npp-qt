#pragma once
#ifndef _WIN32QT_SHIM_H_
#define _WIN32QT_SHIM_H_

/**
 * Win32QtShim.h — Win32 API → Qt compatibility shim
 */
#include <QtWidgets>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QClipboard>
#include <QGuiApplication>
#include <QSystemTrayIcon>
#include <QMimeData>
#include <QDrag>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QStyleFactory>
#include <QCursor>
#include <QApplication>
#include <QStandardPaths>
#include <QThread>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QWidget>
#include <QScreen>
#include <QWindow>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QSplitter>
#include <QScrollBar>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QSlider>
#include <QProgressBar>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QPainter>
#include <QPalette>
#include <QFont>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QRegion>
#include <QPixmap>
#include <QIcon>
#include <QKeySequence>
#include <QEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QPaintEvent>
#include <QCloseEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QFocusEvent>
#include <QInputEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QContextMenuEvent>
#include <QHelpEvent>
#include <QStatusTipEvent>
#include <QWhatsThisClickedEvent>
#include <QEnterEvent>
#include <QMimeData>
#include <QDrag>
#include <QFileInfo>
#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollArea>
#include <QStackedWidget>
#include <QDockWidget>
#include <QMainWindow>
#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QBoxLayout>
#include <QSpacerItem>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPageSetupDialog>
#include <QPainter>
#include <QImageReader>
#include <QImageWriter>
#include <QMovie>
#include <QTextCodec>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QSysInfo>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QSystemTrayIcon>
#include <QStyleFactory>
#include <QStyle>
#include <QProxyStyle>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QColumnView>
#include <QUndoStack>
#include <QUndoCommand>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextLayout>
#include <QTextList>
#include <QTextTable>
#include <QPlainTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QScrollBar>
#include <QDial>
#include <QProgressBar>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QTabWidget>
#include <QStackedWidget>
#include <QSplitter>
#include <QFrame>
#include <QGroupBox>
#include <QTabBar>
#include <QRubberBand>
#include <QDialog>
#include <QWizard>
#include <QWizardPage>
#include <QMessageBox>
#include <QInputDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPageSetupDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QMovie>
#include <QTextCodec>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QSysInfo>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QSystemTrayIcon>
#include <QStyleFactory>
#include <QStyle>
#include <QProxyStyle>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QColumnView>
#include <QUndoStack>
#include <QUndoCommand>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextLayout>
#include <QTextList>
#include <QTextTable>
#include <map>
#include <algorithm>
#include <functional>

// ─── Window / Cursor Functions ──────────────────────────────────────────────────
inline QWidget* getForegroundWindow() { return QApplication::activeWindow(); }
#define GetActiveWindow() QApplication::activeWindow()
#define GetForegroundWindow() QApplication::activeWindow()
inline void BringWindowToTop(QWidget*) {}
inline void SetFocus(QWidget*) {}
inline void SetActiveWindow(QWidget*) {}

inline QCursor* LoadCursor(HINSTANCE, int id) {
    switch (id) {
        case 1:
        case 32512: return new QCursor(Qt::ArrowCursor);       // IDC_ARROW
        case 2:
        case 32516: return new QCursor(Qt::UpArrowCursor);     // IDC_UPARROW
        case 32513: return new QCursor(Qt::CrossCursor);       // IDC_CROSS
        case 32514: return new QCursor(Qt::WaitCursor);        // IDC_WAIT
        case 32515: return new QCursor(Qt::IBeamCursor);       // IDC_IBEAM
        case 32642: return new QCursor(Qt::SizeBDiagCursor);  // IDC_SIZENWSE
        case 32643: return new QCursor(Qt::SizeFDiagCursor);  // IDC_SIZENESW
        case 32644: return new QCursor(Qt::SizeVerCursor);    // IDC_SIZENS
        case 32645: return new QCursor(Qt::SizeHorCursor);    // IDC_SIZEWE
        case 32646: return new QCursor(Qt::SizeAllCursor);     // IDC_SIZEALL
        case 32648: return new QCursor(Qt::PointingHandCursor); // IDC_HAND
        case 32649: return new QCursor(Qt::ForbiddenCursor);   // IDC_NO
        case 32650: return new QCursor(Qt::WhatsThisCursor);  // IDC_HELP
        default: return new QCursor(Qt::ArrowCursor);
    }
}

// Wrap GetTickCount and Sleep to avoid redefinition if already in WindowsCompat.h
// Strategy: If WindowsCompat.h has already set its guard, skip our definitions and let it provide them
// Otherwise define our Qt-based versions
#ifndef _WINDOWS_COMPAT_H_DEFINED

inline DWORD GetTickCount() {
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}
inline void Sleep(DWORD ms) { QThread::msleep(ms); }

#endif  // _WINDOWS_COMPAT_H_DEFINED

inline DWORD GetCurrentThreadId() { return (DWORD)(quintptr)QThread::currentThreadId(); }
inline DWORD GetCurrentProcessId() { return (DWORD)QCoreApplication::applicationPid(); }

#define OutputDebugString(s) qDebug() << s
#define GetLastError() errno

// ─── Memory ───────────────────────────────────────────────────────────────────
#define GlobalAlloc(type, size) malloc(size)
#define GlobalLock(mem) mem
#define GlobalUnlock(mem) (void)0
#define GlobalFree(mem) free(mem)
#define LocalAlloc(type, size) malloc(size)
#define LocalFree(mem) free(mem)

// ─── String / Encoding ────────────────────────────────────────────────────────
#define _T(x) u"" x
#define TEXT(x) u"" x
#define _TEXT(x) u"" x

// ─── Registry ────────────────────────────────────────────────────────────────
inline LONG RegSetValueEx(HKEY, LPCWSTR, DWORD, DWORD, const BYTE*, DWORD) { return 0; }
inline LONG RegQueryValueEx(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE*, LPDWORD) { return 0; }
#define HKEY_CURRENT_USER "HKEY_CURRENT_USER"
#define HKEY_LOCAL_MACHINE "HKEY_LOCAL_MACHINE"
#define HKEY_CLASSES_ROOT "HKEY_CLASSES_ROOT"
#define KEY_WRITE 0x20006
#define KEY_READ 0x20019
#define REG_SZ 1
#define REG_DWORD 4

// ─── Clipboard ────────────────────────────────────────────────────────────────
#define CF_TEXT 1
#define CF_UNICODETEXT 13
#define CF_BITMAP 2

// ─── File Dialogs ─────────────────────────────────────────────────────────────
inline QString getOpenFileName(QWidget* parent, const QString& title,
                                const QString& dir, const QString& filter,
                                QString* selectedFilter = nullptr) {
    return QFileDialog::getOpenFileName(parent, title, dir, filter, selectedFilter);
}
inline QStringList getOpenFileNames(QWidget* parent, const QString& title,
                                    const QString& dir, const QString& filter,
                                    QString* selectedFilter = nullptr) {
    return QFileDialog::getOpenFileNames(parent, title, dir, filter, selectedFilter);
}
inline QString getSaveFileName(QWidget* parent, const QString& title,
                                const QString& dir, const QString& filter,
                                QString* selectedFilter = nullptr) {
    return QFileDialog::getSaveFileName(parent, title, dir, filter, selectedFilter);
}
inline QString getExistingDirectory(QWidget* parent, const QString& title,
                                    const QString& dir) {
    return QFileDialog::getExistingDirectory(parent, title, dir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
}

// ─── Message Boxes ─────────────────────────────────────────────────────────────
#define MB_OK              0x00000000L
#define MB_YESNO           0x00000004L
#define MB_YESNOCANCEL     0x00000003L
#define MB_OKCANCEL        0x00000001L
#define MB_RETRYCANCEL     0x00000005L
#define MB_ICONEXCLAMATION 0x00000030L
#define MB_ICONERROR       0x00000010L
#define MB_ICONINFORMATION 0x00000040L
#define MB_ICONQUESTION    0x00000020L
#define IDOK 1
#define IDCANCEL 2
#define IDYES 6
#define IDNO 7

// ─── Common Dialogs ──────────────────────────────────────────────────────────
inline bool getColor(QWidget* parent, QColor& result, const QColor& initial = QColor()) {
    QColor c = QColorDialog::getColor(initial, parent);
    if (c.isValid()) { result = c; return true; }
    return false;
}
inline QFont getFont(QWidget* parent, const QFont& initial = QFont(), bool* ok = nullptr) {
    return QFontDialog::getFont(ok, initial, parent);
}
inline QString getInput(QWidget* parent, const QString& title, const QString& label,
                         const QString& text = QString()) {
    bool ok = false;
    QString result = QInputDialog::getText(parent, title, label, QLineEdit::Normal, text, &ok);
    return ok ? result : QString();
}

// ─── Shell / File ─────────────────────────────────────────────────────────────
inline bool CreateDirectory(const QString& path, void*) { return QDir().mkpath(path); }
inline bool RemoveDirectory(const QString& path) { return QDir(path).rmdir(path); }
inline DWORD GetFileAttributes(const QString& path) {
    QFileInfo fi(path);
    if (!fi.exists()) return 0xFFFFFFFF;
    DWORD attrs = FILE_ATTRIBUTE_NORMAL;
    if (fi.isDir()) attrs |= FILE_ATTRIBUTE_DIRECTORY;
    if (fi.isReadable()) attrs |= FILE_ATTRIBUTE_READONLY;
    return attrs;
}
#define FILE_ATTRIBUTE_NORMAL 0x80
#define FILE_ATTRIBUTE_DIRECTORY 0x10
#define FILE_ATTRIBUTE_READONLY 0x1
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF

// ─── Process ─────────────────────────────────────────────────────────────────
inline DWORD ShellExecute(QWidget*, const QString& verb, const QString& file,
                           const QString& params, const QString& dir, int) {
    Q_UNUSED(verb);
    QStringList args = params.isEmpty() ? QStringList() : QStringList{params};
    int r = QProcess::execute(file, args);
    return r == 0 ? 42 : 0;
}
#define SE_ERR_OK 42
#define SW_SHOWNORMAL 1
#define SW_SHOWMAXIMIZED 3
#define SW_HIDE 0
#define SW_MAXIMIZE 3
#define SW_MINIMIZE 6
#define SW_RESTORE 9

// ─── Scrolling ─────────────────────────────────────────────────────────────────
#define SB_HORZ 0
#define SB_VERT 1
#define SB_CTL  2
#define SB_BOTH 3
#define SB_LINEUP 0
#define SB_LINEDOWN 1


// ─── Misc macros ───────────────────────────────────────────────────────────────
#define IsWindowVisible(w) (w && w->isVisible())
#define IsWindowEnabled(w) (w && w->isEnabled())
#define EnableWindow(w, e) do { if(w) w->setEnabled(e); } while(false)
#define ShowWindow(w, cmd) do { if(w) { switch(cmd) { case SW_HIDE: w->hide(); break; case SW_SHOWNORMAL: case SW_SHOW: w->show(); break; case SW_SHOWMAXIMIZED: w->showMaximized(); break; case SW_MINIMIZE: case SW_SHOWMINIMIZED: w->showMinimized(); break; case SW_RESTORE: w->showNormal(); break; case SW_MAXIMIZE: w->showMaximized(); break; default: w->show(); } } } while(false)
#define UpdateWindow(w) do { if(w) w->update(); } while(false)
#define InvalidateRect(w, r, e) do { if(w) w->update(); Q_UNUSED(r); Q_UNUSED(e); } while(false)
#define SetWindowText(w, t) do { if(w) w->setWindowTitle(t); } while(false)
#define GetWindowText(w) (w ? w->windowTitle() : QString())
#define MoveWindow(w, x, y, cx, cy, r) do { if(w) { w->move(x, y); if(r) w->resize(cx, cy); } } while(false)
#define DestroyWindow(w) do { if(w) { w->deleteLater(); } } while(false)
#define GetParent(w) (w ? w->parentWidget() : nullptr)
#define IsIconic(w) (w && w->windowState() & Qt::WindowMinimized)
#define IsZoomed(w) (w && w->windowState() & Qt::WindowMaximized)
#define EndDialog(d, r) do { if(d) d->done(r); } while(false)
#define GetDlgItem(p, id) (p ? p->findChild<QWidget*>(QString("dlg_%1").arg(id)) : nullptr)
#define SetDlgItemText(p, id, t) do { auto* w = GetDlgItem(p, id); if(w) w->setWindowTitle(t); } while(false)
#define CheckDlgButton(p, id, c) do { auto* b = qobject_cast<QAbstractButton*>(GetDlgItem(p, id)); if(b) b->setChecked(c != 0); } while(false)
#define LoadLibrary(x) nullptr
inline QString GetDlgItemText(QWidget* p, int id) { auto* w = p ? p->findChild<QWidget*>(QString("dlg_%1").arg(id)) : nullptr; return w ? w->windowTitle() : QString(); }
inline int IsDlgButtonChecked(QWidget*, int) { return 0; }
#define FreeLibrary(m) (void)0
#define GetProcAddress(m, n) nullptr
#define GetModuleHandle(m) nullptr
#define GetCommandLine() QCoreApplication::arguments()
#define MAKEINTRESOURCE(id) QString::number(id)

// ─── Path / Shell ─────────────────────────────────────────────────────────────
inline QString GetWindowsDir() { return qgetenv("WINDIR").constData(); }
inline QString GetSystemDir() { return GetWindowsDir() + "/System32"; }
inline QString GetTempPath() { return QDir::tempPath(); }
inline QString GetKnownFolderPath(const QString&) {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}
#define CSIDL_APPDATA 0x001A
#endif // _WIN32QT_SHIM_H_
