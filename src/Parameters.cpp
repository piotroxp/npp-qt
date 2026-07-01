// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Parameters.h"

// npp-qt: Win32 removed
#include <QCoreApplication>
#include <cstdio>
#include <cxxabi.h>
#include <execinfo.h>
#include <qlogging.h>
#include <sys/stat.h>
#include <fcntl.h>

// Fast file existence check using POSIX access() — avoids QFileInfo slowness on sandboxed filesystems
static bool fileExistsFast(const wchar_t* wpath)
{
    QByteArray pathBa = QString::fromWCharArray(wpath).toLocal8Bit();
    auto start = std::chrono::steady_clock::now();
    bool exists = (::access(pathBa.constData(), F_OK) == 0);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    if (ms > 100)
        fprintf(stderr, "DBG fileExistsFast SLOW: %lldms path='%s'\n", (long long)ms, pathBa.constData());
    return exists;
}

static void qtMsgHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    if (msg.contains("mkpath") || msg.contains("Empty or null")) {
        fprintf(stderr, "QT_WARNING [%s:%d] %s\n", ctx.file ? ctx.file : "?", ctx.line, qUtf8Printable(msg));
        // Print backtrace
        void* buf[32];
        int n = backtrace(buf, 32);
        char** syms = backtrace_symbols(buf, n);
        for (int i = 1; i < n && i < 16; ++i) { // skip this frame
            fprintf(stderr, "  [%d] %s\n", i, syms[i]);
        }
        free(syms);
    }
}
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QElapsedTimer>
#include <QCursor>
#include <QScreen>
#include <QMenu>
#include <QProcessEnvironment>
#include <QSettings>
#include <QMessageBox>
#include <QApplication>
#include <QColor>
#include <QFontDatabase>
#include <QLibrary>
#include <QProcess>
#include <QDateTime>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <exception>
#include <locale>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <Qsci/qsciscintilla.h>
#include "NppSciCompat.h"

#include "ContextMenu.h"
#include "Notepad_plus_Window.h"
#include "Notepad_plus_msgs.h"
#include "NppConstants.h"
#include "NppDarkMode.h"
#include "NppXml.h"

// ToolBar: TODO depends on Notepad_plus

#include "keys.h"
#include "localization.h"
// localizationString: TODO
#include "menuCmdID.h"

#include "shortcut.h"
#include "verifySignedfile.h"

// Stub XML content for generating default shortcuts/context menu files
static const char* SHORTCUT_XML_CONTENT = R"xml(<?xml version="1.0" encoding="UTF-8"?>
<NotepadPlus>
    <Shortcuts>
    </Shortcuts>
</NotepadPlus>)xml";

static const char* CONTEXTMENU_XML_CONTENT = R"xml(<?xml version="1.0" encoding="UTF-8"?>
<NotepadPlus>
    <ContextMenu>
    </ContextMenu>
</NotepadPlus>)xml";

// Stub XML content for default config/stylers/langs when model files are absent
static const char* CONFIG_XML_CONTENT = R"xml(<?xml version="1.0" encoding="UTF-8"?>
<NotepadPlus>
    <Settings>
        <Global fontSize="10" fontFace="Consolas" />
    </Settings>
</NotepadPlus>)xml";

static const char* STYLERS_XML_CONTENT = R"xml(<?xml version="1.0" encoding="UTF-8"?>
<NotepadPlus>
    <LexerStyles>
        <LexerType name="cpp" desc="C++">
        </LexerType>
    </LexerStyles>
</NotepadPlus>)xml";

static const char* LANGS_XML_CONTENT = R"xml(<?xml version="1.0" encoding="UTF-8"?>
<NotepadPlus>
    <Languages>
    </Languages>
</NotepadPlus>)xml";

// Helper: convert QString to std::string safely
static inline std::string qToStd(const QString& s) { return s.toStdString(); }


// MSVC pragma for GetVersionEx removed in Qt6 port

static constexpr const wchar_t localConfFile[] = L"doLocalConf.xml";
static constexpr const wchar_t notepadStyleFile[] = L"asNotepad.xml";

static constexpr int NB_MAX_FINDHISTORY_FIND = 30;
static constexpr int NB_MAX_FINDHISTORY_REPLACE = 30;
static constexpr int NB_MAX_FINDHISTORY_PATH = 30;
static constexpr int NB_MAX_FINDHISTORY_FILTER = 20;

