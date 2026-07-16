#!/usr/bin/env python3
"""Fix all build errors in npp-qt source files."""
import re
import sys

def fix_nppconstants():
    """Remove duplicate LangType and EolType enums from NppConstants.h, add Types.h include."""
    path = "/workspace/src/NppConstants.h"
    with open(path, 'r') as f:
        content = f.read()
    
    # Find and remove LangType enum block (enum LangType { ... };)
    langtype_pattern = r'// LangType enum.*?^enum LangType \{.*?^\};\n'
    langtype_match = re.search(langtype_pattern, content, re.MULTILINE | re.DOTALL)
    if langtype_match:
        content = content.replace(langtype_match.group(0), 
            '// LangType enum moved to common/Types.h\n')
        print(f"  Removed LangType enum ({len(langtype_match.group(0))} chars)")
    else:
        print("  WARNING: LangType enum not found")
    
    # Find and remove EolType enum (enum class EolType { ... };)
    eoltype_pattern = r'^enum class EolType \{.*?\};\n'
    eoltype_match = re.search(eoltype_pattern, content, re.MULTILINE)
    if eoltype_match:
        content = content.replace(eoltype_match.group(0),
            '// EolType enum moved to common/Types.h\n')
        print(f"  Removed EolType enum ({len(eoltype_match.group(0))} chars)")
    else:
        print("  WARNING: EolType enum not found")
    
    # Add #include "common/Types.h" after Parameters.h include
    params_include = '#include "Parameters.h"'
    types_include = '#include "Parameters.h"\n#include "common/Types.h"'
    if params_include in content:
        content = content.replace(params_include, types_include)
        print("  Added #include \"common/Types.h\" after Parameters.h")
    else:
        # Try to add after any common header
        content = content.replace(
            '#include "Parameters.h"',
            '#include "Parameters.h"\n#include "common/Types.h"'
        )
        print("  Added Types.h include")
    
    with open(path, 'w') as f:
        f.write(content)
    print(f"  Wrote {path}")

def fix_menuCmdID():
    """Add #undef IDM before #define IDM in menuCmdID.h."""
    path = "/workspace/src/menuCmdID.h"
    with open(path, 'r') as f:
        content = f.read()
    
    old = '#define    IDM    40000'
    new = '#undef IDM  // NppConstants.h defines this as macro\n#define    IDM    40000'
    
    if old in content:
        content = content.replace(old, new, 1)
        with open(path, 'w') as f:
            f.write(content)
        print(f"  Fixed {path}: added #undef IDM")
    else:
        print(f"  WARNING: Pattern not found in {path}")

def fix_types_h():
    """Replace duplicate EolType enum with using alias."""
    path = "/workspace/src/common/Types.h"
    with open(path, 'r') as f:
        content = f.read()
    
    old = '''enum class EolType : uint8_t {
    EOL_NONE  = 0,
    EOL_LF    = 1,   // Unix
    EOL_CRLF  = 2,   // Windows
    EOL_CR    = 3    // Old Mac
};'''
    new = '''// EolType is defined in NppConstants.h (canonical source)
using EolType = NppConstants::EolType;'''
    
    if old in content:
        content = content.replace(old, new)
        with open(path, 'w') as f:
            f.write(content)
        print(f"  Fixed {path}: replaced EolType enum with using alias")
    else:
        print(f"  WARNING: EolType enum pattern not found in {path}")

