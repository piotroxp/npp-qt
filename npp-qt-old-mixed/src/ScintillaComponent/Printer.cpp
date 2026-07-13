// npp-qt: Printer implementation
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/Printer.cpp
// Target: npp-qt/src/ScintillaComponent/Printer.cpp

#include "Printer.h"
#include "ScintillaComponent.h"
#include "Parameters.h"
#include "localization.h"

#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPainter>
#include <QPageSetupDialog>

using namespace std;

void replaceStr(wstring& str, wstring str2BeReplaced, wstring replacement)
{
	size_t pos = str.find(str2BeReplaced);
	if (pos != str.npos)
		str.replace(pos, str2BeReplaced.length(), replacement);
}

void Printer::init(QWidget* parent, ScintillaComponent* pSEView, bool showDialog,
                   size_t startPos, size_t endPos, bool isRTL)
{
	_pSEView = pSEView;
	_startPos = startPos;
	_endPos = endPos;
	_isRTL = isRTL;
	// QPrinter is created on-demand in doPrint()
}

size_t Printer::doPrint()
{
	QWidget* parent = qobject_cast<QWidget*>(_pSEView->parent());

	// Use QPrintDialog for the native print dialog
	QPrintDialog dialog(parent);
	dialog.setOption(QAbstractPrintDialog::PrintToFile, false);

	if (dialog.exec() != QDialog::Accepted)
		return 0;

	QPrinter* printer = dialog.printer();

	// Configure printer based on NppGUI settings
	const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();

	if (nppGUI._printSettings.isUserMargePresent())
	{
		// Convert hundredths of a millimeter to device units
		const QPageLayout layout = printer->pageLayout();
		QMarginsF margins;
		margins.setLeft(nppGUI._printSettings._marge.left);
		margins.setRight(nppGUI._printSettings._marge.right);
		margins.setTop(nppGUI._printSettings._marge.top);
		margins.setBottom(nppGUI._printSettings._marge.bottom);
		printer->setPageMargins(margins);
	}

	return doPrintImpl(*printer);
}

