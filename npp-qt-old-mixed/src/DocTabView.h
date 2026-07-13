// npp-qt: DocTabView header
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/DocTabView.h
// Target: npp-qt/src/ScintillaComponent/DocTabView.h

#pragma once

#include <QTabWidget>
#include <QVector>
#include <vector>

#include "Buffer.h"
#include "NppConstants.h"
#include "ScintillaComponent.h"

// DocTabView — Qt6 document tab widget
// Win32 TCM_* (Tab Control) → Qt QTabWidget / QTabBar
// IconList → QIcon / QTabBar icons
class DocTabView : public QTabWidget
{
	Q_OBJECT

public:
	DocTabView(QWidget* parent = nullptr);
	~DocTabView() override;

	// Semantic lift: init() → constructor + setup
	void init(ScintillaComponent* pView, unsigned char indexChoice, unsigned char buttonsStatus);

	// Icon set management
	void createIconSets();
	void changeIconSet(unsigned char choice);

	// Buffer management (mirrors Win32 TCM_* messages)
	void addBuffer(BufferID buffer);
	void closeBuffer(BufferID buffer);
	void bufferUpdated(const Buffer* buffer, int mask);
	bool activateBuffer(BufferID buffer);

	BufferID activeBuffer();
	BufferID findBufferByName(const wchar_t* fullfilename);
	int getIndexByBuffer(BufferID id);
	BufferID getBufferByIndex(size_t index);
	void setBuffer(size_t index, BufferID id);

	// DPI / icon resize
	void resizeIconsDpi();

	const ScintillaComponent* getScintillaEditView() const { return _pView; }

	// nbItem() — mirrors Win32 DocTabView via TabBar (Qt6: delegates to QTabWidget::count())
	size_t nbItem() const { return static_cast<size_t>(count()); }

	static void setIndividualTabColour(BufferID bufferId, int colorId);
	int getIndividualTabColourId(int tabIndex) override;

signals:
	void tabChanged(int index);
	void tabCloseRequested(int index);
	void tabOrderChanged();  // emitted when tabs are drag-reordered — used by Notepad_plus for NPPN_DOCORDERCHANGED

protected:
	// DPI-aware resize
	void resizeEvent(QResizeEvent* event) override;

private:
	ScintillaComponent* _pView = nullptr;
	std::vector<BufferID> _buffers;
	int _iconListIndexChoice = -1;

	// Icon lists (mapped from Win32 ImageList)
	// In Qt6 we use QTabBar icons instead
	// Store as QVector<QIcon> for each icon set variant
	QVector<QVector<QIcon>> _iconSets;

	// Map buffer ID → tab index
	int findTabByBuffer(BufferID id) const;

	// Update tab icon based on buffer state
	void updateTabIcon(int tabIndex, const Buffer* buf);

private slots:
	// Handles QTabBar::tabBarMoved — emits tabOrderChanged for NPPN_DOCORDERCHANGED
	void tabMoved(int from, int to);
};
