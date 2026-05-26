// StatusBar.h — Qt6 translation of CStatusBar → QStatusBar
#pragma once

#include <QStatusBar>
#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QMap>

class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget* parent = nullptr);
    ~StatusBar() override;

    // Initialize with N parts (like SB_SETPARTS)
    void init(int nbParts);

    // Set width of a specific part
    bool setPartWidth(int whichPart, int width);

    // Set text in a part
    bool setText(const QString& str, int whichPart);

    // Owner-draw text (for custom rendering like color highlights)
    bool setOwnerDrawText(const QString& str);

    // Adjust part widths based on client width
    void adjustParts(int clientWidth);

    // Get total height
    int getHeight() const override;

    // Dark mode
    void setDarkMode(bool enabled);
    bool isDarkMode() const { return _isDarkMode; }

    // Get/set last text
    QString getLastText() const { return _lastSetText; }

signals:
    void partClicked(int whichPart);
    void segmentClicked(int whichPart);

protected:
    // Override to handle resize
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupLabels(int nbParts);

    QVector<int> _partWidthArray;
    QVector<QLabel*> _partLabels;
    QString _lastSetText;
    bool _isDarkMode = false;
};