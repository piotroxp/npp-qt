// npp-qt: Notepad_plus messaging constants
// Original: PowerEditor/src/MISC/PluginsManager/Notepad_plus_msgs.h
// Mirrors Win32 NPPM_* messages (SendMessage targets) and LangType enum.
#pragma once

#include <QString>

// =============================================================================
// NPPM_* messages — sent via Sci_ReleaseDocument / Sci_GetDocumentPointer
// These are also used as custom Qt signals/slots via NppCustomEvent
// =============================================================================
inline constexpr intptr_t NPPM_GETFULLCURRENTPATH  = (0x0400 + 1);
inline constexpr intptr_t NPPM_GETCURRENTDIRECTORY  = (0x0400 + 2);
inline constexpr intptr_t NPPM_GETFILENAME         = (0x0400 + 3);
inline constexpr intptr_t NPPM_GETNAMEPART         = (0x0400 + 4);
inline constexpr intptr_t NPPM_GETEXTPART          = (0x0400 + 5);
inline constexpr intptr_t NPPM_GETCURRENTLINENUM    = (0x0400 + 6);
inline constexpr intptr_t NPPM_GETCURRENTCOLUMN     = (0x0400 + 7);
inline constexpr intptr_t NPPM_GETNBOPENFILES       = (0x0400 + 8);
inline constexpr intptr_t NPPM_GETNBFILES           = (0x0400 + 9);
inline constexpr intptr_t NPPM_LOADFILE             = (0x0400 + 10);
inline constexpr intptr_t NPPM_CLOSE                = (0x0400 + 11);
inline constexpr intptr_t NPPM_RELOADFILE           = (0x0400 + 12);
inline constexpr intptr_t NPPM_SAVEFILE              = (0x0400 + 13);
inline constexpr intptr_t NPPM_SAVEASFILE           = (0x0400 + 14);
inline constexpr intptr_t NPPM_SAVECURRENTFILE       = (0x0400 + 15);
inline constexpr intptr_t NPPM_SAVECURRENTFILEAS    = (0x0400 + 16);
inline constexpr intptr_t NPPM_GETCURRENTLANG       = (0x0400 + 17);
inline constexpr intptr_t NPPM_GETLANGNAME          = (0x0400 + 18);
inline constexpr intptr_t NPPM_ADDRECENTFILE       = (0x0400 + 19);
inline constexpr intptr_t NPPM_MENUHAPPENED        = (0x0400 + 20);
inline constexpr intptr_t NPPM_SETMENUITEMCHECK    = (0x0400 + 21);
inline constexpr intptr_t NPPM_DMMREGASICMD        = (0x0400 + 22);
inline constexpr intptr_t NPPM_DMMREGASDLG        = (0x0400 + 23);
inline constexpr intptr_t NPPM_CREATESCINTILLAHANDLE = (0x0400 + 24);
inline constexpr intptr_t NPPM_DESTROYSCINTILLAHANDLE = (0x0400 + 25);
inline constexpr intptr_t NPPM_GETCURRENTSCINTILLA = (0x0400 + 26);
inline constexpr intptr_t NPPM_GETAPPLYFUNC         = (0x0400 + 27);
inline constexpr intptr_t NPPM_GETVERSION           = (0x0400 + 28);
inline constexpr intptr_t NPPM_SETSTATUSBAR        = (0x0400 + 29);
inline constexpr intptr_t NPPM_GETFULLPATHFROMBUFFERID = (0x0400 + 40);
inline constexpr intptr_t NPPM_GETBUFFERIDFROMPOS  = (0x0400 + 41);
inline constexpr intptr_t NPPM_GETPOSFROMLINE      = (0x0400 + 42);
inline constexpr intptr_t NPPM_GETBUFFERIDOFPATH   = (0x0400 + 43);
inline constexpr intptr_t NPPM_RELOADBUFFERID      = (0x0400 + 44);
inline constexpr intptr_t NPPM_RELOADFILEPATH      = (0x0400 + 45);
inline constexpr intptr_t NPPM_INTERNAL_REFRESHDARKMODE = (0x0400 + 100);
inline constexpr intptr_t NPPM_INTERNAL_SETMENUITEMCHECK = (0x0400 + 101);
inline constexpr intptr_t NPPM_INTERNAL_ENABLEMENUITEM = (0x0400 + 102);
inline constexpr intptr_t NPPM_INTERNAL_EXECUTEMACRO = (0x0400 + 103);
inline constexpr intptr_t NPPM_HIDETABBAR          = (0x0400 + 111);
inline constexpr intptr_t NPPM_DMMASNOTIFY         = (0x0400 + 112);
inline constexpr intptr_t NPPM_SETPROGRESSBAR      = (0x0400 + 113);
inline constexpr intptr_t NPPM_SETMARGINBITS      = (0x0400 + 114);
inline constexpr intptr_t NPPM_GETTHEMEPATH       = (0x0400 + 115);
inline constexpr intptr_t NPPM_GETSETPLUGINSOPTION = (0x0400 + 116);
inline constexpr intptr_t NPPM_MAKECURRENTBUFFERDIRTY = (0x0400 + 117);
inline constexpr intptr_t NPPM_GETCURRENTBUFFERENCODING = (0x0400 + 118);
inline constexpr intptr_t NPPM_GETBUFFERENCODING   = (0x0400 + 119);
inline constexpr intptr_t NPPM_SETBUFFERENCODING  = (0x0400 + 120);
inline constexpr intptr_t NPPM_GETBUFFERFORMAT    = (0x0400 + 121);
inline constexpr intptr_t NPPM_SETBUFFERFORMAT    = (0x0400 + 122);
inline constexpr intptr_t NPPM_GETBUFFERLANG      = (0x0400 + 123);
inline constexpr intptr_t NPPM_SETBUFFERLANG      = (0x0400 + 124);
inline constexpr intptr_t NPPM_TARGETENCODINGCHANGE = (0x0400 + 125);
inline constexpr intptr_t NPPM_TARGETFORMATCHANGE  = (0x0400 + 126);
inline constexpr intptr_t NPPM_SETCOMPILER            = (0x0400 + 127);
inline constexpr intptr_t NPPM_GETCOMPILER            = (0x0400 + 128);
inline constexpr intptr_t NPPM_SETBUFFERDIRTYSTATE   = (0x0400 + 129);
inline constexpr intptr_t NPPM_ACTIVATEDOC          = (0x0400 + 130);
inline constexpr intptr_t NPPM_LAUNCHFINDINFILESDLG = (0x0400 + 131);
inline constexpr intptr_t NPPM_LAUNCHREPLACEINFILESDLG = (0x0400 + 132);
inline constexpr intptr_t NPPM_SWITCHTOFILE         = (0x0400 + 133);
inline constexpr intptr_t NPPM_SAVEMETAFILE         = (0x0400 + 134);
inline constexpr intptr_t NPPM_GETCARETINFO         = (0x0400 + 135);
inline constexpr intptr_t NPPM_HIDETOOLBAR          = (0x0400 + 136);
inline constexpr intptr_t NPPM_ISTABBARHIDDEN       = (0x0400 + 137);
inline constexpr intptr_t NPPM_GETSHORTCUTBYID      = (0x0400 + 138);
inline constexpr intptr_t NPPM_DOVISIBLETOOLBAR    = (0x0400 + 139);
inline constexpr intptr_t NPPM_CREATETOOLBAR       = (0x0400 + 140);
inline constexpr intptr_t NPPM_DESTROYTOOLBAR      = (0x0400 + 141);
inline constexpr intptr_t NPPM_PROGRESSBARSTART    = (0x0400 + 142);
inline constexpr intptr_t NPPM_PROGRESSBAREND       = (0x0400 + 143);
inline constexpr intptr_t NPPM_SETPROGRESSBARVALUE = (0x0400 + 144);
inline constexpr intptr_t NPPM_GETPLUGINSCONFIGDIR = (0x0400 + 145);
inline constexpr intptr_t NPPM_MSGTOPLUGIN        = (0x0400 + 146);
inline constexpr intptr_t NPPM_TRIGGERRECORDMACRO  = (0x0400 + 147);
inline constexpr intptr_t NPPM_TRIGGERPLAYBACKMACRO = (0x0400 + 148);
inline constexpr intptr_t NPPM_LOADSESSION        = (0x0400 + 149);
inline constexpr intptr_t NPPM_SAVESESSION        = (0x0400 + 150);
inline constexpr intptr_t NPPM_OPENFILEINENV      = (0x0400 + 151);
inline constexpr intptr_t NPPM_REFRESHTABBAR      = (0x0400 + 152);
inline constexpr intptr_t NPPM_SAVEFILEASANSI     = (0x0400 + 153);
inline constexpr intptr_t NPPM_GETFILERACINGREVERSEOPENED = (0x0400 + 154);
inline constexpr intptr_t NPPM_CLEARFILEFLAG      = (0x0400 + 155);
inline constexpr intptr_t NPPM_SETFILEFLAG       = (0x0400 + 156);
inline constexpr intptr_t NPPM_CLOSEAL              = (0x0400 + 157);
inline constexpr intptr_t NPPM_GETCURRENTDOCINDEX  = (0x0400 + 158);
inline constexpr intptr_t NPPM_REORDERBUFFERID    = (0x0400 + 159);
inline constexpr intptr_t NPPM_SHOWWINDOW         = (0x0400 + 160);
inline constexpr intptr_t NPPM_ISDOCPinned         = (0x0400 + 161);
inline constexpr intptr_t NPPM_PINSB             = (0x0400 + 162);
inline constexpr intptr_t NPPM_UNPINSB            = (0x0400 + 163);
inline constexpr intptr_t NPPM_PUSHILANDSCROLL    = (0x0400 + 164);
inline constexpr intptr_t NPPM_POPILANDSCROLL     = (0x0400 + 165);
inline constexpr intptr_t NPPM_ISTRANSPARENT    = (0x0400 + 166);
inline constexpr intptr_t NPPM_GETDARKMODE       = (0x0400 + 167);
inline constexpr intptr_t NPPM_DARKMODEITORTOGGLE = (0x0400 + 168);
inline constexpr intptr_t NPPM_DISABLEAUTOCLOSEBRACKETS = (0x0400 + 169);
inline constexpr intptr_t NPPM_ENABLEAUTOCLOSEBRACKETS  = (0x0400 + 170);

