// BUILD_FIX_2026_07_14_marker_DO_NOT_REMOVE

// NppSciCompat.h — Scintilla/QScintilla compatibility constants
//
// Defines SCI_*, SC_MARK_*, INDIC_*, CARETSTYLE_*, SCLEX_* constants
// that were available via Scintilla.h macros in the original Win32 build.
//
// In Qt6, QScintilla defines these as anonymous-enum values inside
// QsciScintillaBase (class scope). We replicate them here as constexpr
// so bare names (e.g. SCI_GETCURRENTPOS) compile without qualification.
//
// Sci_Position / Sci_PositionCR must be defined before Sci_CharacterRangeFull which uses them.
#ifndef Sci_Position_defined
#define Sci_Position_defined
using Sci_Position = long;
using Sci_PositionCR = long;
#endif

struct Sci_CharacterRangeFull {
    Sci_Position cpMin;
    Sci_Position cpMax;
};


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
    constexpr int SCI_GETTEXT               = 2001;
    constexpr int SCI_SETTEXT               = 2002;
    constexpr int SCI_GETCURLINE            = 2012;
    constexpr int SCI_CONVERTEOLS           = 2030;
    constexpr int SCI_LINESSPLIT            = 2289;
    constexpr int SCI_LOWERCASE             = 2342;
    constexpr int SCI_UPPERCASE             = 2343;
    constexpr int SCI_TITLECASE             = 2344;
    constexpr int SCI_SORTLINES             = 2169;
    constexpr int SC_SORT_NUMERIC           = 1;
    constexpr int SC_SORT_CASEINSENSITIVE   = 2;
    constexpr int SC_SORT_PERCENTAGE        = 3;
    constexpr int SC_SORT_REVERSE           = 0x100;
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
    constexpr int SCI_CALLTIPSETHLT            = 2204;  // highlight Nth parameter
    constexpr int SCI_CALLTIPSETBACK           = 2205;  // set calltip background colour
    constexpr int SCI_CALLTIPSETFORE           = 2206;  // set calltip foreground colour
    constexpr int SCI_CALLTIPSETFOREHLT         = 2207;  // set highlighted parameter colour
    constexpr int SCI_CALLTIPSETOPTIONS         = 2210;  // calltip options
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

    // ==== Annotation and anchor (needed by IncrementalSearchDialog) ====
    constexpr int SCI_ANNOTATION_CLEARALL      = 2128;  // clear all annotations
    constexpr int SCI_ANCHOR                   = 2023;  // anchor position

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
    constexpr int SCFIND_WORDSTART   = 0x00000008;  // match at word start only

    // ==== Universal styles ====
    constexpr int SCE_UNIVERSAL_FOUND_STYLE_SMART = 33;

    // ==== Missing constants needed by AutoCompletion, SmartHighlighter, FindReplaceDlg, EncodingMapper ====
    constexpr int SCI_AUTOCSHOW             = 2100;   // AutoCompletion.cpp
    constexpr int SCI_AUTOCGETSEPARATOR     = 2108;   // auto-completion separator
    constexpr int SCI_GETFIRSTVISIBLELINE  = 2152;   // SmartHighlighter.cpp
    constexpr int SCI_SETSEARCHFLAGS        = 2198;   // FindReplaceDlg.cpp
    constexpr int SCI_FINDTEXT              = 2150;   // SmartHighlighter.cpp — search in document
    constexpr int SCI_INDICATORSTYLE        = 2080;   // SmartHighlighter.cpp — set indicator style
    constexpr int SCI_INDICSETFORE          = 2082;   // SmartHighlighter.cpp — indicator foreground color
    constexpr int SCI_INDICSETBACK          = 2083;   // SmartHighlighter.cpp — indicator background color
    // INDIC_ROUNDBOX, SC_INDICATOR_*, SC_CP_UTF8, SCI_GETSELECTION already in HEAD

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
    // Scrolling / line editing / EOL
    constexpr int SCI_LINESCROLL                = 2168;
    constexpr int SCI_SCROLLCARET               = 2169;
    constexpr int SCI_LINELENGTH                = 2350;
    constexpr int SCI_TEXTHEIGHT                = 2279;
    constexpr int SCI_TEXTWIDTH                 = 2276;
    constexpr int SCI_SETTARGETRANGE            = 2686;
    constexpr int SCI_COLOURISE               = 2613;
    constexpr int SCI_POSITIONFROMPOINT       = 2562;
    constexpr int SCI_POINTXFROMPOSITION      = 2167;
    constexpr int SCI_POINTYFROMPOSITION       = 2166;
    constexpr int SCI_GETWRAPINDENTMODE       = 2472;
    constexpr int SCI_SETWRAPINDENTMODE       = 2473;
    constexpr int SCI_FOLDDISPLAYTEXTSTYLE     = 2665;
    constexpr int SC_EOL_CRLF                  = 0;
    constexpr int SC_EOL_CR                    = 1;
    constexpr int SC_EOL_LF                    = 2;
    constexpr int SCI_GETEOLMODE               = 2030;
    constexpr int SCI_FINDCOLUMN               = 2456;
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

    // Additional Scintilla/QScintilla constants
    constexpr int SCI_GETSELECTION          = 2140;  // get full selection range (cpMin, cpMax)
    constexpr int SCI_GETSELECTIONSTART                                   = 2143;
    constexpr int SCI_GETSELECTIONEND                                     = 2145;
    constexpr int SCI_GETSELECTIONNSTART                                  = 2585;
    constexpr int SCI_GETSELECTIONNEND                                    = 2587;
    constexpr int SCI_GETLASTVISIBLELINE                                  = 2353;
    constexpr int SCI_GETTEXTDIRECTION                                    = 2596;
    constexpr int SCI_MARGINTEXTGETTEXT                                   = 2534;
    constexpr int SCI_SETSELECTIONLAYER                                   = 2560;
    constexpr int SCI_GETSELECTIONLAYER                                   = 2561;
    constexpr int SCI_GETSELBACK                                          = 2686;
    constexpr int SCI_GETCHANGEHISTORY                                    = 2690;
    constexpr int SCI_GETMOUSESELECTIONRECTANGULARPROBABILITYTHRESHOLD    = 2669;
    constexpr int SCI_SETMOUSESELECTIONRECTANGULARPROBABILITYTHRESHOLD    = 2668;
    constexpr int SCI_ANNOTATIONSETMOUSENOTIFICATION                      = 2549;
    constexpr int SCI_ANNOTATIONGETMOUSENOTIFICATION                      = 2550;
    constexpr int SCI_DELETELINE                                          = 2338;
    constexpr int SCI_INSERTLINE                                          = 2337;
    constexpr int SCI_GETPOSITIONCACHESIZE                                = 2510;
    constexpr int SCI_SELECTIONCONTAINS                                   = 2685;

    // Additional constants used by Notepad_plus.cpp
    constexpr int SCI_GETANCHOR              = 2009;
    constexpr int SCI_GETSELECTIONS           = 2570;
    constexpr int SCI_GETSELECTIONMODE       = 2423;
    constexpr int SCI_SETSELECTIONMODE        = 2579;
    constexpr int SC_SEL_RECTANGLE           = 1;
    constexpr int SC_SEL_THIN                = 4;
    constexpr int SC_SEL_LINES               = 2;
    constexpr int SC_SEL_STREAM              = 0;
    constexpr int SCI_SETCURRENTPOS          = 2001;
    constexpr int SCI_SETANCHOR              = 2008;
    constexpr int STYLE_DEFAULT              = 32;
    constexpr int SC_SEARCHRESULTPOS         = 3090;

    // ==== Additional Scintilla constants needed by Notepad_plus.cpp ====
    constexpr int SCI_GETCODEPAGE            = 2137;
    constexpr int SCI_GETTABWIDTH            = 2121;
    constexpr int SCI_GETTARGETTEXT          = 2389;
    constexpr int SCI_GETFOLDLEVEL           = 2223;
    constexpr int SC_FOLDLEVELHEADERFLAG      = 0x2000;
    constexpr int SC_FOLDLEVELBASE           = 0x0400;
    constexpr int SC_FOLDLEVELNUMBERMASK      = 0x0fff;
    constexpr int SCI_GETFOLDEXPANDED         = 2230;
    constexpr int SCI_SETFOLDEXPANDED         = 2228;
    constexpr int SCI_FOLDCHILDREN            = 2227;
    constexpr int SCI_DELETERANGE             = 2386;
    constexpr int SCI_TARGETWHOLEDOCUMENT     = 2690;
    constexpr int SCI_GETMARGINWIDTHN         = 2243;
    constexpr int SCI_SETMARGINWIDTHN         = 2242;

    // ==== Missing SCI_* from QsciScintillaBase ====
    constexpr int SCI_CANUNDO                = 2174;
    constexpr int SCI_CANREDO                = 2016;
    constexpr int SCI_CANPASTE               = 2173;
    constexpr int SCI_SETSELECTIONSTART      = 2142;
    constexpr int SCI_SETSELECTIONEND        = 2144;
    constexpr int SCI_BRACEMATCH             = 2353;
    constexpr int SCI_BRACEBADLIGHT          = 2352;
    constexpr int SCI_SETHIGHLIGHTGUIDE      = 2134;
    constexpr int SCI_ENSUREVISIBLEENFORCEPOLICY = 2234;
    constexpr int SCI_CHOOSECARETX           = 2399;
    constexpr int SCI_USEPOPUP               = 2371;
    constexpr int SCI_INDICATORVALUEAT       = 2507;
    constexpr int SCI_INDICATORSTART         = 2508;
    constexpr int SCI_INDICATOREND           = 2509;
    constexpr int SCI_INDICSETSTYLE          = 2080;
    constexpr int SCI_INDICGETSTYLE          = 2081;
    constexpr int SCI_INDICGETHOVERSTYLE     = 2681;
    constexpr int SCI_INDICSETHOVERSTYLE     = 2680;
    constexpr int SCI_INDICSETALPHA          = 2523;
    constexpr int SCI_INDICSETFLAGS          = 2684;
    constexpr int SCI_SETMARGINLEFT          = 2155;
    constexpr int SCI_SETMARGINRIGHT         = 2157;
    constexpr int SCI_SETENDATLASTLINE       = 2277;
    constexpr int SCI_SETFONTQUALITY         = 2611;
    constexpr int SC_EFF_QUALITY_LCD_OPTIMIZED = 3;
    constexpr int SCI_GETLINESELSTARTPOSITION = 2424;
    constexpr int SCI_GETLINESELENDPOSITION  = 2425;
    constexpr int SCI_GETLINEINDENTPOSITION  = 2128;
    constexpr int SCI_SETAUTOMATICFOLD       = 2663;
    constexpr int SC_AUTOMATICFOLD_SHOW      = 0x0001;
    constexpr int SC_AUTOMATICFOLD_CHANGE    = 0x0004;
    constexpr int SCI_STYLESETCHECKMONOSPACED = 2099;
    constexpr int SCI_SETUNDOSELECTIONHISTORY = 2564;
    constexpr int SC_UNDO_SELECTION_HISTORY_ENABLED = 1;
    constexpr int SC_UNDO_SELECTION_HISTORY_SCROLL = 2;
    constexpr int SCI_SETCARETLINEVISIBLEALWAYS = 2655;
    constexpr int SCI_SETADDITIONALSELECTIONTYPING = 2565;
    constexpr int SCVS_NONE                 = 0;
    constexpr int SCVS_RECTANGULARSELECTION  = 1;
    constexpr int SCVS_USERACCESSIBLE        = 2;
    constexpr int SCVS_NOWRAPLINESTART       = 4;
    constexpr int SCI_SETMULTIPASTE          = 2614;
    constexpr int SC_MULTIPASTE_EACH         = 1;
    constexpr int SCI_AUTOCSETMULTI          = 2636;
    constexpr int SC_MULTIAUTOC_EACH         = 1;
    constexpr int SCI_SETMOUSESELECTIONRECTANGULARSWITCH = 2668;
    constexpr int SCI_GETTEXTLENGTH          = 2183;
    constexpr int SCI_SETSELECTION           = 2572;
    constexpr int SCI_GETOVERTYPE            = 2187;
    constexpr int SCI_SELECTIONISRECTANGLE   = 2372;
    constexpr int SCI_GETXOFFSET             = 2398;
    constexpr int SCI_COUNTCHARACTERS        = 2633;
    constexpr int SCI_SETINDICATORVALUE      = 2502;
    constexpr int SCI_SETVIRTUALSPACEOPTIONS = 2596;
    constexpr int SCI_MARKERPREVIOUS         = 2048;
    constexpr int SCI_MARKERNEXT             = 2047;
    constexpr int SCI_GETSTYLEINDEXAT        = 2010;
    constexpr int SCI_SETCHANGEHISTORY       = 2689;
    constexpr int SC_CHANGE_HISTORY_DISABLED = 0;

    // ==== Missing Scintilla constants for Qt6/QScintilla ====
    constexpr int SCI_CLEARSELECTIONS         = 2577;
    constexpr int SCI_ADDSELECTION           = 2573;
    constexpr int SCI_GETVIRTUALSPACEOPTIONS = 2596;
    constexpr int SCI_SETFIRSTVISIBLELINE   = 2613;
    constexpr int SCI_HIGHLIGHTBRACES        = 2659;
    constexpr int SC_FOLDACTION_EXPAND       = 0;
    constexpr int SC_FOLDACTION_CONTRACT     = 2;

    // ==== SC_MARKNUM_HISTORY_* — for change history markers ====
    constexpr int SC_MARKNUM_HISTORY_REVERTED_TO_ORIGIN     = 21;
    constexpr int SC_MARKNUM_HISTORY_SAVED                  = 22;
    constexpr int SC_MARKNUM_HISTORY_MODIFIED               = 23;
    constexpr int SC_MARKNUM_HISTORY_REVERTED_TO_MODIFIED   = 24;

    // ==== INDIC_* styles ====
    constexpr int INDIC_PLAIN                = 0;
    constexpr int INDIC_HIDDEN               = 5;
    constexpr int INDIC_FULLBOX              = 16;
    constexpr int INDIC_EXPLORERLINK         = 4;
    constexpr int URL_INDIC                  = 4;
    constexpr int SC_INDICFLAG_VALUEFORE     = 2;

    // ==== SC_POPUP_* ====
    constexpr int SC_POPUP_NEVER             = 0;

    // ==== Universal found styles (supplement SciLexer.h defines) ====
    constexpr int SCE_UNIVERSAL_FOUND_STYLE  = 31;
    constexpr int SCE_UNIVERSAL_FOUND_STYLE_EXT1 = 25;
    constexpr int SCE_UNIVERSAL_FOUND_STYLE_EXT2 = 24;
    constexpr int SCE_UNIVERSAL_FOUND_STYLE_EXT3 = 23;
    constexpr int SCE_UNIVERSAL_FOUND_STYLE_EXT4 = 22;
    constexpr int SCE_UNIVERSAL_FOUND_STYLE_EXT5 = 21;

    // ==== STATUSBAR_* constants (from original N++) ====
    constexpr int STATUSBAR_DOC_SIZE        = 1;
    constexpr int STATUSBAR_CUR_POS         = 2;
    constexpr int STATUSBAR_EOF_FORMAT      = 3;
    constexpr int STATUSBAR_UNICODE_TYPE    = 4;
    constexpr int STATUSBAR_TYPING_MODE     = 5;
    constexpr int STATUSBAR_DOC_TYPE        = 6;

    // ==== TABBAR_* constants (from original N++) ====
    constexpr int TABBAR_ACTIVETEXT                = 1;
    constexpr int TABBAR_ACTIVEFOCUSEDINDCATOR     = 2;
    constexpr int TABBAR_ACTIVEUNFOCUSEDINDCATOR   = 3;
    constexpr int TABBAR_INACTIVETEXT              = 4;

    // ==== VIEWZONE_* constants (from original N++) ====
    constexpr int VIEWZONE_DOCUMENTMAP       = 1;
}  // namespace npp_sci

