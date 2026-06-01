#include "WindowsCompat.h"
#include "Notepad_plus_Window.h"
#include "WinControls/SplitterContainer/SplitterContainer.h"
#include "WinControls/DockingWnd/DockingManager.h"
#include "ScintillaComponents/scintilla/qt/ScintillaEdit/ScintillaEdit.h"

#include <QApplication>
#include <QAction>
#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QStatusBar>
#include <QPointer>
#include <QTabWidget>
#include <QTabBar>
#include <QWidget>
#include <QObject>

#include <cwchar>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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

struct NppMenuItem {
	UINT flags = 0;
	UINT_PTR id = 0;
	std::wstring text;
	HMENU submenu = nullptr;
};

struct NppMenuData {
	std::vector<NppMenuItem> items;
	HMENU parent = nullptr;
};

std::unordered_map<HMENU, std::unique_ptr<NppMenuData>>& menuStore()
{
	static std::unordered_map<HMENU, std::unique_ptr<NppMenuData>> store;
	return store;
}

std::unordered_map<HWND, HMENU>& windowMenus()
{
	static std::unordered_map<HWND, HMENU> byWindow;
	return byWindow;
}

NppMenuData* getMenuData(HMENU menu)
{
	const auto it = menuStore().find(menu);
	return (it != menuStore().end()) ? it->second.get() : nullptr;
}

HMENU allocMenu()
{
	auto menu = std::make_unique<NppMenuData>();
	HMENU handle = reinterpret_cast<HMENU>(menu.get());
	menuStore()[handle] = std::move(menu);
	return handle;
}

QString toQString(const std::wstring& text)
{
	return QString::fromWCharArray(text.c_str());
}

struct MainWindowChromeInsets {
	int top = 0;
	int bottom = 0;
};

MainWindowChromeInsets mainWindowChromeInsets(const QMainWindow* main)
{
	MainWindowChromeInsets insets;
	if (!main)
		return insets;

	if (QMenuBar* bar = main->menuBar()) {
		if (bar->isVisible() && !bar->actions().isEmpty()) {
			const int h = bar->height();
			insets.top = h > 0 ? h : bar->sizeHint().height();
		}
	}

	if (QStatusBar* status = main->statusBar()) {
		if (status->isVisible()) {
			const int h = status->height();
			insets.bottom = h > 0 ? h : status->sizeHint().height();
		}
	}
	return insets;
}

void applyClientGeometryToParent(QWidget* widget, int x, int y, int w, int h)
{
	if (!widget)
		return;

	if (auto* main = qobject_cast<QMainWindow*>(widget->parentWidget())) {
		const MainWindowChromeInsets insets = mainWindowChromeInsets(main);
		y += insets.top;
		if (QMenuBar* bar = main->menuBar())
			bar->raise();
	}
	widget->setGeometry(x, y, w, h);
}

HMENU rootMenu(HMENU menu)
{
	HMENU current = menu;
	while (NppMenuData* data = getMenuData(current)) {
		if (!data->parent)
			return current;
		current = data->parent;
	}
	return menu;
}

void buildQtMenuRecursive(QMenu* qmenu, HMENU menu, HWND targetWindow)
{
	NppMenuData* data = getMenuData(menu);
	if (!qmenu || !data)
		return;

	for (const NppMenuItem& item : data->items) {
		if (item.flags & MF_SEPARATOR) {
			qmenu->addSeparator();
			continue;
		}

		if ((item.flags & MF_POPUP) && item.submenu) {
			QMenu* child = qmenu->addMenu(toQString(item.text));
			buildQtMenuRecursive(child, item.submenu, targetWindow);
			continue;
		}

		QAction* action = qmenu->addAction(toQString(item.text));
		const bool disabled = (item.flags & MF_DISABLED) || (item.flags & MF_GRAYED);
		action->setEnabled(!disabled);
		if (item.id != 0) {
			QObject::connect(action, &QAction::triggered, targetWindow, [targetWindow, id = item.id]() {
				SendMessageW(targetWindow, WM_COMMAND, static_cast<WPARAM>(id), 0);
			});
		}
	}
}

