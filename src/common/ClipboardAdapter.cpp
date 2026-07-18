// ClipboardAdapter.cpp — Qt6/Linux clipboard implementation
// Ported from Win32 Clipboard API in notepad-plus-plus-translation
// Copyright (C) 2026 Agent Army | GPL v3

#include "ClipboardAdapter.h"
#include <QRegularExpression>
#include <QUrl>

// ─── Construction ──────────────────────────────────────────────────────────────

ClipboardAdapter::ClipboardAdapter()
    : _clipboard(QGuiApplication::clipboard())
{
    // QGuiApplication::clipboard() is guaranteed to return a valid QClipboard*
    // once a QGuiApplication instance exists.
}

// ─── Text operations ──────────────────────────────────────────────────────────

QString ClipboardAdapter::getText() const
{
    if (!_clipboard) return QString();

    // QClipboard::text() retrieves CF_UNICODETEXT on Win32 and text/plain
    // on Linux/X11; Qt handles encoding conversions automatically.
    return _clipboard->text(QClipboard::Clipboard);
}

bool ClipboardAdapter::setText(const QString& text)
{
    if (!_clipboard) return false;

    // Clears the clipboard and writes plain text.  QClipboard::setText()
    // replaces all existing content with a single "text/plain" format.
    // Win32 equivalent: str2Clipboard()
    //   OpenClipboard() + EmptyClipboard()
    //   + SetClipboardData(CF_UNICODETEXT, hglbCopy)
    //   + CloseClipboard()
    _clipboard->setText(text, QClipboard::Clipboard);
    return true;
}

// ─── MIME format operations ───────────────────────────────────────────────────

bool ClipboardAdapter::hasFormat(const QString& format) const
{
    const QMimeData* md = mimeData();
    if (!md) return false;
    return md->hasFormat(format);
}

QByteArray ClipboardAdapter::getData(const QString& format) const
{
    const QMimeData* md = mimeData();
    if (!md) return QByteArray();

    // Win32 equivalent: GetClipboardData(format)
    // Returns null QByteArray if the format is not available.
    return md->data(format);
}

bool ClipboardAdapter::setData(const QString& format, const QByteArray& data)
{
    if (!_clipboard) return false;

    // Win32 equivalent: SetClipboardData(format, hMem)
    // Note: Unlike Win32's SetClipboardData(), this does NOT clear the
    // clipboard first.  Call clear() explicitly before calling setData()
    // if a clean slate is required.
    //
    // To set multiple formats atomically, use:
    //   clear();
    //   setData("text/plain", ...);
    //   setData("text/html", ...);
    //
    // Fallback for headless/offline Qt platform plugins (offscreen, minimal)
    // where the internal QMimeData is not accessible: use Qt's typed setters
    // for the formats they cover, and return false for everything else.
    if (format == ClipboardFormat::PlainText) {
        _clipboard->setText(QString::fromUtf8(data), QClipboard::Clipboard);
        return true;
    }
    if (format == ClipboardFormat::HtmlText) {
        // text/html on the clipboard may be encoded as UTF-8 bytes.
        QString html = QString::fromUtf8(data);
        QMimeData* md = mimeData();
        if (md) {
            md->setHtml(html);
            return true;
        }
        // Fallback: use setText with the raw bytes.
        _clipboard->setText(html, QClipboard::Clipboard);
        return true;
    }

    QMimeData* md = mimeData();
    if (!md) {
        // Clipboard backend does not expose MIME data (headless mode).
        // Cannot store arbitrary custom formats without a real backend.
        return false;
    }

    md->setData(format, data);
    return true;
}

// ─── Multi-format helpers ─────────────────────────────────────────────────────

QStringList ClipboardAdapter::formats() const
{
    const QMimeData* md = mimeData();
    if (!md) return QStringList();
    return md->formats();
}

void ClipboardAdapter::clear()
{
    if (!_clipboard) return;

    // Win32 equivalent: OpenClipboard() + EmptyClipboard() + CloseClipboard()
    _clipboard->clear(QClipboard::Clipboard);
}

// ─── Image operations ──────────────────────────────────────────────────────────

QImage ClipboardAdapter::getImage() const
{
    const QMimeData* md = mimeData();
    if (!md) return QImage();

    // QClipboard::image() reads image formats (PNG, BMP, etc.) from the
    // clipboard MIME data.
    if (md->hasImage()) {
        return md->imageData().value<QImage>();
    }
    return QImage();
}

bool ClipboardAdapter::setImage(const QImage& image)
{
    if (!_clipboard) return false;

    if (image.isNull()) return false;

    // Win32 equivalent: SetClipboardData(CF_BITMAP / CF_DIB, ...)
    // Qt serialises to image/bmp or image/png depending on clipboard
    // backend.
    _clipboard->setImage(image, QClipboard::Clipboard);
    return true;
}

// ─── Rich text operations ──────────────────────────────────────────────────────

