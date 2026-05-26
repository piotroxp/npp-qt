// StatusBar.cpp — Qt6 translation of CStatusBar → QStatusBar
#include "StatusBar.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QPalette>
#include <QStyle>
#include <QApplication>

StatusBar::StatusBar(QWidget* parent)
    : QStatusBar(parent)
{
    setSizeGripEnabled(true);
    setContentsMargins(0, 0, 0, 0);

    // Default styling
    setStyleSheet(R"(
        QStatusBar {
            background: palette(window);
            border-top: 1px solid palette(dark);
        }
        QLabel {
            background: transparent;
            padding: 2px 4px;
            border: none;
        }
    )");
}

StatusBar::~StatusBar() = default;

void StatusBar::init(int nbParts)
{
    // Remove old labels
    for (QLabel* lbl : _partLabels)
        delete lbl;
    _partLabels.clear();

    _partWidthArray.clear();
    _partWidthArray.resize(nbParts, 5); // default width

    setupLabels(nbParts);
}

void StatusBar::setupLabels(int nbParts)
{
    // Create a container widget for horizontal layout
    QWidget* container = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < nbParts; ++i)
    {
        QLabel* label = new QLabel(container);
        label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        label->setTextInteractionFlags(Qt::NoTextInteraction);
        label->setCursor(Qt::ArrowCursor);
        label->setStyleSheet(R"(
            QLabel {
                border-right: 1px solid palette(dark);
                padding-right: 4px;
            }
        )");

        // Connect click signal
        label->installEventFilter(this);

        _partLabels.append(label);
        layout->addWidget(label);
    }

    layout->addStretch(); // Fill remaining space
    addPermanentWidget(container, 1); // stretch = 1

    adjustParts(width());
}

bool StatusBar::setPartWidth(int whichPart, int width)
{
    if (whichPart < 0 || whichPart >= _partWidthArray.size())
        return false;

    _partWidthArray[whichPart] = width;
    return true;
}

bool StatusBar::setText(const QString& str, int whichPart)
{
    if (whichPart < 0 || whichPart >= _partLabels.size())
        return false;

    _lastSetText = str;
    _partLabels[whichPart]->setText(str);
    return true;
}

bool StatusBar::setOwnerDrawText(const QString& str)
{
    _lastSetText = str;
    // Owner draw would require custom paint — simplified here
    // Full implementation would draw directly via paintEvent
    for (auto* lbl : _partLabels)
    {
        if (lbl->text().isEmpty())
        {
            lbl->setText(str);
            return true;
        }
    }
    return false;
}

void StatusBar::adjustParts(int clientWidth)
{
    // Calculate right-edge positions for each part
    int nWidth = qMax(clientWidth - 20, 0);

    // Compute positions in reverse
    QVector<int> rightEdges(_partWidthArray.size());
    for (int i = _partWidthArray.size() - 1; i >= 0; --i)
    {
        rightEdges[i] = nWidth;
        nWidth -= _partWidthArray[i];
    }

    // Apply to labels
    for (int i = 0; i < _partLabels.size() && i < rightEdges.size(); ++i)
    {
        // Set minimum width for each label
        _partLabels[i]->setMinimumWidth(_partWidthArray[i] > 5 ? _partWidthArray[i] : 50);
    }
}

int StatusBar::getHeight() const
{
    return QStatusBar::sizeHint().height();
}

void StatusBar::setDarkMode(bool enabled)
{
    _isDarkMode = enabled;
    if (enabled)
    {
        setStyleSheet(R"(
            QStatusBar {
                background: #2d2d30;
                color: #cccccc;
                border-top: 1px solid #3e3e42;
            }
            QLabel {
                color: #cccccc;
            }
        )");
    }
    else
    {
        setStyleSheet(R"(
            QStatusBar {
                background: palette(window);
                color: palette(window-text);
                border-top: 1px solid palette(dark);
            }
        )");
    }
}

void StatusBar::resizeEvent(QResizeEvent* event)
{
    QStatusBar::resizeEvent(event);
    adjustParts(event->size().width());
}