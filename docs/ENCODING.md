# Encoding Reference

How Notepad--Qt detects, handles, and converts text encodings.

---

## Supported Encodings

| Encoding | Constant | BOM | Notes |
|----------|----------|-----|-------|
| UTF-8 | `UTF_8` | No | Default |
| UTF-8 with BOM | `UTF_8_BOM` | `EF BB BF` | |
| UTF-16 LE | `UTF_16_LE` | No | |
| UTF-16 LE with BOM | `UTF_16_LE_BOM` | `FF FE` | |
| UTF-16 BE | `UTF_16_BE` | No | |
| UTF-16 BE with BOM | `UTF_16_BE_BOM` | `FE FF` | |
| UTF-32 LE | `UTF_32_LE` | No | |
| UTF-32 BE | `UTF_32_BE` | No | |
| ASCII (7-bit) | `ASCII_7` | No | Pure 7-bit |
| Windows-1252 | `Windows1252` | No | Western European |
| ISO-8859-1 | `ISO88591` | No | Latin-1 |
| OEM (CP437) | `OEM` | No | DOS/console |
| EBCDIC | `EBCDIC` | No | IBM mainframe |
| ANSI | `ANSI` | No | System locale (alias for Windows-1252 on Western systems) |

---

## Detection Pipeline

When a file is opened, `EncodingDetector::detect()` runs this sequence:

```
1. BOM check        → look for UTF-8/16/32 BOM in first 4 bytes
2. UTF-8 validation → test byte sequences for validity
3. HTML/XML probe   → look for <meta charset="..."> or <?xml encoding="...">
4. Byte-frequency   → single-byte charset heuristics (Latin-1, CP437, etc.)
5. CJK detection   → byte-frequency for multi-byte East Asian charsets
6. Default          → UTF-8 if all above fail
```

### BOM Detection

Byte Order Marks are checked first. If found, the encoding is set immediately:

| Bytes | Encoding |
|-------|----------|
| `EF BB BF` | UTF-8 with BOM |
| `FF FE` | UTF-16 LE with BOM |
| `FE FF` | UTF-16 BE with BOM |
| `FF FE 00 00` | UTF-32 LE with BOM |
| `00 00 FE FF` | UTF-32 BE with BOM |

### UTF-8 Validation

Validates multi-byte sequences:
- `0x80–0xBF` — continuation byte (must follow a lead byte)
- `0xC0–0xDF` — 2-byte sequence lead
- `0xE0–0xEF` — 3-byte sequence lead
- `0xF0–0xF7` — 4-byte sequence lead

A byte sequence that violates these rules is **not valid UTF-8**.

### HTML/XML Probing

Scans the first 2 KB for:
```html
<meta charset="UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
<?xml version="1.0" encoding="ISO-8859-1"?>
```

### Byte-Frequency Analysis

For plain text without BOM or XML, byte-frequency heuristics guess the single-byte charset:
- `0x80–0xFF` byte frequency is analyzed
- Certain byte distributions are characteristic of specific charsets
- CJK detection uses bigram/trigram frequency tables

---

## Changing Encoding

### Via Menu
```
Encoding → Convert to UTF-8
Encoding → Convert to UTF-8 with BOM
Encoding → Convert to UTF-16 LE
Encoding → Convert to ANSI
Encoding → Convert to Windows-1252
...
```

### Via Status Bar
Click the encoding indicator in the status bar (bottom right) → encoding picker popup.

### Via Encoding Submenu (Edit)
```
Edit → Character Operations → Convert to UTF-8
Edit → Character Operations → Convert to UTF-8 BOM
Edit → Character Operations → Convert to ANSI
...
```

---

## BOM Behavior

**Reading:** BOM is stripped automatically when the file is loaded. The BOM is used only for detection.

**Writing:** BOM is written only for `UTF_8_BOM`, `UTF_16_LE_BOM`, and `UTF_16_BE_BOM`. Other encodings write no BOM.

To force BOM on a new UTF-8 save:
1. `Encoding → Convert to UTF-8 with BOM`
2. Save — the BOM is preserved on save

---

## EOL Detection

`EncodingDetector::detectEol()` counts line ending types in the file:

| Type | Bytes | Constant |
|------|-------|----------|
| Unix | `\n` | `EOL_LF` |
| Windows | `\r\n` | `EOL_CRLF` |
| Old Mac | `\r` | `EOL_CR` |

Detection priority: CRLF → LF → CR (the most common EOL in the file wins).

Change EOL via `Edit → EOL Conversion → Unix (LF)`, `Windows (CRLF)`, or `Mac (CR)`.

---

## Internal Conversion

All text is converted to UTF-8 internally (QString/UTF-8 in Scintilla). On save, it's converted from UTF-8 to the target encoding:

```cpp
QByteArray bytes = EncodingDetector::fromUtf8(utf8String, targetEncoding);
// bytes is written to file
```

`fromUtf8()` uses Qt's codec system (`QTextCodec`) for non-Unicode encodings.

---

## Configuration

Encoding settings are stored in `~/.config/notepad--qt/config.json`:

```json
{
  "defaultEncoding": "UTF-8",
  "defaultEol": "LF",
  "autoDetectEncoding": true,
  "saveWithoutBOM": true
}
```
