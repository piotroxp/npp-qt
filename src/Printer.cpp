// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
#include "Printer.h"
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QFileInfo>
#include <QDateTime>
#include "Parameters.h"

void Printer::init(QWidget* parent, ScintillaComponent* pSEView,
                  bool showDialog, size_t startPos, size_t endPos, bool isRTL)
{
    _pSEView = pSEView;
    _startPos = startPos;
    _endPos = endPos;
    _isRTL = isRTL;
    if (!showDialog) {
        // Use default printer for silent print
        _printer.setPrinterName(QString());
    }
}

size_t Printer::doPrint(bool justDoIt)
{
    QWidget* parent = _pSEView ? _pSEView->window() : nullptr;
    QPrintDialog dialog(&_printer, parent);

    if (justDoIt) {
        // Silent print — no dialog
    } else {
        if (dialog.exec() != QDialog::Accepted)
            return 0;
    }

    // Paint the editor content to the printer
    QPainter painter;
    if (!painter.begin(&_printer))
        return 0;

    // Configure page margins from settings
    const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
    if (nppGUI._printSettings.isUserMargePresent()) {
        QPageLayout layout = _printer.pageLayout();
        QMarginsF margins(
            nppGUI._printSettings._marge.left() * 100.0 / 2540.0,
            nppGUI._printSettings._marge.top() * 100.0 / 2540.0,
            nppGUI._printSettings._marge.right() * 100.0 / 2540.0,
            nppGUI._printSettings._marge.bottom() * 100.0 / 2540.0);
        layout.setMargins(margins);
        _printer.setPageLayout(layout);
    }

    // Header
    if (nppGUI._printSettings.isHeaderPresent()) {
        QString headerLeft = QString::fromWCharArray(nppGUI._printSettings._headerLeft.c_str());
        QString headerMiddle = QString::fromWCharArray(nppGUI._printSettings._headerMiddle.c_str());
        QString headerRight = QString::fromWCharArray(nppGUI._printSettings._headerRight.c_str());
        headerLeft.replace("$(SHORT_DATE)", QDate::currentDate().toString(Qt::ISODate));
        headerRight.replace("$(CURRENT_PRINTING_PAGE)", "1");
        Q_UNUSED(headerMiddle);
        Q_UNUSED(headerLeft);
    }

    // Footer
    if (nppGUI._printSettings.isFooterPresent()) {
        QString footerLeft = QString::fromWCharArray(nppGUI._printSettings._footerLeft.c_str());
        footerLeft.replace("$(CURRENT_PRINTING_PAGE)", "1");
        Q_UNUSED(footerLeft);
    }

    // Use Scintilla's print rendering via QPaintEngine or direct pixmap
    // For a proper implementation, use Scintilla's SCI_FORMATRANGE with a metafile DC
    // Here we approximate by painting the visible content
    QRect printRect = _printer.pageLayout().paintRectPixels(_printer.resolution());
    if (_pSEView) {
        // Render editor content as pixmap and draw scaled to page
        QPixmap pix = _pSEView->grab(_pSEView->rect());
        // Scale to fit page width
        double scale = double(printRect.width()) / pix.width();
        QPixmap scaled = pix.scaled(
            printRect.width(), int(pix.height() * scale),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(printRect.topLeft(), scaled);
    }

    painter.end();
    return _printer.fromPage() > 0 ? (_printer.toPage() - _printer.fromPage() + 1) : 1;
}
