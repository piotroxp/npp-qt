// NppSciCompat.h — Scintilla/QScintilla compatibility constants
//
// Defines SCI_*, SC_MARK_*, INDIC_*, CARETSTYLE_*, SCLEX_* constants
// that were available via Scintilla.h macros in the original Win32 build.
//
// In Qt6, QScintilla defines these as anonymous-enum values inside
// QsciScintillaBase (class scope). We replicate them here as constexpr
// so bare names (e.g. SCI_GETCURRENTPOS) compile without qualification.
//
// Include this in any file that uses bare SCI_* constants.
#pragma once

// NOTE: We include qsciscintillabase.h to verify QScintilla is available.
// DO NOT use `using QsciScintillaBase::SCI_*` here — those enum values conflict
// with our constexpr npp_sci definitions when both are brought into global scope.
// Use the npp_sci constexpr values exclusively.
#include <Qsci/qsciscintillabase.h>

// Some Scintilla.h macros conflict with QsciScintillaBase anonymous-enum values
// (SC_UPDATE_*, SCEN_*, SCK_*). We undef them BEFORE including Scintilla.h so the
// constexpr definitions below don't get macro-expanded. The Qt enum values are
// accessible via QsciScintillaBase::NAME or via the npp_sci namespace.
#undef SC_UPDATE_NONE
#undef SC_UPDATE_CONTENT
#undef SC_UPDATE_SELECTION
#undef SC_UPDATE_V_SCROLL
#undef SC_UPDATE_H_SCROLL
#undef SCEN_CHANGE
#undef SCEN_SETFOCUS
#undef SCEN_KILLFOCUS
#undef SCK_DOWN
#undef SCK_UP
#undef SCK_LEFT
#undef SCK_RIGHT
#undef SCK_HOME
#undef SCK_END
#undef SCK_PRIOR
#undef SCK_NEXT
#undef SCK_DELETE
#undef SCK_INSERT
#undef SCK_ESCAPE
#undef SCK_BACK
#undef SCK_TAB
#undef SCK_RETURN
#undef SCK_ADD
#undef SCK_SUBTRACT
#undef SCK_DIVIDE

namespace npp_sci {
    // ==== QsciScintillaBase enum values (replicated as constexpr for bare access) ====
    constexpr int SCI_GETCURRENTPOS         = 2008;
    constexpr int SCI_GETLINECOUNT          = 2154;
    constexpr int SCI_GETLINE               = 2153;
    constexpr int SCI_GETCOLUMN             = 2129;
    constexpr int SCI_GETCHARAT             = 2007;
    constexpr int SCI_GETSTYLEAT            = 2010;
    constexpr int SCI_GETLENGTH             = 2006;
    constexpr int SCI_GETTEXTRANGE          = 2162;
    constexpr int SCI_LINEFROMPOSITION       = 2166;
    constexpr int SCI_POSITIONFROMLINE       = 2167;
    constexpr int SCI_GETLINEENDPOSITION     = 2136;
    constexpr int SCI_SEARCHINTARGET         = 2197;
    constexpr int SCI_SETTARGETSTART         = 2190;
    constexpr int SCI_SETTARGETEND           = 2192;
    constexpr int SCI_GETTARGETSTART         = 2191;
    constexpr int SCI_GETTARGETEND           = 2193;
    constexpr int SCI_INDICATORFILLRANGE      = 2504;
    constexpr int SCI_SETINDICATORCURRENT     = 2500;
    constexpr int SCI_GETINDENTATIONGUIDES    = 2133;
    constexpr int SCI_BRACEHIGHLIGHT          = 2351;
    constexpr int SCI_AUTOCCANCEL             = 2101;

    // ==== NOT in QScintilla — values from Scintilla spec ====
    constexpr int SCI_GETCURRENTLINE          = 2169;
    constexpr int SCI_CALLTIPACTIVE           = 2201;
    constexpr int SCI_CALLTIPCANCEL            = 2202;
    constexpr int SCI_CALLTIPSHOW              = 2203;
    constexpr int SCI_ENSUREVISIBLE            = 2232;
    constexpr int SCI_GOTOLINE                 = 2024;
    constexpr int SCI_GOTOPOS                  = 2025;
    constexpr int SCI_SETBRACKETLIGHTLINE      = 2098;
    constexpr int SCI_SETMODEVENTMASK          = 2368;
    constexpr int SCI_POSITIONAFTER            = 2417;
    constexpr int SCI_POSITIONBEFORE           = 2418;
    constexpr int SCI_SETLAZYEND               = 2396;

