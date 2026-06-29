#!/usr/bin/env python3
"""Replace updateStatusBar() function body with Qt-ified version."""

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp') as f:
    lines = f.readlines()

# Find updateStatusBar function
start = None
for i, line in enumerate(lines):
    if 'void Notepad_plus::updateStatusBar()' in line:
        start = i
        break

if start is None:
    print("ERROR: updateStatusBar not found")
    exit(1)

# Find end of function (matching closing brace)
brace_count = 0
end = start
started = False
for i in range(start, len(lines)):
    brace_count += lines[i].count('{') - lines[i].count('}')
    if '{' in lines[i]:
        started = True
    if started and brace_count == 0:
        end = i + 1
        break

print(f"Found updateStatusBar: lines {start+1}-{end} ({end-start} lines)")

# New function body
new_body = '''void Notepad_plus::updateStatusBar()
{
\t// these sections of status bar NOT updated by this function:
\t// STATUSBAR_DOC_TYPE , STATUSBAR_EOF_FORMAT , STATUSBAR_UNICODE_TYPE

\tsize_t docLen = _pEditView->getCurrentDocLen();
\tintptr_t nbLine = _pEditView->execute(SCI_GETLINECOUNT);

\tNativeLangSpeaker* pNativeSpeaker = (NppParameters::getInstance()).getNativeLangSpeaker();
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

\t\t\tif (sel == 0)
\t\t\t{
\t\t\t\tmaxLineCharCount = lineCharCount;
\t\t\t}
\t\t\telse
\t\t\t{
\t\t\t\tif (lineCharCount != maxLineCharCount)
\t\t\t\t{
\t\t\t\t\tsameCharCountOnEveryLine = false;
\t\t\t\t\tif (lineCharCount > maxLineCharCount)
\t\t\t\t\t{
\t\t\t\t\t\tmaxLineCharCount = lineCharCount;
\t\t\t\t\t}
\t\t\t\t}
\t\t\t}
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

lines[start:end] = [new_body]
print(f"Replaced {end-start} lines with new function")

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'w') as f:
    f.writelines(lines)

print("Done!")