// NOTE: `using namespace npp_sci;` is intentionally NOT used here because
// QsciScintillaBase also defines SCI_GETCURRENTPOS, SCI_GETTARGETEND, SCI_GETLINECOUNT,
// SCI_GETSELECTIONS, and other constants in the global namespace. A bare
// `using namespace npp_sci;` would create ambiguity in files that include both
// NppSciCompat.h (via the NppConstants.h chain) AND Qsci headers.
//
// Instead, constants are brought into global scope individually. The conflict
// resolution (Qsci vs npp_sci) is handled per-file: for ambiguous constants,
// use `::CONST` to pick the Qsci version, or `npp_sci::CONST` to force ours.

#ifndef lstrcmp
#define lstrcmp strcmp
#endif

// Win32 API shim — _strnicmp is a Windows-only CRT function; use Qt equivalent
#ifndef _strnicmp
#define _strnicmp strncmp
#endif

// Win32 API shim — _wcsicmp is a wide-char case-insensitive compare; use Qt equivalent
#ifndef _wcsicmp
#include <QString>
inline int _wcsicmp(const wchar_t* a, const wchar_t* b) {
    return QString::compare(QString::fromWCharArray(a), QString::fromWCharArray(b), Qt::CaseInsensitive);
}
#endif

// Bring all npp_sci constexpr constants into global namespace
// so bare SCI_* names (as used throughout the codebase) resolve correctly.
// NOTE: using namespace npp_sci is NOT used (ambiguity with QsciScintillaBase).
// Instead, each constant is individually brought into global scope below.