void applyMenuToWindow(HWND hwnd, HMENU menu)
{
	QMainWindow* mainWindow = qobject_cast<QMainWindow*>(hwnd);
	if (!mainWindow)
		return;

	QMenuBar* bar = mainWindow->menuBar();
	if (!bar)
		return;
	bar->setNativeMenuBar(false);
	bar->setEnabled(true);

	bar->clear();
	if (!menu) {
		windowMenus().erase(hwnd);
		bar->setVisible(false);
		return;
	}
	bar->setVisible(true);

	NppMenuData* data = getMenuData(menu);
	if (!data)
		return;

	windowMenus()[hwnd] = menu;
	for (const NppMenuItem& item : data->items) {
		if (item.flags & MF_SEPARATOR) {
			bar->addSeparator();
			continue;
		}

		if ((item.flags & MF_POPUP) && item.submenu) {
			QMenu* top = bar->addMenu(toQString(item.text));
			top->menuAction()->setEnabled(true);
			buildQtMenuRecursive(top, item.submenu, hwnd);
			continue;
		}

		QAction* action = bar->addAction(toQString(item.text));
		action->setEnabled(true);
		if (item.id != 0) {
			QObject::connect(action, &QAction::triggered, hwnd, [hwnd, id = item.id]() {
				SendMessageW(hwnd, WM_COMMAND, static_cast<WPARAM>(id), 0);
			});
		}
	}
}

void refreshMenuTree(HMENU changedMenu)
{
	if (!changedMenu)
		return;
	const HMENU root = rootMenu(changedMenu);
	for (const auto& [hwnd, mapped] : windowMenus()) {
		if (rootMenu(mapped) == root)
			applyMenuToWindow(hwnd, mapped);
	}
}

