// Semantic Lift: Win32 DockingWnd → Qt6 QDockingSystem Implementation
// Original: PowerEditor/src/WinControls/DockingWnd/
// Target: npp-qt/src/WinControls/DockingWnd.cpp

#include "DockingWnd.h"
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QScreen>
#include <QStackedLayout>

using namespace std;

// =============================================================================
// Helpers
// =============================================================================

static int dpiScale(int base) {
    return static_cast<int>(base * (qApp ? qApp->devicePixelRatio() : 1.0));
}

// =============================================================================
// DockingDlgInterface — base implementation
// Mirrors Win32 DockingDlgInterface.
// =============================================================================

DockingDlgInterface::DockingDlgInterface(int panelID)
    : QWidget(nullptr)
    , _panelID(panelID)
{
    // Window flags for docking panel
    setWindowFlags(Qt::Widget);
}

void DockingDlgInterface::init(QApplication* app, QWidget* parent, int panelID)
{
    Q_UNUSED(app);
    _hParent = parent;
    _panelID = panelID;
    if (parent) {
        setParent(parent);
    }
}

void DockingDlgInterface::display(bool toShow)
{
    if (toShow)
    {
        show();
        raise();
    }
    else
    {
        hide();
    }
    _isVisible = toShow;
}

void DockingDlgInterface::setBackgroundColor(const QColor& col)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, col);
    pal.setColor(QPalette::Base, col);
    setPalette(pal);
    update();
}

void DockingDlgInterface::setForegroundColor(const QColor& col)
{
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, col);
    pal.setColor(QPalette::Text, col);
    setPalette(pal);
    update();
}

// =============================================================================
// DockingContainer
// =============================================================================

DockingContainer::DockingContainer(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(100, 80);

    // Caption bar
    _captionBar = new QWidget(this);
    _captionBar->setFixedHeight(dpiScale(HIGH_CAPTION_DPI));
    _captionBar->installEventFilter(this);

    _captionLabel = new QLabel(_captionBar);
    _captionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _captionLabel->setStyleSheet("background: transparent; padding-left: 4px;");

    // Tab widget for docked panels
    _tabWidget = new QTabWidget(this);
    _tabWidget->setDocumentMode(true);
    _tabWidget->setTabsClosable(true);
    _tabWidget->setMovable(true);
    connect(_tabWidget, &QTabWidget::currentChanged, this, &DockingContainer::onTabChanged);
    connect(_tabWidget, &QTabWidget::tabCloseRequested, this, &DockingContainer::onTabCloseRequested);

    // Stacked widget for actual content
    _clientStack = new QStackedWidget(this);

    // Layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(_captionBar);
    mainLayout->addWidget(_tabWidget);
    setLayout(mainLayout);

    // Default font
    _captionFont = QApplication::font();
    _tabFont = QApplication::font();

    // Start hidden until widgets are added
    hide();
}

DockingContainer::~DockingContainer()
{
    for (auto* data : _widgetData) {
        if (data->iconTab.cacheKey() != 0) {
            // icon was loaded from resource, no explicit cleanup needed in Qt
        }
        delete data;
    }
    _widgetData.clear();
}

int DockingContainer::captionHeight() const {
    return dpiScale(HIGH_CAPTION_DPI);
}

int DockingContainer::captionGap() const {
    return dpiScale(CAPTION_GAP_DPI);
}

int DockingContainer::closeButtonSize() const {
    return dpiScale(CLOSEBTN_SIZE_DPI);
}

QRect DockingContainer::captionRect() const {
    if (_isTopCaption) {
        return QRect(0, 0, width(), captionHeight());
    } else {
        return QRect(0, 0, captionHeight(), height());
    }
}

QRect DockingContainer::closeButtonRect() const {
    int btnSize = closeButtonSize();
    int gap = dpiScale(CLOSEBTN_POS_DPI);
    if (_isTopCaption) {
        return QRect(width() - btnSize - gap, gap, btnSize, btnSize);
    } else {
        return QRect(gap, height() - btnSize - gap, btnSize, btnSize);
    }
}

DockingContainer::MouseRegion DockingContainer::hitTest(const QPoint& pos) const {
    QRect capRect = captionRect();
    QRect closeRect = closeButtonRect();

    if (!capRect.contains(pos)) return MouseRegion::Outside;
    if (closeRect.contains(pos)) return MouseRegion::CloseButton;
    return MouseRegion::Caption;
}

