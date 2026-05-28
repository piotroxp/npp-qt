// DialogShim.h - Qt6 dialog compatibility shim
#pragma once

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QCompleter>
#include <QListWidget>
#include <QTableWidget>
#include <cstddef>

// Forward declarations
class Notepad_plus;

// Find/Replace Dialog placeholder
class FindReplaceDlg : public QDialog {
    Q_OBJECT
public:
    explicit FindReplaceDlg(QWidget* parent = nullptr) : QDialog(parent) {
        setupUI();
    }

    void init(Notepad_plus* parent, QWidget* view, QObject* receiver) {
        _npp = parent;
        _editView = view;
        // Connect signals if receiver provided
    }

    void setSearchText(const QString& text) {
        if (_findEdit) _findEdit->setText(text);
    }

    QString getSearchText() const {
        return _findEdit ? _findEdit->text() : QString();
    }

    void setReplaceText(const QString& text) {
        if (_replaceEdit) _replaceEdit->setText(text);
    }

    QString getReplaceText() const {
        return _replaceEdit ? _replaceEdit->text() : QString();
    }

    // Dummy methods
    virtual void init() {}
    virtual void displayFullScintillaCall(int, int, const QString&) {}

public slots:
    void onFindNext() {}
    void onReplaceNext() {}
    void onReplaceAll() {}
    void onFindPrevious() {}
    void onFindInFiles() {}
    void onMark() {}

signals:
    void findNextRequested(const QString&, bool, bool);
    void replaceRequested(const QString&, const QString&);
    void findInFilesRequested(const QString&, const QStringList&);

protected:
    void setupUI() {
        QVBoxLayout* mainLayout = new QVBoxLayout;
        
        QHBoxLayout* findRow = new QHBoxLayout;
        _findEdit = new QLineEdit(this);
        _findLabel = new QLabel(tr("Find:"), this);
        findRow->addWidget(_findLabel);
        findRow->addWidget(_findEdit);
        
        QHBoxLayout* replaceRow = new QHBoxLayout;
        _replaceEdit = new QLineEdit(this);
        _replaceLabel = new QLabel(tr("Replace:"), this);
        replaceRow->addWidget(_replaceLabel);
        replaceRow->addWidget(_replaceEdit);
        
        _btnFindNext = new QPushButton(tr("Find Next"), this);
        _btnReplace = new QPushButton(tr("Replace"), this);
        _btnReplaceAll = new QPushButton(tr("Replace All"), this);
        
        mainLayout->addLayout(findRow);
        mainLayout->addLayout(replaceRow);
        mainLayout->addWidget(_btnFindNext);
        
        QDialogButtonBox* btns = new QDialogButtonBox(QDialogButtonBox::Close, this);
        mainLayout->addWidget(btns);
        
        setLayout(mainLayout);
        setWindowTitle(tr("Find / Replace"));
    }

    Notepad_plus* _npp = nullptr;
    QWidget* _editView = nullptr;
    QLineEdit* _findEdit = nullptr;
    QLineEdit* _replaceEdit = nullptr;
    QLabel* _findLabel = nullptr;
    QLabel* _replaceLabel = nullptr;
    QPushButton* _btnFindNext = nullptr;
    QPushButton* _btnReplace = nullptr;
    QPushButton* _btnReplaceAll = nullptr;
};

// Go-to-Line Dialog
class GoToLineDlg : public QDialog {
    Q_OBJECT
public:
    explicit GoToLineDlg(QWidget* parent = nullptr) : QDialog(parent) {
        setupUI();
    }

    void init(Notepad_plus* parent, QWidget* view) {
        _npp = parent;
        _editView = view;
    }

    int getLineNumber() const {
        return _spinBox ? _spinBox->value() : 1;
    }

public slots:
    void onGoToClicked() {
        accept();
    }

protected:
    void setupUI() {
        QVBoxLayout* mainLayout = new QVBoxLayout;
        
        QHBoxLayout* row = new QHBoxLayout;
        row->addWidget(new QLabel(tr("Line Number:"), this));
        _spinBox = new QSpinBox(this);
        _spinBox->setMinimum(1);
        _spinBox->setMaximum(999999);
        row->addWidget(_spinBox);
        
        _btnGoTo = new QPushButton(tr("Go To"), this);
        connect(_btnGoTo, &QPushButton::clicked, this, &GoToLineDlg::onGoToClicked);
        
        mainLayout->addLayout(row);
        mainLayout->addWidget(_btnGoTo);
        
        setLayout(mainLayout);
        setWindowTitle(tr("Go To Line"));
    }

    Notepad_plus* _npp = nullptr;
    QWidget* _editView = nullptr;
    QSpinBox* _spinBox = nullptr;
    QPushButton* _btnGoTo = nullptr;
};

// Run Command Dialog  
class RunDlg : public QDialog {
    Q_OBJECT
public:
    explicit RunDlg(QWidget* parent = nullptr) : QDialog(parent) {
        setupUI();
    }

    void init(Notepad_plus* npp) {
        _npp = npp;
    }

    QString getCommand() const {
        return _cmdEdit ? _cmdEdit->text() : QString();
    }

public slots:
    void onRunClicked() {
        accept();
    }
    
    void onBrowseClicked() {
        QString f = QFileDialog::getOpenFileName(this, tr("Select Executable"));
        if (!f.isEmpty() && _cmdEdit) {
            _cmdEdit->setText(f);
        }
    }

signals:
    void runRequested(const QString&);

protected:
    void setupUI() {
        QVBoxLayout* mainLayout = new QVBoxLayout;
        
        QHBoxLayout* row = new QHBoxLayout;
        row->addWidget(new QLabel(tr("Command:"), this));
        _cmdEdit = new QLineEdit(this);
        _btnBrowse = new QPushButton("...", this);
        connect(_btnBrowse, &QPushButton::clicked, this, &RunDlg::onBrowseClicked);
        row->addWidget(_cmdEdit);
        row->addWidget(_btnBrowse);
        
        _btnRun = new QPushButton(tr("Run"), this);
        connect(_btnRun, &QPushButton::clicked, this, &RunDlg::onRunClicked);
        
        mainLayout->addLayout(row);
        mainLayout->addWidget(_btnRun);
        
        setLayout(mainLayout);
        setWindowTitle(tr("Run"));
    }

    Notepad_plus* _npp = nullptr;
    QLineEdit* _cmdEdit = nullptr;
    QPushButton* _btnBrowse = nullptr;
    QPushButton* _btnRun = nullptr;
};

// Column Editor Dialog placeholder
class ColumnEditorDlg : public QDialog {
    Q_OBJECT
public:
    explicit ColumnEditorDlg(QWidget* parent = nullptr) : QDialog(parent) {
        setupUI();
    }
    
    void init(QWidget* editView, const QString& text, int colBegin, int colEnd) {
        _editView = editView;
        _text = text;
    }

public slots:
    void onInsertClicked() {
        accept();
    }

protected:
    void setupUI() {
        setWindowTitle(tr("Column Editor"));
        resize(400, 200);
    }

    QWidget* _editView = nullptr;
    QString _text;
};

// Word styled dialog placeholder  
class WordStyleDlg : public QDialog {
    Q_OBJECT  
public:
    explicit WordStyleDlg(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle(tr("Word Style"));
    }
};