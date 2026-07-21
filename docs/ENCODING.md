# Encoding Reference

How Notepad--Qt detects, handles, and converts text encodings.

---

## Supported Encodings

| Encoding | Constant | BOM | Notes |
|----------|----------|-----|-------|
| UTF-8 |  | No | Default |
| UTF-8 with BOM |  |  | |
| UTF-16 LE |  | No | |
| UTF-16 LE with BOM |  |  | |
| UTF-16 BE |  | No | |
| UTF-16 BE with BOM |  |  | |
| UTF-32 LE |  | No | |
| UTF-32 BE |  | No | |
| ASCII (7-bit) |  | No | Pure 7-bit |
| Windows-1252 |  | No | Western European |
| ISO-8859-1 |  | No | Latin-1 |
| OEM (CP437) |  | No | DOS/console |
| EBCDIC |  | No | IBM mainframe |
| ANSI |  | No | System locale (alias for Windows-1252 on Western systems) |

---

## Detection Pipeline

When a file is opened,  runs this sequence:



### BOM Detection

Byte Order Marks are checked first. If found, the encoding is set immediately:

| Bytes | Encoding |
|-------|----------|
|  | UTF-8 with BOM |
|  | UTF-16 LE with BOM |
|  | UTF-16 BE with BOM |
|  | UTF-32 LE with BOM |
|  | UTF-32 BE with BOM |

### UTF-8 Validation

Validates multi-byte sequences:
-  — continuation byte (must follow a lead byte)
-  — 2-byte sequence lead
-  — 3-byte sequence lead
-  — 4-byte sequence lead

A byte sequence that violates these rules is **not valid UTF-8**.

### HTML/XML Probing

Scans the first 2 KB for:


### Byte-Frequency Analysis

For plain text without BOM or XML, byte-frequency heuristics guess the single-byte charset:
-  byte frequency is analyzed
- Certain byte distributions are characteristic of specific charsets
- CJK detection uses bigram/trigram frequency tables

---

## Changing Encoding

### Via Menu


### Via Status Bar
Click the encoding indicator in the status bar (bottom right) → encoding picker popup.

### Via Encoding Submenu (Edit)


---

## BOM Behavior

**Reading:** BOM is stripped automatically when the file is loaded. The BOM is used only for detection.

**Writing:** BOM is written only for , , and . Other encodings write no BOM.

To force BOM on a new UTF-8 save:
1. 
2. Save — the BOM is preserved on save

---

## EOL Detection

 counts line ending types in the file:

| Type | Bytes | Constant |
|------|-------|----------|
| Unix |  |  |
| Windows |  |  |
| Old Mac |  |  |

Detection priority: CRLF → LF → CR (the most common EOL in the file wins).

Change EOL via , , or .

---

## Internal Conversion

All text is converted to UTF-8 internally (QString/UTF-8 in Scintilla). On save, it's converted from UTF-8 to the target encoding:



 uses Qt's codec system () for non-Unicode encodings.

---

## Configuration

Encoding settings are stored in :