size_t Printer::doPrintImpl(QPrinter& printer)
{
	const NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();

	// Configure print colour mode
	switch (nppGUI._printSettings._printOption)
	{
		case 0: printer.setColorMode(QPrinter::ColorMode::GrayScale); break;
		case 1: printer.setColorMode(QPrinter::ColorMode::Color); break;
		default: break;
	}

	// Determine the print range
	size_t lengthPrinted = 0;
	size_t lengthDoc = _pSEView->getCurrentDocLen();
	size_t lengthDocMax = lengthDoc;

	bool hasSelection = (_startPos != _endPos);
	if (hasSelection)
	{
		if (_startPos > _endPos)
		{
			lengthPrinted = _endPos;
			lengthDoc = _startPos;
		}
		else
		{
			lengthPrinted = _startPos;
			lengthDoc = _endPos;
		}
		if (lengthDoc > lengthDocMax)
			lengthDoc = lengthDocMax;
	}

	// Temporarily hide line numbers if configured
	bool lineNumberMarginWasVisible = _pSEView->hasMarginShown(ScintillaComponent::_SC_MARGE_LINENUMBER);
	if (!nppGUI._printSettings._printLineNumber)
		_pSEView->showMargin(ScintillaComponent::_SC_MARGE_LINENUMBER, false);

	// Prepare header/footer strings
	const wchar_t shortDateVar[] = L"$(SHORT_DATE)";
	const wchar_t longDateVar[] = L"$(LONG_DATE)";
	const wchar_t timeVar[] = L"$(TIME)";

	QDateTime now = QDateTime::currentDateTime();
	QString shortDate = now.toString(Qt::SystemLocaleShortDate);
	QString longDate = now.toString(Qt::SystemLocaleLongDate);
	QString time = now.toString(Qt::SystemLocaleShortDate); // exclude seconds

	QString docName = QString::fromWCharArray(_pSEView->getCurrentBuffer()->getFullPathName());

	int pageNum = 1;

	// Create a QPainter for drawing on the printer
	QPainter painter;
	if (!painter.begin(&printer))
		return 0;

	// Set font for headers/footers
	QFont headerFont = printer.font();
	headerFont.setPointSize(nppGUI._printSettings._headerFontSize
		? nppGUI._printSettings._headerFontSize : 9);
	if (nppGUI._printSettings._headerFontStyle & FONTSTYLE_BOLD)
		headerFont.setBold(true);
	if (nppGUI._printSettings._headerFontStyle & FONTSTYLE_ITALIC)
		headerFont.setItalic(true);

	QFont footerFont = headerFont;
	footerFont.setPointSize(nppGUI._printSettings._footerFontSize
		? nppGUI._printSettings._footerFontSize : 9);
	if (nppGUI._printSettings._footerFontStyle & FONTSTYLE_BOLD)
		footerFont.setBold(true);
	if (nppGUI._printSettings._footerFontStyle & FONTSTYLE_ITALIC)
		footerFont.setItalic(true);

	// Page margins in device pixels
	QMargins pageMargins;
	if (nppGUI._printSettings.isUserMargePresent())
	{
		// Convert 1/100 mm to device pixels
		qreal dpmm = printer.logicalDpiY() / 25.4;
		pageMargins = QMarginsF(
			nppGUI._printSettings._marge.left * dpmm / 100.0,
			nppGUI._printSettings._marge.top * dpmm / 100.0,
			nppGUI._printSettings._marge.right * dpmm / 100.0,
			nppGUI._printSettings._marge.bottom * dpmm / 100.0
		);
	}
	else
	{
		pageMargins = printer.pageMargins();
	}

	// For Qt6 Scintilla print, we use QsciScintilla's print() method
	// which internally uses SCI_FORMATRANGE. Since ScintillaEditBase may not
	// implement SCI_FORMATRANGE fully yet, we provide a basic implementation.
	//
	// The approach: use QPainter to draw each page's content via Scintilla.
	// Full SCI_FORMATRANGE support is pending in ScintillaEditBase.

	QRectF pageRect = printer.pageRect();

	while (lengthPrinted < lengthDoc)
	{
		// Draw header
		if (nppGUI._printSettings.isHeaderPresent())
		{
			painter.save();
			painter.setFont(headerFont);
			painter.setPen(Qt::black);

			qreal headerHeight = painter.fontMetrics().height() * 1.5;

			// Left header
			QString headerL = QString::fromStdWString(nppGUI._printSettings._headerLeft);
			headerL.replace(QString::fromWCharArray(shortDateVar), shortDate);
			headerL.replace(QString::fromWCharArray(longDateVar), longDate);
			headerL.replace(QString::fromWCharArray(timeVar), time);
			headerL.replace("$(CURRENT_PRINTING_PAGE)", QString::number(pageNum));

			if (!headerL.isEmpty())
				painter.drawText(pageRect.left() + 5,
				                 pageRect.top() + painter.fontMetrics().ascent(),
				                 headerL);

			// Right header (page number)
			painter.drawText(pageRect.right() - painter.fontMetrics().horizontalAdvance(QString::number(pageNum)) - 5,
			                 pageRect.top() + painter.fontMetrics().ascent(),
			                 QString::number(pageNum));

			// Header separator line
			painter.setPen(QPen(Qt::black, 0.5));
			painter.drawLine(pageRect.left(),
			                 pageRect.top() + headerHeight,
			                 pageRect.right(),
			                 pageRect.top() + headerHeight);

			// Move content area down
			pageRect.setTop(pageRect.top() + headerHeight + 5);

			painter.restore();
		}

		// Draw footer
		if (nppGUI._printSettings.isFooterPresent())
		{
			painter.save();
			painter.setFont(footerFont);
			painter.setPen(Qt::black);

			qreal footerHeight = painter.fontMetrics().height() * 1.5;

			// Move content area up
			pageRect.setBottom(pageRect.bottom() - footerHeight - 5);

			// Footer separator line
			painter.setPen(QPen(Qt::black, 0.5));
			painter.drawLine(pageRect.left(),
			                 pageRect.bottom() + 5,
			                 pageRect.right(),
			                 pageRect.bottom() + 5);

			// Footer text
			QString footerM = QString::fromStdWString(nppGUI._printSettings._footerMiddle);
			footerM.replace(QString::fromWCharArray(shortDateVar), shortDate);
			footerM.replace(QString::fromWCharArray(longDateVar), longDate);
			footerM.replace(QString::fromWCharArray(timeVar), time);
			footerM.replace("$(CURRENT_PRINTING_PAGE)", QString::number(pageNum));

			if (!footerM.isEmpty())
			{
				int fw = painter.fontMetrics().horizontalAdvance(footerM);
				painter.drawText(pageRect.left() + (pageRect.width() - fw) / 2,
				                 pageRect.bottom() + footerHeight,
				                 footerM);
			}

			painter.restore();
		}

		// Print content area via Scintilla (stub: full SCI_FORMATRANGE pending)
		// For now, use QsciScintilla::print() if available, or render text directly
		{
			// Configure Scintilla print range
			Sci_CharacterRangeFull range;
			range.cpMin = static_cast<Sci_Position>(lengthPrinted);
			range.cpMax = static_cast<Sci_Position>(lengthDoc);

			_pSEView->send(SCI_FORMATRANGEFULL, TRUE, reinterpret_cast<sptr_t>(&range));

			lengthPrinted = static_cast<size_t>(_pSEView->send(SCI_FORMATRANGEFULL, FALSE, 0));
		}

		++pageNum;

		if (printer.fromPage() > 0 && pageNum > printer.toPage())
			break;

		if (!printer.newPage())
			break;
	}

	painter.end();

	// Restore line number margin visibility
	if (!nppGUI._printSettings._printLineNumber)
		_pSEView->showMargin(ScintillaComponent::_SC_MARGE_LINENUMBER, lineNumberMarginWasVisible);

	return static_cast<size_t>(pageNum) - 1;
}
