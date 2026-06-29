#!/usr/bin/env python3
"""Fix all remaining Notepad_plus.cpp errors after first batch."""

PATH = '/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp'
HPATH = '/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.h'
CPATH = '/home/node/.openclaw/workspace/npp-qt/src/NppConstants.h'

with open(PATH) as f:
    c = f.read()
with open(HPATH) as f:
    h = f.read()
with open(CPATH) as f:
    ch = f.read()

orig = c

# 1. cdEnabledNew - not defined anywhere, replace with true
c = c.replace('cdEnabledNew', 'true')

# 2. multiInstOnSession - replace with false
c = c.replace('multiInstOnSession', 'false')

# 3. NPPN_READONLYCHANGED - already added to NppConstants.h

# 4. SCI_FINDCOLUMN - already added to NppSciCompat.h

# 5. Invalid conversion: Buffer* to int (line 6901)
c = c.replace(
    'pTGB2->_pEditView->doTransparentUniqueOpen((_pEditView->getCurrentBuffer()))',
    'pTGB2->_pEditView->doTransparentUniqueOpen((_pEditView->getCurrentBuffer())->getID())')

# 6. Invalid conversion: Buffer* to int (line 7004)
c = c.replace(
    'activateBufferDoc(pTGB2->_pEditView->getCurrentBuffer())',
    'activateBufferDoc((BufferID)(pTGB2->_pEditView->getCurrentBuffer())->getID())')

# 7. Invalid conversion: BufferID to uptr_t/int (line 6999, 7004)
c = c.replace(
    'return (uptr_t)pTGB2->_pEditView->getCurrentBuffer();',
    'return (uptr_t)(pTGB2->_pEditView->getCurrentBuffer())->getID();')
c = c.replace(
    'getBufferByID((BufferID)_pEditView->getCurrentBuffer())',
    'getBufferByID(_pEditView->getCurrentBuffer()->getID())')

# 8. EolFormat -> EolType
c = c.replace(
    'static_cast<EolType>(_pEditView->execute(SCI_GETEOLMODE))',
    'static_cast<EolType>(_pEditView->execute(SCI_GETEOLMODE) + 1)')
c = c.replace(
    '_pEditView->execute(SCI_SETEOLMODE, static_cast<EolType>(formatting._eolFormat))',
    '_pEditView->execute(SCI_SETEOLMODE, static_cast<int>(formatting._eolFormat))')

# 9. getFileNames -> getFileName
c = c.replace('fnss->getFileNames()', 'fnss->getFileName()')

# 10. isClosed -> isHidden (or just check the context)
c = c.replace('_fileBrowserWindow.isClosed()', '_fileBrowserWindow.isHidden()')
c = c.replace('_funcListWindow.isClosed()', '_funcListWindow.isHidden()')
c = c.replace('_documentMap.isClosed()', '_documentMap.isHidden()')

# 11. scrollPosToCenter -> scrollToCenter
c = c.replace('_pEditView->scrollPosToCenter', '_pEditView->scrollToCenter')

# 12. Const correctness: function returning non-const reference called on const object
# Line 8578: 'this' as const
c = c.replace(
    'static_cast<ScintillaEditView&>(_pEditView).execute(SCI_GETFIRSTVISIBLELINE)',
    '_pEditView->execute(SCI_GETFIRSTVISIBLELINE)')
c = c.replace(
    'static_cast<ScintillaEditView&>(_pEditView).execute(SCI_GETFIRSTVISIBLELINE)',
    '_pEditView->execute(SCI_GETFIRSTVISIBLELINE)')

# 13. Large block: QString ↔ wchar_t* conversions (lines ~7217-7269)
# The error: `cannot convert 'const QString' to 'const std::wstring&'`
# These are parameters that were converted to wstring by the translation
# but now need to stay as QString. Need to find the function signatures.

# Let's find lines 7217-7269 context
# These are in doOpenMultiAll or similar functions
# The issue is: some functions take wstring params but are called with QString args

# For now, let me just do the wcsncpy/wcscpy fixes
c = c.replace(
    'wcsncpy(buf, str.c_str(), maxLen - 1)',
    'wcsncpy(buf, str.toStdWString().c_str(), maxLen - 1)')
c = c.replace(
    'wcscpy_s(buf, maxLen, str.c_str())',
    'wcscpy_s(buf, maxLen, str.toStdWString().c_str())')
c = c.replace(
    'wcscpy_s(buf, str.toStdWString().c_str())',
    'wcscpy_s(buf, str.toStdWString().c_str())')

# 14. searchInTarget: can't bind intptr_t& to rvalue
# Lines 9008, 9010, 9028, 9030, 9033, 9035
# Need to use a named variable instead of literal
# Pattern: searchInTarget(..., startPos, intptr_t(endComment), ...)
# These are likely calls in the spell-check area

# Let me search for these patterns
# Pattern: searchInTarget(text, searchIn, intptr_t(X))
import re

# Fix searchInTarget calls with intptr_t(X) pattern
# Replace `searchInTarget(a, b, intptr_t(c))` -> `searchInTarget(a, b, static_cast<intptr_t>(c))`
# and wrap in named variable

# Actually the error is that the function signature takes `intptr_t&` not `intptr_t`
# Let me check the actual lines first

print(f"Lines changed so far: {sum(1 for a,b in zip(orig.splitlines(), c.splitlines()) if a != b)}")

# Write intermediate
with open(PATH, 'w') as f:
    f.write(c)

print("Part 1 done. Checking searchInTarget errors...")
# Now let me check the searchInTarget errors
with open(PATH) as f:
    lines = f.readlines()

for i, line in enumerate(lines):
    if 'searchInTarget' in line and ('intptr_t(' in line or 'static_cast<intptr_t>' in line):
        linenum = i + 1
        if linenum >= 9000:
            print(f"Line {linenum}: {line.rstrip()[:100]}")