using npp_sci::SCI_ADDREFDOCUMENT;
using npp_sci::SCI_ANNOTATIONGETMOUSENOTIFICATION;
using npp_sci::SCI_ANNOTATIONSETMOUSENOTIFICATION;
using npp_sci::SCI_ANNOTATION_CLEARALL;
using npp_sci::SCI_ANCHOR;
using npp_sci::SCI_APPENDTEXT;
using npp_sci::SCI_AUTOCCANCEL;
using npp_sci::SCI_AUTOCGETSEPARATOR;
using npp_sci::SCI_AUTOCSETAUTOHIDE;
using npp_sci::SCI_AUTOCSETDROPRESTOFWORD;
using npp_sci::SCI_AUTOCSETIGNORECASE;
using npp_sci::SCI_AUTOCSETSEPARATOR;
using npp_sci::SCI_AUTOCSHOW;
using npp_sci::SCI_BACKTAB;
using npp_sci::SCI_BEGINUNDOACTION;
using npp_sci::SCI_BRACEHIGHLIGHT;
using npp_sci::SCI_CALLTIPACTIVE;
using npp_sci::SCI_CALLTIPCANCEL;
using npp_sci::SCI_CALLTIPSHOW;
using npp_sci::SCI_CALLTIPSETHLT;
using npp_sci::SCI_CALLTIPSETBACK;
using npp_sci::SCI_CALLTIPSETFORE;
using npp_sci::SCI_CALLTIPSETFOREHLT;
using npp_sci::SCI_CALLTIPSETOPTIONS;
using npp_sci::SCI_CANCEL;
using npp_sci::SCI_CHARLEFT;
using npp_sci::SCI_CHARLEFTEXTEND;
using npp_sci::SCI_CHARLEFTRECTEXTEND;
using npp_sci::SCI_CHARRIGHT;
using npp_sci::SCI_CHARRIGHTEXTEND;
using npp_sci::SCI_CHARRIGHTRECTEXTEND;
using npp_sci::SCI_CLEAR;
using npp_sci::SCI_CLEARALL;
using npp_sci::SCI_COLOURISE;
using npp_sci::SCI_COPY;
using npp_sci::SCI_CREATEDOCUMENT;
using npp_sci::SCI_CUT;
using npp_sci::SCI_DELETEBACK;
using npp_sci::SCI_DELETEBACKNOTLINE;
using npp_sci::SCI_DELETELINE;
using npp_sci::SCI_DELLINELEFT;
using npp_sci::SCI_DELLINERIGHT;
using npp_sci::SCI_DELWORDLEFT;
using npp_sci::SCI_DELWORDRIGHT;
using npp_sci::SCI_DELWORDRIGHTEND;
using npp_sci::SCI_DOCLINEFROMVISIBLE;
using npp_sci::SCI_DOCUMENTEND;
using npp_sci::SCI_DOCUMENTENDEXTEND;
using npp_sci::SCI_DOCUMENTSTART;
using npp_sci::SCI_DOCUMENTSTARTEXTEND;
using npp_sci::SCI_EDITTOGGLEOVERTYPE;
using npp_sci::SCI_EMPTYUNDOBUFFER;
using npp_sci::SCI_ENDUNDOACTION;
using npp_sci::SCI_ENSUREVISIBLE;
using npp_sci::SCI_FOLDDISPLAYTEXTSTYLE;
using npp_sci::SCI_FORMFEED;
using npp_sci::SCI_FINDTEXT;
using npp_sci::SCI_GETANCHOR;
using npp_sci::SCI_GETCHANGEHISTORY;
using npp_sci::SCI_GETCHARACTERPOINTER;
using npp_sci::SCI_GETCHARAT;
using npp_sci::SCI_GETCOLUMN;
using npp_sci::SCI_GETCURRENTLINE;
using npp_sci::SCI_GETCURRENTPOS;
using npp_sci::SCI_GETDOCPOINTER;
using npp_sci::SCI_GETFIRSTVISIBLELINE;
using npp_sci::SCI_GETINDENTATIONGUIDES;
using npp_sci::SCI_GETLASTVISIBLELINE;
using npp_sci::SCI_GETLENGTH;
using npp_sci::SCI_GETLINE;
using npp_sci::SCI_GETLINECOUNT;
using npp_sci::SCI_GETLINEENDPOSITION;
using npp_sci::SCI_GETMOUSESELECTIONRECTANGULARPROBABILITYTHRESHOLD;
using npp_sci::SCI_GETPOSITIONCACHESIZE;
using npp_sci::SCI_GETREADONLY;
using npp_sci::SCI_GETSELBACK;
using npp_sci::SCI_GETSELECTION;
using npp_sci::SCI_GETSELECTIONEMPTY;
using npp_sci::SCI_GETSELECTIONEND;
using npp_sci::SCI_GETSELECTIONLAYER;
using npp_sci::SCI_GETSELECTIONMODE;
using npp_sci::SCI_SETSELECTIONMODE;
using npp_sci::SCI_GETSELECTIONNEND;
using npp_sci::SCI_GETSELECTIONNSTART;
using npp_sci::SCI_GETSELECTIONS;
using npp_sci::SCI_GETSELECTIONSTART;
using npp_sci::SCI_GETSELTEXT;
using npp_sci::SCI_GETSTYLEAT;
using npp_sci::SCI_GETTARGETEND;
using npp_sci::SCI_GETTARGETSTART;
using npp_sci::SCI_GETTEXT;
using npp_sci::SCI_SETTEXT;
using npp_sci::SCI_GETTEXTLENGTH;
using npp_sci::SCI_GETTEXTDIRECTION;
using npp_sci::SCI_GETTEXTRANGE;
using npp_sci::SCI_GETCURLINE;
using npp_sci::SCI_CONVERTEOLS;
using npp_sci::SCI_LINESSPLIT;
using npp_sci::SCI_LOWERCASE;
using npp_sci::SCI_UPPERCASE;
using npp_sci::SCI_TITLECASE;
using npp_sci::SCI_SORTLINES;
using npp_sci::SC_SORT_NUMERIC;
using npp_sci::SC_SORT_CASEINSENSITIVE;
using npp_sci::SC_SORT_PERCENTAGE;
using npp_sci::SC_SORT_REVERSE;
using npp_sci::SCI_GETWRAPINDENTMODE;
using npp_sci::SCI_GETZOOM;
using npp_sci::SCI_GOTOLINE;
using npp_sci::SCI_GOTOPOS;
using npp_sci::SCI_HOME;
using npp_sci::SCI_HOMEDISPLAY;
using npp_sci::SCI_HOMEDISPLAYEXTEND;
using npp_sci::SCI_HOMEEXTEND;
using npp_sci::SCI_HOMERECTEXTEND;
using npp_sci::SCI_HOMEWRAP;
using npp_sci::SCI_HOMEWRAPEXTEND;
using npp_sci::SCI_INDICATORCLEARRANGE;
using npp_sci::SCI_INDICATORFILLRANGE;
using npp_sci::SCI_INDICATORSTYLE;
using npp_sci::SCI_INDICSETSTYLE;
using npp_sci::SCI_INDICSETFORE;
using npp_sci::SCI_INDICSETBACK;
using npp_sci::SCI_INDICSETALPHA;
using npp_sci::SCI_INSERTLINE;
using npp_sci::SCI_INSERTTEXT;
using npp_sci::SCI_LINECOPY;
using npp_sci::SCI_LINECUT;
using npp_sci::SCI_LINEDELETE;
using npp_sci::SCI_LINEDOWN;
using npp_sci::SCI_LINEDOWNEXTEND;
using npp_sci::SCI_LINEDOWNRECTEXTEND;
using npp_sci::SCI_LINEDUPLICATE;
using npp_sci::SCI_LINEEND;
using npp_sci::SCI_LINEENDDISPLAY;
using npp_sci::SCI_LINEENDDISPLAYEXTEND;
using npp_sci::SCI_LINEENDEXTEND;
using npp_sci::SCI_LINEENDRECTEXTEND;
using npp_sci::SCI_LINEENDWRAP;
using npp_sci::SCI_LINEENDWRAPEXTEND;
using npp_sci::SCI_LINEFROMPOSITION;
using npp_sci::SCI_LINELENGTH;
using npp_sci::SCI_LINESCROLL;
using npp_sci::SCI_LINESCROLLDOWN;
using npp_sci::SCI_LINESCROLLUP;
using npp_sci::SCI_LINESJOIN;
using npp_sci::SCI_LINESONSCREEN;
using npp_sci::SCI_LINETRANSPOSE;
using npp_sci::SCI_LINEUP;
using npp_sci::SCI_LINEUPEXTEND;
using npp_sci::SCI_LINEUPRECTEXTEND;
using npp_sci::SCI_MARGINTEXTGETTEXT;
using npp_sci::SCI_MARKERADD;
using npp_sci::SCI_MARKERDELETE;
using npp_sci::SCI_MARKERDELETEALL;
using npp_sci::SCI_MARKERGET;
using npp_sci::SCI_MARKERPREVIOUS;
using npp_sci::SCI_MARKERNEXT;
using npp_sci::SCI_MOVECARETINSIDEVIEW;
using npp_sci::SCI_NEWLINE;
using npp_sci::SCI_PAGEDOWN;
using npp_sci::SCI_PAGEDOWNEXTEND;
using npp_sci::SCI_PAGEDOWNRECTEXTEND;
using npp_sci::SCI_PAGEUP;
using npp_sci::SCI_PAGEUPEXTEND;
using npp_sci::SCI_PAGEUPRECTEXTEND;
using npp_sci::SCI_PARADOWN;
using npp_sci::SCI_PARADOWNEXTEND;
using npp_sci::SCI_PARAUP;
using npp_sci::SCI_PARAUPEXTEND;
using npp_sci::SCI_PASTE;
using npp_sci::SCI_POINTXFROMPOSITION;
using npp_sci::SCI_POINTYFROMPOSITION;
using npp_sci::SCI_POSITIONAFTER;
using npp_sci::SCI_POSITIONBEFORE;
using npp_sci::SCI_POSITIONFROMLINE;
using npp_sci::SCI_POSITIONFROMPOINT;
using npp_sci::SCI_REDO;
using npp_sci::SCI_RELEASEDOCUMENT;
using npp_sci::SCI_REPLACESEL;
using npp_sci::SCI_REPLACETARGET;
using npp_sci::SCI_ROTATESELECTION;
using npp_sci::SCI_SCROLLCARET;
using npp_sci::SCI_SEARCHINTARGET;
using npp_sci::SCI_SELECTALL;
using npp_sci::SCI_SELECTIONCONTAINS;
using npp_sci::SCI_SELECTIONDUPLICATE;
using npp_sci::SCI_SETANCHOR;
using npp_sci::SCI_SETBRACKETLIGHTLINE;
using npp_sci::SCI_SETCODEPAGE;
using npp_sci::SCI_SETCURRENTPOS;
using npp_sci::SCI_SETDOCPOINTER;
using npp_sci::SCI_SETILEXER;
using npp_sci::SCI_SETINDICATORCURRENT;
using npp_sci::SCI_SETLAZYEND;
using npp_sci::SCI_SETMODEVENTMASK;
using npp_sci::SCI_SETMOUSESELECTIONRECTANGULARPROBABILITYTHRESHOLD;
using npp_sci::SCI_SETREADONLY;
using npp_sci::SCI_SETSAVEPOINT;
using npp_sci::SCI_SETSEARCHFLAGS;
using npp_sci::SCI_SETSEL;
using npp_sci::SCI_SETSELECTIONLAYER;
using npp_sci::SCI_SETSTATUS;
using npp_sci::SCI_SETTARGETEND;
using npp_sci::SCI_SETTARGETRANGE;
using npp_sci::SCI_SETTARGETSTART;
using npp_sci::SCI_SETUNDOCOLLECTION;
using npp_sci::SCI_SETWRAPINDENTMODE;
using npp_sci::SCI_SETZOOM;
using npp_sci::SCI_STUTTEREDPAGEDOWN;
using npp_sci::SCI_STUTTEREDPAGEDOWNEXTEND;
using npp_sci::SCI_STUTTEREDPAGEUP;
using npp_sci::SCI_STUTTEREDPAGEUPEXTEND;
using npp_sci::SCI_STYLEGETBACK;
using npp_sci::SCI_STYLEGETFORE;
using npp_sci::SCI_SWAPMAINANCHORCARET;
using npp_sci::SCI_TAB;
using npp_sci::SCI_TARGETFROMSELECTION;
using npp_sci::SCI_TEXTHEIGHT;
using npp_sci::SCI_UNDO;
using npp_sci::SCI_VCHOME;
using npp_sci::SCI_VCHOMEDISPLAY;
using npp_sci::SCI_VCHOMEDISPLAYEXTEND;
using npp_sci::SCI_VCHOMEEXTEND;
using npp_sci::SCI_VCHOMERECTEXTEND;
using npp_sci::SCI_VCHOMEWRAP;
using npp_sci::SCI_VCHOMEWRAPEXTEND;
using npp_sci::SCI_WORDENDPOSITION;
using npp_sci::SCI_WORDLEFT;
using npp_sci::SCI_WORDLEFTEND;
using npp_sci::SCI_WORDLEFTENDEXTEND;
using npp_sci::SCI_WORDLEFTEXTEND;
using npp_sci::SCI_WORDPARTLEFT;
using npp_sci::SCI_WORDPARTLEFTEXTEND;
using npp_sci::SCI_WORDPARTRIGHT;
using npp_sci::SCI_WORDPARTRIGHTEXTEND;
using npp_sci::SCI_WORDRIGHT;
using npp_sci::SCI_WORDRIGHTEND;
using npp_sci::SCI_WORDRIGHTENDEXTEND;
using npp_sci::SCI_WORDRIGHTEXTEND;
using npp_sci::SCI_WORDSTARTPOSITION;
using npp_sci::SCI_ZOOMIN;
using npp_sci::SCI_ZOOMOUT;
// Additional using declarations for constants defined in npp_sci:: but not yet exported
using npp_sci::SCFIND_MATCHCASE;
using npp_sci::SCFIND_WHOLEWORD;
using npp_sci::SCFIND_REGEXP;
using npp_sci::SCFIND_POSIX;
using npp_sci::SCFIND_REGEXP_DOTMATCHESNL;
using npp_sci::SCFIND_WORDSTART;
using npp_sci::SC_SEL_STREAM;
using npp_sci::SC_SEL_RECTANGLE;
using npp_sci::SC_SEL_THIN;
using npp_sci::SCI_CLEARSELECTIONS;
using npp_sci::SCI_ADDSELECTION;
using npp_sci::SCI_GETVIRTUALSPACEOPTIONS;
using npp_sci::SCI_SETFIRSTVISIBLELINE;
using npp_sci::SCI_HIGHLIGHTBRACES;
using npp_sci::SC_FOLDACTION_EXPAND;
using npp_sci::SC_FOLDACTION_CONTRACT;
// Additional using declarations for newly added npp_sci:: constants
using npp_sci::SCI_GETCODEPAGE;
using npp_sci::SCI_GETTABWIDTH;
using npp_sci::SCI_GETTARGETTEXT;
using npp_sci::SCI_GETFOLDLEVEL;
using npp_sci::SC_FOLDLEVELHEADERFLAG;
using npp_sci::SC_FOLDLEVELBASE;
using npp_sci::SC_FOLDLEVELNUMBERMASK;
using npp_sci::SCI_GETFOLDEXPANDED;
using npp_sci::SCI_SETFOLDEXPANDED;
using npp_sci::SCI_FOLDCHILDREN;
using npp_sci::SCI_BRACEMATCH;
using npp_sci::SCI_SETSELECTION;
using npp_sci::SCI_DELETERANGE;
using npp_sci::SCI_GETMARGINWIDTHN;
using npp_sci::SCI_TARGETWHOLEDOCUMENT;
using npp_sci::SCI_SETFONTQUALITY;
using npp_sci::SC_EFF_QUALITY_LCD_OPTIMIZED;
using npp_sci::SCI_SETCARETLINEVISIBLEALWAYS;
using npp_sci::SCI_SETENDATLASTLINE;
using npp_sci::SCI_SETADDITIONALSELECTIONTYPING;
using npp_sci::SCVS_NONE;
using npp_sci::SCVS_RECTANGULARSELECTION;
using npp_sci::SCVS_USERACCESSIBLE;
using npp_sci::SCVS_NOWRAPLINESTART;
using npp_sci::SCI_SETVIRTUALSPACEOPTIONS;
using npp_sci::SCI_SETMULTIPASTE;
using npp_sci::SC_MULTIPASTE_EACH;
using npp_sci::SCI_AUTOCSETMULTI;
using npp_sci::SC_MULTIAUTOC_EACH;
using npp_sci::SCI_SETMOUSESELECTIONRECTANGULARSWITCH;
using npp_sci::SCI_SETAUTOMATICFOLD;
using npp_sci::SC_AUTOMATICFOLD_SHOW;
using npp_sci::SC_AUTOMATICFOLD_CHANGE;
using npp_sci::SCI_SETMARGINLEFT;
using npp_sci::SCI_SETMARGINRIGHT;
using npp_sci::SCI_STYLESETCHECKMONOSPACED;
using npp_sci::STYLE_DEFAULT;
using npp_sci::SCI_SETUNDOSELECTIONHISTORY;
using npp_sci::SC_CHANGE_HISTORY_DISABLED;
using npp_sci::SC_MARKNUM_HISTORY_REVERTED_TO_ORIGIN;
using npp_sci::SC_MARKNUM_HISTORY_SAVED;
using npp_sci::SC_MARKNUM_HISTORY_MODIFIED;
using npp_sci::SC_MARKNUM_HISTORY_REVERTED_TO_MODIFIED;
using npp_sci::SC_UNDO_SELECTION_HISTORY_ENABLED;
using npp_sci::SC_UNDO_SELECTION_HISTORY_SCROLL;

#pragma once
