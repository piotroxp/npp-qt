#include "WebBrowserView.h"

#include <QAction>
#include <QBoxLayout>
#include <QIcon>
#include <QMenu>
#include <QToolButton>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineHistory>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QMessageLogger>
#include <QtDebug>

// ============================================================================
// WebBrowserPage Implementation
// ============================================================================

WebBrowserPage::WebBrowserPage(QWebEngineProfile* profile, QObject* parent)
    : QWebEnginePage(profile, parent)
{
    // Link clicks are handled in acceptNavigationRequest
}

bool WebBrowserPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
    if (type == QWebEnginePage::NavigationTypeLinkClicked && isMainFrame) {
        // Emit signal for link clicks - let the parent decide what to do
        emit linkClickedSignal(url);
        return false;  // Don't navigate automatically
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

// ============================================================================
// WebBrowserView Implementation
// ============================================================================

WebBrowserView::WebBrowserView(QWidget* parent)
    : QWidget(parent)
    , loading_(false)
    , zoomFactor_(1.0)
{
    setupUi();
    applyStyle();
}

WebBrowserView::~WebBrowserView() = default;

void WebBrowserView::setupUi()
{
    // Main layout
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Toolbar
    toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setIconSize(QSize(20, 20));

    // Back button
    backAction = toolbar->addAction(QIcon::fromTheme("go-previous", QIcon(":/icons/go-previous")), QString());
    backAction->setToolTip(tr("Back"));
    connect(backAction, &QAction::triggered, this, &WebBrowserView::goBack);

    // Forward button
    forwardAction = toolbar->addAction(QIcon::fromTheme("go-next", QIcon(":/icons/go-next")), QString());
    forwardAction->setToolTip(tr("Forward"));
    connect(forwardAction, &QAction::triggered, this, &WebBrowserView::goForward);

    // Stop button
    stopAction = toolbar->addAction(QIcon::fromTheme("process-stop", QIcon(":/icons/process-stop")), QString());
    stopAction->setToolTip(tr("Stop"));
    stopAction->setVisible(false);
    connect(stopAction, &QAction::triggered, this, &WebBrowserView::stop);

    // Reload button
    reloadAction = toolbar->addAction(QIcon::fromTheme("view-refresh", QIcon(":/icons/view-refresh")), QString());
    reloadAction->setToolTip(tr("Reload"));
    connect(reloadAction, &QAction::triggered, this, &WebBrowserView::reload);

    toolbar->addSeparator();

    // Zoom out button
    zoomOutAction = toolbar->addAction(QIcon::fromTheme("zoom-out", QIcon(":/icons/zoom-out")), QString());
    zoomOutAction->setToolTip(tr("Zoom Out"));
    connect(zoomOutAction, &QAction::triggered, this, &WebBrowserView::zoomOut);

    // Zoom in button
    zoomInAction = toolbar->addAction(QIcon::fromTheme("zoom-in", QIcon(":/icons/zoom-in")), QString());
    zoomInAction->setToolTip(tr("Zoom In"));
    connect(zoomInAction, &QAction::triggered, this, &WebBrowserView::zoomIn);

    toolbar->addSeparator();

    // Address bar
    addressBar = new QLineEdit(toolbar);
    addressBar->setPlaceholderText(tr("Enter URL or search..."));
    addressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addressBar->setMinimumWidth(300);
    connect(addressBar, &QLineEdit::returnPressed, this, &WebBrowserView::onAddressReturnPressed);

    auto* addressContainer = new QWidget(toolbar);
    auto* addressLayout = new QHBoxLayout(addressContainer);
    addressLayout->setContentsMargins(4, 0, 4, 0);
    addressLayout->setSpacing(0);
    addressLayout->addWidget(addressBar);
    addressContainer->setLayout(addressLayout);

    toolbar->addWidget(addressContainer);

    mainLayout->addWidget(toolbar);

    // Progress bar
    progressBar = new QProgressBar(this);
    progressBar->setMaximumHeight(3);
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setTextVisible(false);
    progressBar->setVisible(false);
    mainLayout->addWidget(progressBar);

    // Web Engine View
    auto* profile = QWebEngineProfile::defaultProfile();

    // Configure WebEngine settings
    QWebEngineSettings* settings = profile->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

    // Set default font on profile settings
    settings->setFontFamily(QWebEngineSettings::StandardFont, "Segoe UI");
    settings->setFontFamily(QWebEngineSettings::SerifFont, "Georgia");
    settings->setFontFamily(QWebEngineSettings::SansSerifFont, "Segoe UI");
    settings->setFontFamily(QWebEngineSettings::CursiveFont, "Comic Sans MS");
    settings->setFontFamily(QWebEngineSettings::FantasyFont, "Impact");
    settings->setFontSize(QWebEngineSettings::MinimumFontSize, 8);
    settings->setFontSize(QWebEngineSettings::MinimumLogicalFontSize, 6);

    auto* page = new WebBrowserPage(profile, this);
    webView = new QWebEngineView(this);
    webView->setPage(page);
    webView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addWidget(webView);

    setLayout(mainLayout);

    // Connect signals from WebEngineView
    connect(webView, &QWebEngineView::loadStarted, this, &WebBrowserView::onLoadStarted);
    connect(webView, &QWebEngineView::loadFinished, this, &WebBrowserView::onLoadFinished);
    connect(webView, &QWebEngineView::loadProgress, this, &WebBrowserView::onLoadProgress);
    connect(webView, &QWebEngineView::urlChanged, this, &WebBrowserView::onUrlChanged);

    // Connect signals from custom page
    connect(page, &WebBrowserPage::linkClickedSignal, this, &WebBrowserView::linkClicked);

    // Update navigation buttons
    connect(webView, &QWebEngineView::urlChanged, this, &WebBrowserView::updateNavigationButtons);

    // Initial state
    updateNavigationButtons();
}

void WebBrowserView::applyStyle()
{
    // Dark theme colors matching npp-qt
    const QString toolbarStyle = R"(
        QToolBar {
            background-color: #2b2b2b;
            border: none;
            padding: 4px;
            spacing: 4px;
        }
        QToolBar::separator {
            background-color: #3d3d3d;
            width: 1px;
            margin: 4px 6px;
        }
        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 3px;
            padding: 4px;
            color: #d4d4d4;
        }
        QToolButton:hover {
            background-color: #3d3d3d;
        }
        QToolButton:pressed {
            background-color: #4d4d4d;
        }
        QToolButton:disabled {
            color: #666666;
        }
        QLineEdit {
            background-color: #1e1e1e;
            color: #d4d4d4;
            border: 1px solid #3d3d3d;
            border-radius: 3px;
            padding: 4px 8px;
            selection-background-color: #264f78;
        }
        QLineEdit:focus {
            border-color: #007acc;
        }
        QLineEdit:disabled {
            background-color: #252526;
            color: #666666;
        }
    )";

    const QString progressBarStyle = R"(
        QProgressBar {
            background-color: #3d3d3d;
            border: none;
            height: 3px;
        }
        QProgressBar::chunk {
            background-color: #007acc;
        }
    )";

    const QString viewStyle = R"(
        QWebEngineView {
            background-color: #ffffff;
        }
    )";

    toolbar->setStyleSheet(toolbarStyle);
    progressBar->setStyleSheet(progressBarStyle);
    webView->setStyleSheet(viewStyle);

    // Set page background color via JavaScript injection
    page()->runJavaScript(
        "document.body ? document.body.style.backgroundColor = '#ffffff' : '';"
    );
}

