#include "WindowsCompat.h"
#include "Notepad_plus_Window.h"
#include "WinControls/SplitterContainer/SplitterContainer.h"
#include "WinControls/DockingWnd/DockingManager.h"
#include "ScintillaComponents/scintilla/qt/ScintillaEdit/ScintillaEdit.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMainWindow>
#include <QPointer>
#include <QTabWidget>
#include <QTabBar>
#include <QWidget>
#include <QObject>

#include <cwchar>
#include <string>
#include <unordered_map>

namespace {

struct NppWndExtra {
	std::wstring className;
	LONG_PTR userData = 0;
};

std::unordered_map<QWidget*, NppWndExtra>& windowExtras()
{
	static std::unordered_map<QWidget*, NppWndExtra> extras;
	return extras;
}

void rememberWindowClass(HWND hwnd, const wchar_t* className)
{
	if (!hwnd || !className)
		return;
	windowExtras()[hwnd].className = className;
}

bool classNameIs(const wchar_t* cls, const wchar_t* expected)
{
	return cls && expected && std::wcscmp(cls, expected) == 0;
}

bool isScintillaClass(const wchar_t* cls)
{
	return classNameIs(cls, L"Scintilla");
}

bool isNotepadClass(const wchar_t* cls)
{
	return classNameIs(cls, Notepad_plus_Window::getClassName());
}

void applyWindowStyle(QWidget* widget, DWORD style, int x, int y, int w, int h)
{
	if (!widget)
		return;

	if ((style & WS_VISIBLE) == 0)
		widget->hide();

	if (x != static_cast<int>(CW_USEDEFAULT) && y != static_cast<int>(CW_USEDEFAULT)
		&& w > 0 && h > 0) {
		widget->setGeometry(x, y, w, h);
	} else if (w > 0 && h > 0) {
		widget->resize(w, h);
	}
}

void forwardScintillaNotify(ScintillaEditBase* sci, Scintilla::NotificationData* scn)
{
	if (!sci || !scn)
		return;

	QWidget* parent = sci->parentWidget();
	if (!parent)
		return;

	SCNotification notification{};
	notification.nmhdr.hwndFrom = sci;
	notification.nmhdr.idFrom = reinterpret_cast<uptr_t>(sci);
	notification.nmhdr.code = static_cast<UINT>(scn->nmhdr.code);
	notification.message = static_cast<int>(scn->message);
	notification.wParam = scn->wParam;
	notification.lParam = scn->lParam;
	notification.position = scn->position;
	notification.ch = scn->ch;
	notification.modifiers = static_cast<int>(scn->modifiers);
	notification.modificationType = static_cast<int>(scn->modificationType);
	notification.text = scn->text;
	notification.length = scn->length;
	notification.linesAdded = scn->linesAdded;
	notification.line = scn->line;
	notification.foldLevelNow = static_cast<int>(scn->foldLevelNow);
	notification.foldLevelPrev = static_cast<int>(scn->foldLevelPrev);
	notification.margin = scn->margin;
	notification.listType = scn->listType;
	notification.x = scn->x;
	notification.y = scn->y;
	notification.annotationLinesAdded = scn->annotationLinesAdded;
	notification.updated = static_cast<int>(scn->updated);
	notification.token = scn->token;

	SendMessageW(parent, WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&notification));
}

QWidget* createScintillaWidget(QWidget* parent)
{
	auto* sci = new ScintillaEdit(parent);
	sci->setFocusPolicy(Qt::StrongFocus);
	sci->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sci->setMinimumSize(200, 150);
	QObject::connect(sci, &ScintillaEditBase::notify, sci,
		[sci](Scintilla::NotificationData* scn) { forwardScintillaNotify(sci, scn); });
	return sci;
}

QWidget* createTabWidget(QWidget* parent)
{
	auto* tabs = new QTabWidget(parent);
	tabs->setDocumentMode(true);
	tabs->tabBar()->setFocusPolicy(Qt::NoFocus);
	return tabs;
}

HWND createDialogWidget(HWND parent, DLGPROC dlgProc, LPARAM initParam)
{
	QWidget* dialog = new QWidget(parent);
	dialog->setWindowFlag(Qt::Dialog, true);
	dialog->setAttribute(Qt::WA_DeleteOnClose, false);
	dialog->resize(480, 90);
	if (dlgProc)
		dlgProc(dialog, WM_INITDIALOG, 0, initParam);
	dialog->show();
	return dialog;
}