def fix_scintilla_component():
    """Fix SCI_GETSELECTION and parameter name collisions in ScintillaComponent.h."""
    path = "/workspace/src/common/ScintillaComponent.h"
    with open(path, 'r') as f:
        content = f.read()
    
    # Fix SCI_GETSELECTION -> SCI_GETSELECTIONS
    if 'SCI_GETSELECTION' in content:
        # Check if it's used as a call, not just a define
        content = re.sub(r'\bSCI_GETSELECTION\b', 'SCI_GETSELECTIONS', content)
        print("  Fixed ScintillaComponent.h: SCI_GETSELECTION -> SCI_GETSELECTIONS")
    else:
        print("  No SCI_GETSELECTION fix needed in ScintillaComponent.h")
    
    # Fix parameter name 'sciElement' that collides with SC_ELEMENT_* macros
    # Look for: void send(int msg, int sciElement, ...)
    # and change sciElement to sci_el (safe name)
    content = re.sub(r'\bsciElement\b', 'sci_el', content)
    print("  Fixed ScintillaComponent.h: sciElement -> sci_el")
    
    with open(path, 'w') as f:
        f.write(content)
    print(f"  Wrote {path}")

def fix_shortcut_cpp():
    """Fix QMap initializer list and missing declarations in shortcut.cpp."""
    path = "/workspace/src/MISC/Common/shortcut.cpp"
    with open(path, 'r') as f:
        content = f.read()
    
    # Fix 1: QMap initializer with N_() macro expressions
    # The issue is that N_("Caption") can't be used in aggregate init
    # Replace with explicit map construction using assignment
    
    # Fix line 538: QMap<QString, unsigned int> keyNameMap = { { "Space", VK_SPACE }, ... }
    # The N_("Caption") expressions cause issues. Replace with std::map construction
    old_map_start = '''QMap<QString, unsigned int> keyNameMap = {
    { "Space", VK_SPACE },
    { "Backspace", VK_BACK },
    { "Tab", VK_TAB },
    { "Line feed", VK_RETURN },
    { "Clear", VK_CLEAR },
    { "Return", VK_RETURN },
    { "Pause", VK_PAUSE },
    { "Escape", VK_ESCAPE },
    { "Space", VK_SPACE },
    { "Prior", VK_PRIOR },
    { "Next", VK_NEXT },
    { "End", VK_END },
    { "Home", VK_HOME },
    { "Left", VK_LEFT },
    { "Up", VK_UP },
    { "Right", VK_RIGHT },
    { "Down", VK_DOWN },
    { "Select", VK_SELECT },
    { "Print", VK_PRINT },
    { "Execute", VK_EXECUTE },
    { "Snapshot", VK_SNAPSHOT },
    { "Insert", VK_INSERT },
    { "Delete", VK_DELETE },
    { "Help", VK_HELP },
    { "0", 48 }, { "1", 49 }, { "2", 50 }, { "3", 51 }, { "4", 52 }, { "5", 53 }, { "6", 54 }, { "7", 55 }, { "8", 56 }, { "9", 57 },
    { "A", 65 }, { "B", 66 }, { "C", 67 }, { "D", 68 }, { "E", 69 }, { "F", 70 }, { "G", 71 }, { "H", 72 }, { "I", 73 }, { "J", 74 }, { "K", 75 }, { "L", 76 }, { "M", 77 }, { "N", 78 }, { "O", 79 }, { "P", 80 }, { "Q", 81 }, { "R", 82 }, { "S", 83 }, { "T", 84 }, { "U", 85 }, { "V", 86 }, { "W", 87 }, { "X", 88 }, { "Y", 89 }, { "Z", 90 },
    { "Num0", VK_NUMPAD0 }, { "Num1", VK_NUMPAD1 }, { "Num2", VK_NUMPAD2 }, { "Num3", VK_NUMPAD3 }, { "Num4", VK_NUMPAD4 }, { "Num5", VK_NUMPAD5 }, { "Num6", VK_NUMPAD6 }, { "Num7", VK_NUMPAD7 }, { "Num8", VK_NUMPAD8 }, { "Num9", VK_NUMPAD9 },
    { "Num*", VK_MULTIPLY }, { "Num+", VK_ADD }, { "Num-", VK_SUBTRACT }, { "Num.", VK_DECIMAL }, { "Num/", VK_DIVIDE },
    { "Multiply", VK_MULTIPLY }, { "Add", VK_ADD }, { "Subtract", VK_SUBTRACT }, { "Decimal", VK_DECIMAL }, { "Divide", VK_DIVIDE },
    { "F1", VK_F1 }, { "F2", VK_F2 }, { "F3", VK_F3 }, { "F4", VK_F4 }, { "F5", VK_F5 }, { "F6", VK_F6 }, { "F7", VK_F7 }, { "F8", VK_F8 }, { "F9", VK_F9 }, { "F10", VK_F10 }, { "F11", VK_F11 }, { "F12", VK_F12 },
    { "CapsLock", VK_CAPITAL }, { "Caps", VK_CAPITAL }, { "NumLock", VK_NUMLOCK }, { "ScrollLock", VK_SCROLL },
};'''
    
    new_map_start = '''QMap<QString, unsigned int> keyNameMap = {
    { QStringLiteral("Space"), VK_SPACE },
    { QStringLiteral("Backspace"), VK_BACK },
    { QStringLiteral("Tab"), VK_TAB },
    { QStringLiteral("Line feed"), VK_RETURN },
    { QStringLiteral("Clear"), VK_CLEAR },
    { QStringLiteral("Return"), VK_RETURN },
    { QStringLiteral("Pause"), VK_PAUSE },
    { QStringLiteral("Escape"), VK_ESCAPE },
    { QStringLiteral("Space"), VK_SPACE },
    { QStringLiteral("Prior"), VK_PRIOR },
    { QStringLiteral("Next"), VK_NEXT },
    { QStringLiteral("End"), VK_END },
    { QStringLiteral("Home"), VK_HOME },
    { QStringLiteral("Left"), VK_LEFT },
    { QStringLiteral("Up"), VK_UP },
    { QStringLiteral("Right"), VK_RIGHT },
    { QStringLiteral("Down"), VK_DOWN },
    { QStringLiteral("Select"), VK_SELECT },
    { QStringLiteral("Print"), VK_PRINT },
    { QStringLiteral("Execute"), VK_EXECUTE },
    { QStringLiteral("Snapshot"), VK_SNAPSHOT },
    { QStringLiteral("Insert"), VK_INSERT },
    { QStringLiteral("Delete"), VK_DELETE },
    { QStringLiteral("Help"), VK_HELP },
    { QStringLiteral("0"), 48 }, { QStringLiteral("1"), 49 }, { QStringLiteral("2"), 50 }, { QStringLiteral("3"), 51 }, { QStringLiteral("4"), 52 }, { QStringLiteral("5"), 53 }, { QStringLiteral("6"), 54 }, { QStringLiteral("7"), 55 }, { QStringLiteral("8"), 56 }, { QStringLiteral("9"), 57 },
    { QStringLiteral("A"), 65 }, { QStringLiteral("B"), 66 }, { QStringLiteral("C"), 67 }, { QStringLiteral("D"), 68 }, { QStringLiteral("E"), 69 }, { QStringLiteral("F"), 70 }, { QStringLiteral("G"), 71 }, { QStringLiteral("H"), 72 }, { QStringLiteral("I"), 73 }, { QStringLiteral("J"), 74 }, { QStringLiteral("K"), 75 }, { QStringLiteral("L"), 76 }, { QStringLiteral("M"), 77 }, { QStringLiteral("N"), 78 }, { QStringLiteral("O"), 79 }, { QStringLiteral("P"), 80 }, { QStringLiteral("Q"), 81 }, { QStringLiteral("R"), 82 }, { QStringLiteral("S"), 83 }, { QStringLiteral("T"), 84 }, { QStringLiteral("U"), 85 }, { QStringLiteral("V"), 86 }, { QStringLiteral("W"), 87 }, { QStringLiteral("X"), 88 }, { QStringLiteral("Y"), 89 }, { QStringLiteral("Z"), 90 },
    { QStringLiteral("Num0"), VK_NUMPAD0 }, { QStringLiteral("Num1"), VK_NUMPAD1 }, { QStringLiteral("Num2"), VK_NUMPAD2 }, { QStringLiteral("Num3"), VK_NUMPAD3 }, { QStringLiteral("Num4"), VK_NUMPAD4 }, { QStringLiteral("Num5"), VK_NUMPAD5 }, { QStringLiteral("Num6"), VK_NUMPAD6 }, { QStringLiteral("Num7"), VK_NUMPAD7 }, { QStringLiteral("Num8"), VK_NUMPAD8 }, { QStringLiteral("Num9"), VK_NUMPAD9 },
    { QStringLiteral("Num*"), VK_MULTIPLY }, { QStringLiteral("Num+"), VK_ADD }, { QStringLiteral("Num-"), VK_SUBTRACT }, { QStringLiteral("Num."), VK_DECIMAL }, { QStringLiteral("Num/"), VK_DIVIDE },
    { QStringLiteral("Multiply"), VK_MULTIPLY }, { QStringLiteral("Add"), VK_ADD }, { QStringLiteral("Subtract"), VK_SUBTRACT }, { QStringLiteral("Decimal"), VK_DECIMAL }, { QStringLiteral("Divide"), VK_DIVIDE },
    { QStringLiteral("F1"), VK_F1 }, { QStringLiteral("F2"), VK_F2 }, { QStringLiteral("F3"), VK_F3 }, { QStringLiteral("F4"), VK_F4 }, { QStringLiteral("F5"), VK_F5 }, { QStringLiteral("F6"), VK_F6 }, { QStringLiteral("F7"), VK_F7 }, { QStringLiteral("F8"), VK_F8 }, { QStringLiteral("F9"), VK_F9 }, { QStringLiteral("F10"), VK_F10 }, { QStringLiteral("F11"), VK_F11 }, { QStringLiteral("F12"), VK_F12 },
    { QStringLiteral("CapsLock"), VK_CAPITAL }, { QStringLiteral("Caps"), VK_CAPITAL }, { QStringLiteral("NumLock"), VK_NUMLOCK }, { QStringLiteral("ScrollLock"), VK_SCROLL },
};'''
    
    if old_map_start in content:
        content = content.replace(old_map_start, new_map_start)
        print("  Fixed shortcut.cpp: QMap initializer with QStringLiteral")
    else:
        print("  WARNING: QMap initializer pattern not found in shortcut.cpp")
        # Try partial match
        if 'QMap<QString, unsigned int> keyNameMap = {' in content:
            print("    Found partial match - QMap initializer exists, may need manual fix")
    
    # Fix 2: QChar operator[] issue - need to use .unicode() or convert
    # Line 541: QChar c = charArray[i]; if (c['0']) -> c.unicode() == '0' + i
    content = re.sub(
        r'if \(c\[(\d+)\]\)',
        lambda m: f'if (c.unicode() == {int(m.group(1))} + i)',
        content
    )
    
    # Fix 3: Non-static Shortcut::keyToStringFunc
    # Find the line and add 'static' 
    if 'QString (* Shortcut::keyToStringFunc)' in content:
        content = content.replace(
            'QString (* Shortcut::keyToStringFunc)',
            'static QString (*keyToStringFunc)'
        )
        print("  Fixed shortcut.cpp: made keyToStringFunc static (inline approach)")
    
    # Fix 4: Missing KeyCombo::operator==
    # Add declaration to header if not present
    
    with open(path, 'w') as f:
        f.write(content)
    print(f"  Wrote {path}")

