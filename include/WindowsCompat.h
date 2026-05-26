#pragma once
/** WindowsCompat.h — Minimal Windows type shim for Qt6/Linux cross-compilation */
#include <QString>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QColor>
#include <QRgb>
#include <QWidget>
#include <QCursor>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QRegion>
#include <QMenu>
#include <QKeySequence>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QSettings>
#include <QClipboard>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QThread>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileSystemWatcher>
#include <QGuiApplication>
#include <QSystemTrayIcon>
#include <QMimeData>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMap>
#include <QList>
#include <QVector>
#include <QPair>
#include <QSet>
#include <QStack>
#include <QQueue>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QByteArray>
#include <QRegularExpression>
#include <QUuid>
#include <QLocale>
#include <QLibrary>
#include <QUrl>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QSysInfo>
#include <QFont>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QPalette>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QScreen>
#include <QWindow>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QStackedWidget>
#include <QSplitter>
#include <QScrollArea>
#include <QGroupBox>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QProgressBar>
#include <QSlider>
#include <QTabBar>
#include <QRubberBand>
#include <QWizard>
#include <QWizardPage>
#include <QScrollBar>
#include <QDockWidget>
#include <QMainWindow>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPageSetupDialog>
#include <QPrintPreviewWidget>
#include <QLibraryInfo>
#include <QTranslator>
#include <QStyleFactory>
#include <QProxyStyle>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QUndoStack>
#include <QUndoCommand>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextTable>
#include <QCompleter>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QCalendarWidget>
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QDrag>
#include <QSyntaxHighlighter>
#include <QInputDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileIconProvider>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QSemaphore>
#include <QWaitCondition>
#include <QRunnable>
#include <QThreadPool>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QCollator>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStorageInfo>
#include <QAbstractNativeEventFilter>
#include <QProxyStyle>
#include <QStyleHints>
#include <QStyleHints>
#include <QEnterEvent>
#include <QPainter>
#include <QPaintDevice>
#include <QRegion>
#include <QPolygon>
#include <QTransform>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <QGenericMatrix>
#include <QImageReader>
#include <QImageWriter>
#include <QMovie>
#include <QBitmap>
#include <QPainterPath>
#include <QRegion>
#include <QPolygonF>
#include <QPainterPathStroker>
#include <QRegion>
#include <QTextLayout>
#include <QTextLine>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QPlainTextDocumentLayout>
#include <QSyntaxHighlighter>
#include <QTextObjectInterface>
#include <QTextLength>
#include <QTextListFormat>
#include <QTextTableFormat>
#include <QTextFrameFormat>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextDocumentWriter>
#include <QPluginLoader>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>
#include <QTimer>
#include <QTimerEvent>
#include <QTranslator>
#include <QUrlQuery>
#include <QUrl>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QDomAttr>
#include <QDomText>
#include <QDomComment>
#include <QDomProcessingInstruction>
#include <QDomEntity>
#include <QDomEntityReference>
#include <QDomCDATASection>
#include <QDomDocumentFragment>
#include <QDomDocumentType>
#include <QDomImplementation>
#include <QtCore5Compat/QtCore5Compat>
#include <QTextCodec>
#include <QBuffer>
#include <QDataStream>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QRegularExpressionValidator>
#include <QObject>
#include <QChildEvent>
#include <QTimerEvent>
#include <QObjectCleanupHandler>
#include <QPluginLoader>
#include <QStorageInfo>
#include <QStorageInfo>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cwchar>
#include <cwctype>
#include <ctime>
#include <cwchar>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <deque>
#include <array>
#include <tuple>
#include <utility>
#include <type_traits>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <ratio>
#include <limits>
#include <locale>
#include <codecvt>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <exception>
#include <stdexcept>
#include <new>
#include <typeinfo>
#include <csignal>
#include <csetjmp>
#include <cstdalign>

