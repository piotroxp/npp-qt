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
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QFontComboBox>

class UdlManager;
struct UdlDefinition;

// Preview editor for real-time UDL testing
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

Q_SIGNALS:
    void udlSaved(const QString& name);
    void testUdlRequested(const QString& name);

private slots:
    void onImportClicked();
    void onExportClicked();
    void onTestClicked();
    void onResetClicked();
    void onAddKeywordClicked(int group);
    void onValidateClicked();
    void updatePreview();

private:
    void setupUi();
    void createKeywordsTab(QTabWidget* tabs);
    void createCommentsTab(QTabWidget* tabs);
    void createNumbersTab(QTabWidget* tabs);
    void createOperatorsTab(QTabWidget* tabs);
    void createDelimitersTab(QTabWidget* tabs);
    void createFoldingTab(QTabWidget* tabs);
    void createStringsTab(QTabWidget* tabs);
    void createPreviewTab(QTabWidget* tabs);

    // Keywords tab helpers
    void setupKeywordTable(QTableWidget* table, const QString& keywords);
    QString keywordsFromTable(QTableWidget* table) const;
    void populateKeywordAutocomplete();

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

    // Import / Export
    bool importFromFile(const QString& path);
    bool exportToFile(const QString& path) const;

    // Validation
    QStringList validateUdl() const;

    // --- Language name & extension ---
    QLineEdit* _nameEdit = nullptr;
    QLineEdit* _extEdit  = nullptr;

    // --- Keywords tab (9 keyword groups: 0-8) ---
    QTableWidget* _keywordTables[9] = {nullptr};
    QColor _keywordColors[9];
    QPushButton* _keywordColorBtns[9] = {nullptr};
    QLabel* _keywordColorPreviews[9] = {nullptr};

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

    // --- Strings tab ---
    QLineEdit* _stringOpenEdit = nullptr;
    QLineEdit* _stringCloseEdit = nullptr;
    QCheckBox* _escapeCharCheck = nullptr;
    QLineEdit* _escapeCharEdit = nullptr;
    QCheckBox* _stringWordCharCheck = nullptr;
    QLineEdit* _stringWordCharEdit = nullptr;

    // --- Preview tab ---
    QPlainTextEdit* _previewEditor = nullptr;
    QLabel* _previewStatusLabel = nullptr;

    // Internal state
    QString _currentUdlName;
    QStringList _allKnownKeywords;  // for autocomplete
};
