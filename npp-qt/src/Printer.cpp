// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Port of Printer.cpp — maps Win32 HDC printing to Qt QPrinter
#include "Printer.h"
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QPainterPath>
#include <QFileInfo>
#include <QDateTime>
#include <QPageLayout>
#include "Parameters.h"
#include "ScintillaComponent.h"
#include "NppConstants.h"

// Expand $(SHORT_DATE), $(LONG_DATE), $(TIME), $(CURRENT_PRINTING_PAGE)
// in header/footer template strings.
static void expandPrintVariables(QString& str, const QString& shortDate,
                                 const QString& longDate, const QString& time,
                                 const QString& pageNum)
{
    str.replace(QLatin1String("$(SHORT_DATE)"), shortDate);
    str.replace(QLatin1String("$(LONG_DATE)"),  longDate);
    str.replace(QLatin1String("$(TIME)"),        time);
    str.replace(QLatin1String("$(CURRENT_PRINTING_PAGE)"), pageNum);
}

// Draw a header or footer band onto the painter.
// Mirrors Win32 ExtTextOut-based header/footer with a separator line.
static void drawPrintBand(QPainter* p, const QString& leftText,
                          const QString& middleText, const QString& rightText,
                          const QRectF& bandRect, const QFont& bandFont,
                          bool isRTL, bool drawSeparator)
{
    if (leftText.isEmpty() && middleText.isEmpty() && rightText.isEmpty())
        return;

    p->save();
    p->setFont(bandFont);
    p->setPen(Qt::black);

    // Split page into three columns for left/middle/right alignment
    const qreal colW = bandRect.width() / 3.0;

    p->drawText(QRectF(bandRect.left(), bandRect.top(), colW, bandRect.height()),
                Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, leftText);

    p->drawText(QRectF(bandRect.left() + colW, bandRect.top(), colW, bandRect.height()),
                Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine, middleText);

    p->drawText(QRectF(bandRect.left() + 2.0 * colW, bandRect.top(), colW, bandRect.height()),
                Qt::AlignRight | Qt::AlignVCenter | Qt::TextSingleLine, rightText);

    if (drawSeparator) {
        p->setPen(QPen(Qt::darkGray, 1));
        p->drawLine(bandRect.left(), bandRect.bottom() - 1,
                    bandRect.right(), bandRect.bottom() - 1);
    }

    p->restore();
}

// -----------------------------------------------------------------------
// NppPrinter::init — maps Win32 Printer::init to Qt6
// Stores print parameters; QPrinter is constructed lazily in doPrint().
// -----------------------------------------------------------------------
void NppPrinter::init(QWidget* parent, ScintillaComponent* pSEView,
                      bool showDialog, size_t startPos, size_t endPos, bool isRTL)
{
    _pSEView     = pSEView;
    _startPos     = startPos;
    _endPos       = endPos;
    _isRTL        = isRTL;
    _showDialog   = showDialog;
    _parent       = parent;
}

