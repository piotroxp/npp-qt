// npp-qt: DocTabView implementation
// Semantic Lift: Win32 → Qt6
// Source: PowerEditor/src/ScintillaComponent/DocTabView.cpp
// Target: npp-qt/src/ScintillaComponent/DocTabView.cpp

#include "DocTabView.h"
#include "Buffer.h"
#include "Parameters.h"
#include "ScintillaComponent.h"

#include <QTabBar>
#include <QResizeEvent>
#include <QIcon>
#include <QPixmap>

// Indexes for tab icons (mirrors Win32 ImgIdx enum)
enum ImgIdx {
	SAVED_IMG_INDEX,
	UNSAVED_IMG_INDEX,
	REDONLY_IMG_INDEX,
	REDONLYSYS_IMG_INDEX,
	MONITORING_IMG_INDEX,
};

DocTabView::DocTabView(QWidget* parent)
	: QTabWidget(parent), _pView(nullptr), _iconListIndexChoice(-1)
{
	setTabsClosable(true);
	setMovable(true);

	// Connect signals
	connect(this, &QTabWidget::currentChanged, this, &DocTabView::tabChanged);
	connect(this, &QTabWidget::tabCloseRequested, this, &DocTabView::tabCloseRequested);
}

void DocTabView::init(ScintillaComponent* pView, unsigned char indexChoice, unsigned char buttonsStatus)
{
	_pView = pView;

	createIconSets();

	// Default icons set
	if (indexChoice >= static_cast<unsigned char>(_iconSets.size()))
		_iconListIndexChoice = 0;
	else
		_iconListIndexChoice = indexChoice;
}

void DocTabView::createIconSets()
{
	// Create icon sets from resources.
	// Win32 icons are loaded via ImageList in original; in Qt6 we use QIcon.
	// The actual icon data comes from resource.h / icon files.
	// For now, create empty sets — icons are loaded from QRC resources.

	_iconSets.clear();

	// 3 icon sets: standard, alt, dark mode
	for (int set = 0; set < 3; ++set)
	{
		QVector<QIcon> icons(5); // 5 icon types
		_iconSets.append(icons);
	}
}

void DocTabView::changeIconSet(unsigned char choice)
{
	if (choice >= static_cast<unsigned char>(_iconSets.size()))
		return;
	_iconListIndexChoice = choice;
}

void DocTabView::addBuffer(BufferID buffer)
{
	if (buffer == BUFFER_INVALID)
		return;
	if (getIndexByBuffer(buffer) != -1)
		return;

	const Buffer* buf = MainFileManager.getBufferByID(buffer);

	// Create tab with buffer ID as data
	int tabIndex = addTab(QString::fromWCharArray(buf->getCompactFileName()));
	setTabData(tabIndex, QVariant::fromValue<void*>(buffer));

	// Ensure the tab bar has enough room
	tabBar()->setExpanding(true);

	updateTabIcon(tabIndex, buf);

	// Update scintilla view size
	if (_pView)
		_pView->resize(size());

	emit tabChanged(tabIndex);
}

void DocTabView::closeBuffer(BufferID buffer)
{
	int indexToClose = getIndexByBuffer(buffer);
	if (indexToClose < 0)
		return;

	removeTab(indexToClose);
	_buffers.erase(_buffers.begin() + indexToClose);

	if (_pView)
		_pView->resize(size());
}

void DocTabView::setIndividualTabColour(BufferID bufferId, int colorId)
{
	bufferId->setDocColorId(colorId);
}

int DocTabView::getIndividualTabColourId(int tabIndex)
{
	BufferID bufferId = getBufferByIndex(tabIndex);
	if (!bufferId)
		return -1;
	return bufferId->getDocColorId();
}

bool DocTabView::activateBuffer(BufferID buffer)
{
	int indexToActivate = getIndexByBuffer(buffer);
	if (indexToActivate == -1)
		return false;

	setCurrentIndex(indexToActivate);
	return true;
}

BufferID DocTabView::activeBuffer()
{
	int index = currentIndex();
	return getBufferByIndex(index);
}

BufferID DocTabView::findBufferByName(const wchar_t* fullfilename)
{
	for (size_t i = 0; i < _buffers.size(); ++i)
	{
		const Buffer* buf = MainFileManager.getBufferByID(_buffers[i]);
		if (buf && QString::fromWCharArray(fullfilename).compare(
			QString::fromWCharArray(buf->getFullPathName()), Qt::CaseInsensitive) == 0)
		{
			return _buffers[i];
		}
	}
	return BUFFER_INVALID;
}