namespace // anonymous namespace
{

struct WinMenuKeyDefinition // more or less matches accelerator table definition, easy copy/paste
{
	int vKey = 0;
	int functionId = 0;
	bool isCtrl = false;
	bool isAlt = false;
	bool isShift = false;
	const wchar_t * specialName = nullptr; // Used when no real menu name exists (in case of toggle for example)
};

/*!
** \brief array of accelerator keys for all std menu items
**
** values can be 0 for vKey, which means its unused
*/
static constexpr WinMenuKeyDefinition winKeyDefs[]
{
	// V_KEY,    COMMAND_ID,                                    Ctrl,  Alt,   Shift, cmdName
	// -------------------------------------------------------------------------------------
	//
	{ VK_N,       IDM_FILE_NEW,                                 true,  false, false, nullptr },
	{ VK_O,       IDM_FILE_OPEN,                                true,  false, false, nullptr },
	{ VK_NULL,    IDM_FILE_OPEN_FOLDER,                         false, false, false, L"Open containing folder in Explorer" },
	{ VK_NULL,    IDM_FILE_OPEN_CMD,                            false, false, false, L"Open containing folder in Command Prompt" },
	{ VK_NULL,    IDM_FILE_OPEN_DEFAULT_VIEWER,                 false, false, false, nullptr },
	{ VK_NULL,    IDM_FILE_OPENFOLDERASWORKSPACE,                false, false, false, nullptr },
	{ VK_R,       IDM_FILE_RELOAD,                              true,  false, false, nullptr },
	{ VK_S,       IDM_FILE_SAVE,                                true,  false, false, nullptr },
	{ VK_S,       IDM_FILE_SAVEAS,                              true,  true,  false, nullptr },
	{ VK_NULL,    IDM_FILE_SAVECOPYAS,                          false, false, false, nullptr },
	{ VK_S,       IDM_FILE_SAVEALL,                             true,  false, true,  nullptr },
	{ VK_NULL,    IDM_FILE_RENAME,                              false, false, false, nullptr },
	{ VK_W,       IDM_FILE_CLOSE,                               true,  false, false, nullptr },
	{ VK_W,       IDM_FILE_CLOSEALL,                            true,  false, true,  nullptr },
	{ VK_NULL,    IDM_FILE_CLOSEALL_BUT_CURRENT,                false, false, false, nullptr },
	{ VK_NULL,    IDM_FILE_CLOSEALL_BUT_PINNED,                 false, false, false, nullptr },
	{ VK_NULL,    IDM_FILE_CLOSEALL_TOLEFT,                     false, false, false, nullptr },
	{ VK_NULL,    IDM_FILE_CLOSEALL_TORIGHT,                    false, false, false, nullptr },
	{ VK_NULL,    IDM_FILE_CLOSEALL_UNCHANGED,                  false, false, false, nullptr },
	{ VK_NULL,    IDM_FILE_DELETE,                              false, false, false, nullptr },
	{ VK_NULL,    IDM_FILE_LOADSESSION,                         false, false, false, nullptr },
	{ VK_NULL,    IDM_FILE_SAVESESSION,                         false, false, false, nullptr },
	{ VK_P,       IDM_FILE_PRINT,                               true,  false, false, nullptr },
	{ VK_NULL,    IDM_FILE_PRINTNOW,                            false, false, false, nullptr },
	{ VK_T,       IDM_FILE_RESTORELASTCLOSEDFILE,               true,  false, true,  L"Restore Recent Closed File" },
	{ VK_F4,      IDM_FILE_EXIT,                                false, true,  false, nullptr },

//	{ VK_NULL,    IDM_EDIT_UNDO,                                false, false, false, nullptr },
//	{ VK_NULL,    IDM_EDIT_REDO,                                false, false, false, nullptr },

	{ VK_DELETE,  IDM_EDIT_CUT,                                 false, false, true,  nullptr },
	{ VK_X,       IDM_EDIT_CUT,                                 true,  false, false, nullptr },

	{ VK_INSERT,  IDM_EDIT_COPY,                                true,  false, false, nullptr },
	{ VK_C,       IDM_EDIT_COPY,                                true,  false, false, nullptr },

	{ VK_INSERT,  IDM_EDIT_PASTE,                               false, false, true,  nullptr },
	{ VK_V,       IDM_EDIT_PASTE,                               true,  false, false, nullptr },

//	{ VK_NULL,    IDM_EDIT_DELETE,                              false, false, false, nullptr },
//	{ VK_NULL,    IDM_EDIT_SELECTALL,                           false, false, false, nullptr },
	{ VK_B,       IDM_EDIT_BEGINENDSELECT,                      true,  false, true,  nullptr },
	{ VK_B,       IDM_EDIT_BEGINENDSELECT_COLUMNMODE,           false, true,  true,  nullptr },

	{ VK_NULL,    IDM_EDIT_FULLPATHTOCLIP,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_FILENAMETOCLIP,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_CURRENTDIRTOCLIP,                    false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_COPY_ALL_NAMES,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_COPY_ALL_PATHS,                      false, false, false, nullptr },

	{ VK_NULL,    IDM_EDIT_INS_TAB,                             false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_RMV_TAB,                             false, false, false, nullptr },
	{ VK_U,       IDM_EDIT_UPPERCASE,                           true,  false, true,  nullptr },
	{ VK_U,       IDM_EDIT_LOWERCASE,                           true,  false, false, nullptr },
	{ VK_U,       IDM_EDIT_PROPERCASE_FORCE,                    false, true,  false, nullptr },
	{ VK_U,       IDM_EDIT_PROPERCASE_BLEND,                    false, true,  true,  nullptr },
	{ VK_U,       IDM_EDIT_SENTENCECASE_FORCE,                  true,  true,  false, nullptr },
	{ VK_U,       IDM_EDIT_SENTENCECASE_BLEND,                  true,  true,  true,  nullptr },
	{ VK_NULL,    IDM_EDIT_INVERTCASE,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_RANDOMCASE,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_REMOVE_CONSECUTIVE_DUP_LINES,        false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_REMOVE_ANY_DUP_LINES,                false, false, false, nullptr },
	{ VK_I,       IDM_EDIT_SPLIT_LINES,                         true,  false, false, nullptr },
	{ VK_J,       IDM_EDIT_JOIN_LINES,                          true,  false, false, nullptr },
	{ VK_UP,      IDM_EDIT_LINE_UP,                             true,  false, true,  nullptr },
	{ VK_DOWN,    IDM_EDIT_LINE_DOWN,                           true,  false, true,  nullptr },
	{ VK_NULL,    IDM_EDIT_REMOVEEMPTYLINES,                    false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_REMOVEEMPTYLINESWITHBLANK,           false, false, false, nullptr },
	{ VK_RETURN,  IDM_EDIT_BLANKLINEABOVECURRENT,               true,  true,  false, nullptr },
	{ VK_RETURN,  IDM_EDIT_BLANKLINEBELOWCURRENT,               true,  true,  true,  nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_LENGTH_ASCENDING,          false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_LENGTH_DESCENDING,         false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_LEXICOGRAPHIC_ASCENDING,   false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_LEXICOGRAPHIC_DESCENDING,  false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_LEXICO_CASE_INSENS_ASCENDING,   false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_LEXICO_CASE_INSENS_DESCENDING,  false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_LOCALE_ASCENDING,          false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_LOCALE_DESCENDING,         false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_INTEGER_ASCENDING,         false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_INTEGER_DESCENDING,        false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_DECIMALCOMMA_ASCENDING,    false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_DECIMALCOMMA_DESCENDING,   false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_DECIMALDOT_ASCENDING,      false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_DECIMALDOT_DESCENDING,     false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_REVERSE_ORDER,             false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SORTLINES_RANDOMLY,                  false, false, false, nullptr },
	{ VK_Q,       IDM_EDIT_BLOCK_COMMENT,                       true,  false, false, nullptr },
	{ VK_K,       IDM_EDIT_BLOCK_COMMENT_SET,                   true,  false, false, nullptr },
	{ VK_K,       IDM_EDIT_BLOCK_UNCOMMENT,                     true,  false, true,  nullptr },
	{ VK_Q,       IDM_EDIT_STREAM_COMMENT,                      true,  false, true,  nullptr },
	{ VK_NULL,    IDM_EDIT_STREAM_UNCOMMENT,                    false, false, false, nullptr },
	{ VK_SPACE,   IDM_EDIT_AUTOCOMPLETE,                        true,  false, false, nullptr },
	{ VK_SPACE,   IDM_EDIT_AUTOCOMPLETE_PATH,                   true,  true,  false, nullptr },
	{ VK_RETURN,  IDM_EDIT_AUTOCOMPLETE_CURRENTFILE,            true,  false, false, nullptr },
	{ VK_SPACE,   IDM_EDIT_FUNCCALLTIP,                         true,  false, true,  nullptr },
	{ VK_UP,      IDM_EDIT_FUNCCALLTIP_PREVIOUS,                false, true,  false, nullptr },
	{ VK_DOWN,    IDM_EDIT_FUNCCALLTIP_NEXT,                    false, true,  false, nullptr },
	{ VK_NULL,    IDM_EDIT_INSERT_DATETIME_SHORT,               false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_INSERT_DATETIME_LONG,                false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_INSERT_DATETIME_CUSTOMIZED,          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_TODOS,                             false, false, false, L"EOL Conversion to Windows (CR LF)" },
	{ VK_NULL,    IDM_FORMAT_TOUNIX,                            false, false, false, L"EOL Conversion to Unix (LF)" },
	{ VK_NULL,    IDM_FORMAT_TOMAC,                             false, false, false, L"EOL Conversion to Macintosh (CR)" },
	{ VK_NULL,    IDM_EDIT_TRIMTRAILING,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_TRIMLINEHEAD,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_TRIM_BOTH,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_EOL2WS,                              false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_TRIMALL,                             false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_TAB2SW,                              false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SW2TAB_ALL,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SW2TAB_LEADING,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_PASTE_AS_HTML,                       false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_PASTE_AS_RTF,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_COPY_BINARY,                         false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_CUT_BINARY,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_PASTE_BINARY,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_OPENASFILE,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_OPENINFOLDER,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SEARCHONINTERNET,                    false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_CHANGESEARCHENGINE,                  false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_MULTISELECTALL,                      false, false, false, L"Multi-select All: Ignore Case and Whole Word" },
	{ VK_NULL,    IDM_EDIT_MULTISELECTALLMATCHCASE,             false, false, false, L"Multi-select All: Match Case Only" },
	{ VK_NULL,    IDM_EDIT_MULTISELECTALLWHOLEWORD,             false, false, false, L"Multi-select All: Match Whole Word Only" },
	{ VK_NULL,    IDM_EDIT_MULTISELECTALLMATCHCASEWHOLEWORD,    false, false, false, L"Multi-select All: Match Case and Whole Word" },
	{ VK_NULL,    IDM_EDIT_MULTISELECTNEXT,                     false, false, false, L"Multi-select Next: Ignore Case and Whole Word" },
	{ VK_NULL,    IDM_EDIT_MULTISELECTNEXTMATCHCASE,            false, false, false, L"Multi-select Next: Match Case Only" },
	{ VK_NULL,    IDM_EDIT_MULTISELECTNEXTWHOLEWORD,            false, false, false, L"Multi-select Next: Match Whole Word Only" },
	{ VK_NULL,    IDM_EDIT_MULTISELECTNEXTMATCHCASEWHOLEWORD,   false, false, false, L"Multi-select Next: Match Case and Whole Word" },
	{ VK_NULL,    IDM_EDIT_MULTISELECTUNDO,                     false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_MULTISELECTSSKIP,                    false, false, false, nullptr },
//  { VK_NULL,    IDM_EDIT_COLUMNMODETIP,                       false, false, false, nullptr },
	{ VK_C,       IDM_EDIT_COLUMNMODE,                          false, true,  false, nullptr },
	{ VK_NULL,    IDM_EDIT_CHAR_PANEL,                          false, false, false, L"Toggle Character Panel" },
	{ VK_NULL,    IDM_EDIT_CLIPBOARDHISTORY_PANEL,              false, false, false, L"Toggle Clipboard History" },
	{ VK_NULL,    IDM_EDIT_TOGGLEREADONLY,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_SETREADONLYFORALLDOCS,               false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_CLEARREADONLYFORALLDOCS,             false, false, false, nullptr },
	{ VK_NULL,    IDM_EDIT_TOGGLESYSTEMREADONLY,                false, false, false, nullptr },
	{ VK_F,       IDM_SEARCH_FIND,                              true,  false, false, nullptr },
	{ VK_F,       IDM_SEARCH_FINDINFILES,                       true,  false, true,  nullptr },
	{ VK_F3,      IDM_SEARCH_FINDNEXT,                          false, false, false, nullptr },
	{ VK_F3,      IDM_SEARCH_FINDPREV,                          false, false, true,  nullptr },
	{ VK_F3,      IDM_SEARCH_SETANDFINDNEXT,                    true,  false, false, nullptr },
	{ VK_F3,      IDM_SEARCH_SETANDFINDPREV,                    true,  false, true,  nullptr },
	{ VK_F3,      IDM_SEARCH_VOLATILE_FINDNEXT,                 true,  true,  false, nullptr },
	{ VK_F3,      IDM_SEARCH_VOLATILE_FINDPREV,                 true,  true,  true,  nullptr },
	{ VK_H,       IDM_SEARCH_REPLACE,                           true,  false, false, nullptr },
	{ VK_I,       IDM_SEARCH_FINDINCREMENT,                     true,  true,  false, nullptr },
	{ VK_F7,      IDM_FOCUS_ON_FOUND_RESULTS,                   false, false, false, nullptr },
	{ VK_F4,      IDM_SEARCH_GOTOPREVFOUND,                     false, false, true,  nullptr },
	{ VK_F4,      IDM_SEARCH_GOTONEXTFOUND,                     false, false, false, nullptr },
	{ VK_G,       IDM_SEARCH_GOTOLINE,                          true,  false, false, nullptr },
	{ VK_B,       IDM_SEARCH_GOTOMATCHINGBRACE,                 true,  false, false, nullptr },
	{ VK_B,       IDM_SEARCH_SELECTMATCHINGBRACES,              true,  true,  false, nullptr },
	{ VK_NULL,    IDM_SEARCH_CHANGED_PREV,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_CHANGED_NEXT,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_CLEAR_CHANGE_HISTORY,              false, false, false, nullptr },
	{ VK_M,       IDM_SEARCH_MARK,                              true,  false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_MARKALLEXT1,                       false, false, false, L"Style all using 1st style" },
	{ VK_NULL,    IDM_SEARCH_MARKALLEXT2,                       false, false, false, L"Style all using 2nd style" },
	{ VK_NULL,    IDM_SEARCH_MARKALLEXT3,                       false, false, false, L"Style all using 3rd style" },
	{ VK_NULL,    IDM_SEARCH_MARKALLEXT4,                       false, false, false, L"Style all using 4th style" },
	{ VK_NULL,    IDM_SEARCH_MARKALLEXT5,                       false, false, false, L"Style all using 5th style" },
	{ VK_NULL,    IDM_SEARCH_MARKONEEXT1,                       false, false, false, L"Style one using 1st style" },
	{ VK_NULL,    IDM_SEARCH_MARKONEEXT2,                       false, false, false, L"Style one using 2nd style" },
	{ VK_NULL,    IDM_SEARCH_MARKONEEXT3,                       false, false, false, L"Style one using 3rd style" },
	{ VK_NULL,    IDM_SEARCH_MARKONEEXT4,                       false, false, false, L"Style one using 4th style" },
	{ VK_NULL,    IDM_SEARCH_MARKONEEXT5,                       false, false, false, L"Style one using 5th style" },
	{ VK_NULL,    IDM_SEARCH_UNMARKALLEXT1,                     false, false, false, L"Clear 1st style" },
	{ VK_NULL,    IDM_SEARCH_UNMARKALLEXT2,                     false, false, false, L"Clear 2nd style" },
	{ VK_NULL,    IDM_SEARCH_UNMARKALLEXT3,                     false, false, false, L"Clear 3rd style" },
	{ VK_NULL,    IDM_SEARCH_UNMARKALLEXT4,                     false, false, false, L"Clear 4th style" },
	{ VK_NULL,    IDM_SEARCH_UNMARKALLEXT5,                     false, false, false, L"Clear 5th style" },
	{ VK_NULL,    IDM_SEARCH_CLEARALLMARKS,                     false, false, false, L"Clear all styles" },
	{ VK_1,       IDM_SEARCH_GOPREVMARKER1,                     true,  false, true,  L"Previous style of 1st style" },
	{ VK_2,       IDM_SEARCH_GOPREVMARKER2,                     true,  false, true,  L"Previous style of 2nd style" },
	{ VK_3,       IDM_SEARCH_GOPREVMARKER3,                     true,  false, true,  L"Previous style of 3rd style" },
	{ VK_4,       IDM_SEARCH_GOPREVMARKER4,                     true,  false, true,  L"Previous style of 4th style" },
	{ VK_5,       IDM_SEARCH_GOPREVMARKER5,                     true,  false, true,  L"Previous style of 5th style" },
	{ VK_0,       IDM_SEARCH_GOPREVMARKER_DEF,                  true,  false, true,  L"Previous style of Find Mark style" },
	{ VK_1,       IDM_SEARCH_GONEXTMARKER1,                     true,  false, false, L"Next style of 1st style" },
	{ VK_2,       IDM_SEARCH_GONEXTMARKER2,                     true,  false, false, L"Next style of 2nd style" },
	{ VK_3,       IDM_SEARCH_GONEXTMARKER3,                     true,  false, false, L"Next style of 3rd style" },
	{ VK_4,       IDM_SEARCH_GONEXTMARKER4,                     true,  false, false, L"Next style of 4th style" },
	{ VK_5,       IDM_SEARCH_GONEXTMARKER5,                     true,  false, false, L"Next style of 5th style" },
	{ VK_0,       IDM_SEARCH_GONEXTMARKER_DEF,                  true,  false, false, L"Next style of Find Mark style" },
	{ VK_NULL,    IDM_SEARCH_STYLE1TOCLIP,                      false, false, false, L"Copy Styled Text of 1st Style" },
	{ VK_NULL,    IDM_SEARCH_STYLE2TOCLIP,                      false, false, false, L"Copy Styled Text of 2nd Style" },
	{ VK_NULL,    IDM_SEARCH_STYLE3TOCLIP,                      false, false, false, L"Copy Styled Text of 3rd Style" },
	{ VK_NULL,    IDM_SEARCH_STYLE4TOCLIP,                      false, false, false, L"Copy Styled Text of 4th Style" },
	{ VK_NULL,    IDM_SEARCH_STYLE5TOCLIP,                      false, false, false, L"Copy Styled Text of 5th Style" },
	{ VK_NULL,    IDM_SEARCH_ALLSTYLESTOCLIP,                   false, false, false, L"Copy Styled Text of All Styles" },
	{ VK_NULL,    IDM_SEARCH_MARKEDTOCLIP,                      false, false, false, L"Copy Styled Text of Find Mark style" },
	{ VK_F2,      IDM_SEARCH_TOGGLE_BOOKMARK,                   true,  false, false, nullptr },
	{ VK_F2,      IDM_SEARCH_NEXT_BOOKMARK,                     false, false, false, nullptr },
	{ VK_F2,      IDM_SEARCH_PREV_BOOKMARK,                     false, false, true, nullptr  },
	{ VK_NULL,    IDM_SEARCH_CLEAR_BOOKMARKS,                   false, false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_CUTMARKEDLINES,                    false, false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_COPYMARKEDLINES,                   false, false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_PASTEMARKEDLINES,                  false, false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_DELETEMARKEDLINES,                 false, false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_DELETEUNMARKEDLINES,               false, false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_INVERSEMARKS,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_SEARCH_FINDCHARINRANGE,                   false, false, false, nullptr },

	{ VK_NULL,    IDM_VIEW_ALWAYSONTOP,                         false, false, false, nullptr },
	{ VK_F11,     IDM_VIEW_FULLSCREENTOGGLE,                    false, false, false, nullptr },
	{ VK_F12,     IDM_VIEW_POSTIT,                              false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_DISTRACTIONFREE,                     false, false, false, nullptr },

	{ VK_NULL,    IDM_VIEW_IN_FIREFOX,                          false, false, false, L"View current file in Firefox" },
	{ VK_NULL,    IDM_VIEW_IN_CHROME,                           false, false, false, L"View current file in Chrome" },
	{ VK_NULL,    IDM_VIEW_IN_IE,                               false, false, false, L"View current file in IE" },
	{ VK_NULL,    IDM_VIEW_IN_EDGE,                             false, false, false, L"View current file in Edge" },

	{ VK_NULL,    IDM_VIEW_TAB_SPACE,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_EOL,                                 false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_ALL_CHARACTERS,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_NPC,                                 false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_NPC_CCUNIEOL,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_INDENT_GUIDE,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_WRAP_SYMBOL,                         false, false, false, nullptr },
//  { VK_NULL,    IDM_VIEW_ZOOMIN,                              false, false, false, nullptr },
//  { VK_NULL,    IDM_VIEW_ZOOMOUT,                             false, false, false, nullptr },
//  { VK_NULL,    IDM_VIEW_ZOOMRESTORE,                         false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_GOTO_START,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_GOTO_END,                            false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_GOTO_ANOTHER_VIEW,                   false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_CLONE_TO_ANOTHER_VIEW,               false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_GOTO_NEW_INSTANCE,                   false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_LOAD_IN_NEW_INSTANCE,                false, false, false, nullptr },

	{ VK_NUMPAD1, IDM_VIEW_TAB1,                                true,  false, false, nullptr },
	{ VK_NUMPAD2, IDM_VIEW_TAB2,                                true,  false, false, nullptr },
	{ VK_NUMPAD3, IDM_VIEW_TAB3,                                true,  false, false, nullptr },
	{ VK_NUMPAD4, IDM_VIEW_TAB4,                                true,  false, false, nullptr },
	{ VK_NUMPAD5, IDM_VIEW_TAB5,                                true,  false, false, nullptr },
	{ VK_NUMPAD6, IDM_VIEW_TAB6,                                true,  false, false, nullptr },
	{ VK_NUMPAD7, IDM_VIEW_TAB7,                                true,  false, false, nullptr },
	{ VK_NUMPAD8, IDM_VIEW_TAB8,                                true,  false, false, nullptr },
	{ VK_NUMPAD9, IDM_VIEW_TAB9,                                true,  false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_TAB_START,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_TAB_END,                             false, false, false, nullptr },
	{ VK_NEXT,    IDM_VIEW_TAB_NEXT,                            true,  false, false, nullptr },
	{ VK_PRIOR,   IDM_VIEW_TAB_PREV,                            true,  false, false, nullptr },
	{ VK_NEXT,    IDM_VIEW_TAB_MOVEFORWARD,                     true,  false, true,  nullptr },
	{ VK_PRIOR,   IDM_VIEW_TAB_MOVEBACKWARD,                    true,  false, true,  nullptr },
	{ VK_TAB,     IDC_PREV_DOC,                                 true,  false, true,  L"Switch to previous document" },
	{ VK_TAB,     IDC_NEXT_DOC,                                 true,  false, false, L"Switch to next document" },
	{ VK_NULL,    IDM_VIEW_WRAP,                                false, false, false, nullptr },
	{ VK_H,       IDM_VIEW_HIDELINES,                           false, true,  false, nullptr },
	{ VK_F8,      IDM_VIEW_SWITCHTO_OTHER_VIEW,                 false, false, false, nullptr },

	{ VK_0,       IDM_VIEW_FOLDALL,                             false, true,  false, nullptr },
	{ VK_0,       IDM_VIEW_UNFOLDALL,                           false, true,  true,  nullptr },
	{ VK_F,       IDM_VIEW_FOLD_CURRENT,                        true,  true,  false, nullptr },
	{ VK_F,       IDM_VIEW_UNFOLD_CURRENT,                      true,  true,  true,  nullptr },
	{ VK_1,       IDM_VIEW_FOLD_1,                              false, true,  false, L"Fold Level 1" },
	{ VK_2,       IDM_VIEW_FOLD_2,                              false, true,  false, L"Fold Level 2" },
	{ VK_3,       IDM_VIEW_FOLD_3,                              false, true,  false, L"Fold Level 3" },
	{ VK_4,       IDM_VIEW_FOLD_4,                              false, true,  false, L"Fold Level 4" },
	{ VK_5,       IDM_VIEW_FOLD_5,                              false, true,  false, L"Fold Level 5" },
	{ VK_6,       IDM_VIEW_FOLD_6,                              false, true,  false, L"Fold Level 6" },
	{ VK_7,       IDM_VIEW_FOLD_7,                              false, true,  false, L"Fold Level 7" },
	{ VK_8,       IDM_VIEW_FOLD_8,                              false, true,  false, L"Fold Level 8" },

	{ VK_1,       IDM_VIEW_UNFOLD_1,                            false, true,  true,  L"Unfold Level 1" },
	{ VK_2,       IDM_VIEW_UNFOLD_2,                            false, true,  true,  L"Unfold Level 2" },
	{ VK_3,       IDM_VIEW_UNFOLD_3,                            false, true,  true,  L"Unfold Level 3" },
	{ VK_4,       IDM_VIEW_UNFOLD_4,                            false, true,  true,  L"Unfold Level 4" },
	{ VK_5,       IDM_VIEW_UNFOLD_5,                            false, true,  true,  L"Unfold Level 5" },
	{ VK_6,       IDM_VIEW_UNFOLD_6,                            false, true,  true,  L"Unfold Level 6" },
	{ VK_7,       IDM_VIEW_UNFOLD_7,                            false, true,  true,  L"Unfold Level 7" },
	{ VK_8,       IDM_VIEW_UNFOLD_8,                            false, true,  true,  L"Unfold Level 8" },
	{ VK_NULL,    IDM_VIEW_SUMMARY,                             false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_PROJECT_PANEL_1,                     false, false, false, L"Toggle Project Panel 1" },
	{ VK_NULL,    IDM_VIEW_PROJECT_PANEL_2,                     false, false, false, L"Toggle Project Panel 2" },
	{ VK_NULL,    IDM_VIEW_PROJECT_PANEL_3,                     false, false, false, L"Toggle Project Panel 3" },
	{ VK_NULL,    IDM_VIEW_FILEBROWSER,                         false, false, false, L"Toggle Folder as Workspace" },
	{ VK_NULL,    IDM_VIEW_DOC_MAP,                             false, false, false, L"Toggle Document Map" },
	{ VK_NULL,    IDM_VIEW_DOCLIST,                             false, false, false, L"Toggle Document List" },
	{ VK_NULL,    IDM_VIEW_FUNC_LIST,                           false, false, false, L"Toggle Function List" },
	{ VK_NULL,    IDM_VIEW_SWITCHTO_PROJECT_PANEL_1,            false, false, false, L"Switch to Project Panel 1" },
	{ VK_NULL,    IDM_VIEW_SWITCHTO_PROJECT_PANEL_2,            false, false, false, L"Switch to Project Panel 2" },
	{ VK_NULL,    IDM_VIEW_SWITCHTO_PROJECT_PANEL_3,            false, false, false, L"Switch to Project Panel 3" },
	{ VK_NULL,    IDM_VIEW_SWITCHTO_FILEBROWSER,                false, false, false, L"Switch to Folder as Workspace" },
	{ VK_NULL,    IDM_VIEW_SWITCHTO_FUNC_LIST,                  false, false, false, L"Switch to Function List" },
	{ VK_NULL,    IDM_VIEW_SWITCHTO_DOCLIST,                    false, false, false, L"Switch to Document List" },
	{ VK_NULL,    IDM_VIEW_TAB_COLOUR_NONE,                     false, false, false, L"Remove Tab Colour" },
	{ VK_NULL,    IDM_VIEW_TAB_COLOUR_1,                        false, false, false, L"Apply Tab Colour 1" },
	{ VK_NULL,    IDM_VIEW_TAB_COLOUR_2,                        false, false, false, L"Apply Tab Colour 2" },
	{ VK_NULL,    IDM_VIEW_TAB_COLOUR_3,                        false, false, false, L"Apply Tab Colour 3" },
	{ VK_NULL,    IDM_VIEW_TAB_COLOUR_4,                        false, false, false, L"Apply Tab Colour 4" },
	{ VK_NULL,    IDM_VIEW_TAB_COLOUR_5,                        false, false, false, L"Apply Tab Colour 5" },
	{ VK_NULL,    IDM_VIEW_SYNSCROLLV,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_VIEW_SYNSCROLLH,                          false, false, false, nullptr },
	{ VK_R,       IDM_EDIT_RTL,                                 true,  true,  false, nullptr },
	{ VK_L,       IDM_EDIT_LTR,                                 true,  true,  false, nullptr },
	{ VK_NULL,    IDM_VIEW_MONITORING,                          false, false, false, nullptr },

	{ VK_NULL,    IDM_FORMAT_ANSI,                              false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_AS_UTF_8,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_UTF_8,                             false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_UTF_16BE,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_UTF_16LE,                          false, false, false, nullptr },

	{ VK_NULL,    IDM_FORMAT_ISO_8859_6,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_WIN_1256,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_13,                       false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_WIN_1257,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_14,                       false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_5,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_MAC_CYRILLIC,                      false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_KOI8R_CYRILLIC,                    false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_KOI8U_CYRILLIC,                    false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_WIN_1251,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_WIN_1250,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_437,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_720,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_737,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_775,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_850,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_852,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_855,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_857,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_858,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_860,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_861,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_862,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_863,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_865,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_866,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_DOS_869,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_BIG5,                              false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_GB2312,                            false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_2,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_7,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_WIN_1253,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_8,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_WIN_1255,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_SHIFT_JIS,                         false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_EUC_KR,                            false, false, false, nullptr },
	//{ VK_NULL,    IDM_FORMAT_ISO_8859_10,                       false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_15,                       false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_4,                        false, false, false, nullptr },
	//{ VK_NULL,    IDM_FORMAT_ISO_8859_16,                       false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_3,                        false, false, false, nullptr },
	//{ VK_NULL,    IDM_FORMAT_ISO_8859_11,                       false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_TIS_620,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_9,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_WIN_1254,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_WIN_1252,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_ISO_8859_1,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_WIN_1258,                          false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_CONV2_ANSI,                        false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_CONV2_AS_UTF_8,                    false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_CONV2_UTF_8,                       false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_CONV2_UTF_16BE,                    false, false, false, nullptr },
	{ VK_NULL,    IDM_FORMAT_CONV2_UTF_16LE,                    false, false, false, nullptr },

	{ VK_NULL,    IDM_LANG_USER_DLG,                            false, false, false, nullptr },
	{ VK_NULL,    IDM_LANG_USER,                                false, false, false, nullptr },
	{ VK_NULL,    IDM_LANG_OPENUDLDIR,                          false, false, false, nullptr },

	{ VK_NULL,    IDM_SETTING_PREFERENCE,                       false, false, false, nullptr },
	{ VK_NULL,    IDM_LANGSTYLE_CONFIG_DLG,                     false, false, false, nullptr },
	{ VK_NULL,    IDM_SETTING_SHORTCUT_MAPPER,                  false, false, false, nullptr },
	{ VK_NULL,    IDM_SETTING_IMPORTPLUGIN,                     false, false, false, nullptr },
	{ VK_NULL,    IDM_SETTING_IMPORTSTYLETHEMES,                false, false, false, nullptr },
	{ VK_NULL,    IDM_SETTING_EDITCONTEXTMENU,                  false, false, false, nullptr },

	{ VK_R,       IDC_EDIT_TOGGLEMACRORECORDING,                true,  false, true,  L"Toggle macro recording" },
	{ VK_NULL,    IDM_MACRO_STARTRECORDINGMACRO,                false, false, false, nullptr },
	{ VK_NULL,    IDM_MACRO_STOPRECORDINGMACRO,                 false, false, false, nullptr },
	{ VK_P,       IDM_MACRO_PLAYBACKRECORDEDMACRO,              true,  false, true,  nullptr },
	{ VK_NULL,    IDM_MACRO_SAVECURRENTMACRO,                   false, false, false, nullptr },
	{ VK_NULL,    IDM_MACRO_RUNMULTIMACRODLG,                   false, false, false, nullptr },

	{ VK_F5,      IDM_EXECUTE,                                  false, false, false, nullptr },

	{ VK_NULL,    IDM_WINDOW_WINDOWS,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_WINDOW_SORT_FN_ASC,                       false, false, false, L"Sort by Name A to Z" },
	{ VK_NULL,    IDM_WINDOW_SORT_FN_DSC,                       false, false, false, L"Sort by Name Z to A" },
	{ VK_NULL,    IDM_WINDOW_SORT_FP_ASC,                       false, false, false, L"Sort by Path A to Z" },
	{ VK_NULL,    IDM_WINDOW_SORT_FP_DSC,                       false, false, false, L"Sort by Path Z to A" },
	{ VK_NULL,    IDM_WINDOW_SORT_FT_ASC,                       false, false, false, L"Sort by Type A to Z" },
	{ VK_NULL,    IDM_WINDOW_SORT_FT_DSC,                       false, false, false, L"Sort by Type Z to A" },
	{ VK_NULL,    IDM_WINDOW_SORT_FS_ASC,                       false, false, false, L"Sort by Content Length Ascending" },
	{ VK_NULL,    IDM_WINDOW_SORT_FS_DSC,                       false, false, false, L"Sort by Content Length Descending" },

	{ VK_NULL,    IDM_CMDLINEARGUMENTS,                         false, false, false, nullptr },
	{ VK_NULL,    IDM_HOMESWEETHOME,                            false, false, false, nullptr },
	{ VK_NULL,    IDM_PROJECTPAGE,                              false, false, false, nullptr },
	{ VK_NULL,    IDM_ONLINEDOCUMENT,                           false, false, false, nullptr },
	{ VK_NULL,    IDM_FORUM,                                    false, false, false, nullptr },
//	{ VK_NULL,    IDM_ONLINESUPPORT,                            false, false, false, nullptr },
//	{ VK_NULL,    IDM_PLUGINSHOME,                              false, false, false, nullptr },

	// The following two commands are not in menu if (nppGUI._doesExistUpdater == 0).
	// They cannot be derived from menu then, only for this reason the text is specified here.
	// In localized environments, the text comes preferably from xml Menu/Main/Commands.
	{ VK_NULL,    IDM_UPDATE_NPP,                               false, false, false, L"Update Notepad++" },
	{ VK_NULL,    IDM_CONFUPDATERPROXY,                         false, false, false, L"Set Updater Proxy..." },
	{ VK_NULL,    IDM_DEBUGINFO,                                false, false, false, nullptr },
	{ VK_F1,      IDM_ABOUT,                                    false, false, false, nullptr }
};

struct ScintillaKeyDefinition
{
	const wchar_t* name = nullptr;
	int functionId = 0;
	bool isCtrl = false;
	bool isAlt = false;
	bool isShift = false;
	int vKey = 0;
	int redirFunctionId = 0; // this gets set when a function is being redirected through Notepad++ if Scintilla doesn't do it properly :)
};

/*!
** \brief array of accelerator keys for all possible scintilla functions
**
** values can be 0 for vKey, which means its unused
*/
static constexpr ScintillaKeyDefinition scintKeyDefs[]
{
	//Scintilla command name,             SCINTILLA_CMD_ID,            Ctrl,  Alt,   Shift, V_KEY,       NOTEPAD++_CMD_ID
	// -------------------------------------------------------------------------------------------------------------------
	//
	//{L"SCI_CUT",                     SCI_CUT,                     true,  false, false, VK_X,        IDM_EDIT_CUT},
	//{L"",                            SCI_CUT,                     false, false, true,  VK_DELETE,   0},
	//{L"SCI_COPY",                    SCI_COPY,                    true,  false, false, VK_C,        IDM_EDIT_COPY},
	//{L"",                            SCI_COPY,                    true,  false, false, VK_INSERT,   0},
	//{L"SCI_PASTE",                   SCI_PASTE,                   true,  false, false, VK_V,        IDM_EDIT_PASTE},
	//{L"SCI_PASTE",                   SCI_PASTE,                   false, false, true,  VK_INSERT,   IDM_EDIT_PASTE},
	{L"SCI_SELECTALL",               SCI_SELECTALL,               true,  false, false, VK_A,        IDM_EDIT_SELECTALL},
	{L"SCI_CLEAR",                   SCI_CLEAR,                   false, false, false, VK_DELETE,   IDM_EDIT_DELETE},
	{L"SCI_CLEARALL",                SCI_CLEARALL,                false, false, false, 0,           0},
	{L"SCI_UNDO",                    SCI_UNDO,                    true,  false, false, VK_Z,        IDM_EDIT_UNDO},
	{L"",                            SCI_UNDO,                    false, true,  false, VK_BACK,     0},
	{L"SCI_REDO",                    SCI_REDO,                    true,  false, false, VK_Y,        IDM_EDIT_REDO},
	{L"",                            SCI_REDO,                    true,  false, true,  VK_Z,        0},
	{L"SCI_NEWLINE",                 SCI_NEWLINE,                 false, false, false, VK_RETURN,   0},
	{L"",                            SCI_NEWLINE,                 false, false, true,  VK_RETURN,   0},
	{L"SCI_TAB",                     SCI_TAB,                     false, false, false, VK_TAB,      0},
	{L"SCI_BACKTAB",                 SCI_BACKTAB,                 false, false, true,  VK_TAB,      0},
	{L"SCI_FORMFEED",                SCI_FORMFEED,                false, false, false, 0,           0},
	{L"SCI_ZOOMIN",                  SCI_ZOOMIN,                  true,  false, false, VK_ADD,      IDM_VIEW_ZOOMIN},
	{L"SCI_ZOOMOUT",                 SCI_ZOOMOUT,                 true,  false, false, VK_SUBTRACT, IDM_VIEW_ZOOMOUT},
	{L"SCI_SETZOOM",                 SCI_SETZOOM,                 true,  false, false, VK_DIVIDE,   IDM_VIEW_ZOOMRESTORE},
	{L"SCI_SELECTIONDUPLICATE",      SCI_SELECTIONDUPLICATE,      true,  false, false, VK_D,        0},
	{L"SCI_LINESJOIN",               SCI_LINESJOIN,               false, false, false, 0,           0},
	{L"SCI_SCROLLCARET",             SCI_SCROLLCARET,             false, false, false, 0,           0},
	{L"SCI_EDITTOGGLEOVERTYPE",      SCI_EDITTOGGLEOVERTYPE,      false, false, false, VK_INSERT,   0},
	{L"SCI_MOVECARETINSIDEVIEW",     SCI_MOVECARETINSIDEVIEW,     false, false, false, 0,           0},
	{L"SCI_LINEDOWN",                SCI_LINEDOWN,                false, false, false, VK_DOWN,     0},
	{L"SCI_LINEDOWNEXTEND",          SCI_LINEDOWNEXTEND,          false, false, true,  VK_DOWN,     0},
	{L"SCI_LINEDOWNRECTEXTEND",      SCI_LINEDOWNRECTEXTEND,      false, true,  true,  VK_DOWN,     0},
	{L"SCI_LINESCROLLDOWN",          SCI_LINESCROLLDOWN,          true,  false, false, VK_DOWN,     0},
	{L"SCI_LINEUP",                  SCI_LINEUP,                  false, false, false, VK_UP,       0},
	{L"SCI_LINEUPEXTEND",            SCI_LINEUPEXTEND,            false, false, true,  VK_UP,       0},
	{L"SCI_LINEUPRECTEXTEND",        SCI_LINEUPRECTEXTEND,        false, true,  true,  VK_UP,       0},
	{L"SCI_LINESCROLLUP",            SCI_LINESCROLLUP,            true,  false, false, VK_UP,       0},
	{L"SCI_PARADOWN",                SCI_PARADOWN,                true,  false, false, VK_OEM_6,    0},
	{L"SCI_PARADOWNEXTEND",          SCI_PARADOWNEXTEND,          true,  false, true,  VK_OEM_6,    0},
	{L"SCI_PARAUP",                  SCI_PARAUP,                  true,  false, false, VK_OEM_4,    0},
	{L"SCI_PARAUPEXTEND",            SCI_PARAUPEXTEND,            true,  false, true,  VK_OEM_4,    0},
	{L"SCI_CHARLEFT",                SCI_CHARLEFT,                false, false, false, VK_LEFT,     0},
	{L"SCI_CHARLEFTEXTEND",          SCI_CHARLEFTEXTEND,          false, false, true,  VK_LEFT,     0},
	{L"SCI_CHARLEFTRECTEXTEND",      SCI_CHARLEFTRECTEXTEND,      false, true,  true,  VK_LEFT,     0},
	{L"SCI_CHARRIGHT",               SCI_CHARRIGHT,               false, false, false, VK_RIGHT,    0},
	{L"SCI_CHARRIGHTEXTEND",         SCI_CHARRIGHTEXTEND,         false, false, true,  VK_RIGHT,    0},
	{L"SCI_CHARRIGHTRECTEXTEND",     SCI_CHARRIGHTRECTEXTEND,     false, true,  true,  VK_RIGHT,    0},
	{L"SCI_WORDLEFT",                SCI_WORDLEFT,                true,  false, false, VK_LEFT,     0},
	{L"SCI_WORDLEFTEXTEND",          SCI_WORDLEFTEXTEND,          true,  false, true,  VK_LEFT,     0},
	{L"SCI_WORDRIGHT",               SCI_WORDRIGHT,               true,  false, false, VK_RIGHT,    0},
	{L"SCI_WORDRIGHTEXTEND",         SCI_WORDRIGHTEXTEND,         false, false, false, 0,           0},
	{L"SCI_WORDLEFTEND",             SCI_WORDLEFTEND,             false, false, false, 0,           0},
	{L"SCI_WORDLEFTENDEXTEND",       SCI_WORDLEFTENDEXTEND,       false, false, false, 0,           0},
	{L"SCI_WORDRIGHTEND",            SCI_WORDRIGHTEND,            false, false, false, 0,           0},
	{L"SCI_WORDRIGHTENDEXTEND",      SCI_WORDRIGHTENDEXTEND,      true,  false, true,  VK_RIGHT,    0},
	{L"SCI_WORDPARTLEFT",            SCI_WORDPARTLEFT,            true,  false, false, VK_OEM_2,    0},
	{L"SCI_WORDPARTLEFTEXTEND",      SCI_WORDPARTLEFTEXTEND,      true,  false, true,  VK_OEM_2,    0},
	{L"SCI_WORDPARTRIGHT",           SCI_WORDPARTRIGHT,           true,  false, false, VK_OEM_5,    0},
	{L"SCI_WORDPARTRIGHTEXTEND",     SCI_WORDPARTRIGHTEXTEND,     true,  false, true,  VK_OEM_5,    0},
	{L"SCI_HOME",                    SCI_HOME,                    false, false, false, 0,           0},
	{L"SCI_HOMEEXTEND",              SCI_HOMEEXTEND,              false, false, false, 0,           0},
	{L"SCI_HOMERECTEXTEND",          SCI_HOMERECTEXTEND,          false, false, false, 0,           0},
	{L"SCI_HOMEDISPLAY",             SCI_HOMEDISPLAY,             false, true,  false, VK_HOME,     0},
	{L"SCI_HOMEDISPLAYEXTEND",       SCI_HOMEDISPLAYEXTEND,       false, false, false, 0,           0},
	{L"SCI_HOMEWRAP",                SCI_HOMEWRAP,                false, false, false, 0,           0},
	{L"SCI_HOMEWRAPEXTEND",          SCI_HOMEWRAPEXTEND,          false, false, false, 0,           0},
	{L"SCI_VCHOME",                  SCI_VCHOME,                  false, false, false, 0,           0},
	{L"SCI_VCHOMEEXTEND",            SCI_VCHOMEEXTEND,            false, false, false, 0,           0},
	{L"SCI_VCHOMERECTEXTEND",        SCI_VCHOMERECTEXTEND,        false, true,  true,  VK_HOME,     0},
	{L"SCI_VCHOMEDISPLAY",           SCI_VCHOMEDISPLAY,           false, false, false, 0,           0},
	{L"SCI_VCHOMEDISPLAYEXTEND",     SCI_VCHOMEDISPLAYEXTEND,     false, false, false, 0,           0},
	{L"SCI_VCHOMEWRAP",              SCI_VCHOMEWRAP,              false, false, false, VK_HOME,     0},
	{L"SCI_VCHOMEWRAPEXTEND",        SCI_VCHOMEWRAPEXTEND,        false, false, true,  VK_HOME,     0},
	{L"SCI_LINEEND",                 SCI_LINEEND,                 false, false, false, 0,           0},
	{L"SCI_LINEENDWRAPEXTEND",       SCI_LINEENDWRAPEXTEND,       false, false, true,  VK_END,      0},
	{L"SCI_LINEENDRECTEXTEND",       SCI_LINEENDRECTEXTEND,       false, true,  true,  VK_END,      0},
	{L"SCI_LINEENDDISPLAY",          SCI_LINEENDDISPLAY,          false, true,  false, VK_END,      0},
	{L"SCI_LINEENDDISPLAYEXTEND",    SCI_LINEENDDISPLAYEXTEND,    false, false, false, 0,           0},
	{L"SCI_LINEENDWRAP",             SCI_LINEENDWRAP,             false, false, false, VK_END,      0},
	{L"SCI_LINEENDEXTEND",           SCI_LINEENDEXTEND,           false, false, false, 0,           0},
	{L"SCI_DOCUMENTSTART",           SCI_DOCUMENTSTART,           true,  false, false, VK_HOME,     0},
	{L"SCI_DOCUMENTSTARTEXTEND",     SCI_DOCUMENTSTARTEXTEND,     true,  false, true,  VK_HOME,     0},
	{L"SCI_DOCUMENTEND",             SCI_DOCUMENTEND,             true,  false, false, VK_END,      0},
	{L"SCI_DOCUMENTENDEXTEND",       SCI_DOCUMENTENDEXTEND,       true,  false, true,  VK_END,      0},
	{L"SCI_PAGEUP",                  SCI_PAGEUP,                  false, false, false, VK_PRIOR,    0},
	{L"SCI_PAGEUPEXTEND",            SCI_PAGEUPEXTEND,            false, false, true,  VK_PRIOR,    0},
	{L"SCI_PAGEUPRECTEXTEND",        SCI_PAGEUPRECTEXTEND,        false, true,  true,  VK_PRIOR,    0},
	{L"SCI_PAGEDOWN",                SCI_PAGEDOWN,                false, false, false, VK_NEXT,     0},
	{L"SCI_PAGEDOWNEXTEND",          SCI_PAGEDOWNEXTEND,          false, false, true,  VK_NEXT,     0},
	{L"SCI_PAGEDOWNRECTEXTEND",      SCI_PAGEDOWNRECTEXTEND,      false, true,  true,  VK_NEXT,     0},
	{L"SCI_STUTTEREDPAGEUP",         SCI_STUTTEREDPAGEUP,         false, false, false, 0,           0},
	{L"SCI_STUTTEREDPAGEUPEXTEND",   SCI_STUTTEREDPAGEUPEXTEND,   false, false, false, 0,           0},
	{L"SCI_STUTTEREDPAGEDOWN",       SCI_STUTTEREDPAGEDOWN,       false, false, false, 0,           0},
	{L"SCI_STUTTEREDPAGEDOWNEXTEND", SCI_STUTTEREDPAGEDOWNEXTEND, false, false, false, 0,           0},
	{L"SCI_DELETEBACK",              SCI_DELETEBACK,              false, false, false, VK_BACK,     0},
	{L"",                            SCI_DELETEBACK,              false, false, true,  VK_BACK,     0},
	{L"SCI_DELETEBACKNOTLINE",       SCI_DELETEBACKNOTLINE,       false, false, false, 0,           0},
	{L"SCI_DELWORDLEFT",             SCI_DELWORDLEFT,             true,  false, false, VK_BACK,     0},
	{L"SCI_DELWORDRIGHT",            SCI_DELWORDRIGHT,            true,  false, false, VK_DELETE,   0},
	{L"SCI_DELLINELEFT",             SCI_DELLINELEFT,             true,  false, true,  VK_BACK,     0},
	{L"SCI_DELLINERIGHT",            SCI_DELLINERIGHT,            true,  false, true,  VK_DELETE,   0},
	{L"SCI_LINEDELETE",              SCI_LINEDELETE,              true,  false, true,  VK_L,        0},
	{L"SCI_LINECUT",                 SCI_LINECUT,                 true,  false, false, VK_L,        0},
	{L"SCI_LINECOPY",                SCI_LINECOPY,                true,  false, true,  VK_X,        0},
	{L"SCI_LINETRANSPOSE",           SCI_LINETRANSPOSE,           true,  false, false, VK_T,        0},
	{L"SCI_LINEDUPLICATE",           SCI_LINEDUPLICATE,           false, false, false, 0,           IDM_EDIT_DUP_LINE},
	{L"SCI_CANCEL",                  SCI_CANCEL,                  false, false, false, VK_ESCAPE,   0},
	{L"SCI_SWAPMAINANCHORCARET",     SCI_SWAPMAINANCHORCARET,     false, false, false, 0,           0},
	{L"SCI_ROTATESELECTION",         SCI_ROTATESELECTION,         false, false, false, 0,           0}
};

#define SHORTCUTSXML_FILENAME L"shortcuts.xml"
#define SHORTCUTSXML_MODEL_FILENAME L"shortcuts.model.xml"

#define CONTEXTMENUXML_FILENAME L"contextMenu.xml"
#define CONTEXTMENUXML_MODEL_FILENAME L"contextMenu.model.xml"

#define SESSION_BACKUP_EXT L".inCaseOfCorruption.bak"

static int strVal(const char* str, int base)
{
	if (!str) return -1;
	if (!str[0]) return 0;

	char* finStr;
	const int result = std::strtol(str, &finStr, base);
	if (*finStr != '\0')
		return -1;
	return result;
}

static int hexStrVal(const char* str)
{
	return strVal(str, 16);
}

static int getKwClassFromName(const char* str)
{
	if(!str) return -1;
	if (std::strcmp(str, "instre1") == 0) return LANG_INDEX_INSTR;
	if (std::strcmp(str, "instre2") == 0) return LANG_INDEX_INSTR2;
	if (std::strcmp(str, "type1") == 0) return LANG_INDEX_TYPE;
	if (std::strcmp(str, "type2") == 0) return LANG_INDEX_TYPE2;
	if (std::strcmp(str, "type3") == 0) return LANG_INDEX_TYPE3;
	if (std::strcmp(str, "type4") == 0) return LANG_INDEX_TYPE4;
	if (std::strcmp(str, "type5") == 0) return LANG_INDEX_TYPE5;
	if (std::strcmp(str, "type6") == 0) return LANG_INDEX_TYPE6;
	if (std::strcmp(str, "type7") == 0) return LANG_INDEX_TYPE7;
	if (std::strcmp(str, "substyle1") == 0) return LANG_INDEX_SUBSTYLE1;
	if (std::strcmp(str, "substyle2") == 0) return LANG_INDEX_SUBSTYLE2;
	if (std::strcmp(str, "substyle3") == 0) return LANG_INDEX_SUBSTYLE3;
	if (std::strcmp(str, "substyle4") == 0) return LANG_INDEX_SUBSTYLE4;
	if (std::strcmp(str, "substyle5") == 0) return LANG_INDEX_SUBSTYLE5;
	if (std::strcmp(str, "substyle6") == 0) return LANG_INDEX_SUBSTYLE6;
	if (std::strcmp(str, "substyle7") == 0) return LANG_INDEX_SUBSTYLE7;
	if (std::strcmp(str, "substyle8") == 0) return LANG_INDEX_SUBSTYLE8;

	if ((str[1] == '\0') && (str[0] >= '0') && (str[0] <= '8')) // up to KEYWORDSET_MAX
		return str[0] - '0';

	return -1;
}

static constexpr unsigned int RGBHEX(QRgb color)
{
	return (((color & 0x0000FF) << 16) | ((color & 0x00FF00)) | ((color & 0xFF0000) >> 16));
}

} // anonymous namespace