void DockingContainer::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);

    // Draw background
    QPalette pal = palette();
    painter.fillRect(rect(), (_isActive ? pal.color(QPalette::Active, QPalette::Window)
                                         : pal.color(QPalette::Inactive, QPalette::Window)));

    drawCaption(painter);
}

void DockingContainer::drawCaption(QPainter& painter) {
    QRect capRect = captionRect();

    // Caption background
    QColor bgColor;
    if (_isActive) {
        bgColor = palette().color(QPalette::Active, QPalette::Highlight);
    } else {
        bgColor = palette().color(QPalette::Inactive, QPalette::Window);
    }

    painter.fillRect(capRect, bgColor);

    // Caption text
    QFontMetrics fm(_captionFont);
    QString elidedText = fm.elidedText(_captionText, Qt::ElideRight, capRect.width() - closeButtonSize() - dpiScale(8));
    painter.setFont(_captionFont);
    painter.setPen(palette().color(_isActive ? QPalette::Normal : QPalette::Inactive, QPalette::WindowText));
    painter.drawText(capRect.adjusted(dpiScale(4), 0, -closeButtonSize() - dpiScale(4), 0),
                     Qt::AlignLeft | Qt::AlignVCenter, elidedText);

    // Close button
    QRect closeRect = closeButtonRect();
    QColor closeBg = (_isMouseOverClose && _isMouseDown) ? Qt::darkRed : (closeRect.contains(mapFromGlobal(QCursor::pos())) ? Qt::red : Qt::transparent);
    painter.fillRect(closeRect, closeBg);
    painter.setPen(Qt::white);
    painter.drawText(closeRect, Qt::AlignCenter, QStringLiteral("×"));
}

void DockingContainer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        _isMouseDown = true;
        _hoverRegion = hitTest(event->pos());
        if (_hoverRegion == MouseRegion::CloseButton) {
            _isMouseOverClose = true;
            update();
        } else if (_hoverRegion == MouseRegion::Caption) {
            // Could initiate drag here
        }
    }
}

void DockingContainer::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (_isMouseOverClose && _isMouseDown) {
            closeAllTabs();
        }
        _isMouseDown = false;
        _isMouseOverClose = false;
        _hoverRegion = MouseRegion::Outside;
        update();
    }
}

void DockingContainer::mouseMoveEvent(QMouseEvent* event) {
    MouseRegion region = hitTest(event->pos());
    if (_isMouseDown && region == MouseRegion::Caption) {
        // Could emit move signal here for drag-and-drop
    }
    if (region != _hoverRegion) {
        _hoverRegion = region;
        update();
    }
}

void DockingContainer::mouseDoubleClickEvent(QMouseEvent* event) {
    if (hitTest(event->pos()) == MouseRegion::Caption) {
        emit floatAllRequested();
    }
}

void DockingContainer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    relayout();
}

bool DockingContainer::event(QEvent* event) {
    if (event->type() == QEvent::Enter) {
        setHoverRegion(hitTest(mapFromGlobal(QCursor::pos())));
    } else if (event->type() == QEvent::Leave) {
        _hoverRegion = MouseRegion::Outside;
        _isMouseOverClose = false;
        update();
    }
    return QWidget::event(event);
}

void DockingContainer::setHoverRegion(MouseRegion region) {
    if (region != _hoverRegion) {
        _hoverRegion = region;
        update();
    }
}

void DockingContainer::relayout() {
    if (!_tabWidget) return;

    int capH = captionHeight();
    int gap = captionGap();

    if (_isTopCaption) {
        _captionBar->setGeometry(0, 0, width(), capH);
        if (_tabWidget->count() > 1) {
            _tabWidget->setGeometry(0, capH + gap, width(), height() - capH - gap);
        } else {
            _tabWidget->setGeometry(0, capH, width(), height() - capH);
        }
    } else {
        _captionBar->setGeometry(0, 0, capH, height());
        if (_tabWidget->count() > 1) {
            _tabWidget->setGeometry(capH + gap, 0, width() - capH - gap, height());
        } else {
            _tabWidget->setGeometry(capH, 0, width() - capH, height());
        }
    }
}

DockedWidgetData* DockingContainer::addDockedWidget(const DockedWidgetData& data) {
    auto* pData = new DockedWidgetData(data);

    // Force child style
    pData->client->setParent(_clientStack);

    // Add to stacked widget
    int index = _clientStack->addWidget(pData->client);

    // Add tab
    QString tabText = QString::fromStdWString(pData->pszName);
    if (!pData->addInfo.isEmpty()) {
        tabText += QStringLiteral(" - ") + pData->addInfo;
    }

    _tabWidget->addTab(pData->client, tabText);
    _tabWidget->setCurrentIndex(index);

    _widgetData.append(pData);

    updateCaption();

    if (!isVisible()) {
        show();
        emit closeRequested(); // trigger parent resize
    }

    relayout();
    return pData;
}