LRESULT dispatchWindowMessage(HWND hwnd, const wchar_t* className, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!hwnd)
		return 0;

	if (isNotepadClass(className) || qobject_cast<QMainWindow*>(hwnd))
		return Notepad_plus_Window::dispatchWin32Message(hwnd, msg, wParam, lParam);

	if (className && classNameIs(className, L"splitterContainer"))
		return SplitterContainer::dispatchWin32Message(hwnd, msg, wParam, lParam);

	if (className && classNameIs(className, DSPC_CLASS_NAME))
		return DockingManager::dispatchWin32Message(hwnd, msg, wParam, lParam);

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void bootstrapWindow(HWND hwnd, const wchar_t* className, void* lpCreateParams)
{
	if (!hwnd || !lpCreateParams)
		return;

	CREATESTRUCTW cs{};
	cs.lpCreateParams = lpCreateParams;
	dispatchWindowMessage(hwnd, className, WM_NCCREATE, 0, reinterpret_cast<LPARAM>(&cs));
	dispatchWindowMessage(hwnd, className, WM_CREATE, 0, 0);
}

class CloseToWin32Bridge final : public QObject {
public:
	explicit CloseToWin32Bridge(QWidget* hwnd)
		: QObject(hwnd)
		, m_hwnd(hwnd)
	{
		hwnd->installEventFilter(this);
	}

	bool eventFilter(QObject* watched, QEvent* event) override
	{
		if (watched != m_hwnd || event->type() != QEvent::Close)
			return QObject::eventFilter(watched, event);

		// Defer WM_CLOSE so we never synchronously delete the window inside QEvent::Close.
		const QPointer<QWidget> guard(m_hwnd);
		QMetaObject::invokeMethod(
			QCoreApplication::instance(),
			[guard]() {
				if (guard)
					SendMessageW(guard, WM_CLOSE, 0, 0);
			},
			Qt::QueuedConnection);
		static_cast<QCloseEvent*>(event)->ignore();
		return true;
	}

private:
	QWidget* m_hwnd;
};

} // namespace

HWND CreateDialogParamW(HINSTANCE, const wchar_t*, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	return createDialogWidget(hWndParent, lpDialogFunc, dwInitParam);
}

HWND CreateDialogIndirectParamW(HINSTANCE, DLGTEMPLATE*, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	return createDialogWidget(hWndParent, lpDialogFunc, dwInitParam);
}

INT_PTR DialogBoxParamW(HINSTANCE, const wchar_t*, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	HWND hDlg = createDialogWidget(hWndParent, lpDialogFunc, dwInitParam);
	return hDlg ? IDOK : -1;
}

INT_PTR DialogBoxIndirectParamW(HINSTANCE, DLGTEMPLATE*, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	HWND hDlg = createDialogWidget(hWndParent, lpDialogFunc, dwInitParam);
	return hDlg ? IDOK : -1;
}

HWND CreateWindowExW(
	DWORD dwExStyle,
	const wchar_t* lpClassName,
	const wchar_t* lpWindowName,
	DWORD dwStyle,
	int x, int y, int nWidth, int nHeight,
	HWND hWndParent,
	HMENU hMenu,
	HINSTANCE hInstance,
	void* lpCreateParams)
{
	Q_UNUSED(dwExStyle);
	Q_UNUSED(hMenu);
	Q_UNUSED(hInstance);

	QWidget* parentWidget = hWndParent;
	QWidget* widget = nullptr;

	if (isScintillaClass(lpClassName)) {
		widget = createScintillaWidget(parentWidget);
	} else if (isNotepadClass(lpClassName)) {
		auto* main = new QMainWindow();
		if (lpWindowName)
			main->setWindowTitle(QString::fromWCharArray(lpWindowName));
		main->setMinimumSize(400, 300);
		if (nWidth <= 0 || nHeight <= 0)
			main->resize(800, 600);
		new CloseToWin32Bridge(main);
		widget = main;
	} else if (classNameIs(lpClassName, WC_TABCONTROL)) {
		widget = createTabWidget(parentWidget);
	} else {
		widget = new QWidget(parentWidget);
	}

	if ((dwStyle & WS_CHILD) && parentWidget)
		widget->setParent(parentWidget);

	rememberWindowClass(widget, lpClassName);
	applyWindowStyle(widget, dwStyle, x, y, nWidth, nHeight);

	if (lpCreateParams)
		bootstrapWindow(widget, lpClassName, lpCreateParams);

	if (dwStyle & WS_VISIBLE)
		widget->show();

	return widget;
}