enum class XmlAttrResult
{
	failed = -1,
	isFalse,
	isTrue
};

static constexpr std::array<const char*, 2> STR_BOOL_YESNO{ { "yes", "no" } };
static constexpr std::array<const char*, 2> STR_BOOL_SHOWHIDE{ { "show", "hide" } };

[[nodiscard]] static bool getBoolAttribute(const NppXml::Element& elem, const char* name, bool defaultVal = false, const std::array<const char*, 2>& strs2cmp = STR_BOOL_YESNO)
{
	QString str = NppXml::attribute(elem, name);
	if (!str.isNull())
	{
		if (str == QLatin1String(strs2cmp[0]))
			return true;
		if (str == QLatin1String(strs2cmp[1]))
			return false;
	}
	return defaultVal;
}

[[nodiscard]] static XmlAttrResult getResultAttribute(const NppXml::Element& elem, const char* name, const std::array<const char*, 2>& strs2cmp = STR_BOOL_YESNO)
{
	QString str = NppXml::attribute(elem, name);
	if (!str.isNull())
	{
		if (str == QLatin1String(strs2cmp[0]))
			return XmlAttrResult::isTrue;
		if (str == QLatin1String(strs2cmp[1]))
			return XmlAttrResult::isFalse;
	}
	return XmlAttrResult::failed;
}