void DockingContainer::removeDockedWidget(QWidget* client) {
    for (int i = 0; i < _widgetData.size(); ++i) {
        if (_widgetData[i]->client == client) {
            int tabIdx = tabIndexOf(client);
            if (tabIdx >= 0) {
                _tabWidget->removeTab(tabIdx);
            }
            _clientStack->removeWidget(client);
            delete _widgetData[i];
            _widgetData.remove(i);
            updateCaption();
            if (_widgetData.isEmpty()) {
                hide();
            }
            break;
        }
    }
}

DockedWidgetData* DockingContainer::findDockedWidgetByWnd(QWidget* client) {
    for (auto* data : _widgetData) {
        if (data->client == client) return data;
    }
    return nullptr;
}

DockedWidgetData* DockingContainer::findDockedWidgetByName(const QString& name) {
    for (auto* data : _widgetData) {
        if (QString::fromStdWString(data->pszName) == name) return data;
    }
    return nullptr;
}

void DockingContainer::showDockedWidget(DockedWidgetData* data, bool show) {
    if (show) {
        viewDockedWidget(data);
    } else {
        hideDockedWidget(data);
    }
}

int DockingContainer::hideDockedWidget(DockedWidgetData* data, bool hideClient) {
    int tabIdx = tabIndexOf(data->client);
    if (tabIdx < 0) return -1;

    bool hadFocus = data->client->hasFocus();

    _tabWidget->removeTab(tabIdx);
    if (hideClient) {
        data->client->hide();
    }

    if (_tabWidget->count() == 0) {
        hide();
        emit closeRequested();
    }

    if (hadFocus) {
        // Could emit focus restore signal here
        Q_UNUSED(hadFocus);
    }

    relayout();
    return tabIdx;
}

void DockingContainer::viewDockedWidget(DockedWidgetData* data) {
    int idx = _clientStack->indexOf(data->client);
    if (idx >= 0) {
        _tabWidget->setCurrentIndex(idx);
        if (!isVisible()) {
            show();
            emit closeRequested();
        }
        data->client->show();
        data->client->setFocus();
    }
    relayout();
}

int DockingContainer::tabIndexOf(QWidget* client) const {
    int idx = _clientStack->indexOf(client);
    if (idx >= 0) {
        // Find matching tab - tabs and stack may differ due to hidden tabs
        for (int i = 0; i < _tabWidget->count(); ++i) {
            if (_tabWidget->widget(i) == client) return i;
        }
    }
    return -1;
}

void DockingContainer::selectTab(int index) {
    if (index < 0 || index >= _tabWidget->count()) return;

    int stackIdx = _clientStack->indexOf(_tabWidget->widget(index));
    if (stackIdx >= 0) {
        _clientStack->setCurrentIndex(stackIdx);
        _tabWidget->setCurrentIndex(index);
        QWidget* w = _tabWidget->widget(index);
        if (w) w->setFocus();
    }
}

void DockingContainer::setActiveTab(int index) {
    selectTab(index);
}

int DockingContainer::activeTabIndex() const {
    return _tabWidget->currentIndex();
}

DockedWidgetData* DockingContainer::activeWidgetData() const {
    int idx = activeTabIndex();
    if (idx >= 0 && idx < _widgetData.size()) {
        return _widgetData[idx];
    }
    return nullptr;
}

QVector<DockedWidgetData*> DockingContainer::allWidgetData() const {
    return _widgetData;
}

QVector<DockedWidgetData*> DockingContainer::visibleWidgetData() const {
    return _widgetData; // all in this container are visible
}

bool DockingContainer::isWidgetVisible(DockedWidgetData* data) const {
    int idx = tabIndexOf(data->client);
    return idx >= 0 && _tabWidget->isTabVisible(idx);
}

void DockingContainer::setCaptionTop(bool isTop) {
    if (_isTopCaption != isTop) {
        _isTopCaption = isTop;
        relayout();
        update();
    }
}

void DockingContainer::setActive(bool active) {
    if (_isActive != active) {
        _isActive = active;
        update();
    }
}

void DockingContainer::setFloating(bool floating) {
    _isFloating = floating;
    if (floating) {
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        _captionBar->hide();
    } else {
        setWindowFlags({});
        _captionBar->show();
    }
}