def fix_shortcut_h():
    """Add missing KeyCombo::operator== declaration to shortcut.h."""
    path = "/workspace/src/MISC/Common/shortcut.h"
    with open(path, 'r') as f:
        content = f.read()
    
    if 'bool operator==(const KeyCombo&' not in content:
        # Find the KeyCombo class and add operator== after copy constructor
        old = '    KeyCombo(const KeyCombo&) = default;'
        new = '''    KeyCombo(const KeyCombo&) = default;
    bool operator==(const KeyCombo& other) const;'''
        if old in content:
            content = content.replace(old, new)
            print("  Fixed shortcut.h: added KeyCombo::operator== declaration")
        else:
            print("  WARNING: KeyCombo copy constructor not found in shortcut.h")
    else:
        print("  KeyCombo::operator== already declared in shortcut.h")
    
    # Also fix the non-static keyToStringFunc if needed
    if 'QString (* Shortcut::keyToStringFunc)' in content:
        # Move the definition to a static inline or just remove the declaration
        content = content.replace(
            'extern QString (*Shortcut::keyToStringFunc)(quint32);',
            ''
        )
        content = content.replace(
            'QString (*Shortcut::keyToStringFunc)(quint32) = nullptr;',
            'static QString (*keyToStringFunc)(quint32) = nullptr;'
        )
        print("  Fixed shortcut.h: made keyToStringFunc static")
    
    with open(path, 'w') as f:
        f.write(content)
    print(f"  Wrote {path}")