// ─── Fundamental type aliases ────────────────────────────────────────────────
using int8_t   = qint8;
using int16_t  = qint16;
using int32_t  = qint32;
using uint8_t  = quint8;
using uint16_t = quint16;
using uint32_t = quint32;
using byte = quint8;

using BYTE    = quint8;
using WORD    = quint16;
using DWORD    = quint32;
using LONG    = qint32;
using LONGLONG = qint64;
using ULONG    = quint32;
using ULONG_PTR = quintptr;
using DWORD_PTR = quintptr;
using BOOL = int;
#define TRUE  1
#define FALSE 0
using LONG_PTR = qptrdiff;
using INT_PTR = qptrdiff;
using UINT    = quint32;
using INT     = int;
using WPARAM  = quint64;
using LPARAM  = quint64;
using LRESULT = qint64;
using ATOM    = quint16;
using HRESULT = LONG;
using SIZE_T  = quintptr;
using SSIZE_T = qptrdiff;
using HALF_PTR = void*;
using UHALF_PTR = void*;
using LONG32 = qint32;
using ULONG32 = quint32;
using DWORD32 = quint32;
using INT64  = qint64;
using UINT64 = quint64;
using DWORD64 = quint64;
using LONG64  = qint64;
using ULONG64 = quint64;
using INT_PTR = qptrdiff;
using UINT_PTR = quintptr;

#define MAXUINT  ((UINT)-1)
#define MAXULONG ((ULONG)-1)
#define MAXDWORD ((DWORD)-1)
#define MAXLONG  ((LONG)(0x7FFFFFFF))
#define INFINITE 0xFFFFFFFF
#define MAX_PATH 260

// ─── Handle types ──────────────────────────────────────────────────────────────
using HANDLE = void*;
using HINSTANCE = void*;
using HMODULE = void*;
using HICON = QPixmap*;
using HCURSOR = QCursor*;
using HDC = QPainter*;
using HBITMAP = QPixmap*;
using HBRUSH = QBrush*;
using HPEN = QPen*;
using HFONT = QFont*;
using HRGN = QRegion*;
using HMENU = QMenu*;
using HKEY = void*;
using HKL = void*;
using HACCEL = void*;
using HDWP = void*;
using HGDIOBJ = void*;
using HWND = QWidget*;
using HGLOBAL = void*;
using HLOCAL = void*;
using GLOBALHANDLE = void*;
using LOCALHANDLE = void*;
using HWINSTA = void*;
using HDESK = void*;
using HHOOK = void*;
using HINSTANCE__ = void*;
#define HMODULE void*
#define HACCEL void*
#define HDWP void*

// ─── String pointer types ──────────────────────────────────────────────────────
using LPSTR = char*;
using LPCSTR = const char*;
using LPWSTR = wchar_t*;
using LPCWSTR = const wchar_t*;
using LPTSTR = wchar_t*;
using LPCTSTR = const wchar_t*;
using PSTR = char*;
using PCSTR = const char*;
using PWSTR = wchar_t*;
using PCWSTR = const wchar_t*;
using PVOID = void*;
using LPVOID = void*;
using LPCVOID = const void*;
using LPDWORD = DWORD*;
using LPBYTE = BYTE*;
using LPWORD = WORD*;
using LPLONG = LONG*;
using LPBOOL = int*;
using PBOOL = int*;
using LPCOLORREF = QRgb*;
using COLORREF = QRgb;