void ensureDefaultMainMenu(QMainWindow* mainWindow)
{
	if (!mainWindow || windowMenus().find(mainWindow) != windowMenus().end())
		return;

	HMENU root = allocMenu();
	static constexpr const wchar_t* kTopMenus[] = {
		L"&File", L"&Edit", L"&Search", L"&View", L"F&ormat",
		L"&Language", L"&Settings", L"&Tools", L"&Macro", L"&Run",
		L"&Plugins", L"&Window", L"&?", L"", L"&List"
	};

	NppMenuData* rootData = getMenuData(root);
	std::vector<HMENU> topSubmenus;
	for (const wchar_t* title : kTopMenus) {
		HMENU popup = allocMenu();
		if (NppMenuData* popupData = getMenuData(popup))
			popupData->parent = root;
		topSubmenus.push_back(popup);
		rootData->items.push_back(
			{ static_cast<UINT>(MF_BYPOSITION | MF_POPUP), reinterpret_cast<UINT_PTR>(popup), title, popup });
	}

	auto addCmd = [](HMENU menu, UINT id, const wchar_t* text) {
		NppMenuData* data = getMenuData(menu);
		if (!data)
			return;
		data->items.push_back({ MF_BYPOSITION, id, text ? text : L"", nullptr });
	};
	auto addSep = [](HMENU menu) {
		NppMenuData* data = getMenuData(menu);
		if (!data)
			return;
		data->items.push_back({ static_cast<UINT>(MF_BYPOSITION | MF_SEPARATOR), 0, L"", nullptr });
	};

	// File
	addCmd(topSubmenus[MENUINDEX_FILE], IDM_FILE_NEW, L"&New");
	addCmd(topSubmenus[MENUINDEX_FILE], IDM_FILE_OPEN, L"&Open...");
	addCmd(topSubmenus[MENUINDEX_FILE], IDM_FILE_SAVE, L"&Save");
	addCmd(topSubmenus[MENUINDEX_FILE], IDM_FILE_SAVEAS, L"Save &As...");
	addSep(topSubmenus[MENUINDEX_FILE]);
	addCmd(topSubmenus[MENUINDEX_FILE], IDM_FILE_CLOSE, L"&Close");
	addCmd(topSubmenus[MENUINDEX_FILE], IDM_FILE_CLOSEALL, L"Close A&ll");
	addSep(topSubmenus[MENUINDEX_FILE]);
	addCmd(topSubmenus[MENUINDEX_FILE], IDM_FILE_EXIT, L"E&xit");

	// Edit
	addCmd(topSubmenus[MENUINDEX_EDIT], IDM_EDIT_UNDO, L"&Undo");
	addCmd(topSubmenus[MENUINDEX_EDIT], IDM_EDIT_REDO, L"&Redo");
	addSep(topSubmenus[MENUINDEX_EDIT]);
	addCmd(topSubmenus[MENUINDEX_EDIT], IDM_EDIT_CUT, L"Cu&t");
	addCmd(topSubmenus[MENUINDEX_EDIT], IDM_EDIT_COPY, L"&Copy");
	addCmd(topSubmenus[MENUINDEX_EDIT], IDM_EDIT_PASTE, L"&Paste");
	addSep(topSubmenus[MENUINDEX_EDIT]);
	addCmd(topSubmenus[MENUINDEX_EDIT], IDM_EDIT_SELECTALL, L"Select A&ll");

	// Search
	addCmd(topSubmenus[MENUINDEX_SEARCH], IDM_SEARCH_FIND, L"&Find...");
	addCmd(topSubmenus[MENUINDEX_SEARCH], IDM_SEARCH_REPLACE, L"&Replace...");
	addCmd(topSubmenus[MENUINDEX_SEARCH], IDM_SEARCH_FINDNEXT, L"Find &Next");
	addCmd(topSubmenus[MENUINDEX_SEARCH], IDM_SEARCH_FINDPREV, L"Find &Previous");
	addSep(topSubmenus[MENUINDEX_SEARCH]);
	addCmd(topSubmenus[MENUINDEX_SEARCH], IDM_SEARCH_FINDINFILES, L"Find in Fi&les...");
	addCmd(topSubmenus[MENUINDEX_SEARCH], IDM_SEARCH_GOTOLINE, L"&Go to...");

	// View
	addCmd(topSubmenus[MENUINDEX_VIEW], IDM_VIEW_ZOOMIN, L"Zoom &In");
	addCmd(topSubmenus[MENUINDEX_VIEW], IDM_VIEW_ZOOMOUT, L"Zoom &Out");
	addCmd(topSubmenus[MENUINDEX_VIEW], IDM_VIEW_ZOOMRESTORE, L"&Restore Default Zoom");
	addSep(topSubmenus[MENUINDEX_VIEW]);
	addCmd(topSubmenus[MENUINDEX_VIEW], IDM_VIEW_FULLSCREENTOGGLE, L"&Full Screen");

	// Settings
	addCmd(topSubmenus[MENUINDEX_SETTINGS], IDM_SETTING_PREFERENCE, L"&Preferences...");
	addCmd(topSubmenus[MENUINDEX_SETTINGS], IDM_SETTING_SHORTCUT_MAPPER, L"&Shortcut Mapper...");

	// Macro
	addCmd(topSubmenus[MENUINDEX_MACRO], IDM_MACRO_STARTRECORDINGMACRO, L"&Start Recording");
	addCmd(topSubmenus[MENUINDEX_MACRO], IDM_MACRO_STOPRECORDINGMACRO, L"Sto&p Recording");
	addCmd(topSubmenus[MENUINDEX_MACRO], IDM_MACRO_PLAYBACKRECORDEDMACRO, L"&Playback");

	// Run
	addCmd(topSubmenus[MENUINDEX_RUN], IDM_EXECUTE, L"&Run...");

	// Help
	addCmd(topSubmenus[MENUINDEX_HELP], IDM_ABOUT, L"&About Notepad++");

	applyMenuToWindow(mainWindow, root);
}