// -----------------------------------------------------------------------
// doPrint — full print with header/footer, margin, colour-mode support
// Returns number of pages printed.
// -----------------------------------------------------------------------
size_t NppPrinter::doPrint(bool justDoIt)
{
    ScintillaComponent* view = qobject_cast<ScintillaComponent*>(_pSEView);
    if (!view)
        return 0;

    const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();

    // --- QPrinter ---
    QPrinter printer(QPrinter::HighResolution);

    // --- Print dialog ---
    if (!justDoIt) {
        QPrintDialog dialog(&printer, _parent);
        if (dialog.exec() != QDialog::Accepted)
            return 0;
    }

    // --- Document name ---
    QString docName;
    if (auto* buf = view->getCurrentBuffer()) {
        docName = QString::fromWCharArray(buf->getFullPathName());
    }
    if (docName.isEmpty())
        docName = QStringLiteral("Untitled");
    printer.setDocName(docName);

    // --- Header/footer settings ---
    bool hasHeader = nppGUI._printSettings.isHeaderPresent();
    bool hasFooter = nppGUI._printSettings.isFooterPresent();

    QDateTime now = QDateTime::currentDateTime();
    // Qt6 removed Qt::SystemLocaleShortDate/LongDate/LocalDate.
    // Use QLocale for locale-aware formatting instead.
    const QString shortDate = QLocale::system().toString(now.date(), QLocale::ShortFormat);
    const QString longDate  = QLocale::system().toString(now.date(), QLocale::LongFormat);
    const QString timeStr   = QLocale::system().toString(now.time(), QLocale::ShortFormat);

    // Header/footer fonts
    auto makeBandFont = [&](const PrintSettings& ps) {
        QString face = ps._headerFontName.empty()
                           ? QStringLiteral("Arial")
                           : QString::fromStdWString(ps._headerFontName);
        int pt = ps._headerFontSize ? ps._headerFontSize : 9;
        QFont f(face, pt);
        if (ps._headerFontStyle & FONTSTYLE_BOLD)   f.setBold(true);
        if (ps._headerFontStyle & FONTSTYLE_ITALIC)  f.setItalic(true);
        return f;
    };
    auto makeFooterFont = [&](const PrintSettings& ps) {
        QString face = ps._footerFontName.empty()
                           ? QStringLiteral("Arial")
                           : QString::fromStdWString(ps._footerFontName);
        int pt = ps._footerFontSize ? ps._footerFontSize : 9;
        QFont f(face, pt);
        if (ps._footerFontStyle & FONTSTYLE_BOLD)   f.setBold(true);
        if (ps._footerFontStyle & FONTSTYLE_ITALIC) f.setItalic(true);
        return f;
    };
    QFont hdrFont = makeBandFont(nppGUI._printSettings);
    QFont ftrFont = makeFooterFont(nppGUI._printSettings);

    // --- Margins ---
    if (nppGUI._printSettings.isUserMargePresent()) {
        const auto& m = nppGUI._printSettings._marge;
        // nppGUI margins are in 1/100 inch; QPageLayout uses mm
        QMarginsF margins(m.left() / 3.937f, m.top() / 3.937f,
                         m.right() / 3.937f, m.bottom() / 3.937f);
        QPageLayout layout = printer.pageLayout();
        layout.setMargins(margins);
        printer.setPageLayout(layout);
    }

    // --- Hide line-number margin ---
    bool lnWasVisible = view->hasMarginShown(ScintillaComponent::_SC_MARGE_LINENUMBER);
    if (!nppGUI._printSettings._printLineNumber)
        view->showMargin(ScintillaComponent::_SC_MARGE_LINENUMBER, false);

    // --- Scintilla colour mode ---
    // Map Win32 _printOption to Scintilla SCI_SETPRINTCOLOURMODE values:
    // Win32:       0=colour,  2=invert,    3=bg-white, 4=black-on-white
    // Scintilla:   0=normal,  1=reverse,   3=col-white, 2=black-on-white
    int winOpt = nppGUI._printSettings._printOption;
    int sciOpt = (winOpt == 0) ? 0
                : (winOpt == 2) ? 1    // invert → reverse
                : (winOpt == 3) ? 3    // bg-white → colour on white
                : (winOpt == 4) ? 2    // black-on-white
                : 0;
    view->execute(SCI_SETPRINTCOLOURMODE, sciOpt);

    // --- Determine print range ---
    size_t printStart = _startPos;
    size_t printEnd   = _endPos;
    size_t docLen    = static_cast<size_t>(view->getCurrentDocLen());

    if (printStart == printEnd) {
        printStart = 0;
        printEnd   = docLen;
    } else if (printStart > printEnd) {
        std::swap(printStart, printEnd);
    }
    if (printEnd > docLen)
        printEnd = docLen;

    // --- Page range ---
    int fromPage = printer.fromPage();
    int toPage   = printer.toPage();
    if (fromPage == 0) fromPage = 1;
    if (toPage   == 0) toPage   = 9999;

    // --- Begin painting ---
    QPainter painter;
    if (!painter.begin(&printer))
        return 0;

    if (winOpt == 2)  // invert → difference composition
        painter.setCompositionMode(QPainter::CompositionMode_Difference);
    else if (winOpt == 4)  // black-on-white → source only
        painter.setCompositionMode(QPainter::CompositionMode_Source);

    if (winOpt == 3)  // bg-white → fill white
        painter.fillRect(painter.viewport(), Qt::white);

    // Font metrics for band sizing
    const qreal hdrHeight = QFontMetrics(hdrFont, &printer).height() * 1.5;
    const qreal ftrHeight = QFontMetrics(ftrFont, &printer).height() * 1.5;

    Sci_CharacterRangeFull range;
    range.cpMin = static_cast<Sci_Position>(printStart);
    range.cpMax = static_cast<Sci_Position>(printEnd);

    int pageNum = 0;
    QString pageStr;

    do {
        ++pageNum;
        pageStr = QString::number(pageNum);

        QRectF pageRect = printer.pageRect(QPrinter::DevicePixel);

        // Expand page-variable in header/footer
        auto expandedHdr = [&](QString s) {
            expandPrintVariables(s, shortDate, longDate, timeStr, pageStr);
            return s;
        };
        auto expandedFtr = [&](QString s) {
            expandPrintVariables(s, shortDate, longDate, timeStr, pageStr);
            return s;
        };

        // --- Draw header band ---
        if (hasHeader) {
            QRectF hdrRect(pageRect.left(), pageRect.top(),
                           pageRect.width(), hdrHeight);
            drawPrintBand(&painter,
                         expandedHdr(QString::fromStdWString(nppGUI._printSettings._headerLeft)),
                         expandedHdr(QString::fromStdWString(nppGUI._printSettings._headerMiddle)),
                         expandedHdr(QString::fromStdWString(nppGUI._printSettings._headerRight)),
                         hdrRect, hdrFont, _isRTL, true);
        }

        // --- Draw footer band ---
        if (hasFooter) {
            QRectF ftrRect(pageRect.left(),
                           pageRect.bottom() - ftrHeight,
                           pageRect.width(), ftrHeight);
            drawPrintBand(&painter,
                         expandedFtr(QString::fromStdWString(nppGUI._printSettings._footerLeft)),
                         expandedFtr(QString::fromStdWString(nppGUI._printSettings._footerMiddle)),
                         expandedFtr(QString::fromStdWString(nppGUI._printSettings._footerRight)),
                         ftrRect, ftrFont, _isRTL, true);
        }

        // Shrink content rect by header/footer
        qreal contentTop    = pageRect.top()    + (hasHeader ? hdrHeight + 2 : 0);
        qreal contentBottom = pageRect.bottom() - (hasFooter ? ftrHeight + 2 : 0);

        // Shift QPainter to content origin so Scintilla renders in the right spot
        painter.save();
        painter.translate(pageRect.left(), contentTop);

        // Configure Scintilla print range for the current page
        Sci_CharacterRangeFull pageRange;
        pageRange.cpMin = range.cpMin;
        pageRange.cpMax = range.cpMax;

        // Set the drawing area for Scintilla via page rect
        // (Scintilla uses the current painter transform + clip rect)
        painter.setClipRect(QRectF(0, 0,
                                   pageRect.width(),
                                   contentBottom - contentTop));

        // Draw page via Scintilla FORMATRANGEFULL
        bool doDraw = justDoIt && (pageNum >= fromPage && pageNum <= toPage);
        sptr_t cpMinAfter = view->execute(SCI_FORMATRANGEFULL,
                                       doDraw ? TRUE : FALSE,
                                       reinterpret_cast<sptr_t>(&pageRange));

        painter.restore();

        // Advance range for next page
        range.cpMin = static_cast<Sci_Position>(cpMinAfter);
        if (cpMinAfter == 0 || cpMinAfter == range.cpMin)
            break;  // no progress — exit loop

        if (pageNum >= toPage)
            break;

        if (!printer.newPage())
            break;

    } while (range.cpMin < range.cpMax);

    painter.end();

    // Release Scintilla's FORMATRANGE resources
    view->execute(SCI_FORMATRANGEFULL, FALSE, 0);

    // --- Restore line-number margin ---
    view->showMargin(ScintillaComponent::_SC_MARGE_LINENUMBER, lnWasVisible);

    return static_cast<size_t>(pageNum > 0 ? pageNum : 1);
}