// ─── Simple structs ─────────────────────────────────────────────────────────────
struct POINT { long x = 0; long y = 0; POINT() = default; POINT(long xx, long yy) : x(xx), y(yy) {} };
struct SIZE  { long cx = 0; long cy = 0; SIZE() = default; SIZE(long w, long h) : cx(w), cy(h) {} };
struct RECT  {
    long left = 0, top = 0, right = 0, bottom = 0;
    RECT() = default;
    RECT(long l, long t, long r, long b) : left(l), top(t), right(r), bottom(b) {}
    long width() const { return right - left; }
    long height() const { return bottom - top; }
    bool isEmpty() const { return width() <= 0 || height() <= 0; }
    QRect toQRect() const { return QRect(left, top, width(), height()); }
    static RECT fromQRect(const QRect& r) { return {r.x(), r.y(), r.x()+r.width(), r.y()+r.height()}; }
};
using LPRECT = RECT*;
using LPCRECT = const RECT*;
using LPPOINT = POINT*;
using LPCPOINT = const POINT*;
using LPSIZE = SIZE*;
using LPCSIZE = const SIZE*;

struct MSG {
    QWidget* hwnd = nullptr; UINT message = 0; WPARAM wParam = 0; LPARAM lParam = 0;
    DWORD time = 0; POINT pt{0,0};
};
struct PAINTSTRUCT { QPainter* hdc = nullptr; bool fErase = false; RECT rcPaint; };
struct CREATESTRUCTW {
    LPVOID lpCreateParams = nullptr; HINSTANCE hInstance = nullptr; HMENU hMenu = nullptr;
    QWidget* hwndParent = nullptr; int cy = 0, cx = 0, y = 0, x = 0;
    long style = 0; LPCWSTR lpszName = nullptr; LPCWSTR lpszClass = nullptr; DWORD dwExStyle = 0;
};
struct WINDOWPLACEMENT {
    UINT length = 0; UINT flags = 0; UINT showCmd = 0;
    POINT ptMinPosition{0,0}, ptMaxPosition{0,0}; RECT rcNormalPosition;
};
struct MINMAXINFO { POINT ptReserved{0,0}, ptMaxSize{0,0}, ptMaxPosition{0,0}, ptMinTrackSize{0,0}, ptMaxTrackSize{0,0}; };
struct NCCALCSIZE_PARAMS { RECT rgrc[3]; PWSTR lppos = nullptr; };
struct COPYDATASTRUCT { ULONG_PTR dwData = 0; DWORD cbData = 0; PVOID lpData = nullptr; };
struct HDITEMW { DWORD mask = 0; int cxy = 0; LPWSTR pszText = nullptr; HBITMAP hbm = nullptr;
    int cchTextMax = 0; int fmt = 0; LPARAM lParam = 0; int iImage = 0; int iOrder = 0; UINT type = 0; };
struct LVITEMW { DWORD mask = 0; int iItem = 0; int iSubItem = 0; UINT state = 0; UINT stateMask = 0;
    LPWSTR pszText = nullptr; int cchTextMax = 0; int iImage = 0; LPARAM lParam = 0; int iIndent = 0; };
struct NMHDR { QWidget* hwndFrom = nullptr; quint64 idFrom = 0; quint64 code = 0; };
struct NMITEMACTIVATE { NMHDR hdr{nullptr,0,0}; int iItem = -1; int iSubItem = 0; UINT uNewState = 0; UINT uOldState = 0;
    UINT uChanged = 0; POINT ptAction{0,0}; LPARAM lParam = 0; UINT uKeyFlags = 0; };
struct NMTVDISPINFOW { NMHDR hdr{nullptr,0,0}; LVITEMW item; };
struct LVDISPINFOW { NMHDR hdr{nullptr,0,0}; LVITEMW item; };
struct NMLVCUSTOMDRAW { NMHDR hdr{nullptr,0,0}; DWORD dwDrawStage = 0; HDC hdc = nullptr;
    RECT rc{0,0,0,0}; DWORD_PTR dwItemSpec = 0; UINT uItemState = 0; LPARAM lItemlParam = 0; int iSubItem = 0; };
struct NMLVGETINFOTIPW { NMHDR hdr{nullptr,0,0}; DWORD dwFlags = 0; int iItem = 0; int iSubItem = 0;
    LPWSTR pszText = nullptr; int cchTextMax = 0; LPARAM lParam = 0; POINT ptAction{0,0}; UINT uKeyFlags = 0; };