    // SC_MARK_* values NOT in QScintilla
    constexpr int SC_MARK_DOT                  = 22;

    // INDIC_* values NOT in QScintilla
    constexpr int INDIC_HYPERLINK              = 5;
    constexpr int INDIC_SPELL                  = 8;

    // CARETSTYLE_* NOT in QScintilla
    constexpr int CARETSTYLE_LINE_1            = 0x101;

    // ==== Print options (Scintilla spec) ====
    constexpr int SC_PRINT_NORMAL              = 0;
    constexpr int SC_PRINT_REVERSE_COLOUR     = 1;
    constexpr int SC_PRINT_BLACKONWHITE        = 2;
    constexpr int SC_PRINT_COLOURONWHITE        = 3;
    constexpr int SC_PRINT_COLOURONWHITEDEFAULTBG = 4;
    constexpr int SC_PRINT_BLACKONBLUE          = 5;
    constexpr int SC_PRINT_SCREENCOLOURS       = 6;

    // ==== User defined language keyword lists ====
    constexpr int SCE_USER_KWLIST_COMMENTS      = 0;
    constexpr int SCE_USER_KWLIST_DELIMITERS    = 1;
    constexpr int SCE_USER_KWLIST_KEYWORDS1    = 2;
    constexpr int SCE_USER_KWLIST_KEYWORDS2    = 3;
    constexpr int SCE_USER_KWLIST_KEYWORDS3    = 4;
    constexpr int SCE_USER_KWLIST_KEYWORDS4    = 5;
    constexpr int SCE_USER_KWLIST_KEYWORDS5    = 6;
    constexpr int SCE_USER_KWLIST_KEYWORDS6    = 7;
    constexpr int SCE_USER_KWLIST_KEYWORDS7    = 8;
    constexpr int SCE_USER_KWLIST_KEYWORDS8    = 9;
    constexpr int SCE_USER_KWLIST_KEYWORDS9    = 10;
    constexpr int SCE_USER_KWLIST_TOTAL        = 28;
    constexpr int SCE_USER_TOTAL_KEYWORD_GROUPS = 8;

    // ==== Scintilla modification event types ====
    constexpr int SC_MOD_INSERTTEXT            = 0x0001;
    constexpr int SC_MOD_DELETETEXT            = 0x0002;
    constexpr int SC_MOD_CHANGEFIRST           = 0x0004;
    constexpr int SC_MOD_CHANGELAST            = 0x0008;
    constexpr int SC_MOD_CHANGEMARKER          = 0x0010;
    constexpr int SC_MOD_CHANGEFOLD            = 0x0020;
    constexpr int SC_MOD_PERFORMED_UNDO        = 0x0200;
    constexpr int SC_MOD_PERFORMED_REDO        = 0x0400;
    constexpr int SC_MOD_CHANGEINDICATOR       = 0x4000;

    // Aliases matching original N++ source naming
    constexpr int SC_PERFORMED_UNDO            = SC_MOD_PERFORMED_UNDO;
    constexpr int SC_PERFORMED_REDO            = SC_MOD_PERFORMED_REDO;

    // ==== Windows PE machine types (for archType()) ====
    constexpr int IMAGE_FILE_MACHINE_I386   = 0x014c;
    constexpr int IMAGE_FILE_MACHINE_AMD64  = 0x8664;
    constexpr int IMAGE_FILE_MACHINE_ARM64  = 0xaa64;

    // ==== ARCH_TYPE for Linux/x86_64 ====
    constexpr int ARCH_TYPE = IMAGE_FILE_MACHINE_AMD64;  // x86_64

    // ==== SCLEX_* values NOT in QScintilla ====
    constexpr int SCLEX_INNO                = 73;
    constexpr int SCLEX_OBJECTIVEC           = 42;