QString ClipboardAdapter::getHtml() const
{
    const QMimeData* md = mimeData();
    if (!md) return QString();

    if (md->hasHtml()) {
        return md->html();
    }
    // Fallback: if no HTML is present, return empty string.
    return QString();
}

bool ClipboardAdapter::setHtml(const QString& html, const QString& plainFallback)
{
    if (html.isEmpty()) return false;
    if (!_clipboard) return false;

    // Set the HTML format via QMimeData if available.
    QMimeData* md = mimeData();
    if (md) {
        md->setHtml(html);
        QString plain = plainFallback.isEmpty() ? htmlToPlainText(html) : plainFallback;
        md->setText(plain);
    } else {
        // Fallback for headless/offline Qt platform plugins (offscreen, minimal)
        // where the internal QMimeData is not accessible: store as plain text.
        QString plain = plainFallback.isEmpty() ? htmlToPlainText(html) : plainFallback;
        _clipboard->setText(plain, QClipboard::Clipboard);
    }
    return true;
}

// ─── URI / file list operations ───────────────────────────────────────────────

QList<QUrl> ClipboardAdapter::getUrls() const
{
    const QMimeData* md = mimeData();
    if (!md) return QList<QUrl>();

    // Win32 equivalent: GetClipboardData(CF_HDROP)
    // On Linux/X11 this returns file:// URIs copied from a file manager.
    if (md->hasUrls()) {
        return md->urls();
    }
    return QList<QUrl>();
}

bool ClipboardAdapter::setUrls(const QList<QUrl>& urls)
{
    QMimeData* md = mimeData();
    if (!md) return false;

    // Win32 equivalent: SetClipboardData(CF_HDROP, hDrop)
    md->setUrls(urls);
    return true;
}

// ─── Round-trip helper ────────────────────────────────────────────────────────

void ClipboardAdapter::pushClipboard()
{
    PushedState state;
    const QMimeData* md = mimeData();

    if (md) {
        for (const QString& fmt : md->formats()) {
            state.formats.append(qMakePair(fmt, md->data(fmt)));
        }
        // Store URLs as serialised strings to avoid QList<QUrl>
        // (QUrl is not nothrow-destructible with Qt6's container guards).
        if (md->hasUrls()) {
            for (const QUrl& u : md->urls()) {
                state.urls.append(u.toString());
            }
        }
    }

    _pushStack.append(state);
}

bool ClipboardAdapter::popClipboard()
{
    if (_pushStack.isEmpty()) return false;

    PushedState state = _pushStack.takeLast();
    if (!_clipboard) return false;

    QMimeData* md = mimeData();
    if (!md) {
        // Clipboard backend does not expose MIME data (headless/offline mode).
        // Cannot restore formats without a real backend.
        return false;
    }

    // Restore all formats that were captured at push time.
    // Note: on Wayland, not all formats may round-trip identically.
    for (const auto& pair : state.formats) {
        md->setData(pair.first, pair.second);
    }

    // Restore URLs that were serialised as strings.
    if (!state.urls.isEmpty()) {
        QList<QUrl> restoredUrls;
        restoredUrls.reserve(state.urls.size());
        for (const QString& s : state.urls) {
            restoredUrls.append(QUrl(s));
        }
        md->setUrls(restoredUrls);
    }

    return true;
}

bool ClipboardAdapter::hasPushedState() const
{
    return !_pushStack.isEmpty();
}

// ─── Convenience ──────────────────────────────────────────────────────────────

bool ClipboardAdapter::copyText(const QString& text)
{
    return setText(text);
}

// ─── Private helpers ──────────────────────────────────────────────────────────

const QMimeData* ClipboardAdapter::mimeData() const
{
    if (!_clipboard) return nullptr;
    return const_cast<QMimeData*>(_clipboard->mimeData(QClipboard::Clipboard));
}

QMimeData* ClipboardAdapter::mimeData()
{
    if (!_clipboard) return nullptr;
    // Safe: the non-const overload calls the same underlying object.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    return const_cast<QMimeData*>(_clipboard->mimeData(QClipboard::Clipboard));
}

QString ClipboardAdapter::htmlToPlainText(const QString& html)
{
    if (html.isEmpty()) return QString();

    QString plain = html;

    // Strip HTML tags.
    plain.replace(QRegularExpression("<[^>]*>"), QString());

    // Decode common named entities.
    plain.replace("&nbsp;", " ");
    plain.replace("&amp;", "&");
    plain.replace("&lt;", "<");
    plain.replace("&gt;", ">");
    plain.replace("&quot;", "\"");
    plain.replace("&apos;", "'");
    plain.replace("&#39;", "'");

    // Decode numeric entities: &#N; and &#xN;
    plain.replace(QRegularExpression("&#x[0-9a-fA-F]+;"), QString());
    plain.replace(QRegularExpression("#[0-9]+;"), QString());

    // Collapse runs of whitespace.
    plain.replace(QRegularExpression("[ \\t]+"), " ");
    plain.replace(QRegularExpression("\\n{3,}"), "\n\n");

    return plain.trimmed();
}