struct NMTBHOTITEM { NMHDR hdr{nullptr,0,0}; int idOld = 0; int idNew = 0; UINT dwFlags = 0; };
struct NMPGINFO { NMHDR hdr{nullptr,0,0}; DWORD dwContext = 0; DWORD dwSize = 0;
    RECT rcParent{0,0,0,0}; int iActive = 0; int iCol = -1; };
struct PSHNOTIFY { NMHDR hdr{nullptr,0,0}; LPARAM lParam = 0; };
struct MEASUREITEMSTRUCT { UINT CtlType = 0; UINT CtlID = 0; UINT itemID = 0;
    UINT itemWidth = 0; UINT itemHeight = 0; ULONG_PTR itemData = 0; };
struct DRAWITEMSTRUCT { UINT CtlType = 0; UINT CtlID = 0; UINT itemID = 0; UINT itemAction = 0;
    UINT itemState = 0; QWidget* hwndItem = nullptr; HDC hDC = nullptr; RECT rcItem{0,0,0,0}; ULONG_PTR itemData = 0; };
struct WIN32_FIND_DATAW {
    DWORD dwFileAttributes = 0; DWORD ftCreationTime[2] = {0,0}; DWORD ftLastAccessTime[2] = {0,0};
    DWORD ftLastWriteTime[2] = {0,0}; DWORD nFileSizeHigh = 0; DWORD nFileSizeLow = 0;
    DWORD dwReserved0 = 0; DWORD dwReserved1 = 0; wchar_t cFileName[260]; wchar_t cAlternateFileName[14];
};
struct SECURITY_ATTRIBUTES { DWORD nLength = 0; LPVOID lpSecurityDescriptor = nullptr; BOOL bInheritHandle = FALSE; };
struct OVERLAPPED {
    ULONG_PTR Internal = 0; ULONG_PTR InternalHigh = 0; DWORD Offset = 0; DWORD OffsetHigh = 0; HANDLE hEvent = nullptr;
};

// ─── Forward declarations ─────────────────────────────────────────────────────
class Buffer; class FileManager; class ScintillaEditView; class ScintillaCtrls;
class NppParameters; class NppGUI; class CmdLineParams; class SciVer; class Font; class ShortcutMapping;
class NativeLangSpeaker; class DockingManager; class AutoCompletion; class SmartHighlighter;
class DocTabView; class SplitterContainer; class ContextMenu; class FindReplaceDlg;
class ToolBar; class StatusBar; class TabBar; class StaticDialog; class AboutDlg; class RunDlg;
class GoToLineDlg; class ColumnEditorDlg; class WordStyleDlg; class ColourPicker;
class PreferenceDlg; class NppConverter;

// ─── Constants ────────────────────────────────────────────────────────────────