    // ==== SC_UPDATE_* / SCEN_* / SCK_* — undef'd above, now define as constexpr ====
    // These use the same values as QsciScintillaBase anonymous enum.
    constexpr int SC_UPDATE_NONE       = 0x0;
    constexpr int SC_UPDATE_CONTENT    = 0x1;
    constexpr int SC_UPDATE_SELECTION  = 0x2;
    constexpr int SC_UPDATE_V_SCROLL   = 0x4;
    constexpr int SC_UPDATE_H_SCROLL   = 0x8;
    constexpr int SCEN_CHANGE   = 768;
    constexpr int SCEN_SETFOCUS = 512;
    constexpr int SCEN_KILLFOCUS = 256;
    constexpr int SCK_HOME      = 304;
    constexpr int SCK_END       = 305;
    constexpr int SCK_PRIOR    = 306;
    constexpr int SCK_NEXT     = 307;
    constexpr int SCK_DELETE   = 308;
    constexpr int SCK_INSERT   = 309;
    constexpr int SCK_ESCAPE   = 0x1B;
    constexpr int SCK_BACK     = 0x08;
    constexpr int SCK_TAB       = 0x09;
    constexpr int SCK_RETURN   = 0x0D;

    // ==== Scintilla document/message macros used in Buffer.cpp ====
    constexpr int SCI_GETDOCPOINTER         = 2357;
    constexpr int SCI_SETDOCPOINTER         = 2358;
    constexpr int SCI_CREATEDOCUMENT        = 2354;
    constexpr int SCI_RELEASEDOCUMENT       = 2355;
    constexpr int SCI_ADDREFDOCUMENT        = 2356;
    constexpr int SCI_GETCHARACTERPOINTER    = 2457;
    constexpr int SCI_SETUNDOCOLLECTION     = 2426;
    constexpr int SCI_INDICATORCLEARRANGE   = 2506;
    constexpr int SCI_MARKERADD             = 2042;
    constexpr int SCI_MARKERDELETE          = 2044;
    constexpr int SCI_MARKERGET             = 2043;
    constexpr int SCI_MARKERDELETEALL       = 2045;
    // Scintilla virtual key codes
    constexpr int SCK_UP       = 0x65;  // 101
    constexpr int SCK_DOWN     = 0x66;  // 102
    constexpr int SCK_LEFT     = 0x67;  // 103
    constexpr int SCK_RIGHT    = 0x68;  // 104
    constexpr int SCK_ADD      = 0x6B;  // 107
    constexpr int SCK_SUBTRACT = 0x6D;  // 109
    constexpr int SCK_DIVIDE    = 0x6F;  // 111

    // More Scintilla messages needed by Buffer.cpp
    constexpr int SCI_SETSTATUS        = 2014;
    constexpr int SCI_GETREADONLY     = 2146;
    constexpr int SCI_SETREADONLY    = 2147;
    constexpr int SCI_CLEARALL       = 2004;
    constexpr int SCI_SETILEXER      = 2372;
    constexpr int SCI_SETCODEPAGE    = 2440;
    constexpr int SCI_APPENDTEXT      = 2006;
    constexpr int SCI_EMPTYUNDOBUFFER = 2060;
    constexpr int SCI_SETSAVEPOINT   = 2012;
    constexpr int SCI_LINESONSCREEN   = 2370;
    constexpr int SCI_GETSELECTIONEMPTY = 2650;
    constexpr int SCI_WORDSTARTPOSITION = 2266;
    constexpr int SCI_WORDENDPOSITION   = 2267;
    constexpr int SCI_AUTOCSETSEPARATOR = 2106;
    constexpr int SCI_AUTOCSETIGNORECASE = 2115;
    constexpr int SCI_AUTOCSETAUTOHIDE   = 2118;
    constexpr int SCI_AUTOCSETDROPRESTOFWORD = 2270;
    constexpr int SCI_REPLACESEL         = 2170;
    constexpr int SCI_SETSEL              = 2160;
    constexpr int SCI_DOCLINEFROMVISIBLE  = 2221;
    constexpr int SCI_REPLACETARGET       = 2194;
    constexpr int SCI_TARGETFROMSELECTION = 2287;
    constexpr int SCI_BEGINUNDOACTION      = 2078;
    constexpr int SCI_ENDUNDOACTION        = 2079;
    constexpr int SCI_INSERTTEXT           = 2003;

    // ==== Search flags (SCFIND_*) ====
    constexpr int SCFIND_MATCHCASE   = 4;
    constexpr int SCFIND_WHOLEWORD   = 2;
    constexpr int SCFIND_REGEXP      = 0x00200000;
    constexpr int SCFIND_POSIX       = 0x00400000;  // from Scintilla.iface
    constexpr int SCFIND_REGEXP_DOTMATCHESNL = 0x10000000;  // from BoostRegexSearch.h

