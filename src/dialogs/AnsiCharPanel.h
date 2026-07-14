// AnsiCharPanel.h - ASCII/ANSI character insertion panel
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDockWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>

class AnsiCharPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit AnsiCharPanel(QWidget* parent = nullptr);
    ~AnsiCharPanel() override;

    // Switch the displayed code page
    void setCodePage(int cp);

    // The current codepage index (0 = ASCII/ANSI, etc.)
    int currentCodePage() const { return _currentCp; }

public slots:
    void refreshPanel();

private slots:
    void onCellClicked(int row, int col);
    void onCodePageChanged(int index);
    void onCharDoubleClicked(int row, int col);

private:
    void setupUi();
    void buildGrid();
    QString charLabel(int code) const;
    QString charTooltip(int code) const;

    // Color coding based on character class
    QColor cellColor(int code) const;
    bool isPrintable(int code) const;
    bool isControlChar(int code) const;
    bool isExtendedChar(int code) const;

    QTableWidget* _grid = nullptr;
    QComboBox*    _cpCombo = nullptr;
    QLabel*       _cpLabel = nullptr;

    int _currentCp = 1252;  // default Windows-1252 (Western European)

    // Map of known code page indices to Windows codepage numbers
    static const QMap<int, int> codePageMap;
};