#define CP_ACP       0
#define CP_UTF8      65001
#define SC_CP_UTF8   65001
#define WM_USER      0x0400
#define MAX_LANGNAME 32
#define MAX_EXTERNAL 16
#define MAX_PRIVATE  256
#define IDOK               1
#define IDCANCEL           2
#define IDABORT            3
#define IDRETRY            4
#define IDIGNORE           5
#define IDYES              6
#define IDNO               7
#define IDCLOSE            8
#define IDHELP             9
#define MAKELANGID(p,s)    (((WORD)(p) << 10) | (WORD)(s))
#define LANG_NEUTRAL       0x00
#define SUBLANG_DEFAULT    0x01
#define INVALID_HANDLE_VALUE nullptr
#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2
#define GENERIC_READ    0x80000000
#define GENERIC_WRITE   0x40000000
#define FILE_SHARE_READ  0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5
#define FILE_ATTRIBUTE_NORMAL       0x80
#define FILE_ATTRIBUTE_DIRECTORY    0x10
#define FILE_ATTRIBUTE_READONLY    0x01
#define FILE_ATTRIBUTE_HIDDEN      0x02
#define FILE_ATTRIBUTE_SYSTEM      0x04
#define FILE_ATTRIBUTE_ARCHIVE     0x20
#define FILE_FLAG_RANDOM_ACCESS    0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN  0x08000000
#define MB_OK               0x00000000L
#define MB_OKCANCEL         0x00000001L
#define MB_YESNO            0x00000004L
#define MB_YESNOCANCEL      0x00000003L
#define MB_RETRYCANCEL      0x00000005L
#define MB_ICONEXCLAMATION  0x00000030L
#define MB_ICONERROR        0x00000010L
#define MB_ICONINFORMATION  0x00000040L
#define MB_ICONQUESTION     0x00000020L
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWNOACTIVATE   7
#define SW_SHOWNA           8
#define SW_RESTORE          9
#define SWP_NOSIZE         0x0001
#define SWP_NOMOVE         0x0002
#define SWP_NOZORDER       0x0004
#define SWP_NOACTIVATE     0x0010
#define SWP_SHOWWINDOW     0x0040
#define SWP_HIDEWINDOW     0x0080
#define HWND_TOP        nullptr
#define GWL_STYLE      -16
#define GWL_EXSTYLE   -20
#define GWL_ID         -12
#define WS_OVERLAPPED  0x00000000L
#define WS_POPUP       0x80000000L
#define WS_CHILD       0x40000000L
#define WS_MINIMIZE    0x20000000L
#define WS_VISIBLE     0x10000000L
#define WS_DISABLED    0x08000000L
#define WS_CLIPSIBLINGS 0x04000000L
#define WS_CLIPCHILDREN 0x02000000L
#define WS_MAXIMIZE    0x01000000L
#define WS_CAPTION     0x00C00000L
#define WS_BORDER      0x00800000L
#define WS_DLGFRAME    0x00400000L
#define WS_VSCROLL     0x00200000L
#define WS_HSCROLL     0x00100000L
#define WS_SYSMENU     0x00080000L
#define WS_THICKFRAME  0x00040000L
#define WS_MINIMIZEBOX 0x00020000L
#define WS_MAXIMIZEBOX 0x00010000L
#define WS_TILEDWINDOW (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX)
#define WS_EX_TOOLWINDOW 0x00000080L
#define WS_EX_APPWINDOW  0x00040000L
#define WS_EX_CLIENTEDGE  0x00000200L
#define WS_EX_WINDOWEDGE  0x00000100L
#define WS_EX_DLGMODALFRAME 0x00000001L
#define WS_EX_RIGHT       0x00001000L
#define WS_EX_RTLREADING 0x00002000L
#define WS_EX_STATICEDGE  0x00020000L
#define WS_EX_NOACTIVATE 0x08000000L
#define TVS_HASBUTTONS   0x0001
#define TVS_HASLINES     0x0002
#define TVS_EDITLABELS    0x0008
#define TVS_SHOWSELALWAYS 0x0020
#define TVS_SINGLEEXPAND   0x0400
#define TVS_FULLROWSELECT  0x1000
#define TVS_NOSCROLL      0x8000
#define TVS_CHECKBOXES    0x0100
#define TVM_GETITEMHEIGHT (0x1100+28)
#define TVM_SETITEMHEIGHT (0x1100+27)
#define TVM_DELETEITEM    (0x1100+1)
#define TVM_EXPAND       (0x1100+2)
#define TVE_COLLAPSE     0x0001
#define TVE_EXPAND       0x0002
#define TVE_TOGGLE       0x0003
#define TVM_SELECTITEM   (0x1100+11)
#define TVM_GETNEXTITEM  (0x1100+0xa)
#define TVGN_ROOT        0x0
#define TVGN_NEXT        0x1
#define TVGN_PREVIOUS    0x2
#define TVGN_PARENT      0x3
#define TVGN_CHILD       0x4
#define TVGN_CARET       0x9
#define TVM_EDITLABEL    (0x1100+13)
#define TVM_GETITEMSTATE  (0x1100+39)
#define TVM_GETCOUNT      (0x1100+4)
#define TVIS_SELECTED     0x0002
#define TVIS_EXPANDED     0x0020
#define TVIS_STATEIMAGEMASK 0xF000
#define TVM_SETBKCOLOR   (0x1100+29)
#define TVM_SETTEXTCOLOR (0x1100+30)
#define TVM_CREATEDRAGIMAGE (0x1100+18)
#define TVN_SELCHANGED    (-2)
#define TVN_DELETEITEMA    (-9)
#define TVN_BEGINDRAGA     (-56)
#define TVIF_TEXT        0x0001
#define TVIF_IMAGE       0x0002
#define TVIF_PARAM       0x0004
#define TVIF_STATE       0x0008
#define TVIF_DI_SETITEM  0x1000
#define LVS_ICON         0x0000
#define LVS_REPORT        0x0001
#define LVS_SMALLICON    0x0002
#define LVS_LIST         0x0003
#define LVS_SINGLESEL    0x0004
#define LVS_SHOWSELALWAYS 0x0008
#define LVS_SORTASCENDING 0x0010
#define LVS_SORTDESCENDING 0x0030
#define LVS_NOCOLUMNHEADER 0x4000
#define LVS_NOSORTHEADER  0x8000
#define LVS_OWNERDATA    0x1000
#define LVM_GETITEMCOUNT  (0x1000+4)
#define LVM_GETITEM       (0x1000+5)
#define LVM_SETITEM       (0x1000+6)
#define LVM_DELETEITEM    (0x1000+8)
#define LVM_DELETEALLITEMS (0x1000+9)
#define LVM_INSERTITEM    (0x1000+7)
#define LVM_GETITEMTEXT   (0x1000+77)
#define LVM_SETITEMTEXT   (0x1000+76)
#define LVM_GETSELECTEDCOUNT (0x1000+50)
#define LVM_GETITEMSTATE  (0x1000+44)
#define LVM_SETITEMSTATE  (0x1000+43)
#define LVM_GETNEXTITEM   (0x1000+0xa)
#define LVM_ENSUREVISIBLE (0x1000+13)
#define LVM_GETCOLUMN     (0x1000+2b)
#define LVM_SETCOLUMN     (0x1000+2c)
#define LVM_INSERTCOLUMN  (0x1000+2d)
#define LVM_DELETECOLUMN  (0x1000+2e)
#define LVIS_FOCUSED     0x0001
#define LVIS_SELECTED    0x0002
#define LVIF_TEXT        0x0001
#define LVIF_IMAGE       0x0002
#define LVIF_PARAM       0x0004
#define LVIF_STATE       0x0008
#define LVN_ITEMCHANGED   0xFFFFFF9C
#define LVN_DELETEITEM    0xFFFFFF9A
#define LVN_DELETEALLITEMS 0xFFFFFF99
#define LVN_BEGINLABELEDITW 0xFFFFFF97
#define LVN_ENDLABELEDITW 0xFFFFFF96
#define LVN_COLUMNCLICK  0xFFFFFF98
#define LVN_GETDISPINFOW 0xFFFFFF63
#define LVM_REDRAWITEMS  (0x1000+15)
#define LVM_ARRANGE      (0x1000+11)
#define LVM_SCROLL       (0x1000+20)
#define LVM_SETCOLUMNWIDTH (0x1000+30)
#define LVM_GETTOPINDEX  (0x1000+77)
#define HDM_GETITEMCOUNT (0x1200+0)
#define HDM_INSERTITEM   (0x1200+1)
#define HDM_DELETEITEM   (0x1200+2)
#define HDM_GETITEM      (0x1200+3)
#define HDM_SETITEM      (0x1200+4)
#define HDS_DRAGDROP     0x0040
#define HDS_FULLDRAG     0x0080
#define HDS_CHECKBOXES   0x0200
#define HDN_ITEMCHANGED  0xFFFFFFBD
#define HDN_TRACK        0xFFFFFFC8
#define HDI_FORMAT       0x0004
#define HDF_LEFT         0x0000
#define HDF_RIGHT        0x0001
#define HDF_CENTER       0x0002
#define HDF_BITMAP       0x2000
#define HDF_OWNERDRAW    0x8000
#define HDF_STRING       0x4000
#define SB_SETTEXTA     (0x0400+1)
#define SB_GETTEXTA     (0x0400+2)
#define SB_GETTEXTLENGTHA (0x0400+3)
#define SB_SETPARTS     (0x0400+4)
#define SB_GETPARTS     (0x0400+6)
#define SB_SETICON      (0x0400+0x3F)
#define SB_SETBKCOLOR   (0x0400+1)
#define SB_SIMPLE       (0x0400+0x0F)
#define SB_ISSIMPLE     (0x0400+0x12)
#define SBARS_SIZEGRIP  0x0100
#define EM_GETSEL       (0x00B0+0)
#define EM_SETSEL       (0x00B0+0)
#define EM_GETRECT      (0x00B0+2)
#define EM_SETRECT      (0x00B0+3)
#define EM_SCROLL       (0x00B0+5)
#define EM_LINESCROLL   (0x00B0+6)
#define EM_GETMODIFY    (0x00B0+8)
#define EM_SETMODIFY    (0x00B0+9)
#define EM_GETLINECOUNT  (0x00B0+0xb)
#define EM_LINEINDEX    (0x00B0+0xe)
#define EM_LINELENGTH   (0x00B0+0xf)
#define EM_GETLINE      (0x00B0+0x14)
#define EM_CANUNDO      (0x00C0+0xe)
#define EM_UNDO         (0x00C0+0x07)
#define EM_REPLACESEL   (0x00C2+0x0a)
#define CB_GETCOUNT     (0x0150+3)
#define CB_GETCURSEL    (0x0150+7)
#define CB_ADDSTRING    (0x0150+3)
#define CB_INSERTSTRING (0x0150+4)
#define CB_DELETESTRING (0x0150+6)
#define CB_GETLBTEXT    (0x0150+8)
#define CB_GETLBTEXTLEN  (0x0150+9)
#define CB_RESETCONTENT  (0x0150+5)
#define CB_SETCURSEL    (0x0150+14)
#define CB_SHOWDROPDOWN  (0x0150+0x14f)
#define CB_GETDROPPEDSTATE (0x0150+0x157)
#define BN_CLICKED      0
#define EN_CHANGE       0x0300
#define EN_UPDATE       0x0400
#define EN_SETFOCUS     0x0100
#define EN_KILLFOCUS    0x0200
#define TVM_GETINDENT   (0x1100+10)
#define TVM_SETINDENT   (0x1100+11)
#define TVM_GETITEMW    (0x1100+62)
#define TVM_SETITEMW    (0x1100+63)
#define RB_INSERTBANDA  (0x0400+1)
#define RB_DELETEBAND   (0x0400+2)
#define RB_GETBARHEIGHT (0x0400+27)
#define RB_GETBANDCOUNT (0x0400+12c)
#define RBN_HEIGHTCHANGE 0xFFFFFFFF-0x0e9a
#define SB_HORZ 0
#define SB_VERT 1
#define SB_CTL  2
#define SB_BOTH 3
#define SB_LINEUP   0
#define SB_LINEDOWN 1
#define SB_PAGEUP   2
#define SB_PAGEDOWN 3
#define SB_THUMBPOSITION 4
#define SB_THUMBTRACK    5
#define SB_ENDSCROLL 8
#define SC_SIZE         0xF000
#define SC_MOVE         0xF010
#define SC_MINIMIZE     0xF