NppMenuItem* findItemByCommand(HMENU menu, UINT cmd)
{
	NppMenuData* data = getMenuData(menu);
	if (!data)
		return nullptr;
	for (NppMenuItem& item : data->items) {
		if (!(item.flags & MF_POPUP) && item.id == cmd)
			return &item;
		if (item.submenu) {
			if (NppMenuItem* nested = findItemByCommand(item.submenu, cmd))
				return nested;
		}
	}
	return nullptr;
}

const NppMenuItem* findItemByCommandConst(HMENU menu, UINT cmd)
{
	return findItemByCommand(menu, cmd);
}

NppMenuItem* findItemByPosition(HMENU menu, UINT pos)
{
	NppMenuData* data = getMenuData(menu);
	if (!data || pos >= data->items.size())
		return nullptr;
	return &data->items[pos];
}

const NppMenuItem* resolveMenuItem(HMENU menu, UINT item, UINT flags)
{
	if (flags & MF_BYPOSITION)
		return findItemByPosition(menu, item);
	return findItemByCommandConst(menu, item);
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
		ensureDefaultMainMenu(main);
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

HMENU CreateMenu()
{
	return allocMenu();
}

HMENU CreatePopupMenu()
{
	return allocMenu();
}

BOOL InsertMenuW(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, const wchar_t* lpNewItem)
{
	NppMenuData* data = getMenuData(hMenu);
	if (!data)
		return FALSE;

	NppMenuItem item{};
	item.flags = uFlags;
	item.id = uIDNewItem;
	if (lpNewItem)
		item.text = lpNewItem;

	if (uFlags & MF_POPUP) {
		item.submenu = reinterpret_cast<HMENU>(uIDNewItem);
		if (NppMenuData* child = getMenuData(item.submenu))
			child->parent = hMenu;
	}

	const bool byPosition = (uFlags & MF_BYPOSITION) != 0;
	if (!byPosition || uPosition >= data->items.size())
		data->items.push_back(std::move(item));
	else
		data->items.insert(data->items.begin() + uPosition, std::move(item));

	refreshMenuTree(hMenu);
	return TRUE;
}

HMENU AppendMenuW(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, const wchar_t* lpNewItem)
{
	NppMenuData* data = getMenuData(hMenu);
	const UINT pos = data ? static_cast<UINT>(data->items.size()) : 0;
	if (!InsertMenuW(hMenu, pos, uFlags | MF_BYPOSITION, uIDNewItem, lpNewItem))
		return nullptr;
	return hMenu;
}

HMENU GetSubMenu(HMENU hMenu, int nPos)
{
	NppMenuData* data = getMenuData(hMenu);
	if (!data || nPos < 0 || static_cast<size_t>(nPos) >= data->items.size())
		return nullptr;
	return data->items[static_cast<size_t>(nPos)].submenu;
}

HMENU GetMenu(HWND hWnd)
{
	const auto it = windowMenus().find(hWnd);
	return (it != windowMenus().end()) ? it->second : nullptr;
}

BOOL SetMenu(HWND hwnd, HMENU hMenu)
{
	applyMenuToWindow(hwnd, hMenu);
	return TRUE;
}

BOOL RemoveMenu(HMENU hMenu, UINT uPosition, UINT uFlags)
{
	NppMenuData* data = getMenuData(hMenu);
	if (!data)
		return FALSE;

	if (uFlags & MF_BYPOSITION) {
		if (uPosition < data->items.size()) {
			data->items.erase(data->items.begin() + uPosition);
			refreshMenuTree(hMenu);
			return TRUE;
		}
		return FALSE;
	}

	for (auto it = data->items.begin(); it != data->items.end(); ++it) {
		if (!(it->flags & MF_POPUP) && it->id == uPosition) {
			data->items.erase(it);
			refreshMenuTree(hMenu);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL DeleteMenu(HMENU hMenu, UINT uPosition, UINT uFlags)
{
	return RemoveMenu(hMenu, uPosition, uFlags);
}

int GetMenuItemCount(HMENU hMenu)
{
	NppMenuData* data = getMenuData(hMenu);
	return data ? static_cast<int>(data->items.size()) : 0;
}

BOOL GetMenuItemInfoW(HMENU hMenu, UINT item, BOOL byPosition, MENUITEMINFOW* info)
{
	if (!info)
		return FALSE;
	const UINT flags = byPosition ? MF_BYPOSITION : MF_BYCOMMAND;
	const NppMenuItem* menuItem = resolveMenuItem(hMenu, item, flags);
	if (!menuItem)
		return FALSE;

	info->fType = (menuItem->flags & MF_SEPARATOR) ? MFT_SEPARATOR : MFT_STRING;
	info->fState = ((menuItem->flags & MF_DISABLED) || (menuItem->flags & MF_GRAYED))
		? static_cast<UINT>(MF_DISABLED | MF_GRAYED)
		: static_cast<UINT>(0);
	info->wID = static_cast<UINT>(menuItem->id);
	info->hSubMenu = menuItem->submenu;
	if (info->dwTypeData && info->cch > 0) {
		std::wcsncpy(info->dwTypeData, menuItem->text.c_str(), info->cch);
		info->dwTypeData[info->cch - 1] = L'\0';
	}
	return TRUE;
}

int GetMenuStringW(HMENU hMenu, UINT item, wchar_t* out, int cchMax, UINT flags)
{
	if (!out || cchMax <= 0)
		return 0;
	const NppMenuItem* menuItem = resolveMenuItem(hMenu, item, flags);
	if (!menuItem)
		return 0;
	std::wcsncpy(out, menuItem->text.c_str(), static_cast<size_t>(cchMax));
	out[cchMax - 1] = L'\0';
	return static_cast<int>(std::wcslen(out));
}

BOOL EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, UINT uEnable)
{
	NppMenuItem* menuItem = (uEnable & MF_BYPOSITION)
		? findItemByPosition(hMenu, uIDEnableItem)
		: findItemByCommand(hMenu, uIDEnableItem);
	if (!menuItem)
		return FALSE;

	menuItem->flags &= ~(MF_DISABLED | MF_GRAYED);
	if ((uEnable & MF_DISABLED) || (uEnable & MF_GRAYED))
		menuItem->flags |= (uEnable & (MF_DISABLED | MF_GRAYED));
	refreshMenuTree(hMenu);
	return TRUE;
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

	if (auto* main = qobject_cast<QMainWindow*>(hwnd)) {
		const MainWindowChromeInsets insets = mainWindowChromeInsets(main);
		lpRect->bottom -= insets.top + insets.bottom;
	}
	return TRUE;
}

BOOL MoveWindow(HWND hwnd, int x, int y, int w, int h, BOOL /*bRepaint*/)
{
	if (!hwnd)
		return FALSE;
	applyClientGeometryToParent(hwnd, x, y, w, h);
	return TRUE;
}

BOOL SetWindowPos(HWND hwnd, HWND /*hwndInsertAfter*/, int x, int y, int cx, int cy, UINT uFlags)
{
	if (!hwnd)
		return FALSE;

	const bool keepPos = (uFlags & SWP_NOMOVE) != 0;
	const bool keepSize = (uFlags & SWP_NOSIZE) != 0;
	QRect geom = hwnd->geometry();
	if (!keepPos) {
		geom.moveTopLeft(QPoint(x, y));
		if (auto* main = qobject_cast<QMainWindow*>(hwnd->parentWidget()))
			geom.moveTop(geom.top() + mainWindowChromeInsets(main).top);
	}
	if (!keepSize)
		geom.setSize(QSize(cx, cy));
	hwnd->setGeometry(geom);
	if (auto* main = qobject_cast<QMainWindow*>(hwnd->parentWidget())) {
		if (QMenuBar* bar = main->menuBar())
			bar->raise();
	}
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
