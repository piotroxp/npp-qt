// Printer.h — Print and print-preview support for Notepad--Qt
// Uses QPrinter + QPainter to render Scintilla document content.
// Copyright (C) 2026 Agent Army / GPL v3

#pragma once

#include <QObject>
#include <QString>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

class ScintillaEditor;

class Printer : public QObject {
    Q_OBJECT

public:
    explicit Printer(QObject* parent = nullptr);
    ~Printer() override;

    /// Configure the internal QPrinter before calling print() or printPreview()
    void setupPrinter(const QString& title);

    /// Show print dialog and print the document
    void print(ScintillaEditor* editor);

    /// Show print-preview dialog (render pages without committing to paper)
    void printPreview(ScintillaEditor* editor);

    /// Internal: render the entire document to a QPrinter
    void renderDocument(ScintillaEditor* editor, QPrinter* printer);

Q_SIGNALS:
    /// Emitted after a print job completes successfully
    void printCompleted();

private Q_SLOTS:
    void onPrintRequested(QPrinter* printer);
    void onPreviewPaintRequest(QPrinter* printer);

private:
    void renderPage(QPainter* painter, QPrinter* printer, ScintillaEditor* editor);
    void drawHeader(QPainter* painter, QPrinter* printer, ScintillaEditor* editor, int pageNum);
    void drawFooter(QPainter* painter, QPrinter* printer, int pageNum, int totalPages);
    void renderPage(QPainter* painter, QPrinter* printer, ScintillaEditor* editor, int pageNum);

    /// Convert page rect to usable content rect (subtract margins + header/footer)
    QRectF contentRect(QPrinter* printer) const;

    /// Total pages — computed from line count, font metrics, and page height
    int totalPages(ScintillaEditor* editor, QPrinter* printer) const;

    /// First visible line on a given page (1-indexed page)
    int firstLineOnPage(int page, ScintillaEditor* editor, QPrinter* printer) const;

    QPrinter* _printer = nullptr;
    QString _documentTitle;
    QFont _printFont;
    static constexpr int HeaderHeightPt = 20;
    static constexpr int FooterHeightPt = 20;
    static constexpr int LeftMarginPt   = 40;
    static constexpr int RightMarginPt  = 40;
    static constexpr int TopMarginPt    = 40;
    static constexpr int BottomMarginPt = 40;
};