void WebBrowserView::load(const QUrl& url)
{
    webView->load(url);
}

void WebBrowserView::loadHtml(const QString& html, const QString& baseUrl)
{
    QUrl base = baseUrl.isEmpty() ? QUrl("about:blank") : QUrl(baseUrl);
    webView->setContent(html.toUtf8(), "text/html;charset=UTF-8", base);
}

QUrl WebBrowserView::url() const
{
    return webView->url();
}

void WebBrowserView::goBack()
{
    if (webView->history()->canGoBack()) {
        webView->back();
    }
}

void WebBrowserView::goForward()
{
    if (webView->history()->canGoForward()) {
        webView->forward();
    }
}

void WebBrowserView::reload()
{
    webView->reload();
}

void WebBrowserView::stop()
{
    webView->stop();
}

void WebBrowserView::zoomIn()
{
    if (zoomFactor_ < kMaxZoomFactor) {
        zoomFactor_ = qMin(kMaxZoomFactor, zoomFactor_ + kZoomStep);
        zoomFactor_ = qRound(zoomFactor_ * 10) / 10.0;  // Round to 1 decimal
        webView->setZoomFactor(zoomFactor_);
    }
}

void WebBrowserView::zoomOut()
{
    if (zoomFactor_ > kMinZoomFactor) {
        zoomFactor_ = qMax(kMinZoomFactor, zoomFactor_ - kZoomStep);
        zoomFactor_ = qRound(zoomFactor_ * 10) / 10.0;  // Round to 1 decimal
        webView->setZoomFactor(zoomFactor_);
    }
}

