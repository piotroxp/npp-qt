#pragma once

#include <QWidget>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QLineEdit>
#include <QToolBar>
#include <QProgressBar>
#include <QLabel>

class WebBrowserView : public QWidget {
    Q_OBJECT

public:
    explicit WebBrowserView(QWidget* parent = nullptr);
    ~WebBrowserView() override;

    void load(const QUrl& url);
    void loadHtml(const QString& html, const QString& baseUrl = QString());
    QUrl url() const;

    void goBack();
    void goForward();
    void reload();
    void stop();

    void zoomIn();
    void zoomOut();
    void resetZoom();
    int zoomFactor() const { return qRound(zoomFactor_ * 100); }

    bool isLoading() const { return loading_; }

    // Print support
    void printToPdf(const QString& filePath);

    // Access to the underlying page
    QWebEnginePage* page() const;

signals:
    void loadingStarted();
    void loadingFinished(bool ok);
    void loadingProgress(int percent);
    void urlChanged(const QUrl& url);
    void linkClicked(const QUrl& url);

private slots:
    void onLoadStarted();
    void onLoadFinished(bool ok);
    void onLoadProgress(int percent);
    void onUrlChanged(const QUrl& url);
    void onAddressReturnPressed();
    void updateNavigationButtons();

private:
    void setupUi();
    void applyStyle();

    QWebEngineView* webView = nullptr;
    QLineEdit* addressBar = nullptr;
    QToolBar* toolbar = nullptr;
    QProgressBar* progressBar = nullptr;
    QAction* backAction = nullptr;
    QAction* forwardAction = nullptr;
    QAction* reloadAction = nullptr;
    QAction* stopAction = nullptr;
    QAction* zoomInAction = nullptr;
    QAction* zoomOutAction = nullptr;
    QAction* resetZoomAction = nullptr;

    bool loading_ = false;
    double zoomFactor_ = 1.0;

    static constexpr double kMinZoomFactor = 0.5;
    static constexpr double kMaxZoomFactor = 2.0;
    static constexpr double kZoomStep = 0.1;
};

// Custom WebEnginePage to handle link clicks
class WebBrowserPage : public QWebEnginePage {
    Q_OBJECT

public:
    explicit WebBrowserPage(QWebEngineProfile* profile, QObject* parent = nullptr);

protected:
    bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override;

signals:
    void linkClickedSignal(const QUrl& url);
};