template <typename T>
[[nodiscard]] static T getRangeClampAttribute(const NppXml::Element& elem, const char* name, T minVal, T maxVal, T defaultVal)
{
	const int val = NppXml::intAttribute(elem, name, static_cast<int>(defaultVal));
	if (val != static_cast<int>(defaultVal))
	{
		return static_cast<T>(std::max<int>(static_cast<int>(minVal), std::min<int>(val, static_cast<int>(maxVal))));
	}
	return defaultVal;
}

template <typename T>
[[nodiscard]] static T getRangeDefaultAttribute(const NppXml::Element& elem, const char* name, T minVal, T maxVal, T defaultVal)
{
	const int val = NppXml::intAttribute(elem, name, static_cast<int>(defaultVal));
	if (val < static_cast<int>(minVal))
		return defaultVal;
	if (val > static_cast<int>(maxVal))
		return defaultVal;
	return static_cast<T>(val);
}

static void setBoolAttribute(NppXml::Element& elem, const char* name, bool isTrue, const std::array<const char*, 2>& strs2set = STR_BOOL_YESNO)
{
	NppXml::setAttribute(elem, name, isTrue ? strs2set[0] : strs2set[1]);
}

[[nodiscard]] static bool getBoolChildTextNode(const NppXml::Element& elemParent, bool defaultVal = false, const std::array<const char*, 2>& strs2cmp = STR_BOOL_YESNO)
{
	NppXml::Node n = NppXml::firstChild(elemParent);
	if (!n.isNull())
	{
		const char* val = NppXml::value(n);
		if (val)
		{
			if (std::strcmp(val, strs2cmp[0]) == 0)
				return true;
			if (std::strcmp(val, strs2cmp[1]) == 0)
				return false;
		}
	}
	return defaultVal;
}

[[nodiscard]] static XmlAttrResult getResultChildTextNode(const NppXml::Element& elemParent, const std::array<const char*, 2>& strs2cmp = STR_BOOL_YESNO)
{
	NppXml::Node n = NppXml::firstChild(elemParent);
	if (!n.isNull())
	{
		const char* val = NppXml::value(n);
		if (val)
		{
			if (std::strcmp(val, strs2cmp[0]) == 0)
				return XmlAttrResult::isTrue;
			if (std::strcmp(val, strs2cmp[1]) == 0)
				return XmlAttrResult::isFalse;
		}
	}
	return XmlAttrResult::failed;
}

void cutString(const wchar_t* str2cut, std::vector<std::wstring>& patternVect)
{
	if (str2cut == nullptr) return;

	const wchar_t *pBegin = str2cut;
	const wchar_t *pEnd = pBegin;

	static const auto& loc = std::locale::classic();

	while (*pEnd != L'\0')
	{
		if (std::isspace(*pEnd, loc))
		{
			if (pBegin != pEnd)
				patternVect.emplace_back(pBegin, pEnd);
			pBegin = pEnd + 1;

		}
		++pEnd;
	}

	if (pBegin != pEnd)
		patternVect.emplace_back(pBegin, pEnd);
}

void cutStringBy(const wchar_t* str2cut, std::vector<std::wstring>& patternVect, wchar_t byChar, bool allowEmptyStr)
{
	if (str2cut == nullptr) return;

	const wchar_t* pBegin = str2cut;
	const wchar_t* pEnd = pBegin;

	while (*pEnd != L'\0')
	{
		if (*pEnd == byChar)
		{
			if (allowEmptyStr)
				patternVect.emplace_back(pBegin, pEnd);
			else if (pBegin != pEnd)
				patternVect.emplace_back(pBegin, pEnd);
			pBegin = pEnd + 1;
		}
		++pEnd;
	}
	if (allowEmptyStr)
		patternVect.emplace_back(pBegin, pEnd);
	else if (pBegin != pEnd)
		patternVect.emplace_back(pBegin, pEnd);
}

