#pragma once

#include <QObject>
#include <QUrl>

class WebBrowserView;
class QWidget;

class WebBrowserViewFactory {
public:
    static WebBrowserView* create(QWidget* parent = nullptr);
    static QString markdownToHtml(const QString& markdown);
    static QString localFileToDataUrl(const QString& filePath);

private:
    WebBrowserViewFactory() = default;
    ~WebBrowserViewFactory() = default;
};