void WebBrowserView::resetZoom()
{
    zoomFactor_ = 1.0;
    webView->setZoomFactor(1.0);
}

void WebBrowserView::printToPdf(const QString& filePath)
{
    QWebEnginePage* page = webView->page();
    if (page) {
        page->printToPdf(filePath, QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait, QMarginsF(15, 15, 15, 15)));
    }
}

void WebBrowserView::onLoadStarted()
{
    loading_ = true;

    // Update UI
    progressBar->setVisible(true);
    progressBar->setValue(0);

    // Show stop button, hide reload button
    stopAction->setVisible(true);
    reloadAction->setVisible(false);

    updateNavigationButtons();

    emit loadingStarted();
}

void WebBrowserView::onLoadFinished(bool ok)
{
    loading_ = false;

    // Update UI
    progressBar->setVisible(false);

    // Show reload button, hide stop button
    reloadAction->setVisible(true);
    stopAction->setVisible(false);

    updateNavigationButtons();

    emit loadingFinished(ok);
}

void WebBrowserView::onLoadProgress(int percent)
{
    if (progressBar->isVisible()) {
        progressBar->setValue(percent);
    }
    emit loadingProgress(percent);
}

void WebBrowserView::onUrlChanged(const QUrl& url)
{
    // Update address bar (without triggering signal)
    if (addressBar->text() != url.toString()) {
        addressBar->setText(url.toString());
    }

    updateNavigationButtons();
    emit urlChanged(url);
}

void WebBrowserView::onAddressReturnPressed()
{
    QString input = addressBar->text().trimmed();

    if (input.isEmpty()) {
        return;
    }

    QUrl url;

    // Check if it looks like a URL
    if (input.contains('.') && !input.contains(' ') &&
        !input.startsWith("http://") && !input.startsWith("https://") &&
        !input.startsWith("file://")) {
        // Add https:// for common domains
        url = QUrl("https://" + input);
        if (!url.isValid() || url.host().isEmpty()) {
            // Try as search query
            url = QUrl("https://www.google.com/search?q=" + QUrl::toPercentEncoding(input));
        }
    } else if (input.startsWith("file://")) {
        url = QUrl(input);
    } else if (input.startsWith("http://") || input.startsWith("https://")) {
        url = QUrl(input);
    } else {
        // Treat as search query
        url = QUrl("https://www.google.com/search?q=" + QUrl::toPercentEncoding(input));
    }

    if (url.isValid()) {
        load(url);
    }
}

void WebBrowserView::updateNavigationButtons()
{
    QWebEngineHistory* history = webView->history();
    if (history) {
        backAction->setEnabled(history->canGoBack());
        forwardAction->setEnabled(history->canGoForward());
    } else {
        backAction->setEnabled(false);
        forwardAction->setEnabled(false);
    }

    // Update zoom button states
    zoomInAction->setEnabled(zoomFactor_ < kMaxZoomFactor);
    zoomOutAction->setEnabled(zoomFactor_ > kMinZoomFactor);
}

QWebEnginePage* WebBrowserView::page() const
{
    return webView->page();
}