void LocalizationSwitcher::setFileName(const QString& fn)
{
    _fileName = fn.toStdString();
}

std::wstring LocalizationSwitcher::getLangFromXmlFileName(const wchar_t* fn)
{
	for (const auto& locDef : _localizationList)
	{
		if (wcscmp(fn, locDef.first.c_str()) == 0)
			return locDef.second;
	}
	return L"";
}

std::wstring LocalizationSwitcher::getXmlFilePathFromLangName(const wchar_t *langName) const
{
	for (size_t i = 0, len = _localizationList.size(); i < len ; ++i)
	{
		if (std::wcscmp(langName, _localizationList[i].first.c_str()) == 0)
			return _localizationList[i].second;
	}
	return std::wstring();
}

bool LocalizationSwitcher::addLanguageFromXml(const std::wstring& xmlFullPath)
{
	// Qt6: PathFindFileNameW → extract filename from path manually
	auto lastSep = xmlFullPath.find_last_of(L"\\/");
	const wchar_t* fn = (lastSep == std::wstring::npos) ? xmlFullPath.c_str() : xmlFullPath.c_str() + lastSep + 1;
	const std::wstring foundLang = getLangFromXmlFileName(fn);
	if (!foundLang.empty())
	{
		_localizationList.emplace_back(foundLang, xmlFullPath);
		return true;
	}
	return false;
}

bool LocalizationSwitcher::switchToLang(const wchar_t *lang2switch) const
{
	const std::wstring langPath = getXmlFilePathFromLangName(lang2switch);
	if (langPath.empty())
		return false;

	return QFile::copy(QString::fromStdWString(langPath), QString::fromStdWString(_nativeLangPath));
}

std::wstring ThemeSwitcher::getThemeFromXmlFileName(const wchar_t* xmlFullPath)
{
	if (!xmlFullPath || !xmlFullPath[0])
		return L"";
	std::wstring fn = QFileInfo(QString::fromStdWString(xmlFullPath)).fileName().toStdWString();
	// PathRemoveExtension: fn stays as is (handled differently);
	return fn;
}

int DynamicMenu::getTopLevelItemNumber() const
{
	int nb = 0;
	std::wstring previousFolderName;
	for (const MenuItemUnit& i : _menuItems)
	{
		if (i._parentFolderName.empty())
		{
			++nb;
		}
		else
		{
			if (previousFolderName.empty())
			{
				++nb;
				previousFolderName = i._parentFolderName;
			}
			else // previousFolderName is not empty
			{
				if (i._parentFolderName.empty())
				{
					++nb;
					previousFolderName = i._parentFolderName;
				}
				else if (previousFolderName == i._parentFolderName)
				{
					// maintain the number and do nothing
				}
				else
				{
					++nb;
					previousFolderName = i._parentFolderName;
				}
			}
		}
	}

	return nb;
}

bool DynamicMenu::attach(QMenu* hMenu, unsigned int posBase, int lastCmd, const std::wstring& lastCmdLabel)
{
	if (!hMenu) return false;

	_hMenu = hMenu;
	_posBase = posBase;
	_lastCmd = lastCmd;
	_lastCmdLabel = lastCmdLabel;

	return createMenu();
}

bool DynamicMenu::clearMenu() const
{
	if (!_hMenu) return false;

	int nbTopItem = getTopLevelItemNumber();
	for (int i = nbTopItem + 1; i >= 0 ; --i)
	{
		// DeleteMenu replaced_hMenu, _posBase + i, 0 /* MF_BYPOSITION */);
	}

	return true;
}

bool DynamicMenu::createMenu() const
{
	if (!_hMenu) return false;

	bool lastIsSep = false;
	QMenu* hParentFolder = nullptr;
	std::wstring currentParentFolderStr;
	int j = 0;

	size_t nb = _menuItems.size();
	size_t i = 0;
	for (; i < nb; ++i)
	{
		const MenuItemUnit& item = _menuItems[i];
		if (item._parentFolderName.empty())
		{
			currentParentFolderStr.clear();
			hParentFolder = nullptr;
			j = 0;
		}
		else
		{
			if (item._parentFolderName != currentParentFolderStr)
			{
				currentParentFolderStr = item._parentFolderName;
				// hParentFolder = CreateMenu() — TODO: port to Qt QMenu
				j = 0;

				// InsertMenu replaced_hMenu, static_cast<UINT>(_posBase + i), 0 /* MF_BYPOSITION */ | 0 /* MF_POPUP */, reinterpret_cast<UINT_PTR>(hParentFolder), currentParentFolderStr.c_str());
			}
		}

		unsigned int flag = 0 /* MF_BYPOSITION */ | ((item._cmdID == 0) ? 0 /* MF_SEPARATOR */ : 0);
		if (hParentFolder)
		{
			// InsertMenu replacedhParentFolder, j++, flag, item._cmdID, item._itemName.c_str());
			lastIsSep = false;
		}
		else if ((i == 0 || i == _menuItems.size() - 1) && item._cmdID == 0)
		{
			lastIsSep = true;
		}
		else if (item._cmdID != 0)
		{
			// InsertMenu replaced_hMenu, static_cast<UINT>(_posBase + i), flag, item._cmdID, item._itemName.c_str());
			lastIsSep = false;
		}
		else if (!lastIsSep)
		{
			// InsertMenu replaced_hMenu, static_cast<UINT>(_posBase + i), flag, item._cmdID, item._itemName.c_str());
			lastIsSep = true;
		}
		else // last item is separator and current item is separator
		{
			lastIsSep = true;
		}
	}

	if (nb > 0)
	{
		// InsertMenu replaced_hMenu, static_cast<UINT>(_posBase + i), 0 /* MF_BYPOSITION */ | 0 /* MF_SEPARATOR */, 0, nullptr);
		// InsertMenu replaced_hMenu, static_cast<UINT>(_posBase + i + 2), 0 /* MF_BYCOMMAND */, _lastCmd, _lastCmdLabel.c_str());
	}

	return true;
}

winVer NppParameters::getWindowsVersion()
{
	// On Linux, we cannot detect Windows versions. Use QSysInfo for platform detection.
#if defined(Q_OS_WIN)
	return WV_WIN10; // Windows detection not implemented in this stub
#elif defined(Q_OS_LINUX)
	_platForm = PF_X64;
	const auto arch = QSysInfo::currentCpuArchitecture();
	if (arch == "arm64")
		_platForm = PF_ARM64;
	else if (arch == "x86_64")
		_platForm = PF_X64;
	else if (arch == "i386" || arch == "x86")
		_platForm = PF_X86;
	return WV_UNKNOWN; // Not a Windows system
#else
	_platForm = PF_UNKNOWN;
	return WV_UNKNOWN;
#endif
}

NppParameters::NppParameters()
{
    qInstallMessageHandler(qtMsgHandler);
	// Get windows version
	_winVersion = getWindowsVersion();

	// Get current system code page
	_currentSystemCodepage = 65001 /* UTF-8 default */;

	// Prepare for default path — use QString throughout to avoid
	// wchar_t-size assumptions (2 bytes on Windows, 4 bytes on Linux).
	QString appPath = QCoreApplication::applicationFilePath();
	QFileInfo fi_npPath(appPath);
	_nppPath = fi_npPath.absolutePath().toStdWString();
	fprintf(stderr, "DBG _nppPath init: '%ls'\n", _nppPath.c_str());

	// Initialize current directory to startup directory
	_currentDirectory = QDir::currentPath().toStdWString();

	_appdataNppDir.clear();
	std::wstring notepadStylePath(_nppPath);
	pathAppend(notepadStylePath, notepadStyleFile);

	_asNotepadStyle = QFileInfo(QString::fromStdWString(notepadStylePath)).exists();

	//Load initial accelerator key definitions
	initMenuKeys();
	initScintillaKeys();
}

NppParameters::~NppParameters()
{
	for (auto& docPath : _pXmlExternalLexerDoc)
	{
		delete docPath._doc;
		docPath._doc = nullptr;
	}

	_pXmlExternalLexerDoc.clear();
}

bool NppParameters::reloadStylers(const wchar_t* stylePath)
{
	delete _pXmlUserStylerDoc._doc;

	_pXmlUserStylerDoc._path = stylePath ? stylePath : _stylerPath.c_str();
	_pXmlUserStylerDoc._doc = NppXml::NewDocument();

	const bool loadOkay = NppXml::loadFile(_pXmlUserStylerDoc._doc, _pXmlUserStylerDoc._path.c_str());
	if (!loadOkay)
	{
		if (!_pNativeLangSpeaker)
		{
			QMessageBox::critical(nullptr, QString::fromWCharArray(_pXmlUserStylerDoc._path.c_str()), QStringLiteral("Load stylers.xml failed"), QMessageBox::Ok);
		}
		else
		{
			_pNativeLangSpeaker->messageBox("LoadStylersFailed",
				nullptr,
				QStringLiteral("Load \"$STR_REPLACE$\" failed!"),
				QStringLiteral("Load stylers.xml failed"),
				QMessageBox::Ok,
				0,
				QString::fromWCharArray(_pXmlUserStylerDoc._path.c_str()));
		}
		delete _pXmlUserStylerDoc._doc;
		_pXmlUserStylerDoc._doc = nullptr;
		return false;
	}
	_lexerStylerVect.clear();
	_widgetStyleArray.clear();

	getUserStylersFromXmlTree();

	//  Reload plugin styles.
	for (size_t i = 0; i < getExternalLexerDoc()->size(); ++i)
	{
		NppXml::Document externalLexerDoc = getExternalLexerDoc()->at(i)._doc;
		NppXml::Element root = NppXml::firstChildElement(externalLexerDoc, "NotepadPlus");
		if (!root.isNull())
			feedStylerArray(root);
	}
	return true;
}

bool NppParameters::reloadLang()
{
	// use user path
	std::wstring nativeLangPath(_localizationSwitcher._nativeLangPath);

	// if "nativeLang.xml" does not exist, use npp path
	if (!QFileInfo(QString::fromStdWString(nativeLangPath)).exists())
	{
		nativeLangPath = _nppPath;
		pathAppend(nativeLangPath, std::wstring(L"nativeLang.xml"));
		if (!QFileInfo(QString::fromStdWString(nativeLangPath)).exists())
			return false;
	}

	delete _pXmlNativeLangDoc;

	_pXmlNativeLangDoc = NppXml::NewDocument();
	const bool loadOkay = NppXml::loadFileNativeLang(_pXmlNativeLangDoc, nativeLangPath.c_str());
	if (!loadOkay)
	{
		delete _pXmlNativeLangDoc;
		_pXmlNativeLangDoc = nullptr;
		return false;
	}
	return loadOkay;
}

std::wstring NppParameters::getSpecialFolderLocation(int folderKind)
{
	// folderKind maps Win32 CSIDL values to QStandardPaths
	// 0x001A = CSIDL_APPDATA → QStandardPaths::AppDataLocation
	// 0x0026 = CSIDL_PROGRAM_FILES → QStandardPaths::ApplicationsLocation
	// Other values fall back to AppDataLocation
	QStandardPaths::StandardLocation loc = QStandardPaths::AppDataLocation;
	if (folderKind == 0x0026) {
		loc = QStandardPaths::ApplicationsLocation;  // Qt6 replaced ProgramFilesPath
	}
	QString pathStr = QStandardPaths::writableLocation(loc);

	std::wstring result;
	if (!pathStr.isEmpty())
	{
		result = pathStr.toStdWString();
	}
	return result;
}

std::wstring NppParameters::getSettingsFolder() const
{
	if (_isLocal)
		return _nppPath;

	std::wstring settingsFolderPath = getSpecialFolderLocation(0x001A /* CSIDL_APPDATA */);

	if (settingsFolderPath.empty())
		return _nppPath;

	pathAppend(settingsFolderPath, L"Notepad++");
	return settingsFolderPath;
}