// RUNCOMMAND_USER base and sub-commands (used by NPPM_GETNPPDIRECTORY, GETCURRENTWORD, etc.)
// WM_USER = 0x0400; RUNCOMMAND_USER = WM_USER + 3000 = 0x0BB8
inline constexpr intptr_t RUNCOMMAND_USER      = 0x0400 + 3000;
inline constexpr intptr_t FULL_CURRENT_PATH    = 1;
inline constexpr intptr_t CURRENT_DIRECTORY    = 2;
inline constexpr intptr_t FILE_NAME            = 3;
inline constexpr intptr_t NAME_PART            = 4;
inline constexpr intptr_t EXT_PART             = 5;
inline constexpr intptr_t CURRENT_WORD         = 6;
inline constexpr intptr_t NPP_DIRECTORY        = 7;
inline constexpr intptr_t NPP_FULL_FILE_PATH   = 8;
inline constexpr intptr_t GETFILENAMEATCURSOR  = 9;
inline constexpr intptr_t CURRENT_LINESTR     = 10;
inline constexpr intptr_t CURRENT_LINE        = 11;
inline constexpr intptr_t CURRENT_COLUMN       = 12;
// NPPM_GETBUFFERIDFROMPATH lParam flags
inline constexpr intptr_t NPPREADING_OPEN         = 0;
inline constexpr intptr_t NPPREADING_FORCEREAD     = 1;
inline constexpr intptr_t NPPREADING_DONTOPENNEW   = 2;

