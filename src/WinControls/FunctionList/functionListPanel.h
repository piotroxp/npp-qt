// functionListPanel.h — Qt6 translation
#pragma once

#include <QDockWidget>
#include <QTreeWidget>
#include <QToolBar>
#include <QLineEdit>
#include <QRegExp>

struct FunctionInfo
{
    QString name;
    QString category;
    int lineNumber = -1;
};

// Function parser interface
class FunctionParser
{
public:
    virtual ~FunctionParser() = default;
    virtual QVector<FunctionInfo> parse(const QString& code) = 0;
};

// Simple regex-based parser
class RegexFunctionParser : public FunctionParser
{
public:
    explicit RegexFunctionParser(const QString& pattern);
    QVector<FunctionInfo> parse(const QString& code) override;

private:
    QRegExp _pattern;
};

// Function list panel
class FunctionListPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit FunctionListPanel(QWidget* parent = nullptr);
    ~FunctionListPanel() override;

    // Parse document
    void reload();

    // Tree operations
    void expandAll();
    void collapseAll();
    void sortItems(bool ascending = true);

    // Search
    void search(const QString& text);
    void clearSearch();

    // Selection
    void selectFunction(const QString& name, int line);
    int currentLine() const { return _currentLine; }

signals:
    void functionSelected(int line);
    void searchTextChanged(const QString& text);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onSearchTextChanged(const QString& text);

private:
    void buildTree(const QVector<FunctionInfo>& functions);
    void rebuildParser();

    QTreeWidget* _tree = nullptr;
    QLineEdit* _searchBox = nullptr;
    QToolBar* _toolbar = nullptr;
    FunctionParser* _parser = nullptr;
    int _currentLine = -1;
    bool _isSorted = false;
};
