// Printer.cpp — Print and print-preview implementation for Notepad--Qt
// Uses QPrinter + QPainter to render Scintilla document content.
// Copyright (C) 2026 Agent Army / GPL v3

#include "Printer.h"
#include "../editor/ScintillaEditor.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QPageLayout>
#include <QPageSize>
#include <QMarginsF>
#include <QDebug>

Printer::Printer(QObject* parent)
    : QObject(parent)
    , _printFont("Courier New", 10)
{
}

Printer::~Printer() = default;

void Printer::setupPrinter(const QString& title) {
    if (!_printer) {
        _printer = new QPrinter(QPrinter::HighResolution);
    }
    _printer->setPrinterName(title);
    _printer->setDocName(title);
    _documentTitle = title;
}

void Printer::print(ScintillaEditor* editor) {
    if (!editor) return;

    if (!_printer) {
        _printer = new QPrinter(QPrinter::HighResolution);
    }
    if (!_documentTitle.isEmpty()) {
        _printer->setDocName(_documentTitle);
    }

    QPrintDialog dialog(_printer, editor->window());
    dialog.setWindowTitle("Print");
    if (dialog.exec() != QDialog::Accepted) return;

    renderDocument(editor, _printer);
    Q_EMIT printCompleted();
}

void Printer::printPreview(ScintillaEditor* editor) {
    if (!editor) return;

    if (!_printer) {
        _printer = new QPrinter(QPrinter::HighResolution);
    }
    if (!_documentTitle.isEmpty()) {
        _printer->setDocName(_documentTitle);
    }

    QPrintPreviewDialog preview(_printer, editor->window());
    preview.setWindowTitle("Print Preview");
    connect(&preview, &QPrintPreviewDialog::paintRequested,
            this, [this, editor](QPrinter* p) {
                renderDocument(editor, p);
            });
    preview.exec();
    Q_EMIT printCompleted();
}

void Printer::renderDocument(ScintillaEditor* editor, QPrinter* printer) {
    if (!editor || !printer) return;

    QPainter painter(printer);
    if (!painter.isActive()) return;

    // Use high-quality rendering
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    int total = totalPages(editor, printer);
    if (total == 0) total = 1;

    for (int page = 1; page <= total; ++page) {
        if (page > 1) {
            if (!printer->newPage()) break;
        }
        drawHeader(&painter, printer, editor, page);
        renderPage(&painter, printer, editor, page);
        drawFooter(&painter, printer, page, total);
    }

    painter.end();
}

void Printer::onPrintRequested(QPrinter* printer) {
    QPainter painter(printer);
    painter.end();
}

void Printer::onPreviewPaintRequest(QPrinter* printer) {
    QPainter painter(printer);
    painter.end();
}

QRectF Printer::contentRect(QPrinter* printer) const {
    if (!printer) return QRectF();

    // Page margins in points (1 pt = 1/72 inch)
    QMarginsF margins(TopMarginPt, LeftMarginPt, RightMarginPt, BottomMarginPt);

    // Content rect: page area minus header, footer, and margins
    QRectF pageRect = printer->pageLayout().fullRectPoints();
    qreal top    = pageRect.top()    + margins.top()    + HeaderHeightPt;
    qreal left   = pageRect.left()   + margins.left();
    qreal right  = pageRect.right()  - margins.right();
    qreal bottom = pageRect.bottom() - margins.bottom() - FooterHeightPt;

    return QRectF(left, top, right - left, bottom - top);
}

int Printer::totalPages(ScintillaEditor* editor, QPrinter* printer) const {
    if (!editor || !printer) return 0;

    QPainter dummy;
    dummy.begin(printer);
    dummy.setFont(_printFont);
    QFontMetricsF fm(_printFont);
    dummy.end();

    qreal pageHeight = contentRect(printer).height();
    if (pageHeight <= 0) return 1;

    int lineCount = editor->lineCount();
    // Approximate lines per page using font height (including line spacing)
    qreal lineHeight = fm.lineSpacing();
    if (lineHeight <= 0) lineHeight = 14;

    int linesPerPage = qMax(1, qFloor(pageHeight / lineHeight));
    int pages = qMax(1, qCeil(qreal(lineCount) / linesPerPage));
    return pages;
}

int Printer::firstLineOnPage(int page, ScintillaEditor* editor, QPrinter* printer) const {
    if (!editor || !printer || page < 1) return 0;

    QPainter dummy;
    dummy.begin(printer);
    dummy.setFont(_printFont);
    QFontMetricsF fm(_printFont);
    dummy.end();

    qreal pageHeight = contentRect(printer).height();
    qreal lineHeight = fm.lineSpacing();
    if (lineHeight <= 0) lineHeight = 14;

    int linesPerPage = qMax(1, qFloor(pageHeight / lineHeight));
    return (page - 1) * linesPerPage;
}