    // ==== Universal styles ====
    constexpr int SCE_UNIVERSAL_FOUND_STYLE_SMART = 33;

    // ==== Missing constants needed by AutoCompletion, SmartHighlighter, FindReplaceDlg, EncodingMapper ====
    constexpr int SCI_AUTOCSHOW             = 2100;   // AutoCompletion.cpp
    constexpr int SCI_AUTOCGETSEPARATOR     = 2108;   // auto-completion separator
    constexpr int SCI_GETFIRSTVISIBLELINE  = 2152;   // SmartHighlighter.cpp
    constexpr int SCI_SETSEARCHFLAGS        = 2198;   // FindReplaceDlg.cpp
    // SC_CP_UTF8 defined globally in ScintillaComponent.h — no duplicate here

    // ==== Additional SCI_* constants needed by Parameters.cpp ====
    constexpr int SCI_SELECTALL              = 2013;
    constexpr int SCI_CLEAR                  = 2003;
    constexpr int SCI_UNDO                   = 2001;
    constexpr int SCI_REDO                   = 2002;
    constexpr int SCI_NEWLINE                = 2329;
    constexpr int SCI_TAB                    = 2327;
    constexpr int SCI_BACKTAB                = 2328;
    constexpr int SCI_FORMFEED               = 2330;
    constexpr int SCI_ZOOMIN                 = 2333;
    constexpr int SCI_ZOOMOUT                = 2334;
    constexpr int SCI_SETZOOM               = 2373;
    constexpr int SCI_GETZOOM               = 2374;
    constexpr int SCI_EDITTOGGLEOVERTYPE     = 2324;
    constexpr int SCI_MOVECARETINSIDEVIEW    = 2401;
    constexpr int SCI_LINEDOWN               = 2300;
    constexpr int SCI_LINEDOWNEXTEND         = 2302;
    constexpr int SCI_LINEDOWNRECTEXTEND     = 2427;
    constexpr int SCI_LINESCROLLDOWN         = 2340;
    constexpr int SCI_LINEUP                 = 2301;
    constexpr int SCI_LINEUPEXTEND           = 2303;
    constexpr int SCI_LINEUPRECTEXTEND       = 2428;
    constexpr int SCI_LINESCROLLUP           = 2341;
    constexpr int SCI_CHARLEFT               = 2304;
    constexpr int SCI_CHARLEFTEXTEND        = 2305;
    constexpr int SCI_CHARRIGHT             = 2306;
    constexpr int SCI_CHARRIGHTEXTEND       = 2307;
    constexpr int SCI_WORDLEFT              = 2268;
    constexpr int SCI_WORDLEFTEXTEND        = 2269;
    constexpr int SCI_WORDRIGHT             = 2270;
    constexpr int SCI_WORDRIGHTEXTEND       = 2271;
    constexpr int SCI_HOME                  = 2310;
    constexpr int SCI_HOMEEXTEND            = 2311;
    constexpr int SCI_LINEEND               = 2312;
    constexpr int SCI_LINEENDEXTEND        = 2313;
    constexpr int SCI_DOCUMENTSTART         = 2315;
    constexpr int SCI_DOCUMENTSTARTEXTEND  = 2316;
    constexpr int SCI_DOCUMENTEND           = 2317;
    constexpr int SCI_DOCUMENTENDEXTEND    = 2318;
    constexpr int SCI_PAGEUP               = 2320;
    constexpr int SCI_PAGEUPEXTEND         = 2321;
    constexpr int SCI_PAGEDOWN             = 2322;
    constexpr int SCI_PAGEDOWNEXTEND       = 2323;
    constexpr int SCI_CUT                   = 2172;
    constexpr int SCI_COPY                   = 2173;
    constexpr int SCI_PASTE                  = 2174;
    constexpr int SCI_GETSELTEXT             = 2161;
    constexpr int SCI_CANCEL                = 2325;
    constexpr int SCI_PARADOWN              = 2413;
    constexpr int SCI_PARADOWNEXTEND        = 2414;
    constexpr int SCI_PARAUP                = 2415;
    constexpr int SCI_PARAUPEXTEND          = 2416;
    constexpr int SCI_CHARLEFTRECTEXTEND    = 2428;
    constexpr int SCI_CHARRIGHTRECTEXTEND   = 2429;
    constexpr int SCI_WORDLEFTEND            = 2439;
    constexpr int SCI_WORDLEFTENDEXTEND      = 2440;
    constexpr int SCI_WORDRIGHTEND           = 2441;
    constexpr int SCI_WORDRIGHTENDEXTEND     = 2442;
    constexpr int SCI_WORDPARTLEFT           = 2390;
    constexpr int SCI_WORDPARTLEFTEXTEND     = 2391;
    constexpr int SCI_WORDPARTRIGHT          = 2392;
    constexpr int SCI_WORDPARTRIGHTEXTEND    = 2393;
    constexpr int SCI_HOMERECTEXTEND         = 2430;
    constexpr int SCI_HOMEDISPLAY            = 2345;
    constexpr int SCI_HOMEDISPLAYEXTEND      = 2346;
    constexpr int SCI_HOMEWRAP              = 2349;
    constexpr int SCI_HOMEWRAPEXTEND         = 2450;
    constexpr int SCI_VCHOME                 = 2331;
    constexpr int SCI_VCHOMEEXTEND           = 2332;
    constexpr int SCI_VCHOMERECTEXTEND        = 2431;
    constexpr int SCI_VCHOMEDISPLAY           = 2652;
    constexpr int SCI_VCHOMEDISPLAYEXTEND     = 2653;
    constexpr int SCI_VCHOMEWRAP             = 2453;
    constexpr int SCI_VCHOMEWRAPEXTEND        = 2454;
    constexpr int SCI_LINEENDWRAPEXTEND       = 2452;
    constexpr int SCI_LINEENDRECTEXTEND       = 2432;
    constexpr int SCI_LINEENDDISPLAY          = 2347;
    constexpr int SCI_LINEENDDISPLAYEXTEND    = 2348;

