#!/usr/bin/env python3
"""Fix remaining errors by line number in reverted+fix_min.py Notepad_plus.cpp."""

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp') as f:
    lines = f.readlines()

changed = False

# === 1. doSaveOrNot (lines 2504-2513) ===
# Replace lines 2504-2513 with fixed version
lines[2503] = '\t{\n'
lines[2504] = '\t\tQString title, msg;\n'
lines[2505] = '\n'
lines[2506] = '\t\tif (!_nativeLangSpeaker.getDoSaveOrNotStrings(title, msg))\n'
lines[2507] = '\t\t{\n'
lines[2508] = '\t\t\ttitle = QStringLiteral("Save");\n'
lines[2509] = '\t\t\tmsg = QStringLiteral("Save file \\"%1\\" ?").arg(QString::fromWCharArray(fn));\n'
lines[2510] = '\t\t}\n'
lines[2511] = '\t\telse\n'
lines[2512] = '\t\t{\n'
lines[2513] = '\t\t\tmsg = msg.arg(QString::fromWCharArray(fn));\n'
# Find and remove the stringReplace line (was at 2511 in original)
# Actually we already overwrote it with else block - let me check what comes after
# Remove lines 2514 onwards that are part of the old stringReplace
# The old block was:
# msg = stringReplace(msg, L"$STR_REPLACE$", fn); (line ~2511)
# return QMessageBox::warning(...) (line ~2512)
# So lines 2514-2516 should be: return QMessageBox..., closing brace, DoSaveOrNotBox
# Let me check what's currently there
# We need to keep: return QMessageBox... and } and DoSaveOrNotBox...
# Actually the current array has: [2503]={\n, [2504]=\t\tQString...\n, [2505]=\n, [2506]=\t\tif...\n, [2507]=\t\t{\n, [2508]=\t\t\ttitle...\n, [2509]=\t\t\tmsg...\n, [2510]=\t\t}\n, [2511]=\t\telse\n, [2512]=\t\t{\n, [2513]=\t\t\tmsg...\n
# Lines 2514-2516 need to be: \t\t}\n, return QMessageBox...\n, \t}\n
# Let's overwrite 2514 onwards
lines[2514] = '\t\t}\n'
lines[2515] = '\t\treturn QMessageBox::warning(static_cast<QWidget*>(this->getHSelf()), msg, title, QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Save);\n'
lines[2516] = '\t}\n'
changed = True
print("1. Fixed doSaveOrNot")

# === 2. getGenericText (lines 3040-3048) ===
# Need to replace wchar_t* buf allocation with getGenericTextAsString
# Lines: [3040]=auto lineLen\n, [3041]=\n, [3042]=auto lineBegin\n, [3043]=\n, [3044]=wchar_t * buf\n, ...
lines[3042] = '\tauto lineBegin = _pEditView->execute(SCI_POSITIONFROMLINE, ln);\n'
lines[3043] = '\tauto lineLen = _pEditView->execute(SCI_LINELENGTH, ln);\n'
lines[3044] = '\twstring line = _pEditView->getGenericTextAsString(lineBegin, lineBegin + lineLen);\n'
# Remove lines 3045-3048 (the old wchar_t* buf code)
lines[3045] = '\n'
lines[3046] = '\n'
lines[3047] = '\n'
lines[3048] = '\n'
changed = True
print("2. Fixed getGenericText")

# === 3. addHotSpot - replace entire function with stub ===
# Find the function
for i in range(3590, 3610):
    if 'void Notepad_plus::addHotSpot' in lines[i]:
        # Find end of function
        brace_count = 0
        j = i
        started = False
        while j < len(lines):
            brace_count += lines[j].count('{') - lines[j].count('}')
            if '{' in lines[j]: started = True
            if started and brace_count == 0: break
            j += 1
        stub = '''void Notepad_plus::addHotSpot(ScintillaEditView* view)
{
\tif (_isAttemptingCloseOnQuit) return;
\tScintillaEditView* pView = view ? view : _pEditView;
\tBuffer* currentBuf = pView->getCurrentBuffer();
\tif (!currentBuf->allowClickableLink()) return;
\tintptr_t startPos = 0, endPos = -1;
\tpView->getVisibleStartAndEndPosition(&startPos, &endPos);
\tif (startPos >= endPos) return;
\tpView->execute(SCI_SETINDICATORCURRENT, URL_INDIC);
\tpView->execute(SCI_INDICATORCLEARRANGE, startPos, endPos - startPos);
}
'''
        lines[i:j+1] = [stub]
        changed = True
        print(f"3. Fixed addHotSpot (replaced {j-i+1} lines)")
        break

