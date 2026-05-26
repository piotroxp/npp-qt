// functionListPanel.cpp — Qt6 translation
#include "functionListPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QToolButton>

// RegexFunctionParser
RegexFunctionParser::RegexFunctionParser(const QString& pattern)
    : _pattern(pattern)
{
}

QVector<FunctionInfo> RegexFunctionParser::parse(const QString& code)
{
    QVector<FunctionInfo> results;
    int pos = 0;
    while ((pos = _pattern.indexIn(code, pos)) != -1)
    {
        FunctionInfo info;
        info.name = _pattern.cap(1);
        info.category = _pattern.cap(2);
        results.append(info);
        pos += _pattern.matchedLength();
    }
    return results;
}

// FunctionListPanel
FunctionListPanel::FunctionListPanel(QWidget* parent)
    : QDockWidget("Function List", parent)
{
    setObjectName("FunctionList");

    QWidget* content = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->setContentsMargins(0, 0, 0, 0);

    // Toolbar
    _toolbar = new QToolBar(content);
    QToolButton* sortBtn = new QToolButton(_toolbar);
    sortBtn->setText("Sort A-Z");
    _toolbar->addWidget(sortBtn);
    _toolbar->addAction("Expand All", this, &FunctionListPanel::expandAll);
    _toolbar->addAction("Collapse All", this, &FunctionListPanel::collapseAll);
    _toolbar->addAction("Refresh", this, &FunctionListPanel::reload);
    layout->addWidget(_toolbar);

    // Search box
    _searchBox = new QLineEdit(content);
    _searchBox->setPlaceholderText("Search functions...");
    layout->addWidget(_searchBox);

    // Tree
    _tree = new QTreeWidget(content);
    _tree->setHeaderHidden(true);
    _tree->setIndentation(20);
    _tree->setSortingEnabled(true);
    layout->addWidget(_tree);

    setWidget(content);

    // Default parser for C-like languages
    _parser = new RegexFunctionParser(R"(^(.+?)\s*\([^)]*\)\s*\{)"); // Simple function pattern

    connect(_searchBox, &QLineEdit::textChanged, this, &FunctionListPanel::onSearchTextChanged);
    connect(_tree, &QTreeWidget::itemDoubleClicked, this, &FunctionListPanel::onItemDoubleClicked);
}

FunctionListPanel::~FunctionListPanel() = default;

void FunctionListPanel::reload()
{
    // Would get current editor text
    QString code = "";
    QVector<FunctionInfo> functions = _parser->parse(code);
    buildTree(functions);
}

void FunctionListPanel::expandAll()
{
    _tree->expandAll();
}

void FunctionListPanel::collapseAll()
{
    _tree->collapseAll();
}

void FunctionListPanel::sortItems(bool ascending)
{
    _tree->setSortingEnabled(true);
    _tree->sortByColumn(0, ascending ? Qt::AscendingOrder : Qt::DescendingOrder);
    _isSorted = ascending;
}

void FunctionListPanel::search(const QString& text)
{
    _searchBox->setText(text);
}

void FunctionListPanel::clearSearch()
{
    _searchBox->clear();
}

void FunctionListPanel::selectFunction(const QString& name, int line)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(_tree);
    item->setText(0, name);
    _currentLine = line;
    emit functionSelected(line);
}

void FunctionListPanel::resizeEvent(QResizeEvent* event)
{
    QDockWidget::resizeEvent(event);
}

void FunctionListPanel::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (item)
    {
        bool ok = false;
        int line = item->data(0, Qt::UserRole).toInt(&ok);
        if (ok)
            emit functionSelected(line);
    }
}

void FunctionListPanel::onSearchTextChanged(const QString& text)
{
    emit searchTextChanged(text);
}

void FunctionListPanel::buildTree(const QVector<FunctionInfo>& functions)
{
    _tree->clear();

    QMap<QString, QTreeWidgetItem*> categories;
    for (const auto& func : functions)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_tree);
        item->setText(0, func.name);
        item->setData(0, Qt::UserRole, func.lineNumber);
        _tree->addTopLevelItem(item);
    }
}
