//
//          Copyright (c) 1990-2011, Scientific Toolworks, Inc.
//
// The License.txt file describes the conditions under which this software may be distributed.
//
// Author: Jason Haslam
//
// Additions Copyright (c) 2011 Archaeopteryx Software, Inc. d/b/a Wingware
// @file ScintillaQt.h - Qt specific subclass of ScintillaBase

#ifndef SCINTILLAQT_H
#define SCINTILLAQT_H

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <algorithm>
#include <memory>

#include "ScintillaComponents/scintilla/include/ScintillaTypes.h"
#include "ScintillaComponents/scintilla/include/ScintillaMessages.h"
#include "ScintillaComponents/scintilla/include/ScintillaStructures.h"
#include "ScintillaComponents/scintilla/include/Scintilla.h"
#include "ScintillaComponents/scintilla/src/Debugging.h"
#include "ScintillaComponents/scintilla/src/Geometry.h"
#include "ScintillaComponents/scintilla/src/Platform.h"
#include "ScintillaComponents/scintilla/include/ILoader.h"
#include "ScintillaComponents/scintilla/include/ILexer.h"
#include "ScintillaComponents/scintilla/src/CharacterCategoryMap.h"
#include "ScintillaComponents/scintilla/src/Position.h"
#include "ScintillaComponents/scintilla/src/UniqueString.h"
#include "ScintillaComponents/scintilla/src/SplitVector.h"
#include "ScintillaComponents/scintilla/src/Partitioning.h"
#include "ScintillaComponents/scintilla/src/RunStyles.h"
#include "ScintillaComponents/scintilla/src/ContractionState.h"
#include "ScintillaComponents/scintilla/src/CellBuffer.h"
#include "ScintillaComponents/scintilla/src/CallTip.h"
#include "ScintillaComponents/scintilla/src/KeyMap.h"
#include "ScintillaComponents/scintilla/src/Indicator.h"
#include "ScintillaComponents/scintilla/src/LineMarker.h"
#include "ScintillaComponents/scintilla/src/Style.h"
#include "ScintillaComponents/scintilla/src/AutoComplete.h"
#include "ScintillaComponents/scintilla/src/ViewStyle.h"
#include "ScintillaComponents/scintilla/src/CharClassify.h"
#include "ScintillaComponents/scintilla/src/Decoration.h"
#include "ScintillaComponents/scintilla/src/CaseFolder.h"
#include "ScintillaComponents/scintilla/src/Document.h"
#include "ScintillaComponents/scintilla/src/Selection.h"
#include "ScintillaComponents/scintilla/src/PositionCache.h"
#include "ScintillaComponents/scintilla/src/EditModel.h"
#include "ScintillaComponents/scintilla/src/MarginView.h"
#include "ScintillaComponents/scintilla/src/EditView.h"
#include "ScintillaComponents/scintilla/src/Editor.h"
#include "ScintillaComponents/scintilla/src/ScintillaBase.h"
#include "ScintillaComponents/scintilla/src/CaseConvert.h"

#include <QObject>
#include <QAbstractScrollArea>
#include <QAction>
#include <QClipboard>
#include <QPaintEvent>

class ScintillaEditBase;

namespace Scintilla::Internal {

class ScintillaQt : public QObject, public ScintillaBase {
	Q_OBJECT

public:
	explicit ScintillaQt(QAbstractScrollArea *parent);
	virtual ~ScintillaQt();

signals:
	void horizontalScrolled(int value);
	void verticalScrolled(int value);
	void horizontalRangeChanged(int max, int page);
	void verticalRangeChanged(int max, int page);

	void notifyParent(Scintilla::NotificationData scn);
	void notifyChange();

	// Clients can use this hook to add additional
	// formats (e.g. rich text) to the MIME data.
	void aboutToCopy(QMimeData *data);

	void command(Scintilla::uptr_t wParam, Scintilla::sptr_t lParam);

private slots:
	void onIdle();
	void execCommand(QAction *action);
	void SelectionChanged();

private:
	void Init();
	void Finalise() override;
	bool DragThreshold(Point ptStart, Point ptNow) override;
	bool ValidCodePage(int codePage) const override;
	std::string UTF8FromEncoded(std::string_view encoded) const override;
	std::string EncodedFromUTF8(std::string_view utf8) const override;

private:
	void ScrollText(Sci::Line linesToMove) override;
	void SetVerticalScrollPos() override;
	void SetHorizontalScrollPos() override;
	bool ModifyScrollBars(Sci::Line nMax, Sci::Line nPage) override;
	void ReconfigureScrollBars() override;
	void CopyToModeClipboard(const SelectionText &selectedText, QClipboard::Mode clipboardMode_);
	void Copy() override;
	void CopyToClipboard(const SelectionText &selectedText) override;
	void PasteFromMode(QClipboard::Mode clipboardMode_);
	void Paste() override;
	void ClaimSelection() override;
	void NotifyChange() override;
	void NotifyFocus(bool focus) override;
	void NotifyParent(Scintilla::NotificationData scn) override;
	void NotifyURIDropped(const char *uri);
	int timers[static_cast<size_t>(TickReason::dwell)+1]{};
	bool FineTickerRunning(TickReason reason) override;
	void FineTickerStart(TickReason reason, int millis, int tolerance) override;
	void CancelTimers();
	void FineTickerCancel(TickReason reason) override;
	bool ChangeIdle(bool on);
	bool SetIdle(bool on) override;
	void SetMouseCapture(bool on) override;
	bool HaveMouseCapture() override;
	void StartDrag() override;
	Scintilla::CharacterSet CharacterSetOfDocument() const;
	const char *CharacterSetIDOfDocument() const;
	QString StringFromDocument(const char *s) const;
	QByteArray BytesForDocument(const QString &text) const;
	std::unique_ptr<CaseFolder> CaseFolderForEncoding() override;
	std::string CaseMapString(const std::string &s, CaseMapping caseMapping) override;

	void CreateCallTipWindow(PRectangle rc) override;
	void AddToPopUp(const char *label, int cmd, bool enabled) override;
	sptr_t WndProc(Scintilla::Message iMessage, uptr_t wParam, sptr_t lParam) override;
	sptr_t DefWndProc(Scintilla::Message iMessage, uptr_t wParam, sptr_t lParam) override;

	static sptr_t DirectFunction(sptr_t ptr,
				     unsigned int iMessage, uptr_t wParam, sptr_t lParam);
	static sptr_t DirectStatusFunction(sptr_t ptr,
				     unsigned int iMessage, uptr_t wParam, sptr_t lParam, int *pStatus);

protected:

	void PartialPaint(const PRectangle &rect);

	void DragEnter(const Point &point);
	void DragMove(const Point &point);
	void DragLeave();
	void Drop(const Point &point, const QMimeData *data, bool move);
	void DropUrls(const QMimeData *data);

	void timerEvent(QTimerEvent *event) override;

private:
	QAbstractScrollArea *scrollArea;

	int vMax, hMax;   // Scroll bar maximums.
	int vPage, hPage; // Scroll bar page sizes.

	bool haveMouseCapture;
	bool dragWasDropped;
	int rectangularSelectionModifier;

	friend class ::ScintillaEditBase;
};

}

#endif /* SCINTILLAQT_H */