bool NppParameters::load()
{
	L_END = L_EXTERNAL;
	bool isAllLoaded = true;

	_isx64 = sizeof(void *) == 8;

	// Make localConf.xml path
	std::wstring localConfPath(_nppPath);
	pathAppend(localConfPath, localConfFile);

	// Test if doLocalConf.xml exists
	_isLocal = QFileInfo(QString::fromStdWString(localConfPath)).exists();

	// Under vista and windows 7, the usage of doLocalConf.xml is not allowed
	// if Notepad++ is installed in "program files" directory, because of UAC
	if (_isLocal)
	{
		// We check if OS is Vista or greater version
		if (_winVersion >= WV_VISTA)
		{
			std::wstring progPath = getSpecialFolderLocation(0x0026 /* CSIDL_PROGRAM_FILES */);
			QString nppDirStr = QString::fromStdWString(_nppPath); int ls = nppDirStr.lastIndexOf(QDir::separator()); if(ls>0) nppDirStr.truncate(ls); // PathRemoveFileSpec

			if (progPath == nppDirStr.toStdWString())
				_isLocal = false;
		}
	}

	_pluginRootDir = _nppPath;
	pathAppend(_pluginRootDir, L"plugins");

	//
	// the 3rd priority: general default configuration
	//
	std::wstring nppPluginRootParent;
	if (_isLocal)
	{
		_userPath = nppPluginRootParent = _nppPath;
		_userPluginConfDir = _pluginRootDir;
		pathAppend(_userPluginConfDir, L"Config");
	}
	else
	{
		_userPath = getSpecialFolderLocation(0x001A /* CSIDL_APPDATA */);
		fprintf(stderr, "DBG _userPath after getSpecialFolder: '%ls'\n", _userPath.c_str());

		pathAppend(_userPath, L"Notepad++");
		fprintf(stderr, "DBG _userPath after pathAppend: '%ls'\n", _userPath.c_str());
		if (!doesDirectoryExist(QString::fromStdWString(_userPath)))
			QDir().mkpath(QString::fromStdWString(_userPath));

		nppPluginRootParent = _userPath;  // Qt6: plugin conf lives under user config dir
		_appdataNppDir = _userPluginConfDir = _userPath;

		pathAppend(_userPluginConfDir, L"plugins");
		if (!doesDirectoryExist(QString::fromStdWString(_userPluginConfDir)))
			QDir().mkpath(QString::fromStdWString(_userPluginConfDir));

		pathAppend(_userPluginConfDir, L"Config");
		if (!doesDirectoryExist(QString::fromStdWString(_userPluginConfDir)))
			QDir().mkpath(QString::fromStdWString(_userPluginConfDir));

		// For PluginAdmin to launch the wingup with UAC
		setElevationRequired(true);
	}

	_pluginConfDir = _pluginRootDir; // for plugin list home
	pathAppend(_pluginConfDir, L"Config");

	fprintf(stderr, "DBG nppPluginRootParent: '%ls', _pluginRootDir: '%ls'\n",
	        nppPluginRootParent.c_str(), _pluginRootDir.c_str());
	fprintf(stderr, "DBG: about to check nppPluginRootParent dir...\n"); fflush(stderr);

	if (!doesDirectoryExist(QString::fromStdWString(nppPluginRootParent)))
	{
		fprintf(stderr, "DBG: creating nppPluginRootParent dir...\n"); fflush(stderr);
		QDir().mkpath(QString::fromStdWString(nppPluginRootParent));
		fprintf(stderr, "DBG: created nppPluginRootParent dir\n"); fflush(stderr);
	}
	fprintf(stderr, "DBG: about to check _pluginRootDir dir...\n"); fflush(stderr);
	if (!doesDirectoryExist(QString::fromStdWString(_pluginRootDir)))
	{
		fprintf(stderr, "DBG: creating _pluginRootDir dir...\n"); fflush(stderr);
		QDir().mkpath(QString::fromStdWString(_pluginRootDir));
		fprintf(stderr, "DBG: created _pluginRootDir dir\n"); fflush(stderr);
	}
	fprintf(stderr, "DBG: dirs done, setting _sessionPath...\n"); fflush(stderr);

	// Ensure required directories exist
	if (!doesDirectoryExist(QString::fromStdWString(nppPluginRootParent)))
		QDir().mkpath(QString::fromStdWString(nppPluginRootParent));
	if (!doesDirectoryExist(QString::fromStdWString(_pluginRootDir)))
		QDir().mkpath(QString::fromStdWString(_pluginRootDir));

	_sessionPath = _userPath; // Session stores the absolute file path, it should never be on cloud
	fprintf(stderr, "DBG: _sessionPath set, about to detect cloud settings...\n"); fflush(stderr);

	// Detection cloud settings
	std::wstring cloudChoicePath{_userPath};
	cloudChoicePath += L"\\cloud\\choice";

	//
	// the 2nd priority: Cloud Choice Path
	//
	_isCloud = QFileInfo(QString::fromStdWString(cloudChoicePath)).exists();
	if (_isCloud)
	{
		// Read cloud choice
		std::wstring cloudChoiceStrW = L"";
		bool bLoadingFailed = false;
		std::string cloudChoiceStr = getFileContent(QString::fromStdWString(cloudChoicePath), &bLoadingFailed).toStdString();
		if (!bLoadingFailed)
		{
			WcharMbcsConvertor& wmc = WcharMbcsConvertor::getInstance();
			cloudChoiceStrW = wmc.char2wchar(cloudChoiceStr.c_str(), SC_CP_UTF8);
		}
		if (!cloudChoiceStrW.empty() && doesDirectoryExist(QString::fromStdWString(cloudChoiceStrW)))
		{
			_userPath = cloudChoiceStrW;
			_nppGUI._cloudPath = cloudChoiceStrW;
			_initialCloudChoice = _nppGUI._cloudPath;
		}
		else
		{
			_isCloud = false;
		}
	}

	//
	// the 1st priority: custom settings dir via command line argument
	//
	if (!_cmdSettingsDir.empty())
	{
		if (!doesDirectoryExist(QString::fromStdWString(_cmdSettingsDir)))
		{
			// The following text is not translatable.
			// _pNativeLangSpeaker is initialized AFTER _userPath being determined because nativeLang.xml is from _userPath.
			std::wstring errMsg = L"The given path\r";
			errMsg += _cmdSettingsDir;
			errMsg += L"\nvia command line \"-settingsDir=\" is not a valid directory.\rThis argument will be ignored.";
			QMessageBox::warning(nullptr, QStringLiteral("Invalid directory"), QString::fromWCharArray(errMsg.c_str()), QMessageBox::Ok);
		}
		else
		{
			_userPath = _cmdSettingsDir;
			_sessionPath = _userPath; // reset session path
		}
	}

	//--------------------------//
	// langs.xml : for per-user //
	//--------------------------//
	std::wstring langs_xml_path(_userPath);
	pathAppend(langs_xml_path, L"langs.xml");

	std::wstring modelLangsPath(_nppPath);
	pathAppend(modelLangsPath, L"langs.model.xml");

	// Always generate langs.xml with valid content (stat/QFileInfo can fail on sandboxed filesystems)
	generateXmlFromScratch(langs_xml_path.c_str(), LANGS_XML_CONTENT);

	_pXmlDoc._path = langs_xml_path;
	_pXmlDoc._doc = NppXml::NewDocument();

	bool loadOkay = NppXml::loadFile(_pXmlDoc._doc, _pXmlDoc._path.c_str());
	if (!loadOkay)
	{
		if (_pNativeLangSpeaker)
		{
			_pNativeLangSpeaker->messageBox("LoadLangsFailedFinal", nullptr, QString::fromWCharArray(L"Load langs.xml failed!"), QString::fromWCharArray(L"Configurator"),
				QMessageBox::Ok);
		}
		else
		{
			QMessageBox::critical(nullptr, QString::fromWCharArray(L"Load langs.xml failed!"), QString::fromWCharArray(L"Configurator"), QMessageBox::Ok);
		}

		delete _pXmlDoc._doc;
		_pXmlDoc._doc = nullptr;
		isAllLoaded = false;
	}
	else
		getLangKeywordsFromXmlTree();

	//---------------------------//
	// config.xml : for per-user //
	//---------------------------//
	std::wstring configPath(_userPath);
	pathAppend(configPath, L"config.xml");

	std::wstring srcConfigPath(_nppPath);
	pathAppend(srcConfigPath, L"config.model.xml");

	// Always generate config.xml with valid content (stat/QFileInfo can fail on sandboxed filesystems)
	generateXmlFromScratch(configPath.c_str(), CONFIG_XML_CONTENT);

	_xmlUserDoc._path = configPath;
	_xmlUserDoc._doc = NppXml::NewDocument();
	loadOkay = NppXml::loadFile(_xmlUserDoc._doc, _xmlUserDoc._path.c_str());

	if (!loadOkay)
	{
		NppXml::createNewDeclaration(_xmlUserDoc._doc);
	}
	else
	{
		getUserParametersFromXmlTree();
	}

	//----------------------------//
	// stylers.xml : for per-user //
	//----------------------------//

	_stylerPath = _userPath;
	pathAppend(_stylerPath, L"stylers.xml");
	// Always generate stylers.xml with valid content — stat()/QFileInfo can fail on sandboxed filesystems
	generateXmlFromScratch(_stylerPath.c_str(), STYLERS_XML_CONTENT);

	if (_nppGUI._themeName.empty() || (!QFileInfo(QString::fromStdWString(_nppGUI._themeName)).exists()))
		_nppGUI._themeName.assign(_stylerPath);

	_pXmlUserStylerDoc._path = _nppGUI._themeName;
	_pXmlUserStylerDoc._doc = NppXml::NewDocument();

	loadOkay = NppXml::loadFile(_pXmlUserStylerDoc._doc, _pXmlUserStylerDoc._path.c_str());
	if (!loadOkay)
	{
		if (_pNativeLangSpeaker)
		{
			_pNativeLangSpeaker->messageBox("LoadStylersFailed",
				nullptr,
				QStringLiteral("Load \"$STR_REPLACE$\" failed!"),
				QStringLiteral("Load stylers.xml failed"),
				QMessageBox::Ok,
				0,
				QString::fromWCharArray(_stylerPath.c_str()));
		}
		else
		{
			QMessageBox::critical(nullptr, QStringLiteral("Load stylers.xml failed"), QString::fromWCharArray(_stylerPath.c_str()), QMessageBox::Ok);
		}
		delete _pXmlUserStylerDoc._doc;
		_pXmlUserStylerDoc._doc = nullptr;
		isAllLoaded = false;
	}
	else
		getUserStylersFromXmlTree();

	_themeSwitcher._stylesXmlPath = _stylerPath;
	// Firstly, add the default theme
	_themeSwitcher.addDefaultThemeFromXml(_stylerPath);

	//-----------------------------------//
	// userDefineLang.xml : for per-user //
	//-----------------------------------//
	_userDefineLangsFolderPath = _userDefineLangPath = _userPath;
	pathAppend(_userDefineLangPath, L"userDefineLang.xml");
	pathAppend(_userDefineLangsFolderPath, L"userDefineLangs");

	std::vector<std::wstring> udlFiles;
	getFilesInFolder(udlFiles, L"*.xml", _userDefineLangsFolderPath);

	_pXmlUserLangDoc._path = _userDefineLangPath;
	_pXmlUserLangDoc._doc = NppXml::NewDocument();
	loadOkay = NppXml::loadFileUDL(_pXmlUserLangDoc._doc, _userDefineLangPath.c_str());
	if (!loadOkay)
	{
		delete _pXmlUserLangDoc._doc;
		_pXmlUserLangDoc._doc = nullptr;
		isAllLoaded = false;
	}
	else
	{
		auto r = addUserDefineLangsFromXmlTree(_pXmlUserLangDoc._doc);
		if (r.second - r.first > 0)
			_pXmlUserLangsDoc.emplace_back(_pXmlUserLangDoc._doc, _pXmlUserLangDoc._path, false, true, r);
	}

	for (const auto& i : udlFiles)
	{
		NppXml::Document udlDoc = NppXml::NewDocument();
		loadOkay = NppXml::loadFileUDL(udlDoc, i.c_str());
		if (!loadOkay)
		{
			delete udlDoc;
		}
		else
		{
			auto r = addUserDefineLangsFromXmlTree(udlDoc);
			if (r.second - r.first > 0)
			{
				_pXmlUserLangsDoc.emplace_back(udlDoc, i, false, false, r);
			}
			else
			{
				delete udlDoc;
			}
		}
	}

	//----------------------------------------------//
	// nativeLang.xml : for per-user				//
	// In case of absence of user's nativeLang.xml, //
	// We'll look in the Notepad++ Dir.			    //
	//----------------------------------------------//

	std::wstring nativeLangPath;
	nativeLangPath = _userPath;
	pathAppend(nativeLangPath, L"nativeLang.xml");

	// LocalizationSwitcher should use always user path
	_localizationSwitcher._nativeLangPath = nativeLangPath;

	if (!_startWithLocFileName.empty()) // localization argument detected, use user wished localization
	{
		// overwrite nativeLangPath variable
		nativeLangPath = _nppPath;
		pathAppend(nativeLangPath, L"localization\\");
		pathAppend(nativeLangPath, _startWithLocFileName);
	}
	else // use %appdata% location, or (if absence then) npp installed location
	{
		if (!QFileInfo(QString::fromStdWString(nativeLangPath)).exists())
		{
			nativeLangPath = _nppPath;
			pathAppend(nativeLangPath, L"nativeLang.xml");
		}
	}

	_pXmlNativeLangDoc = NppXml::NewDocument();
	loadOkay = NppXml::loadFileNativeLang(_pXmlNativeLangDoc, nativeLangPath.c_str());
	if (!loadOkay)
	{
		delete _pXmlNativeLangDoc;
		_pXmlNativeLangDoc = nullptr;
		isAllLoaded = false;
	}

	//---------------------------------------//
	// toolbarButtonsConf.xml : for per-user //
	//---------------------------------------//
	std::wstring toolbarButtonsConfXmlPath(_userPath);
	pathAppend(toolbarButtonsConfXmlPath, L"toolbarButtonsConf.xml");

	_pXmlToolButtonsConfDoc = NppXml::NewDocument();
	loadOkay = NppXml::loadFile(_pXmlToolButtonsConfDoc, toolbarButtonsConfXmlPath.c_str());
	if (!loadOkay)
	{
		delete _pXmlToolButtonsConfDoc;
		_pXmlToolButtonsConfDoc = nullptr;
		isAllLoaded = false;
	}

	//------------------------------//
	// shortcuts.xml : for per-user //
	//------------------------------//
	_shortcutsPath = _userPath;
	pathAppend(_shortcutsPath, SHORTCUTSXML_FILENAME);
	// Always generate shortcuts.xml (stat/QFileInfo can fail on sandboxed filesystems)
	generateXmlFromScratch(_shortcutsPath.c_str(), SHORTCUT_XML_CONTENT);

	_pXmlShortcutDoc = NppXml::NewDocument();
	loadOkay = NppXml::loadFileShortcut(_pXmlShortcutDoc, _shortcutsPath.c_str());
	if (!loadOkay)
	{
		delete _pXmlShortcutDoc;
		_pXmlShortcutDoc = nullptr;
		isAllLoaded = false;
	}
	else
	{
		getShortcutsFromXmlTree();
		getMacrosFromXmlTree();
		getUserCmdsFromXmlTree();

		// fill out _scintillaModifiedKeys :
		// those user-defined Scintilla keys will be used to remap the Scintilla Key Array
		getScintKeysFromXmlTree();
	}

	//--------------------------------//
	// contextMenu.xml : for per-user //
	//--------------------------------//
	_contextMenuPath = _userPath;
	pathAppend(_contextMenuPath, CONTEXTMENUXML_FILENAME);
	// Always generate contextMenu.xml (stat/QFileInfo can fail on sandboxed filesystems)
	generateXmlFromScratch(_contextMenuPath.c_str(), CONTEXTMENU_XML_CONTENT);

	_pXmlContextMenuDoc = NppXml::NewDocument();
	fprintf(stderr, "DBG: about to loadFileContextMenu, path='%ls'...\n", _contextMenuPath.c_str()); fflush(stderr);
	loadOkay = NppXml::loadFileContextMenu(_pXmlContextMenuDoc, _contextMenuPath.c_str());
	fprintf(stderr, "DBG: loadFileContextMenu ok=%d\n", loadOkay); fflush(stderr);
	if (!loadOkay)
	{
		delete _pXmlContextMenuDoc;
		_pXmlContextMenuDoc = nullptr;
		isAllLoaded = false;
	}

	//---------------------------------------------//
	// tabContextMenu.xml : for per-user, optional //
	//---------------------------------------------//
	_tabContextMenuPath = _userPath;
	pathAppend(_tabContextMenuPath, L"tabContextMenu.xml");

	_pXmlTabContextMenuDoc = NppXml::NewDocument();
	loadOkay = NppXml::loadFileContextMenu(_pXmlTabContextMenuDoc, _tabContextMenuPath.c_str());
	fprintf(stderr, "DBG: loadFileContextMenu tabContextMenu ok=%d\n", loadOkay); fflush(stderr);
	if (!loadOkay)
	{
		delete _pXmlTabContextMenuDoc;
		_pXmlTabContextMenuDoc = nullptr;
	}
	fprintf(stderr, "DBG: about to append session.xml...\n"); fflush(stderr);

	//----------------------------//
	// session.xml : for per-user //
	//----------------------------//

	pathAppend(_sessionPath, L"session.xml");
	fprintf(stderr, "DBG: about to getNppGUI for rememberLastSession...\n"); fflush(stderr);
	fprintf(stderr, "DBG: calling NppParameters::getInstance...\n"); fflush(stderr);

	// Don't load session.xml if not required in order to speed up!!
	NppParameters& inst = NppParameters::getInstance();
	fprintf(stderr, "DBG: got instance, calling getNppGUI...\n"); fflush(stderr);
	const NppGUI & nppGUI = inst.getNppGUI();
	fprintf(stderr, "DBG: got nppGUI\n"); fflush(stderr);
	fprintf(stderr, "DBG: _rememberLastSession=%d\n", nppGUI._rememberLastSession); fflush(stderr);
	if (nppGUI._rememberLastSession)
	{
		NppXml::Document pXmlSessionDoc = NppXml::NewDocument();
		loadOkay = NppXml::loadFile(pXmlSessionDoc, _sessionPath.c_str());
		if (loadOkay)
		{
			loadOkay = getSessionFromXmlTree(pXmlSessionDoc, _session);
		}

		if (!loadOkay)
		{
			std::wstring sessionInCaseOfCorruption_bak = _sessionPath;
			sessionInCaseOfCorruption_bak += SESSION_BACKUP_EXT;
			if (fileExistsFast(sessionInCaseOfCorruption_bak.c_str()))
			{
				bool bFileSwapOk = false;
				QString sessionPath = QString::fromStdWString(_sessionPath);
				QString sessionBackup = QString::fromStdWString(sessionInCaseOfCorruption_bak);

				if (QFileInfo(sessionPath).exists())
				{
					// Session file exists but is corrupt: rename it to backup path, rename good backup to session path
					// Use a temp intermediate file to avoid collisions on same filesystem
					QString corruptBackup = sessionPath + QStringLiteral(".corrupt_backup");
					if (QFile::rename(sessionPath, corruptBackup))
					{
						bFileSwapOk = QFile::rename(sessionBackup, sessionPath);
						if (!bFileSwapOk)
						{
							// Restore on failure
							QFile::rename(corruptBackup, sessionPath);
						}
					}
				}
				else
				{
					// No session file — just rename the backup to become the session
					bFileSwapOk = QFile::rename(sessionBackup, sessionPath);
				}

				if (bFileSwapOk)
				{
					NppXml::Document pXmlSessionBackupDoc = NppXml::NewDocument();
					loadOkay = NppXml::loadFile(pXmlSessionBackupDoc, _sessionPath.c_str());
					if (loadOkay)
						loadOkay = getSessionFromXmlTree(pXmlSessionBackupDoc, _session);

					delete pXmlSessionBackupDoc;
				}

				if (!loadOkay)
					isAllLoaded = false; // either the backup file is also invalid or cannot be swapped with the session.xml
			}
			else
			{
				// no backup file
				isAllLoaded = false;
			}
		}

		delete pXmlSessionDoc;
	fprintf(stderr, "DBG: deleted pXmlSessionDoc, about to loop extLexers...\n"); fflush(stderr);

		for (auto& extDoc : _pXmlExternalLexerDoc)
		{
			if (extDoc._doc)
			{
				delete extDoc._doc;
				extDoc._doc = nullptr;
			}
		}
	}

	std::wstring filePath, filePath2, issueFileName;
	//-------------------------------------------------------------//
	// nppLogNetworkDriveIssue.xml                                 //
	// This empty xml file is optional - user adds this empty file //
	// It's for debugging use only                                 //
	//-------------------------------------------------------------//
	filePath = _nppPath;
	issueFileName = L"nppLogNetworkDriveIssue";
	issueFileName += L".xml";
	pathAppend(filePath, issueFileName);
	_doNppLogNetworkDriveIssue = fileExistsFast(filePath.c_str());
	if (!_doNppLogNetworkDriveIssue)
	{
		filePath2 = _userPath;
		pathAppend(filePath2, issueFileName);
		_doNppLogNetworkDriveIssue = fileExistsFast(filePath2.c_str());
	}

	//-------------------------------------------------------------//
	// noRestartAutomatically.xml                                  //
	// This empty xml file is optional - user adds this empty file //
	// manually in order to prevent Notepad++ registration         //
	// for the Win10+ OS app-restart feature.                      //
	//-------------------------------------------------------------//
	filePath = _nppPath;
	std::wstring noRegForOSAppRestartTrigger = L"noRestartAutomatically.xml";
	pathAppend(filePath, noRegForOSAppRestartTrigger);
	_isRegForOSAppRestartDisabled = fileExistsFast(filePath.c_str());
	if (!_isRegForOSAppRestartDisabled)
	{
		filePath = _userPath;
		pathAppend(filePath, noRegForOSAppRestartTrigger);
		_isRegForOSAppRestartDisabled = fileExistsFast(filePath.c_str());
	}

	//-------------------------------------------------------------//
	// disableNppAutoUpdate.xml                                    //
	// This empty xml file is optional. If it exists, auto-update  //
	// will be disabled, even though WinGUp is present.            //
	//-------------------------------------------------------------//
	filePath = _nppPath;
	std::wstring disableNppAutoUpdateFileName = L"disableNppAutoUpdate.xml";
	pathAppend(filePath, disableNppAutoUpdateFileName);
	_isNppAutoUpdateDisabled = fileExistsFast(filePath.c_str());

	return isAllLoaded;
}