BOOL DestroyWindow(HWND hwnd)
{
	if (!hwnd)
		return FALSE;

	const auto it = windowExtras().find(hwnd);
	const wchar_t* className = (it != windowExtras().end()) ? it->second.className.c_str() : nullptr;

	dispatchWindowMessage(hwnd, className, WM_DESTROY, 0, 0);
	windowExtras().erase(hwnd);
	delete hwnd;
	return TRUE;
}

void PostQuitMessage(int)
{
	QCoreApplication* app = QCoreApplication::instance();
	if (!app)
		return;
	// Queue quit so WM_DESTROY / DestroyWindow can finish before aboutToQuit runs.
	QMetaObject::invokeMethod(app, &QCoreApplication::quit, Qt::QueuedConnection);
}

LRESULT SendMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!hwnd)
		return 0;

	if (auto* sci = qobject_cast<ScintillaEditBase*>(hwnd))
		return sci->send(msg, wParam, lParam);

	if (auto* tabs = qobject_cast<QTabWidget*>(hwnd)) {
		switch (msg) {
		case TCM_GETCURSEL:
			return tabs->currentIndex();
		case TCM_SETCURSEL:
			if (static_cast<int>(wParam) >= 0 && static_cast<int>(wParam) < tabs->count())
				tabs->setCurrentIndex(static_cast<int>(wParam));
			return 0;
		case TCM_GETITEMCOUNT:
			return tabs->count();
		case TCM_DELETEALLITEMS:
			while (tabs->count() > 0)
				tabs->removeTab(0);
			return TRUE;
		default:
			break;
		}
	}

	const auto it = windowExtras().find(hwnd);
	const wchar_t* className = (it != windowExtras().end()) ? it->second.className.c_str() : nullptr;
	if (className || qobject_cast<QMainWindow*>(hwnd))
		return dispatchWindowMessage(hwnd, className, msg, wParam, lParam);

	return 0;
}

LONG_PTR SetWindowLongPtrW(HWND hwnd, int index, LONG_PTR value)
{
	if (!hwnd)
		return 0;

	auto& data = windowExtras()[hwnd];
	const LONG_PTR previous = data.userData;
	if (index == GWLP_USERDATA)
		data.userData = value;
	return previous;
}

LONG_PTR GetWindowLongPtrW(HWND hwnd, int index)
{
	if (!hwnd)
		return 0;

	if (index == GWLP_USERDATA) {
		const auto it = windowExtras().find(hwnd);
		if (it != windowExtras().end())
			return it->second.userData;
	}
	return 0;
}

BOOL ShowWindow(HWND hwnd, int nCmdShow)
{
	if (!hwnd)
		return FALSE;

	switch (nCmdShow) {
	case SW_HIDE:
		hwnd->hide();
		break;
	case SW_SHOWMINIMIZED:
		hwnd->showMinimized();
		break;
	case SW_MAXIMIZE: // SW_SHOWMAXIMIZED has the same value (3)
		hwnd->showMaximized();
		break;
	default:
		hwnd->show();
		break;
	}
	return TRUE;
}

BOOL SetWindowPlacement(HWND hwnd, const WINDOWPLACEMENT* lpwndpl)
{
	if (!hwnd || !lpwndpl)
		return FALSE;

	const RECT& rc = lpwndpl->rcNormalPosition;
	const int width = rc.right - rc.left;
	const int height = rc.bottom - rc.top;
	if (width > 0 && height > 0)
		hwnd->setGeometry(rc.left, rc.top, width, height);

	switch (lpwndpl->showCmd) {
	case SW_MAXIMIZE: // SW_SHOWMAXIMIZED has the same value (3)
		hwnd->showMaximized();
		break;
	case SW_SHOWMINIMIZED:
		hwnd->showMinimized();
		break;
	case SW_HIDE:
		hwnd->hide();
		break;
	default:
		if (lpwndpl->showCmd != SW_HIDE)
			hwnd->show();
		break;
	}
	return TRUE;
}

BOOL GetClientRect(HWND hwnd, RECT* lpRect)
{
	if (!hwnd || !lpRect)
		return FALSE;

	const QRect r = hwnd->rect();
	lpRect->left = 0;
	lpRect->top = 0;
	lpRect->right = r.width();
	lpRect->bottom = r.height();
	return TRUE;
}

BOOL GetWindowRect(HWND hwnd, RECT* lpRect)
{
	if (!hwnd || !lpRect)
		return FALSE;

	const QRect r = hwnd->frameGeometry();
	lpRect->left = r.x();
	lpRect->top = r.y();
	lpRect->right = r.x() + r.width();
	lpRect->bottom = r.y() + r.height();
	return TRUE;
}
