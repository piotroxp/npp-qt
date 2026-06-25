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

}  // namespace npp_sci

// Bring npp_sci constants into global namespace as bare names
using namespace npp_sci;

// Win32 API shim — lstrcmp is a Windows-only API; map to strcmp on Unix
#ifndef lstrcmp
#define lstrcmp strcmp
#endif
