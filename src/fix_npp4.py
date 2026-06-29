#!/usr/bin/env python3
"""Fix remaining Notepad_plus.cpp errors - precise byte-level."""

with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'rb') as f:
    content = f.read()

decoded = content.decode('utf-8', errors='replace')
changes = 0

def do_replace(old_bytes, new_bytes, desc):
    global content, decoded, changes
    if isinstance(old_bytes, str):
        old_bytes = old_bytes.encode('utf-8')
    if isinstance(new_bytes, str):
        new_bytes = new_bytes.encode('utf-8')
    if old_bytes in content:
        content = content.replace(old_bytes, new_bytes, 1)
        decoded = content.decode('utf-8', errors='replace')
        changes += 1
        print(f"  ✓ Fix {changes}: {desc}")
        return True
    else:
        print(f"  ✗ MISSED: {desc}")
        return False

# ── Fix 1: Add Win32 wrapper functions before addHotSpot ──
wrapper = b'''static int winMultiByteToWideChar(unsigned int cp, const char* src, int srcLen, wchar_t* dst, int dstLen)
{
#ifdef _WIN32
    return ::MultiByteToWideChar(cp, 0, src, srcLen, dst, dstLen);
#else
    Q_UNUSED(cp);
    if (!dst) return srcLen;
    int n = qMin(srcLen, dstLen - 1);
    for (int i = 0; i < n; ++i) dst[i] = static_cast<wchar_t>(static_cast<unsigned char>(src[i]));
    dst[n] = 0;
    return n;
#endif
}

static int winWideCharToMultiByte(unsigned int cp, const wchar_t* src, int srcLen, char* dst, int dstLen)
{
#ifdef _WIN32
    return ::WideCharToMultiByte(cp, 0, src, srcLen, dst, dstLen, nullptr, nullptr);
#else
    Q_UNUSED(cp);
    if (!dst) return srcLen;
    int n = qMin(srcLen, dstLen - 1);
    for (int i = 0; i < n; ++i) dst[i] = static_cast<char>(src[i]);
    dst[n] = 0;
    return n;
#endif
}

'''
old = b'void Notepad_plus::addHotSpot(ScintillaEditView* view)\n{'
new = wrapper + old
do_replace(old, new, "Add Win32 wrapper functions before addHotSpot")

# ── Fix 2: wcscpy_s title_temp.c_str() → title_temp.toStdWString().c_str() ──
# Exact bytes: title[32] is at 3-tab indent inside an if
old = b'\t\tstatic wchar_t title[32];\n\t\tif (title_temp.length() < 32)\n\t\t{\n\t\t\twcscpy_s(title, title_temp.c_str());'
new = b'\t\tstatic wchar_t title[32];\n\t\tif (title_temp.length() < 32)\n\t\t{\n\t\t\tQString ts = title_temp;\n\t\t\twcscpy_s(title, ts.toStdWString().c_str());'
do_replace(old, new, "wcscpy_s: title_temp.c_str() -> ts.toStdWString().c_str()")

# ── Write back ──
with open('/home/node/.openclaw/workspace/npp-qt/src/Notepad_plus.cpp', 'wb') as f:
    f.write(content)

print(f"\nTotal changes: {changes}")