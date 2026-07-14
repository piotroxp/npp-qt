#include "WebBrowserViewFactory.h"

#include "WebBrowserView.h"

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QMessageLogger>
#include <QtDebug>

// ============================================================================
// WebBrowserViewFactory Implementation
// ============================================================================

WebBrowserView* WebBrowserViewFactory::create(QWidget* parent)
{
    return new WebBrowserView(parent);
}

QString WebBrowserViewFactory::markdownToHtml(const QString& markdown)
{
    if (markdown.isEmpty()) {
        return QString();
    }

    // Basic markdown to HTML conversion
    // For full markdown support, consider using a proper library like discount,
    // hoedown, or cmark. This implementation handles common cases.

    QString html = markdown;

    // Escape HTML special characters first (but preserve markdown formatting)
    // We'll do this after processing markdown syntax

    // Preserve code blocks (```...```)
    QMap<QString, QString> codeBlocks;
    int codeBlockIndex = 0;
    {
        QRegularExpression codeBlockRe(R"(```(\w*)\n(.*?)\n```)", QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatchIterator it = codeBlockRe.globalMatch(html);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString placeholder = QString("__CODEBLOCK_%1__").arg(codeBlockIndex++);
            QString codeBlock = match.captured(0);
            // Escape HTML in code blocks
            codeBlock.replace("&", "&amp;");
            codeBlock.replace("<", "&lt;");
            codeBlock.replace(">", "&gt;");
            codeBlocks[placeholder] = codeBlock;
            html.replace(match.capturedStart(), match.capturedLength(), placeholder);
        }
    }

    // Preserve inline code (`...`)
    QMap<QString, QString> inlineCodeBlocks;
    int inlineCodeIndex = 0;
    {
        QRegularExpression inlineCodeRe(R"(`([^`]+)`)");
        QRegularExpressionMatchIterator it = inlineCodeRe.globalMatch(html);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString placeholder = QString("__INLINECODE_%1__").arg(inlineCodeIndex++);
            QString code = match.captured(1);
            // Escape HTML in inline code
            code.replace("&", "&amp;");
            code.replace("<", "&lt;");
            code.replace(">", "&gt;");
            inlineCodeBlocks[placeholder] = "<code>" + code + "</code>";
            html.replace(match.capturedStart(), match.capturedLength(), placeholder);
        }
    }

    // Process markdown formatting

    // Headers (# heading through ###### heading)
    html.replace(QRegularExpression(R"(^###### (.+)$)", QRegularExpression::MultilineOption), "<h6>\\1</h6>");
    html.replace(QRegularExpression(R"(^##### (.+)$)", QRegularExpression::MultilineOption), "<h5>\\1</h5>");
    html.replace(QRegularExpression(R"(^#### (.+)$)", QRegularExpression::MultilineOption), "<h4>\\1</h4>");
    html.replace(QRegularExpression(R"(^### (.+)$)", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
    html.replace(QRegularExpression(R"(^## (.+)$)", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
    html.replace(QRegularExpression(R"(^# (.+)$)", QRegularExpression::MultilineOption), "<h1>\\1</h1>");

    // Bold (**text** or __text__)
    html.replace(QRegularExpression(R"(\*\*(.+?)\*\*)"), "<strong>\\1</strong>");
    html.replace(QRegularExpression(R"__(__(.+?)__+)__"), "<strong>\\1</strong>");

    // Italic (*text* or _text_)
    html.replace(QRegularExpression(R"(\*([^*]+?)\*)"), "<em>\\1</em>");
    html.replace(QRegularExpression(R"(_([^_]+?)_)"), "<em>\\1</em>");

    // Strikethrough (~~text~~)
    html.replace(QRegularExpression(R"(~~(.+?)~~)"), "<del>\\1</del>");

    // Links [text](url)
    html.replace(QRegularExpression(R"(\[([^\]]+)\]\(([^)]+)\))"), "<a href=\"\\2\">\\1</a>");

    // Images ![alt](url)
    html.replace(QRegularExpression(R"(!\[([^\]]*)\]\(([^)]+)\))"), "<img src=\"\\2\" alt=\"\\1\">");

    // Blockquotes (> quote)
    {
        QRegularExpression blockquoteRe(R"(^&gt; (.+)$)", QRegularExpression::MultilineOption);
        html.replace(blockquoteRe, "<blockquote>\\1</blockquote>");
    }

    // Horizontal rules (--- or *** or ___)
    html.replace(QRegularExpression(R"(^[-*_]{3,}$)", QRegularExpression::MultilineOption), "<hr>");

    // Unordered lists (- item or * item or + item)
    // Convert each list item to <li>
    {
        QRegularExpression ulRe(R"(^[\*+-] (.+)$)", QRegularExpression::MultilineOption);
        html.replace(ulRe, "<li>\\1</li>");
    }

    // Ordered lists (1. item)
    {
        QRegularExpression olRe(R"(^\d+\. (.+)$)", QRegularExpression::MultilineOption);
        html.replace(olRe, "<li>\\1</li>");
    }

    // Wrap consecutive <li> elements in <ul>
    // This is a simplified approach - a proper implementation would track list boundaries

    // Restore code blocks
    for (auto it = codeBlocks.constBegin(); it != codeBlocks.constEnd(); ++it) {
        html.replace(it.key(), it.value());
    }

    // Restore inline code blocks
    for (auto it = inlineCodeBlocks.constBegin(); it != inlineCodeBlocks.constEnd(); ++it) {
        html.replace(it.key(), it.value());
    }

    // Convert newlines to <br> in paragraph-like contexts
    // Split by double newlines for paragraphs
    if (!html.contains("<pre>") && !html.contains("<code") && !html.startsWith("<li>") && !html.startsWith("<h") && !html.startsWith("<blockquote>")) {
        QStringList lines = html.split('\n', Qt::SkipEmptyParts);
        QString processed;
        bool inBlock = false;
        for (const QString& line : lines) {
            if (line.startsWith("<pre") || line.startsWith("<code") || line.startsWith("<li") ||
                line.startsWith("<h") || line.startsWith("<blockquote") || line.startsWith("<hr")) {
                processed += line + "\n";
                inBlock = (line.startsWith("<pre>") || line.startsWith("<code>"));
            } else if (inBlock) {
                processed += line + "\n";
            } else if (!line.trimmed().isEmpty()) {
                if (!line.startsWith("<ul") && !line.startsWith("<ol") && !line.startsWith("</ul") && !line.startsWith("</ol")) {
                    processed += "<p>" + line.trimmed() + "</p>\n";
                } else {
                    processed += line + "\n";
                }
            }
        }
        html = processed;
    }

    return html;
}

QString WebBrowserViewFactory::localFileToDataUrl(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "WebBrowserViewFactory: Failed to open file:" << filePath;
        return QString();
    }

    QByteArray data = file.readAll();
    file.close();

    // Detect MIME type from file extension
    QString mimeType = "application/octet-stream";
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();

    static const QMap<QString, QString> mimeTypes = {
        {"txt", "text/plain"},
        {"html", "text/html"},
        {"htm", "text/html"},
        {"css", "text/css"},
        {"js", "application/javascript"},
        {"json", "application/json"},
        {"xml", "application/xml"},
        {"svg", "image/svg+xml"},
        {"png", "image/png"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"gif", "image/gif"},
        {"webp", "image/webp"},
        {"ico", "image/x-icon"},
        {"pdf", "application/pdf"},
        {"woff", "font/woff"},
        {"woff2", "font/woff2"},
        {"ttf", "font/ttf"},
        {"eot", "application/vnd.ms-fontobject"},
    };

    auto it = mimeTypes.find(ext);
    if (it != mimeTypes.end()) {
        mimeType = it.value();
    }

    // Encode as base64 data URL
    QString base64 = QString::fromLatin1(data.toBase64(QByteArray::Base64Encoding));
    return QString("data:%1;base64,%2").arg(mimeType, base64);
}
