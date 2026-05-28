// ToolbarShim.h - Qt6 toolbar compatibility shim
#pragma once

#include <QIcon>
#include <QPixmap>
#include <QAction>
#include <QToolBar>
#include <QToolButton>
#include <map>
#include <string>

// Define missing Win32 icons as Qt icons
// These would normally come from resources, but we create them programmatically
class ToolbarIcons {
public:
    static QIcon getIcon(int id) {
        static std::map<int, QIcon> iconCache;
        auto it = iconCache.find(id);
        if (it != iconCache.end()) return it->second;
        
        QPixmap pm(16, 16);
        pm.fill(Qt::transparent);
        
        QPainter p(&pm);
        p.setPen(Qt::white);
        p.setBrush(Qt::darkGray);
        
        switch (id) {
            case IDI_CLOSEDOCUMENT:
                // X icon
                p.drawLine(2, 2, 13, 13);
                p.drawLine(13, 2, 2, 13);
                break;
            case IDI_SAVE:
                // Disk icon  
                p.drawRect(3, 2, 10, 12);
                p.drawLine(5, 5, 10, 5);
                break;
            case IDI_SAVEALL:
                // Multiple disks
                p.drawRect(1, 4, 6, 10);
                p.drawRect(9, 2, 6, 10);
                break;
            case IDI_PRINT:
                // Printer
                p.drawRect(2, 2, 12, 10);
                p.drawRect(4, 4, 8, 4);
                break;
            default:
                // Generic document
                p.setBrush(Qt::lightGray);
                p.drawRect(2, 1, 12, 14);
                break;
        }
        p.end();
        
        QIcon ico(pm);
        iconCache[id] = ico;
        return ico;
    }

    // Helper to get icon by name
    static QIcon getIcon(const char* name) {
        if (!strcmp(name, "close")) return getIcon(1000);
        if (!strcmp(name, "save")) return getIcon(1001);
        if (!strcmp(name, "saveall")) return getIcon(1002);
        if (!strcmp(name, "print")) return getIcon(1003);
        return QIcon();
    }
};

// Toolbar button definition for toolBarIcons array
struct ToolBarButtonUnit {
    int _iDoc;
    int _iBitmap;
    int _idMessage;
    int _iUser;
    int _uMask;
    QString _pszTip;
    
    ToolBarButtonUnit(int doc = 0, int bitmap = -1, int msg = 0, int user = 0, int mask = 0)
        : _iDoc(doc), _iBitmap(bitmap), _idMessage(msg), _iUser(user), _uMask(mask) {}
    
    ToolBarButtonUnit(int doc, int bitmap, int msg, int user, int mask, const char* tip)
        : _iDoc(doc), _iBitmap(bitmap), _idMessage(msg), _iUser(user), _uMask(mask), _pszTip(tip) {}
};

// Toolbar button array (global)
extern std::vector<ToolBarButtonUnit> toolBarIcons;

// Initialize toolbar icons
static void initToolbarIcons() {
    if (!toolBarIcons.empty()) return;
    
    toolBarIcons = {
        // IDM_FILE_NEW
        ToolBarButtonUnit(37, 0, 1001, 0, 0),
        // IDM_FILE_OPEN
        ToolBarButtonUnit(38, 1, 1002, 0, 0),
        // IDM_FILE_OPENINFOLDER
        ToolBarButtonUnit(38, 1, 1002, 0, 0),
        // IDM_FILE_CLOSE
        ToolBarButtonUnit(39, 3, 1003, 0, 0),
        // IDM_FILE_CLOSEALL
        ToolBarButtonUnit(IDCLOSEALL, 40, 1004, 0, 0),
        // IDM_FILE_SAVE  
        ToolBarButtonUnit(41, 20, 1005, 0, 0),
        // IDM_FILE_SAVEAS
        ToolBarButtonUnit(42, 21, 1006, 0, 0),
        // IDM_FILE_SAVEALL
        ToolBarButtonUnit(43, 22, 1007, 0, 0),
        // IDM_EDIT_UNDO
        ToolBarButtonUnit(44, 4, 1008, 0, 0),
        // IDM_EDIT_REDO
        ToolBarButtonUnit(45, 5, 1009, 0, 0),
        // IDM_EDIT_CUT
        ToolBarButtonUnit(46, 6, 1010, 0, 0),
        // IDM_EDIT_COPY
        ToolBarButtonUnit(47, 7, 1011, 0, 0),
        // IDM_EDIT_PASTE
        ToolBarButtonUnit(48, 8, 1012, 0, 0),
        // IDM_EDIT_DELETE
        ToolBarButtonUnit(49, 9, 1013, 0, 0),
        // IDM_EDIT_SELECTALL
        ToolBarButtonUnit(50, 10, 1014, 0, 0),
        // IDM_SEARCH_FIND
        ToolBarButtonUnit(51, 11, 1015, 0, 0),
        // IDM_SEARCH_REPLACE
        ToolBarButtonUnit(52, 12, 1016, 0, 0),
        // IDM_SEARCH_GOTOLINE  
        ToolBarButtonUnit(53, 30, 1017, 0, 0),
    };
}