int DocTabView::getIndexByBuffer(BufferID id)
{
	for (size_t i = 0; i < _buffers.size(); ++i)
	{
		if (_buffers[i] == id)
			return static_cast<int>(i);
	}
	return -1;
}

BufferID DocTabView::getBufferByIndex(size_t index)
{
	if (index >= static_cast<size_t>(count()))
		return BUFFER_INVALID;

	QVariant data = tabData(static_cast<int>(index));
	if (data.isNull())
		return BUFFER_INVALID;

	return reinterpret_cast<BufferID>(data.value<void*>());
}

int DocTabView::findTabByBuffer(BufferID id) const
{
	for (int i = 0; i < count(); ++i)
	{
		QVariant data = tabData(i);
		if (!data.isNull() && reinterpret_cast<BufferID>(data.value<void*>()) == id)
			return i;
	}
	return -1;
}

void DocTabView::updateTabIcon(int tabIndex, const Buffer* buf)
{
	if (!buf || tabIndex < 0 || tabIndex >= count())
		return;

	// Determine icon index based on buffer state
	int iconIdx = SAVED_IMG_INDEX;
	if (buf->isDirty())
		iconIdx = UNSAVED_IMG_INDEX;
	else if (buf->isMonitoringOn())
		iconIdx = MONITORING_IMG_INDEX;
	else if (buf->getFileReadOnly())
		iconIdx = REDONLYSYS_IMG_INDEX;
	else if (buf->getUserReadOnly())
		iconIdx = REDONLY_IMG_INDEX;

	// Apply icon from the current icon set
	if (_iconListIndexChoice >= 0 && _iconListIndexChoice < _iconSets.size())
	{
		const QVector<QIcon>& set = _iconSets[_iconListIndexChoice];
		if (iconIdx < set.size() && !set[iconIdx].isNull())
			tabBar()->setTabIcon(tabIndex, set[iconIdx]);
	}
}

void DocTabView::bufferUpdated(const Buffer* buffer, int mask)
{
	int index = findTabByBuffer(buffer->getID());
	if (index == -1)
		return;

	if (mask & BufferChangeReadonly || mask & BufferChangeDirty)
	{
		updateTabIcon(index, buffer);
	}

	if (mask & BufferChangeFilename)
	{
		setTabText(index, QString::fromWCharArray(buffer->getCompactFileName()));
	}

	if (mask & BufferChangeRecentTag)
	{
		// Tab order may have changed — nothing to do in Qt6
	}
}

void DocTabView::setBuffer(size_t index, BufferID id)
{
	if (index >= static_cast<size_t>(count()))
		return;

	_buffers[index] = id;
	setTabData(static_cast<int>(index), QVariant::fromValue<void*>(id));
	bufferUpdated(MainFileManager.getBufferByID(id), BufferChangeMask);
}

void DocTabView::resizeEvent(QResizeEvent* event)
{
	QTabWidget::resizeEvent(event);

	NppParameters& nppParam = NppParameters::getInstance();
	const NppGUI& nppGUI = nppParam.getNppGUI();

	if (nppGUI._tabStatus & TAB_HIDE)
	{
		// Hide tab bar, editor takes full space
		tabBar()->hide();
		if (_pView)
			_pView->resize(size());
	}
	else
	{
		tabBar()->show();
		int borderWidth = nppParam.getSVP()._borderWidth;
		if (_pView)
		{
			QRect editorRect = rect();
			editorRect.adjust(borderWidth, borderWidth,
			                  -borderWidth * 2, -borderWidth * 2);
			_pView->setGeometry(editorRect);
		}
	}
}

void DocTabView::resizeIconsDpi()
{
	createIconSets();

	if (_iconListIndexChoice < 0 || _iconListIndexChoice >= _iconSets.size())
		_iconListIndexChoice = 0;

	// Re-apply icons to all tabs
	for (int i = 0; i < count(); ++i)
	{
		BufferID buf = getBufferByIndex(i);
		if (buf)
		{
			const Buffer* buffer = MainFileManager.getBufferByID(buf);
			if (buffer)
				updateTabIcon(i, buffer);
		}
	}
}
