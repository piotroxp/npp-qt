// npp-qt: Printer header
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/Printer.h
// Target: npp-qt/src/ScintillaComponent/Printer.h

#pragma once

#include <QPrinter>

class ScintillaComponent;

class Printer
{
public:
	Printer() = default;

	// Semantic lift: init() → Qt6 printer setup
	// Win32: PRINTDLG, hwndOwner → QPrinter parent widget
	// Qt6: QPrinter setup without dialog initially
	void init(QWidget* parent, ScintillaComponent* pSEView, bool showDialog,
	          size_t startPos, size_t endPos, bool isRTL = false);

	// Semantic lift: doPrint() → QPrintDialog + print
	size_t doPrint();

private:
	// Internal print implementation
	size_t doPrintImpl(QPrinter& printer);

	ScintillaComponent* _pSEView = nullptr;
	size_t _startPos = 0;
	size_t _endPos = 0;
	bool _isRTL = false;
};