void DockingContainer::updateCaption() {
    DockedWidgetData* active = activeWidgetData();
    if (active) {
        _captionText = QString::fromStdWString(active->pszName);
        if (!active->addInfo.isEmpty()) {
            _captionText += QStringLiteral(" - ") + active->addInfo;
        }
        setWindowTitle(_captionText);
    } else {
        _captionText = QString();
        setWindowTitle(QString());
    }
    _captionLabel->setText(_captionText);
    update();
}

void DockingContainer::onTabChanged(int index) {
    if (index >= 0 && index < _widgetData.size()) {
        _clientStack->setCurrentWidget(_widgetData[index]->client);
        updateCaption();
        emit tabChanged(index);
    }
}

void DockingContainer::onTabCloseRequested(int index) {
    if (index >= 0 && index < _widgetData.size()) {
        DockedWidgetData* data = _widgetData[index];
        // Notify parent
        emit widgetHidden(data->client);
        hideDockedWidget(data, true);
    }
}

void DockingContainer::closeAllTabs() {
    while (!_widgetData.isEmpty()) {
        DockedWidgetData* data = _widgetData.last();
        emit widgetHidden(data->client);
        removeDockedWidget(data->client);
    }
}

QSize DockingContainer::minimumSizeHint() const {
    return QSize(dpiScale(100), dpiScale(80));
}

// =============================================================================
// DockingSplitter
// =============================================================================

DockingSplitter::DockingSplitter(Orientation orient, QWidget* parent)
    : QWidget(parent)
    , _orientation(orient)
{
    setMinimumSize(SPLITTER_WIDTH, SPLITTER_WIDTH);
    setMaximumSize(SPLITTER_WIDTH, SPLITTER_WIDTH);
    setCursor(orient == Orientation::Vertical ? Qt::SizeHorCursor : Qt::SizeVerCursor);
}

void DockingSplitter::setRange(int min, int max) {
    _rangeMin = min;
    _rangeMax = max;
}

void DockingSplitter::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        _isDragging = true;
        _dragStartPos = event->globalPos();
        update();
    }
}

void DockingSplitter::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    _isDragging = false;
    update();
}

void DockingSplitter::mouseMoveEvent(QMouseEvent* event) {
    if (_isDragging) {
        int delta = 0;
        if (_orientation == Orientation::Vertical) {
            delta = event->globalPos().x() - _dragStartPos.x();
        } else {
            delta = event->globalPos().y() - _dragStartPos.y();
        }

        int newPos = _currentPos + delta;
        newPos = qBound(_rangeMin, newPos, _rangeMax);

        if (newPos != _currentPos) {
            _currentPos = newPos;
            _dragStartPos = event->globalPos();
            emit splitterMoved(newPos);
        }
    }
}

void DockingSplitter::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), palette().color(QPalette::Mid));
}

// =============================================================================
// DockingManager
// =============================================================================

DockingManager::DockingManager(QWidget* parent)
    : QWidget(parent)
{
    // Create four dock containers
    for (int i = 0; i < DOCKCONT_MAX; ++i) {
        auto* container = new DockingContainer(this);
        _containers.append(container);

        auto* splitter = new DockingSplitter(
            (i == CONT_LEFT || i == CONT_RIGHT) ? DockingSplitter::Orientation::Vertical
                                               : DockingSplitter::Orientation::Horizontal,
            this);
        _splitters.append(splitter);
    }

    // Initial layout
    relayout();
}

DockingManager::~DockingManager() {
    for (auto* c : _containers) delete c;
    for (auto* s : _splitters) delete s;
}

void DockingManager::init(QWidget* mainWindow) {
    _mainWindow = mainWindow;
    // Place containers around the main window
    relayout();
}

void DockingManager::setClientWindow(QWidget* client) {
    _mainWindow = client;
    relayout();
}

void DockingManager::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    relayout();
}

