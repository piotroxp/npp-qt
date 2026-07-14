// UserDefineDialog.h - User-Defined Language editor dialog
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QDialog>
#include <QTabWidget>
#include <QTableWidget>
#include <QColorDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

class UdlManager;
struct UdlDefinition;

class UserDefineDialog : public QDialog {
    Q_OBJECT

public:
    explicit UserDefineDialog(QWidget* parent = nullptr);
    ~UserDefineDialog() override;

    // Load settings from a named UDL definition
    void loadFromManager(UdlManager* manager, const QString& udlName);
    // Save settings back to the named UDL definition
    void saveToManager(UdlManager* manager, const QString& udlName);

    // Create a fresh UDL
    void createNew(const QString& name);

    QString currentName() const { return _nameEdit->text(); }

private:
    void setupUi();
    void createKeywordsTab(QTabWidget* tabs);
    void createCommentsTab(QTabWidget* tabs);
    void createNumbersTab(QTabWidget* tabs);
    void createOperatorsTab(QTabWidget* tabs);
    void createDelimitersTab(QTabWidget* tabs);
    void createFoldingTab(QTabWidget* tabs);

    // Keywords tab helpers
    void setupKeywordTable(QTableWidget* table, const QString& keywords);
    QString keywordsFromTable(QTableWidget* table) const;

    // Operators table helpers
    void setupOperatorTable();
    QString operatorsFromTable() const;

    // Delimiter helpers
    void setupDelimiterTable();
    QString delimitersFromTable() const;

    // Number style preview
    void updateNumberPreviews();

    // Style color helpers
    QString getColorString(const QColor& c) const;

    // --- Language name ---
    QLineEdit* _nameEdit = nullptr;
    QLineEdit* _extEdit  = nullptr;

    // --- Keywords tab (8 keyword groups) ---
    QTableWidget* _keywordTables[9];  // words0..words8

    // --- Comments tab ---
    QLineEdit* _lineCommentEdit = nullptr;
    QLineEdit* _blockCommentStartEdit = nullptr;
    QLineEdit* _blockCommentEndEdit = nullptr;
    QCheckBox* _enableCommentFold = nullptr;

    // --- Numbers tab ---
    QLabel*   _numberFgPreview = nullptr;
    QLabel*   _numberBgPreview = nullptr;
    QPushButton* _numberFgBtn = nullptr;
    QPushButton* _numberBgBtn = nullptr;
    QColor    _numberFgColor;
    QColor    _numberBgColor;
    QCheckBox* _numberBold = nullptr;
    QCheckBox* _numberItalic = nullptr;

    // --- Operators tab ---
    QTableWidget* _operatorTable = nullptr;

    // --- Delimiters tab ---
    QTableWidget* _delimiterTable = nullptr;
    QCheckBox*    _delimiterForceNested = nullptr;
    QCheckBox*    _delimiterFarHighlight = nullptr;

    // --- Folding tab ---
    QCheckBox* _foldOnKeyword = nullptr;
    QLineEdit* _foldKeywordEdit = nullptr;
    QCheckBox* _foldOnComment = nullptr;
    QCheckBox* _foldOnPreproc = nullptr;
    QLineEdit* _preprocEdit = nullptr;
    QLineEdit* _folderOpenEdit = nullptr;
    QLineEdit* _folderCloseEdit = nullptr;
};