// =============================================================================
// Buffer/file open modes
// =============================================================================
inline constexpr int NPPOPEN = 0;
inline constexpr int NPPSAVE = 1;

// =============================================================================
// Additional NPPM messages needed for source compatibility
// (added from notepad-plus-plus source notepad_plus_msgs.h)
// =============================================================================
inline constexpr intptr_t NPPM_SAVECURRENTSESSION         = (0x0400 + 171);
inline constexpr intptr_t NPPM_GETCURRENTBUFFERID          = (0x0400 + 172);
inline constexpr intptr_t NPPM_GETMENUHANDLE               = (0x0400 + 173);
inline constexpr intptr_t NPPM_DMMUPDATEDISPINFO           = (0x0400 + 174);
inline constexpr intptr_t NPPM_GETPOSFROMBUFFERID           = (0x0400 + 175);
inline constexpr intptr_t NPPM_GETBUFFERLANGTYPE           = (0x0400 + 176);
inline constexpr intptr_t NPPM_SETBUFFERLANGTYPE           = (0x0400 + 177);
inline constexpr intptr_t NPPM_CREATELEXER                  = (0x0400 + 178);
inline constexpr intptr_t NPPM_GETNPPFULLFILEPATH          = (RUNCOMMAND_USER + NPP_FULL_FILE_PATH);
inline constexpr intptr_t NPPM_GETFILENAMEATCURSOR         = (RUNCOMMAND_USER + GETFILENAMEATCURSOR);
inline constexpr intptr_t NPPM_ADDSCNMODIFIEDFLAGS          = (0x0400 + 179);
inline constexpr intptr_t NPPM_ADDTOOLBARICON_DEPRECATED   = (0x0400 + 41);
inline constexpr intptr_t NPPM_ADDTOOLBARICON_FORDARKMODE  = (0x0400 + 101);
inline constexpr intptr_t NPPM_TRIGGERTABBARCONTEXTMENU     = (0x0400 + 49);
inline constexpr intptr_t NPPM_ISTOOLBARHIDDEN             = (0x0400 + 71);
inline constexpr intptr_t NPPM_ISMENUHIDDEN                = (0x0400 + 73);
inline constexpr intptr_t NPPM_HIDESTATUSBAR               = (0x0400 + 74);
inline constexpr intptr_t NPPM_ISSTATUSBARHIDDEN           = (0x0400 + 75);
inline constexpr intptr_t NPPM_GETTOOLBARICONSETCHOICE     = (0x0400 + 118);