# === 4. AutoIndentMode::none (line 3737) ===
for i in range(3735, 3740):
    if 'AutoIndentMode::none' in lines[i]:
        lines[i] = lines[i].replace('AutoIndentMode::none', 'AutoIndentMode::AI_NONE')
        changed = True
        print(f"4. Fixed AutoIndentMode at {i+1}")

# === 5. processTabSwitchAccel (lines ~3997) ===
for i in range(3990, 4020):
    if 'bool Notepad_plus::processTabSwitchAccel(MSG* msg)' in lines[i]:
        brace_count = 0
        j = i
        started = False
        while j < len(lines):
            brace_count += lines[j].count('{') - lines[j].count('}')
            if '{' in lines[j]: started = True
            if started and brace_count == 0: break
            j += 1
        stub = '''bool Notepad_plus::processTabSwitchAccel(QEvent* /*msg*/) const
{
\treturn false; // Qt uses QShortcut on QAction
}
'''
        lines[i:j+1] = [stub]
        changed = True
        print(f"5. Fixed processTabSwitchAccel at {i+1}")
        break

# === 6. uni7Bit (line 4446) ===
for i in range(4443, 4450):
    if 'uni7Bit' in lines[i] and 'UniMode' not in lines[i]:
        lines[i] = lines[i].replace('uni7Bit', 'UniMode::uni7Bit')
        changed = True
        print(f"6. Fixed uni7Bit at {i+1}")

# === 7. display -> setText (line 4717) ===
for i in range(4713, 4725):
    if '->display(' in lines[i]:
        lines[i] = lines[i].replace('->display(', '->setText(')
        changed = True
        print(f"7. Fixed display at {i+1}")

# === 8. DocTabView* -> Window* (line 4741) ===
for i in range(4738, 4750):
    if 'Window* pTb = _pDocTab;' in lines[i]:
        lines[i] = lines[i].replace('Window* pTb = _pDocTab;', 'Window* pTb = static_cast<Window*>(_pDocTab);')
        changed = True
        print(f"8. Fixed DocTabView* at {i+1}")
    if 'Window* pTb = _subDocTab;' in lines[i]:
        lines[i] = lines[i].replace('Window* pTb = _subDocTab;', 'Window* pTb = static_cast<Window*>(_subDocTab);')
        changed = True
        print(f"8b. Fixed DocTabView* sub at {i+1}")