void NppParameters::destroyInstance()
{
	delete _pXmlDoc._doc;
	delete _xmlUserDoc._doc;
	delete _pXmlUserStylerDoc._doc;

	//delete _pXmlUserLangDoc; will be deleted in the vector
	for (const auto& l : _pXmlUserLangsDoc)
	{
		delete l._udlXmlDoc;
	}

	delete _pXmlNativeLangDoc;
	delete _pXmlToolButtonsConfDoc;
	delete _pXmlShortcutDoc;
	delete _pXmlContextMenuDoc;
	delete _pXmlTabContextMenuDoc;
	delete 	getInstancePointer();
}

void NppParameters::saveConfig_xml() const
{
	if (_xmlUserDoc._doc)
		static_cast<void>(NppXml::saveFile(_xmlUserDoc._doc, _xmlUserDoc._path.c_str()));
}

void NppParameters::setWorkSpaceFilePath(int i, const wchar_t* wsFile)
{
	if (i < 0 || i > 2 || !wsFile)
		return;
	_workSpaceFilePaths[i] = wsFile;
}

void NppParameters::removeTransparent(QWidget* hwnd)
{
	if (hwnd != nullptr)
	{
		// SetWindowLongPtr stub — no-op on Qt6
	}
}

void NppParameters::setTransparent(QWidget* hwnd, int percent)
{
	// SetWindowLongPtr stub — no-op on Qt6
	if (percent > 255)
		percent = 255;
	else if (percent < 0)
		percent = 0;
	// SetLayeredWindowAttributes: use QWidget::setWindowOpacity — TODO
}

bool NppParameters::isExistingExternalLangName(const char* newName) const
{
	if ((!newName) || (!newName[0]))
		return true;

	for (int i = 0 ; i < _nbExternalLang ; ++i)
	{
		if (_externalLangArray[i]->_name == newName)
			return true;
	}
	return false;
}

const wchar_t* NppParameters::getUserDefinedLangNameFromExt(const wchar_t* ext, const wchar_t* fullName) const
{
	if ((!ext) || (!ext[0]))
		return nullptr;

	std::vector<std::wstring> extVect;
	int iMatched = -1;
	for (int i = 0 ; i < _nbUserLang ; ++i)
	{
		extVect.clear();
		cutString(_userLangArray[i]->_ext.c_str(), extVect);

		// Force to use dark mode UDL in dark mode or to use  light mode UDL in light mode
		for (const auto& extStr : extVect)
		{
			if (wcscmp(extStr.c_str(), ext) == 0 || (std::wcschr(fullName, L'.') && wcscmp(extStr.c_str(), fullName) == 0))
			{
				// preserve ext matched UDL
				iMatched = i;

				if (((NppDarkMode::isEnabled_Static() && _userLangArray[i]->_isDarkModeTheme)) ||
					((!NppDarkMode::isEnabled_Static() && !_userLangArray[i]->_isDarkModeTheme)))
					return _userLangArray[i]->_name.c_str();
			}
		}
	}

	// In case that we are in dark mode but no dark UDL or we are in light mode but no light UDL
	// We use it anyway
	if (iMatched >= 0)
	{
		return _userLangArray[iMatched]->_name.c_str();
	}

	return nullptr;
}

int NppParameters::getExternalLangIndexFromName(const wchar_t* externalLangName) const
{
	WcharMbcsConvertor& wmc = WcharMbcsConvertor::getInstance();
	const char* extNameNarrow = wmc.wchar2char(externalLangName, SC_CP_UTF8);
	for (int i = 0 ; i < _nbExternalLang ; ++i)
	{
		if (strcmp(extNameNarrow, _externalLangArray[i]->_name.c_str()) == 0)
			return i;
	}
	return -1;
}

const UserLangContainer* NppParameters::getULCFromName(const wchar_t* userLangName) const
{
	for (int i = 0 ; i < _nbUserLang ; ++i)
	{
		if (userLangName == _userLangArray[i]->_name)
			return _userLangArray[i].get();
	}

	//qui doit etre jamais passer
	return nullptr;
}

QRgb NppParameters::getCurLineHilitingColour()
{
	const Style* pStyle = _widgetStyleArray.findByName(L"Current line background colour");
	if (!pStyle)
		return static_cast<QRgb>(-1);
	return pStyle->_bgColor;
}

void NppParameters::setCurLineHilitingColour(QRgb colour2Set)
{
	Style * pStyle = _widgetStyleArray.findByName(L"Current line background colour");
	if (!pStyle)
		return;
	pStyle->_bgColor = colour2Set;
}

// npp-qt: EnumFontFamExProc removed — Win32 font enumeration replaced by QFontDatabase
// static int CALLBACK EnumFontFamExProc(...) { ... }

void NppParameters::setFontList(QWidget* /*hWnd*/)
{
	//---------------//
	// Sys font list //
	//---------------//
	_fontlist.clear();
	_fontlist.reserve(64); // arbitrary
	_fontlist.emplace_back(L"");

	// Qt6: QFontDatabase enumerates available fonts without a DC.
	for (const QString &family : QFontDatabase().families()) { _fontlist.push_back(family.toStdWString()); }
}

bool NppParameters::isInFontList(const std::wstring& fontName2Search) const
{
	if (fontName2Search.empty())
		return false;

	for (size_t i = 0, len = _fontlist.size(); i < len; i++)
	{
		if (_fontlist[i] == fontName2Search)
			return true;
	}
	return false;
}

void NppParameters::getLangKeywordsFromXmlTree()
{
	NppXml::Element root = NppXml::firstChildElement(_pXmlDoc._doc, "NotepadPlus");
	if (root.isNull()) return;
	updateFromModelXml(root, ConfXml::lang);	// updateKeyWordsFromModelXml(root);
	feedKeyWordsParameters(root);
}

void NppParameters::getExternalLexerFromXmlTree(NppXml::Document externalLexerDoc)
{
	NppXml::Element root = NppXml::firstChildElement(externalLexerDoc, "NotepadPlus");
	if (root.isNull()) return;
	feedKeyWordsParameters(root);
	feedStylerArray(root);
}

int NppParameters::addExternalLangToEnd(std::unique_ptr<ExternalLangContainer> externalLang)
{
	_externalLangArray[_nbExternalLang] = std::move(externalLang);
	++_nbExternalLang;
	++L_END;
	return _nbExternalLang - 1;
}

bool NppParameters::getUserStylersFromXmlTree()
{
	NppXml::Element root = NppXml::firstChildElement(_pXmlUserStylerDoc._doc, "NotepadPlus");
	if (root.isNull())
		return false;

	addDefaultStyles(root);	// make sure that GlobalStyles > WidgetStyles has certain elements, allowing defaults to be populated based on other existing WidgetStyles if needed
	updateFromModelXml(root, ConfXml::styles);	// look for any WidgetStyles or LexerType>WordsStyles that are missing in the current XML and populate from Model if needed
	return feedStylerArray(root);	// transfer the XML data structure into Notepad++'s internal data structure
}

bool NppParameters::updateFromModelXml(NppXml::Element& rootUser, ConfXml whichConf)
{
	// Determine conf-specific information first
	std::wstring modelXmlFilename;
	NppXml::Document pXmlDocument = nullptr;
	std::string mainElementName;
	std::wstring docPath;
	switch (whichConf)
	{
		case ConfXml::lang:
		{
			modelXmlFilename = L"langs.model.xml";
			pXmlDocument = _pXmlDoc._doc;
			docPath = _pXmlDoc._path;
			mainElementName = "Languages";
			break;
		}
		case ConfXml::styles:
		{
			modelXmlFilename = L"stylers.model.xml";
			pXmlDocument = _pXmlUserStylerDoc._doc;
			docPath = _pXmlUserStylerDoc._path;
			mainElementName = "LexerStyles";
			break;
		}
		default:
		{
			// if it's an unknown config file, return immediately, as there's nothing to do
			return false;
		}
	}

	// Need the name of the XML model document
	std::wstring modelXmlPath(_nppPath);
	pathAppend(modelXmlPath, modelXmlFilename);

	// compare the *.model.xml's filesystem "modified" timestamp (date only) to the value stored in the user file modelFileLastModifiedDate attribute
	const int userModelLastModifDate = NppXml::intAttribute(rootUser, "modelFileLastModifiedDate", 0);

	// read the actual timestamp from the model file; if there's a problem reading the attributes, just exit out (don't need to warn the user, since the main XML has already been loaded)
#ifdef _WIN32
	WIN32_FILE_ATTRIBUTE_DATA attributes{};
	// GetFileAttributesExW stubbed — use QFileInfo
		return false;

	int modifiedDate = 0;
	if (!fileTimeToYMD(attributes.ftLastWriteTime, modifiedDate))
		return false;
#else
	// Qt6: use QFileInfo to get modification date
	QFileInfo modelInfo(QString::fromStdWString(modelXmlPath));
	if (!modelInfo.exists() || !modelInfo.isReadable())
		return false;
	int modifiedDate = modelInfo.lastModified().date().year() * 10000
		+ modelInfo.lastModified().date().month() * 100
		+ modelInfo.lastModified().date().day();
#endif

	// if modifiedDate is not later than user stored model timestamp, no need to check more.
	// Note: in case of absence of attribute "modelModifDate", userModelModifTimestamp will be 0
	if (userModelLastModifDate >= modifiedDate)
		return false;

	// update immediately the modelModifDate stored in the active XML
	NppXml::setAttribute(rootUser, "modelFileLastModifiedDate", modifiedDate);

	// At this point, need to parse the model file
	// if there's a problem loading the model XML, just exit out (don't need to warn the user, since the main XML has already been loaded
	// the same logic will be used for any other errors while trying to do this XML merge)
	NppXml::Document pXmlModel = NppXml::NewDocument();

	auto handleErrorThenExit = [&]() {
		switch (whichConf)
		{
			case ConfXml::lang:
			{
				static_cast<void>(NppXml::saveFile(pXmlDocument, docPath.c_str()));
				break;
			}
			case ConfXml::styles:
			{
				writeStyles(_lexerStylerVect, _widgetStyleArray);
				break;
			}
		}

		delete pXmlModel;
		return false;
	};

	if (!NppXml::loadFile(pXmlModel, modelXmlPath.c_str()))
	{
		return handleErrorThenExit();
	}

	NppXml::Element rootModel = NppXml::firstChildElement(pXmlModel, "NotepadPlus");
	if (!rootModel)
	{
		return handleErrorThenExit();
	}

	// compare the *.model.xml's modelDate to that of the active XML
	const int v_model = NppXml::intAttribute(rootModel, "modelDate", 0);

	if (v_model == 0) // in case modelDate is absent in *.model.xml, no update will happen
	{
		return handleErrorThenExit();
	}

	const int v_user = NppXml::intAttribute(rootUser, "modelDate", 0);

	// v_user is 0 when "modelDate" is absent.
	if (v_user >= v_model)
	{
		return handleErrorThenExit();
	}

	// update (or add) the modelDate stored in the active XML
	NppXml::setAttribute(rootUser, "modelDate", v_model);

	// get the main internal <Languages> element from both user and model
	NppXml::Element mainElemUser = NppXml::firstChildElement(rootUser, mainElementName.c_str());
	NppXml::Element mainElemModel = NppXml::firstChildElement(rootModel, mainElementName.c_str());
	if (!mainElemUser || !mainElemModel)
	{
		return handleErrorThenExit();
	}

	switch (whichConf)
	{
		case ConfXml::lang:
		{
			updateLangXml(mainElemUser, mainElemModel);
			static_cast<void>(NppXml::saveFile(pXmlDocument, docPath.c_str()));
			break;
		}
		case ConfXml::styles:
		{
			updateStylesXml(rootUser, docPath, rootModel, mainElemUser, mainElemModel);
			writeStyles(_lexerStylerVect, _widgetStyleArray);
			break;
		}
	}

	delete pXmlModel;
	return true;
}

