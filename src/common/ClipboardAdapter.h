// ClipboardAdapter.h — Qt6/Linux clipboard abstraction wrapping QClipboard
// Ported from Win32 Clipboard API in notepad-plus-plus-translation
// Copyright (C) 2026 Agent Army | GPL v3
//
// Provides a platform-neutral clipboard interface for:
//   - Plain text get/set  (→ QClipboard::text())
//   - Arbitrary MIME format get/set  (→ QMimeData)
//   - Copy/paste round-trip preservation
//
// Win32 source equivalents:
//   str2Clipboard()     → setText()
//   strFromClipboard()  → getText()
//   GetClipboardData()  → getData(format)
//   SetClipboardData()  → setData(format, data)
//
// Platform notes:
//   - Linux/X11: uses the X11 CLIPBOARD selection (not PRIMARY/secondary)
//   - Linux/Wayland: QT_QPA_PLATFORM=wayland relies on the Wayland clipboard
//     protocol (wl_data_device_manager).  Round-trip support is
//     compositor-dependent; some compositors do not preserve multiple
//     formats on a single paste event.  Prefer plain-text operations when
//     portability across Wayland compositors is required.

#pragma once

#include <QString>
#include <QByteArray>
#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <QImage>
#include <QList>
#include <QPair>
#include <memory>

// Forward declarations — full definitions for QUrl require Qt6::Network.
class QUrl;

// ─── Format constants (mirrors clipboardFormats.h from Win32) ────────────────
namespace ClipboardMime {
    // Standard MIME types used as internal format keys
    constexpr const char* PlainText   = "text/plain";
    constexpr const char* HtmlText    = "text/html";
    constexpr const char* RichText    = "text/rtf";
    constexpr const char* UriList     = "text/uri-list";
    constexpr const char* ImagePng    = "image/png";
    constexpr const char* ImageBmp    = "image/bmp";

    // Legacy Win32-style format names (informational — not Windows constants)
    // These are provided for documentation and cross-platform compatibility.
    constexpr const char* NppBinaryLen = "application/x-npp-binary-length"; // Notepad++ internal
}

class ClipboardAdapter {
public:
    // ─── Construction ──────────────────────────────────────────────────────
    // Obtains QGuiApplication::clipboard().  Must be called after
    // QGuiApplication (or QApplication) has been constructed.
    explicit ClipboardAdapter();

    // ─── Text operations ───────────────────────────────────────────────────
    // Get plain text from the clipboard.
    // Returns an empty QString if the clipboard contains no text.
    // Win32 equivalent: strFromClipboard() → GetClipboardData(CF_UNICODETEXT)
    QString getText() const;

    // Set plain text on the clipboard.
    // Clears all other formats and replaces with the given text.
    // Win32 equivalent: str2Clipboard() → SetClipboardData(CF_UNICODETEXT)
    bool setText(const QString& text);

    // ─── Arbitrary MIME-format operations ─────────────────────────────────
    // Check whether the clipboard contains data in a given MIME format.
    // Uses QMimeData::hasFormat() internally.
    bool hasFormat(const QString& format) const;

    // Get raw bytes for a given MIME format.
    // Returns a null QByteArray if the format is not present.
    // Win32 equivalent: GetClipboardData(format) for any registered format.
    QByteArray getData(const QString& format) const;

    // Set raw bytes for a given MIME format.
    // This does NOT clear existing formats — call clear() first for a clean
    // slate, or use setText() / setImage() for a single-format replacement.
    // Win32 equivalent: SetClipboardData(format, data)
    bool setData(const QString& format, const QByteArray& data);

    // ─── Multi-format operations ─────────────────────────────────────────
    // Return the list of MIME formats currently available on the clipboard.
    // Uses QMimeData::formats() internally.
    QStringList formats() const;

    // Clear the entire clipboard (all formats).
    // Win32 equivalent: OpenClipboard() + EmptyClipboard() + CloseClipboard()
    void clear();

    // ─── Image operations ─────────────────────────────────────────────────
    // Get image from clipboard if one is present.
    // Returns a null QImage if no image is on the clipboard.
    // Win32 equivalent: GetClipboardData(CF_DIB) / CF_BITMAP handling
    QImage getImage() const;

    // Set image on the clipboard.
    // Win32 equivalent: SetClipboardData(CF_DIB / CF_BITMAP)
    bool setImage(const QImage& image);

    // ─── Rich text operations ─────────────────────────────────────────────
    // Get HTML-formatted rich text from the clipboard.
    // Falls back to plain text if "text/html" is not present.
    QString getHtml() const;

    // Set HTML-formatted rich text alongside plain text.
    // Sets both "text/html" and "text/plain" (derived from the HTML by
    // stripping tags) so paste targets that only understand plain text
    // still receive the content.
    bool setHtml(const QString& html, const QString& plainFallback = QString());

    // ─── URI / file list operations ────────────────────────────────────────
    // Get list of file paths from clipboard (e.g. from a file-manager copy).
    // Returns an empty list if no URI list is present.
    QList<QUrl> getUrls() const;

    // Set a list of file URLs on the clipboard.
    bool setUrls(const QList<QUrl>& urls);

    // ─── Round-trip helper ────────────────────────────────────────────────
    // Push the current clipboard state (all formats) into an internal
    // history stack.  Call popClipboard() to restore it.
    // Useful when the application needs to temporarily replace clipboard
    // content (e.g. for "paste and don't lose original" scenarios).
    void pushClipboard();

    // Restore clipboard to the state saved by the most recent pushClipboard().
    // Returns false if the stack is empty; the clipboard is left unchanged.
    bool popClipboard();

    // Returns true when push/pop stack is non-empty.
    bool hasPushedState() const;

    // ─── Convenience: copy selected text from the editor ─────────────────
    // Combines setText() and sets the internal "wasInternalCopy" flag so
    // that paste events can be detected as application-originated.
    bool copyText(const QString& text);

    // ─── Accessor ─────────────────────────────────────────────────────────
    QClipboard* clipboard() const { return _clipboard; }

private:
    QClipboard* _clipboard = nullptr;

    // Round-trip state: a MIME data snapshot taken at pushClipboard() time.
    // QUrl is not nothrow-destructible so we store URLs as their string
    // representations to avoid QList<QUrl> instantiation issues with Qt6's
    // static-assert guard in QArrayDataOps.
    struct PushedState {
        QList<QPair<QString, QByteArray>> formats; // format → data
        QStringList urls;                          // serialised URL strings
    };
    QList<PushedState> _pushStack;

    // Helper: convert HTML to a plain-text approximation for the text/plain
    // companion of setHtml().
    static QString htmlToPlainText(const QString& html);

    // Helper: obtain the internal QMimeData pointer.
    const QMimeData* mimeData() const;
    QMimeData* mimeData();
};