# === 9. Statusbar section - REPLACE entire updateStatusBar() function ===
# Find updateStatusBar
for i in range(4395, 4420):
    if 'void Notepad_plus::updateStatusBar()' in lines[i]:
        # Find end
        brace_count = 0
        j = i
        started = False
        while j < len(lines):
            brace_count += lines[j].count('{') - lines[j].count('}')
            if '{' in lines[j]: started = True
            if started and brace_count == 0: break
            j += 1
        new_func = '''void Notepad_plus::updateStatusBar()
{
\tNativeLangSpeaker* pNativeSpeaker = (NppParameters::getInstance()).getNativeLangSpeaker();

\tsize_t docLen = _pEditView->getCurrentDocLen();
\tintptr_t nbLine = _pEditView->execute(SCI_GETLINECOUNT);
\tQString statusbarLengthLinesStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-length-lines", "length: %1    lines: %2");
\tstatusbarLengthLinesStr = statusbarLengthLinesStr.arg(commafyInt(docLen), commafyInt(nbLine));
\t_statusBar.setText(statusbarLengthLinesStr, STATUSBAR_DOC_SIZE);

\tQString statusbarSelStr;
\tsize_t nbSelections = _pEditView->execute(SCI_GETSELECTIONS);
\tif (nbSelections == 1)
\t{
\t\tif (_pEditView->execute(SCI_GETSELECTIONEMPTY))
\t\t{
\t\t\tsize_t currPos = _pEditView->execute(SCI_GETCURRENTPOS);
\t\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Pos", "Pos: ") + commafyInt(currPos + 1);
\t\t}
\t\telse
\t\t{
\t\t\tconst std::pair<size_t, size_t> oneSelCharsAndLines = _pEditView->getSelectedCharsAndLinesCount();
\t\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Sel", "Sel: ") + commafyInt(oneSelCharsAndLines.first) + QStringLiteral(" | ") + commafyInt(oneSelCharsAndLines.second);
\t\t}
\t}
\telse if (_pEditView->execute(SCI_SELECTIONISRECTANGLE))
\t{
\t\tconst std::pair<size_t, size_t> rectSelCharsAndLines = _pEditView->getSelectedCharsAndLinesCount();
\t\tbool sameCharCountOnEveryLine = true;
\t\tsize_t maxLineCharCount = 0;
\t\tfor (size_t sel = 0; sel < nbSelections; ++sel)
\t\t{
\t\t\tsize_t s = _pEditView->execute(SCI_GETSELECTIONNSTART, sel);
\t\t\tsize_t e = _pEditView->execute(SCI_GETSELECTIONNEND, sel);
\t\t\tsize_t lineCharCount = _pEditView->execute(SCI_COUNTCHARACTERS, s, e);
\t\t\tif (sel == 0) maxLineCharCount = lineCharCount;
\t\t\telse if (lineCharCount != maxLineCharCount) { sameCharCountOnEveryLine = false; if (lineCharCount > maxLineCharCount) maxLineCharCount = lineCharCount; }
\t\t}
\t\tQString nbSelectionsStr = commafyInt(nbSelections);
\t\tQString maxLineCharCountStr = commafyInt(maxLineCharCount);
\t\tQString opStr = sameCharCountOnEveryLine ? QStringLiteral(" = ") : QStringLiteral(" -> ");
\t\tQString rectSelCharsStr = commafyInt(rectSelCharsAndLines.first);
\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Sel", "Sel: ") + nbSelectionsStr + QStringLiteral("x") + maxLineCharCountStr + opStr + rectSelCharsStr;
\t}
\telse
\t{
\t\tconst int maxSelsToProcessLineCount = 99;
\t\tconst std::pair<size_t, size_t> multipleSelCharsAndLines = _pEditView->getSelectedCharsAndLinesCount(maxSelsToProcessLineCount);
\t\tQString nbSelectionsStr = commafyInt(nbSelections);
\t\tQString multipleSelChars = commafyInt(multipleSelCharsAndLines.first);
\t\tQString multipleSelLines = (nbSelections <= maxSelsToProcessLineCount) ? commafyInt(multipleSelCharsAndLines.second) : QStringLiteral("...");
\t\tstatusbarSelStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Sel-number", "Sel") + QStringLiteral(" ") + nbSelectionsStr + QStringLiteral(" : ") + multipleSelChars + QStringLiteral(" | ") + multipleSelLines;
\t}

\tQString lnStr = commafyInt(_pEditView->getCurrentLineNumber() + 1);
\tQString colStr = commafyInt(_pEditView->getCurrentColumnNumber() + 1);
\tQString statusbarLnColStr = pNativeSpeaker->getLocalizedStrFromID("statusbar-Ln-Col", "Ln: %1    Col: %2").arg(lnStr, colStr);
\tQString statusbarLnColSelStr = statusbarLnColStr + QStringLiteral("    ") + statusbarSelStr;
\t_statusBar.setText(statusbarLnColSelStr, STATUSBAR_CUR_POS);
\t_statusBar.setText(_pEditView->execute(SCI_GETOVERTYPE) ? QStringLiteral("OVR") : QStringLiteral("INS"), STATUSBAR_TYPING_MODE);
}
'''
        lines[i:j+1] = [new_func]
        changed = True
        print(f"9. Replaced updateStatusBar ({j-i+1} → {len(new_func.splitlines())} lines)")
        break

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'w') as f:
    f.writelines(lines)

print(f"\nDone. changed={changed}")