void Printer::renderPage(QPainter* painter, QPrinter* printer,
                          ScintillaEditor* editor, int pageNum) {
    if (!painter || !printer || !editor) return;

    QRectF rect = contentRect(printer);
    if (rect.isEmpty()) return;

    painter->setFont(_printFont);
    QFontMetricsF fm(_printFont);
    qreal lineHeight = fm.lineSpacing();
    qreal tabWidth = _printFont.pointSize() * 4; // approximate tab width

    int firstLine = firstLineOnPage(pageNum, editor, printer);
    int lastLine  = editor->lineCount();

    // Estimate lines per page from available height
    int linesPerPage = qMax(1, qFloor(rect.height() / lineHeight));
    int maxLine = qMin(firstLine + linesPerPage, lastLine);

    painter->save();
    painter->setClipRect(rect);

    qreal y = rect.top();
    for (int line = firstLine; line < maxLine && y < rect.bottom(); ++line) {
        QString text = editor->text().section('\n', line, line);
        // Expand tabs for printing
        text.replace('\t', QString(tabWidth, QChar(' ')));

        // Draw background for alternating lines (subtle)
        if (line % 2 == 0) {
            painter->fillRect(QRectF(rect.left(), y, rect.width(), lineHeight),
                              QColor(245, 245, 245));
        }

        // Draw line number gutter
        QString lineNumStr = QString::number(line + 1).rightJustified(4, ' ');
        painter->setPen(QColor(128, 128, 128));
        painter->drawText(QRectF(rect.left(), y, rect.width() * 0.08, lineHeight),
                          Qt::AlignRight | Qt::AlignVCenter, lineNumStr);

        // Draw separator between gutter and text
        painter->setPen(QColor(200, 200, 200));
        painter->drawLine(QPointF(rect.left() + rect.width() * 0.08, y),
                          QPointF(rect.left() + rect.width() * 0.08, y + lineHeight));

        // Draw text
        painter->setPen(Qt::black);
        QRectF textRect(rect.left() + rect.width() * 0.09, y,
                        rect.width() * 0.91, lineHeight);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

        y += lineHeight;
    }

    painter->restore();
}

void Printer::drawHeader(QPainter* painter, QPrinter* printer,
                          ScintillaEditor* editor, int pageNum) {
    if (!painter || !printer) return;

    QRectF pageRect = printer->pageLayout().fullRectPoints();
    QFont headerFont(_printFont);
    headerFont.setPointSize(8);
    headerFont.setItalic(true);
    painter->setFont(headerFont);
    painter->setPen(QColor(100, 100, 100));

    qreal y = pageRect.top() + TopMarginPt;

    // Left: document title or filename
    QString leftText = _documentTitle.isEmpty() ? "Notepad--Qt" : _documentTitle;
    painter->drawText(pageRect.left() + LeftMarginPt, y,
                      Qt::AlignLeft | Qt::AlignBottom, leftText);

    // Center: "(No Title)" or first line of document as header
    if (editor && !editor->text().isEmpty()) {
        QString firstLine = editor->text().section('\n', 0, 0);
        if (firstLine.length() > 60) firstLine = firstLine.left(60) + "...";
        painter->drawText(pageRect, Qt::AlignHCenter | Qt::AlignBottom, firstLine);
    }

    // Right: page number label
    QString rightText = QString("Page %1").arg(pageNum);
    painter->drawText(pageRect.right() - RightMarginPt, y,
                      Qt::AlignRight | Qt::AlignBottom, rightText);

    // Separator line under header
    painter->setPen(QColor(180, 180, 180));
    qreal sepY = pageRect.top() + TopMarginPt + HeaderHeightPt;
    painter->drawLine(QPointF(pageRect.left() + LeftMarginPt, sepY),
                      QPointF(pageRect.right() - RightMarginPt, sepY));
}

void Printer::drawFooter(QPainter* painter, QPrinter* printer, int pageNum, int totalPages) {
    if (!painter || !printer) return;

    QRectF pageRect = printer->pageLayout().fullRectPoints();
    QFont footerFont(_printFont);
    footerFont.setPointSize(8);
    footerFont.setItalic(true);
    painter->setFont(footerFont);
    painter->setPen(QColor(100, 100, 100));

    qreal sepY = pageRect.bottom() - BottomMarginPt - FooterHeightPt;

    // Separator line above footer
    painter->setPen(QColor(180, 180, 180));
    painter->drawLine(QPointF(pageRect.left() + LeftMarginPt, sepY),
                      QPointF(pageRect.right() - RightMarginPt, sepY));

    // Footer text: right-aligned page X of Y
    qreal y = pageRect.bottom() - BottomMarginPt;
    QString footer = QString("Page %1 of %2").arg(pageNum).arg(totalPages);
    painter->drawText(pageRect.right() - RightMarginPt, y,
                      Qt::AlignRight | Qt::AlignTop, footer);
}