    // Style get foreground/background colour (2481, 2482)
    constexpr int SCI_STYLEGETFORE             = 2481;
    constexpr int SCI_STYLEGETBACK             = 2482;

    // Missing constants from Scintilla.h (not in QScintilla subset)
    // Scrolling / selection navigation
    constexpr int SCI_SCROLLCARET               = 2169;
    constexpr int SCI_PAGEUPRECTEXTEND         = 2433;
    constexpr int SCI_PAGEDOWNRECTEXTEND        = 2434;
    constexpr int SCI_STUTTEREDPAGEUP           = 2435;
    constexpr int SCI_STUTTEREDPAGEUPEXTEND     = 2436;
    constexpr int SCI_STUTTEREDPAGEDOWN         = 2437;
    constexpr int SCI_STUTTEREDPAGEDOWNEXTEND   = 2438;

    // Line/selection editing
    constexpr int SCI_LINESJOIN                 = 2288;
    constexpr int SCI_DELETEBACK                = 2326;
    constexpr int SCI_DELETEBACKNOTLINE         = 2344;
    constexpr int SCI_DELWORDLEFT               = 2335;
    constexpr int SCI_DELWORDRIGHT             = 2336;
    constexpr int SCI_DELWORDRIGHTEND          = 2518;
    constexpr int SCI_DELLINELEFT              = 2395;
    constexpr int SCI_DELLINERIGHT             = 2396;
    constexpr int SCI_LINECUT                  = 2337;
    constexpr int SCI_LINEDELETE               = 2338;
    constexpr int SCI_LINETRANSPOSE            = 2339;
    constexpr int SCI_LINEDUPLICATE           = 2404;
    constexpr int SCI_LINECOPY                = 2455;
    constexpr int SCI_SELECTIONDUPLICATE       = 2469;
    constexpr int SCI_ROTATESELECTION          = 2606;
    constexpr int SCI_SWAPMAINANCHORCARET      = 2607;

    // Line end types (added for NPP 8.6+)
    constexpr int SCI_LINEENDWRAP              = 2451;
}  // namespace npp_sci

// Bring npp_sci constants into global namespace as bare names
using namespace npp_sci;

// Win32 API shim — lstrcmp is a Windows-only API; map to strcmp on Unix
#ifndef lstrcmp
#define lstrcmp strcmp
#endif

// Bring all npp_sci constexpr constants into global namespace
// so bare SCI_* names (as used throughout the codebase) resolve correctly.
using namespace npp_sci;