// -----------------------------------------------------------------------
// printPreview — show Qt print-preview dialog
// -----------------------------------------------------------------------
void NppPrinter::printPreview()
{
    if (!_pSEView)
        return;

    QPrinter previewPrinter(QPrinter::HighResolution);
    QPrintPreviewDialog dialog(&previewPrinter, _parent);
    dialog.setWindowTitle(QStringLiteral("Print Preview"));

    QObject::connect(&dialog, &QPrintPreviewDialog::paintRequested,
                     this, [this](QPrinter* pr) {
        ScintillaComponent* view = qobject_cast<ScintillaComponent*>(_pSEView);
        if (!view) return;

        const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();

        QPainter painter;
        if (!painter.begin(pr))
            return;

        // Page rect
        QRectF pageRect = pr->pageRect(QPrinter::DevicePixel);
        bool hasHeader  = nppGUI._printSettings.isHeaderPresent();
        bool hasFooter  = nppGUI._printSettings.isFooterPresent();

        // Fonts
        QFont hdrFont(QStringLiteral("Arial"),
                      nppGUI._printSettings._headerFontSize
                          ? nppGUI._printSettings._headerFontSize : 9);
        QFont ftrFont(QStringLiteral("Arial"),
                      nppGUI._printSettings._footerFontSize
                          ? nppGUI._printSettings._footerFontSize : 9);

        const qreal hdrH = QFontMetrics(hdrFont, pr).height() * 1.5;
        const qreal ftrH = QFontMetrics(ftrFont, pr).height() * 1.5;

        Sci_CharacterRangeFull range;
        range.cpMin = 0;
        range.cpMax = static_cast<Sci_Position>(view->getCurrentDocLen());

        QDateTime now = QDateTime::currentDateTime();
        // Qt6 removed Qt::SystemLocaleShortDate/LongDate/LocalDate.
        // Use QLocale for locale-aware formatting instead.
        const QString shortDate = QLocale::system().toString(now.date(), QLocale::ShortFormat);
        const QString longDate  = QLocale::system().toString(now.date(), QLocale::LongFormat);
        const QString timeStr   = QLocale::system().toString(now.time(), QLocale::ShortFormat);

        int pageNum = 0;
        do {
            ++pageNum;
            QString pn = QString::number(pageNum);

            // Header
            if (hasHeader) {
                QRectF hdrRect(pageRect.left(), pageRect.top(),
                               pageRect.width(), hdrH);
                auto expL = QString::fromStdWString(nppGUI._printSettings._headerLeft);
                auto expM = QString::fromStdWString(nppGUI._printSettings._headerMiddle);
                auto expR = QString::fromStdWString(nppGUI._printSettings._headerRight);
                expandPrintVariables(expL, shortDate, longDate, timeStr, pn);
                expandPrintVariables(expM, shortDate, longDate, timeStr, pn);
                expandPrintVariables(expR, shortDate, longDate, timeStr, pn);
                drawPrintBand(&painter, expL, expM, expR, hdrRect, hdrFont, _isRTL, true);
            }

            // Footer
            if (hasFooter) {
                QRectF ftrRect(pageRect.left(),
                               pageRect.bottom() - ftrH,
                               pageRect.width(), ftrH);
                auto expL = QString::fromStdWString(nppGUI._printSettings._footerLeft);
                auto expM = QString::fromStdWString(nppGUI._printSettings._footerMiddle);
                auto expR = QString::fromStdWString(nppGUI._printSettings._footerRight);
                expandPrintVariables(expL, shortDate, longDate, timeStr, pn);
                expandPrintVariables(expM, shortDate, longDate, timeStr, pn);
                expandPrintVariables(expR, shortDate, longDate, timeStr, pn);
                drawPrintBand(&painter, expL, expM, expR, ftrRect, ftrFont, _isRTL, true);
            }

            qreal top    = pageRect.top()    + (hasHeader ? hdrH + 2 : 0);
            qreal bottom = pageRect.bottom() - (hasFooter ? ftrH + 2 : 0);

            painter.save();
            painter.translate(pageRect.left(), top);
            painter.setClipRect(QRectF(0, 0, pageRect.width(), bottom - top));

            view->execute(SCI_FORMATRANGEFULL, TRUE,
                       reinterpret_cast<sptr_t>(&range));
            painter.restore();

            if (!pr->newPage())
                break;

        } while (range.cpMin < range.cpMax);

        view->execute(SCI_FORMATRANGEFULL, FALSE, 0);
    });

    dialog.exec();
}