void NppParameters::updateLangXml(NppXml::Element& mainElemUser, const NppXml::Element& mainElemModel)
{
	// map each of the user-file's languages -> element-pointer, to keep track of the languages already in the user-file
	std::map<std::string, NppXml::Element> mapUserLanguages{};
	for (NppXml::Element langFromUser = NppXml::firstChildElement(mainElemUser, "Language");
		!langFromUser.isNull();
		langFromUser = NppXml::nextSiblingElement(langFromUser, "Language"))
	{
		QString languageName = NppXml::attribute(langFromUser, "name");
		if (!languageName.isEmpty())
			mapUserLanguages[languageName.toStdString()] = langFromUser;
	}

	// for each language in the Model,
	for (NppXml::Element langFromModel = NppXml::firstChildElement(mainElemModel, "Language");
		!langFromModel.isNull();
		langFromModel = NppXml::nextSiblingElement(langFromModel, "Language"))
	{
		QString modelLanguageName = NppXml::attribute(langFromModel, "name", "");
		if (modelLanguageName.isEmpty())
			continue;

		// see if language already exists in UserLanguages
		if (mapUserLanguages.contains(modelLanguageName.toStdString()))
		{
			// if so, see if I need to update individual entries

			// first, enumerate each keywords name -> element pointer, so I know what's already there
			std::map<std::string, NppXml::Element> mapUserKeywords{};
			for (NppXml::Element keywordsFromUser = NppXml::firstChildElement(mapUserLanguages[modelLanguageName.toStdString()], "Keywords");
				!keywordsFromUser.isNull();
				keywordsFromUser = NppXml::nextSiblingElement(keywordsFromUser, "Keywords"))
			{
				QString keywordsName = NppXml::attribute(keywordsFromUser, "name");
				if (!keywordsName.isEmpty())
					mapUserKeywords[keywordsName.toStdString()] = keywordsFromUser;
			}

			// then, for each Keywords entry in the model, check to see if it already exists in the user list
			for (NppXml::Element keywordsFromModel = NppXml::firstChildElement(langFromModel, "Keywords");
				!keywordsFromModel.isNull();
				keywordsFromModel = NppXml::nextSiblingElement(keywordsFromModel, "Keywords"))
			{
				QString modelKwName = NppXml::attribute(keywordsFromModel, "name");
				QString modelKeywordsNameQ = modelKwName;
				if (modelKeywordsNameQ.isEmpty())
					continue;

				// does this Keywords element exist in User already?
				if (mapUserKeywords.contains(modelKeywordsNameQ.toStdString()))
				{
					// if Keywords element in user langs.xml, need to check to see if any words are missing from its contents

					// start by extracting the list of words in the user version of this Keywords element
					NppXml::Node pKwsValue = NppXml::firstChild(mapUserKeywords[modelKeywordsNameQ.toStdString()]);
					std::string sText = !pKwsValue.isNull() ? NppXml::value(pKwsValue) : "";
					std::vector<std::string> vsUserWords{};
					std::map<std::string, bool> mapUserWords{};
					if (!sText.empty())
					{
						std::string sToken;
							std::istringstream strm(sText);
						while (strm >> sToken)
						{
							vsUserWords.push_back(sToken);
							mapUserWords[sToken] = true;
						}
					}

					// then go through each word in the model, and add it to the list if it's not already there
					int nWordsAdded = 0;
					NppXml::Node pKwsValueModel = NppXml::firstChild(keywordsFromModel);
					QString sTextModelQ = !pKwsValueModel.isNull() ? NppXml::value(pKwsValueModel) : QString();
					if (pKwsValue.isNull())
					{
						if (pKwsValueModel.isNull())
						{
							NppXml::insertEndChild(mapUserKeywords[modelKeywordsNameQ.toStdString()], pKwsValueModel);
						}
					}
					else
					{
						if (!sTextModelQ.isEmpty())
						{
							std::string sToken;
							std::istringstream strm(sTextModelQ.toStdString());
							while (strm >> sToken)
							{
								if (!mapUserWords.contains(sToken))
								{
									vsUserWords.push_back(sToken);
									++nWordsAdded;
								}
							}
						}

						// if there were any words added to the list, need to update the element contents
						if (nWordsAdded)
						{
							// sort the words in standard case-sensitive alphabetical order
							std::sort(vsUserWords.begin(), vsUserWords.end());

							// convert that list into space-separated string, with at most 8000 characters per line
							size_t lineLength = 0;
							static constexpr size_t maxLineLength = 8000;
							bool first = true;
							std::string sOutputWords;
							for (const auto& sWord : vsUserWords)
							{
								if (!first)
								{
									// space between words
									sOutputWords += " ";
									lineLength += 1;
								}
								first = false;

								if (lineLength + sWord.length() >= maxLineLength)
								{
									// start next line
									lineLength = 0;
									sOutputWords += "\n                ";
								}

								// add this word to the output string
								sOutputWords += sWord;
								lineLength += sWord.length();
							}

							// and update the XML's value
							NppXml::setValue(pKwsValue.toElement(), sOutputWords);
						}
					}
				}
				else
				{
					// if this Keywords element doesn't exist in user list, need to clone it from model to the right parent language in the user list
					NppXml::insertEndChild(mapUserLanguages[modelLanguageName.toStdString()], keywordsFromModel);
				}
			}

			// Also, since <Language name="..." ...> can have other attributes, need to check to make sure that
			// the user langs copy of language isn't missing any of the attributes from the model
			NppXml::Element thisLanguageFromUser = mapUserLanguages[modelLanguageName.toStdString()];
			for (NppXml::Attribute attrModel = NppXml::firstAttribute(langFromModel);				!attrModel.isNull();
				attrModel = NppXml::next(attrModel))
			{
				// if attribute not in user, need to add it (but leave it alone if the user-langs has it, but is just an empty string, because that's intentionally blank)
				const char* attrName = NppXml::name(attrModel);
				QString pcUserValue = NppXml::attribute(thisLanguageFromUser, attrName);
				if (pcUserValue.isEmpty())
					NppXml::setAttribute(thisLanguageFromUser, attrName, NppXml::value(attrModel));
				else if (std::strcmp(attrName, "ext"))
				{
					// Get both user and model values for the ext attribute
					QString sExtValuesQ = pcUserValue + " " + QString::fromLatin1(NppXml::value(attrModel));
					std::string sExtUpdated;
					std::map<std::string, bool> isExtDone{};
					std::string sToken;
					std::istringstream strm(sExtValuesQ.toStdString());
					while (strm >> sToken)
					{
						if (!isExtDone.contains(sToken))
						{
							if (!sExtUpdated.empty())
								sExtUpdated += " ";
							sExtUpdated += sToken;
							isExtDone[sToken] = true;
						}
					}
					NppXml::setAttribute(thisLanguageFromUser, attrName, QString::fromStdString(sExtUpdated));
				}
			}
		}
		else
		{
			// otherwise, since Language doesn't exist in User Languages, need to duplicate/clone from model to user-langs structure
			NppXml::insertEndChild(mainElemUser, langFromModel);
		}
	}

	return;
}

void NppParameters::updateStylesXml(const NppXml::Element&, const std::wstring&, const NppXml::Element&, NppXml::Element&, const NppXml::Element&)
{
	// Stub: style merging from model to user XML — no-op for Qt6 build
}


void NppParameters::initMenuKeys()
{
	// Stub: initialize menu key shortcuts — no-op for Qt6
}

void NppParameters::initScintillaKeys()
{
	// Stub: initialize scintilla key commands — no-op for Qt6
}

bool NppParameters::reloadContextMenuFromXmlTree(QMenuBar*, QMenu*)
{
	return false;
}

int NppParameters::getCmdIdFromMenuEntryItemName(QMenu*, const std::wstring&, const std::wstring&)
{
	return -1;
}

int NppParameters::getPluginCmdIdFromMenuEntryItemName(QMenu*, const std::wstring&, const std::wstring&)
{
	return -1;
}

bool NppParameters::getContextMenuFromXmlTree(QMenuBar*, QMenu*, bool)
{
	return false;
}

void NppParameters::setWorkingDir(const wchar_t*)
{
}

bool NppParameters::loadSession(Session& session, const wchar_t* sessionFileName, const bool bSuppressErrorMsg)
{
	NppXml::Document pXmlSessionDocument = NppXml::NewDocument();
	bool loadOkay = NppXml::loadFile(pXmlSessionDocument, sessionFileName);
	if (loadOkay)
		loadOkay = getSessionFromXmlTree(pXmlSessionDocument, session);

	if (!loadOkay && !bSuppressErrorMsg)
	{
		_pNativeLangSpeaker->messageBox("SessionFileInvalidError", nullptr, QString::fromWCharArray(L"Session file is either corrupted or not valid."), QString::fromWCharArray(L"Could not Load Session"),
			QMessageBox::Ok);
	}

	delete pXmlSessionDocument;
	return loadOkay;
}

bool NppParameters::getSessionFromXmlTree(const NppXml::Document& pSessionDoc, Session& session)
{
	return false;
}

void NppParameters::feedFileListParameters(const NppXml::Element& element)
{
}

void NppParameters::feedFileBrowserParameters(const NppXml::Element& element)
{
}

void NppParameters::feedProjectPanelsParameters(const NppXml::Element& element)
{
}

void NppParameters::feedColumnEditorParameters(const NppXml::Element& element)
{
}

void NppParameters::feedFindHistoryParameters(const NppXml::Element& element)
{
}

void NppParameters::feedShortcut(const NppXml::Element& element)
{
}

void NppParameters::feedMacros(const NppXml::Element& element)
{
}

void NppParameters::getActions(const NppXml::Element& element, Macro& macro)
{
}

void NppParameters::feedUserCmds(const NppXml::Element& element)
{
}

void NppParameters::feedScintKeys(const NppXml::Element& element)
{
}

void NppParameters::feedUserKeywordList(const NppXml::Element& element)
{
}

void NppParameters::feedUserStyles(const NppXml::Element& element)
{
}

bool NppParameters::feedStylerArray(const NppXml::Element& element)
{
	return false;
}

void NppParameters::feedKeyWordsParameters(const NppXml::Element& element)
{
}

void NppParameters::feedGUIParameters(const NppXml::Element& element)
{
}

void NppParameters::feedScintillaParam(const NppXml::Element& element)
{
}


void NppParameters::feedDockingManager(const NppXml::Element& element)
{
}

bool NppParameters::writeScintillaParams()
{
	return false;
}

void NppParameters::createXmlTreeFromGUIParams()
{
}


bool NppParameters::writeFindHistory()
{
	return true;
}

void NppParameters::insertDockingParamNode(NppXml::Element& GUIRoot) const
{
}

void NppParameters::writePrintSetting(NppXml::Element& element) const
{
}

void NppParameters::writeExcludedLangList(NppXml::Element& element) const
{
}

int NppParameters::langTypeToCommandID(LangType lt) const
{
	return -1;
}

bool NppParameters::insertTabInfo(const wchar_t* langName, int tabInfo, bool backspaceUnindent)
{
	return false;
}

void NppParameters::writeStyle2Element(const Style& style2Write, Style& style2Sync, NppXml::Element& element)
{
}

void NppParameters::insertUserLang2Tree(NppXml::Element& node, const UserLangContainer* userLang)
{
}

void NppParameters::addUserModifiedIndex(size_t index)
{
}

void NppParameters::addPluginModifiedIndex(size_t index)
{
}

void NppParameters::addScintillaModifiedIndex(int index)
{
}

void NppParameters::setUdlXmlDirtyFromIndex(size_t i)
{
}

void NppParameters::removeIndexFromXmlUdls(size_t i)
{
}

void NppParameters::setUdlXmlDirtyFromXmlDoc(const NppXml::Document& xmlDoc)
{
}

void NppParameters::initTabCustomColors()
{
}

void NppParameters::setIndividualTabColor(QRgb colour2Set, int colourIndex, bool doUpdate)
{
}

void NppParameters::initFindDlgStatusMsgCustomColors()
{
}

void NppParameters::setFindDlgStatusMsgIndexColor(QRgb colour2Set, int colourIndex)
{
}

void NppParameters::buildGupParams(std::wstring& params)
{
}

void NppParameters::writeNeed2SaveUDL()
{
}



bool NppParameters::writeColumnEditorSettings() {}
void NppParameters::writeShortcuts() {}


bool NppParameters::writeProjectPanelsSettings() { return true; }



// === CmdLineParamsDTO implementation ===
CmdLineParamsDTO CmdLineParamsDTO::FromCmdLineParams(const CmdLineParams& params)
{
    CmdLineParamsDTO dto;
    dto._isReadOnly = params._isReadOnly;
    dto._isNoSession = params._isNoSession;
    dto._isSessionFile = params._isSessionFile;
    dto._isRecursive = params._isRecursive;
    dto._openFoldersAsWorkspace = params._openFoldersAsWorkspace;
    dto._monitorFiles = params._monitorFiles;
    dto._line2go = params._line2go;
    dto._column2go = params._column2go;
    dto._pos2go = params._pos2go;
    dto._langType = params._langType;

    if (!params._udlName.empty()) {
        wcsncpy(dto._udlName, params._udlName.c_str(), 259);
        dto._udlName[259] = L'\0';
    }
    if (!params._pluginMessage.empty()) {
        wcsncpy(dto._pluginMessage, params._pluginMessage.c_str(), 2047);
        dto._pluginMessage[2047] = L'\0';
    }
    return dto;
}

// === MORE STUBS ===
std::wstring NppParameters::writeStyles(LexerStylerArray&, StyleArray&) { return {}; }
void NppParameters::addDefaultStyles(const NppXml::Element&) {}
std::pair<unsigned char, unsigned char> NppParameters::addUserDefineLangsFromXmlTree(NppXml::Document) { return {}; }
bool NppParameters::getUserParametersFromXmlTree() { return true; }
bool NppParameters::getShortcutsFromXmlTree() { return true; }
bool NppParameters::getMacrosFromXmlTree() { return true; }
bool NppParameters::getUserCmdsFromXmlTree() { return true; }
bool NppParameters::getScintKeysFromXmlTree() { return true; }
void NppParameters::generateXmlFromScratch(const wchar_t* filePath, const char* xmlContent)
{
	QFile f(QString::fromWCharArray(filePath));
	// Truncate ensures we overwrite any existing (possibly stale/corrupt) file
	if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		f.write(xmlContent);
		f.close();
		fprintf(stderr, "DBG: generateXmlFromScratch wrote %zd bytes to %ls\n", strlen(xmlContent), filePath);
	}
	else
	{
		fprintf(stderr, "DBG: generateXmlFromScratch FAILED to write %ls: %s\n", filePath, qPrintable(f.errorString()));
	}
	fflush(stderr);
}