void DockingManager::relayout() {
    if (!_mainWindow) return;

    int w = width();
    int h = height();
    int splitW = SPLITTER_WIDTH;
    int splitH = SPLITTER_WIDTH;

    // Default: side panels 200px wide, top/bottom 150px tall
    int sideW = qMin(200, w / 4);
    int topH = qMin(150, h / 4);

    QRect leftRect(0, 0, sideW, h);
    QRect rightRect(w - sideW, 0, sideW, h);
    QRect topRect(0, 0, w, topH);
    QRect bottomRect(0, h - topH, w, topH);

    // Container 0: Left
    if (_containers[CONT_LEFT]->isVisible() || _containers[CONT_LEFT]->hasWidgets()) {
        _containers[CONT_LEFT]->setGeometry(leftRect);
    }

    // Container 1: Right
    if (_containers[CONT_RIGHT]->isVisible() || _containers[CONT_RIGHT]->hasWidgets()) {
        _containers[CONT_RIGHT]->setGeometry(rightRect);
    }

    // Container 2: Top
    if (_containers[CONT_TOP]->isVisible() || _containers[CONT_TOP]->hasWidgets()) {
        _containers[CONT_TOP]->setGeometry(topRect);
    }

    // Container 3: Bottom
    if (_containers[CONT_BOTTOM]->isVisible() || _containers[CONT_BOTTOM]->hasWidgets()) {
        _containers[CONT_BOTTOM]->setGeometry(bottomRect);
    }

    // Splitters between regions
    // Left-Right divider (vertical)
    _splitters[0]->setGeometry(sideW, 0, splitW, h);

    // Top-Bottom splitter (horizontal)
    _splitters[1]->setGeometry(0, topH, w, splitH);

    // Show/hide based on whether containers have content
    for (int i = 0; i < DOCKCONT_MAX; ++i) {
        bool hasContent = _containers[i]->hasWidgets();
        _containers[i]->setVisible(hasContent);
    }

    // Splitters visible when adjacent containers have content
    _splitters[0]->setVisible(_containers[CONT_LEFT]->hasWidgets() &&
                               _containers[CONT_RIGHT]->hasWidgets());
    _splitters[1]->setVisible(_containers[CONT_TOP]->hasWidgets() &&
                               _containers[CONT_BOTTOM]->hasWidgets());
}

void DockingManager::showFloatingContainers(bool show) {
    for (int i = 0; i < DOCKCONT_MAX; ++i) {
        if (_containers[i]->isFloating()) {
            _containers[i]->setVisible(show);
        }
    }
}

void DockingManager::createDockableDialog(DockedWidgetData data, int container, bool visible) {
    if (container < 0 || container >= DOCKCONT_MAX) container = CONT_LEFT;

    auto* dlg = qobject_cast<QWidget*>(data.client);
    if (!dlg) return;

    DockingContainer* cont = _containers[container];
    cont->addDockedWidget(data);

    if (visible) {
        cont->showDockedWidget(cont->findDockedWidgetByWnd(dlg), true);
    }

    relayout();
}

void DockingManager::showDockableDialog(QWidget* dlg, bool show) {
    for (auto* cont : _containers) {
        DockedWidgetData* data = cont->findDockedWidgetByWnd(dlg);
        if (data) {
            cont->showDockedWidget(data, show);
            return;
        }
    }
}

void DockingManager::showDockableDialogByName(const QString& name, bool show) {
    for (auto* cont : _containers) {
        DockedWidgetData* data = cont->findDockedWidgetByName(name);
        if (data) {
            cont->showDockedWidget(data, show);
            return;
        }
    }
}

void DockingManager::setActiveTab(int container, int index) {
    if (container >= 0 && container < DOCKCONT_MAX) {
        _containers[container]->setActiveTab(index);
    }
}

int DockingManager::dockedContainerSize(int container) const {
    if (container < 0 || container >= DOCKCONT_MAX) return 0;
    QRect geo = _containers[container]->geometry();
    if (container == CONT_LEFT || container == CONT_RIGHT) return geo.width();
    return geo.height();
}

void DockingManager::setDockedContainerSize(int container, int size) {
    Q_UNUSED(container);
    Q_UNUSED(size);
    relayout();
}

void DockingManager::setStyleCaption(bool captionOnTop) {
    for (int i = 0; i < DOCKCONT_MAX; ++i) {
        _containers[i]->setCaptionTop(captionOnTop);
    }
}

int DockingManager::containerIndexOf(QWidget* client) const {
    for (int i = 0; i < DOCKCONT_MAX; ++i) {
        if (_containers[i]->findDockedWidgetByWnd(client)) {
            return i;
        }
    }
    return -1;
}

void DockingManager::updateContainerInfo(QWidget* client) {
    Q_UNUSED(client);
    relayout();
}

std::vector<DockingContainer*> DockingManager::getContainerInfo() const {
    return std::vector<DockingContainer*>(_containers.begin(), _containers.end());
}

QWidget* DockingManager::findDockedWidgetByName(const QString& name) {
    for (auto* cont : _containers) {
        DockedWidgetData* data = cont->findDockedWidgetByName(name);
        if (data) return data->client;
    }
    return nullptr;
}