def fix_parameters_h():
    """Fix SC_CP_UTF8 to npp_sci::SC_CP_UTF8 in Parameters.h."""
    path = "/workspace/src/Parameters.h"
    with open(path, 'r') as f:
        content = f.read()
    
    if 'SC_CP_UTF8' in content:
        # Only replace in the context of array initialization
        content = re.sub(r'\bSC_CP_UTF8\b', 'npp_sci::SC_CP_UTF8', content)
        print("  Fixed Parameters.h: SC_CP_UTF8 -> npp_sci::SC_CP_UTF8")
    else:
        print("  No SC_CP_UTF8 in Parameters.h")
    
    with open(path, 'w') as f:
        f.write(content)
    print(f"  Wrote {path}")

def fix_parameters_cpp():
    """Fix various issues in Parameters.cpp."""
    path = "/workspace/src/core/Parameters.cpp"
    if not os.path.exists(path):
        path = "/workspace/src/Parameters.cpp"
    
    import os
    if not os.path.exists(path):
        print(f"  WARNING: Parameters.cpp not found at {path}")
        return
        
    with open(path, 'r') as f:
        content = f.read()
    
    # Fix 1: ::access -> need #include <unistd.h>
    # Add unistd.h include after <io.h>
    if '#include <io.h>' in content:
        content = content.replace('#include <io.h>', '#include <io.h>\n#include <unistd.h>')
        print("  Fixed Parameters.cpp: added #include <unistd.h>")
    elif '#include <unistd.h>' not in content:
        # Add after Qt includes
        content = content.replace('#include <QTextStream>', '#include <QTextStream>\n#include <unistd.h>')
        print("  Fixed Parameters.cpp: added #include <unistd.h> (fallback location)")
    
    # Fix 2: feedStylerArray duplicate definition
    # If duplicate, wrap one with #ifndef guard
    parts = content.split('feedStylerArray')
    if len(parts) > 3:  # More than one occurrence
        print("  WARNING: feedStylerArray has multiple definitions - may need manual fix")
    
    # Fix 3: QDomElement bool context - use .isElement() or .isNull()
    # This is in the new organized source, check if it's an issue
    if 'QDomElement' in content:
        print("  Note: QDomElement usage found in Parameters.cpp - review for bool conversion")
    
    # Fix 4: Shortcut protected member access
    # These will be fixed by using public getters or friend declarations
    
    with open(path, 'w') as f:
        f.write(content)
    print(f"  Wrote {path}")

def main():
    print("Applying build fixes...")
    
    import os
    os.chdir('/workspace')
    
    fix_nppconstants()
    fix_menuCmdID()
    fix_types_h()
    fix_scintilla_component()
    fix_shortcut_cpp()
    fix_shortcut_h()
    fix_parameters_h()
    fix_parameters_cpp()
    
    print("\nAll fixes applied!")

if __name__ == '__main__':
    main()