// =============================================================================
// Additional NPPM messages from original notepad-plus-plus source
inline constexpr intptr_t NPPM_GETNATIVELANGFILENAME = (0x0400 + 19);
inline constexpr intptr_t NPPM_GETNPPSETTINGSDIRPATH = (0x0400 + 35);
inline constexpr intptr_t NPPM_GETCURRENTNATIVELANGENCODING = (0x0400 + 37);
inline constexpr intptr_t NPPM_ALLOCATESUPPORTED_DEPRECATED = (0x0400 + 53);
inline constexpr intptr_t NPPM_ALLOCATECMDID = (0x0400 + 54);
inline constexpr intptr_t NPPM_ALLOCATEMARKER = (0x0400 + 55);
inline constexpr intptr_t NPPM_GETLANGUAGEDESC = (0x0400 + 84);
inline constexpr intptr_t NPPM_GETLANGUAGENAME = (0x0400 + 87);
inline constexpr intptr_t NPPM_HIDEMENU = (0x0400 + 89);
inline constexpr intptr_t NPPM_GETSHORTCUTBYCMDID = NPPM_GETSHORTCUTBYID;
inline constexpr intptr_t NPPM_INTERNAL_GETSCRATCHPADDIR = (0x0400 + 91);

// Custom Qt event type for Notepad++ inter-component messaging
// Used in place of Win32 RegisterWindowMessage/WM_COPYDATA
// =============================================================================
inline constexpr int NPP_CUSTOM_EVENT_BASE = 0x0400 + 2000;

// =============================================================================
// LangType enum — language identifiers for buffers
// Original: PowerEditor/src/MISC/PluginsManager/Notepad_plus_msgs.h
// =============================================================================

// enum class ExternalLexerAutoIndentMode — defined in NppConstants.h
enum class MacroStatus { Idle, RecordInProgress, RecordingStopped, PlayingBack };
