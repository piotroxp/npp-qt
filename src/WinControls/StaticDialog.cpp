// Semantic Lift: Win32 StaticDialog → Qt6 QDialog implementation
// Original: PowerEditor/src/WinControls/StaticDialog/
// Target: npp-qt/src/WinControls/StaticDialog.cpp

#include "StaticDialog.h"
#include "Window.h"
#include "NppConstants.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

// LOWORD/HIWORD macros (normally from <windows.h>)
inline unsigned int LOWORD(unsigned int x) { return x & 0xFFFF; }
inline unsigned int HIWORD(unsigned int x) { return (x >> 16) & 0xFFFF; }
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QFileDialog>
#include <QApplication>
#include <QWindow>
#include <QScreen>

// =============================================================================
// StaticDialog
// =============================================================================

StaticDialog::StaticDialog(QWidget* parent)
    : QWidget(parent)
{
    // Non-modal dialog by default
    setAttribute(Qt::WA_DeleteOnClose, false);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
}

StaticDialog::~StaticDialog() {
    // Subclass cleanup in destroy()
}

void StaticDialog::create(int dialogID, bool isRTL, bool msgDestParent) {
    _dialogId = dialogID;
    _isRTL = isRTL;
    _msgDestParent = msgDestParent;
    _isCreated = true;

    if (isRTL) {
        setLayoutDirection(Qt::RightToLeft);
    }

    // Subclasses should add their UI in reimplementations
}

void StaticDialog::display(bool toShow) {
    if (toShow) {
        if (!isVisible()) {
            // First time: go to center
            if (!_isCreated || geometry().isNull()) {
                goToCenter();
            }
            show();
            activateWindow();
            raise();
        } else {
            show();
            activateWindow();
            raise();
        }
    } else {
        hide();
    }
}

void StaticDialog::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    // Save geometry when first shown
    if (_rc.isNull()) {
        _rc = geometry();
    }
}

void StaticDialog::hideEvent(QHideEvent* event) {
    // Save geometry on hide
    _rc = geometry();
    QWidget::hideEvent(event);
}

void StaticDialog::closeEvent(QCloseEvent* event) {
    QWidget::closeEvent(event);
    emit dialogClosed();
}

bool StaticDialog::event(QEvent* event) {
    // Handle DPI change
    if (event->type() == QEvent::ScreenChangeInternal) {
        handleDpiChange();
    }
    return QWidget::event(event);
}

void StaticDialog::goToCenter() {
    QWidget* parent = parentWidget();
    QScreen* screen = QApplication::screenAt(QCursor::pos());
    if (!screen) screen = QApplication::primaryScreen();
    if (!screen) return;

    QRect screenGeom = screen->geometry();
    QRect geom = _rc.isNull() ? geometry() : _rc;

    if (geom.isNull() || geom.width() == 0 || geom.height() == 0) {
        geom = QRect(0, 0, 400, 200);
    }

    if (parent) {
        QPoint center = parent->mapToGlobal(parent->rect().center());
        geom.moveCenter(center);
    } else {
        geom.moveCenter(screenGeom.center());
    }

    // Make sure it stays on screen
    if (geom.left() < screenGeom.left()) geom.moveLeft(screenGeom.left());
    if (geom.top() < screenGeom.top()) geom.moveTop(screenGeom.top());
    if (geom.right() > screenGeom.right()) geom.moveRight(screenGeom.right());
    if (geom.bottom() > screenGeom.bottom()) geom.moveBottom(screenGeom.bottom());

    setGeometry(geom);
    _rc = geom;
}

void StaticDialog::moveToCenter() {
    goToCenter();
}

bool StaticDialog::handleDpiChange() {
    // Get current DPI and compare to saved
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) return false;

    int newDpi = screen->logicalDotsPerInch();
    QRect geom = geometry();

    // Scale geometry proportionally
    // This is a simplified approach — full implementation would scale individual controls
    setGeometry(geom);
    updateGeometryForDpi(newDpi);
    return true;
}

void StaticDialog::updateGeometryForDpi(int newDpi) {
    Q_UNUSED(newDpi);
    // Full implementation would recalculate control positions and sizes
    // based on stored base DPI values
}

QRect StaticDialog::childRect(int childId) const {
    QWidget* child = findChild<QWidget*>(QString::number(childId));
    if (!child) return QRect();
    return QRect(child->mapTo(this, QPoint(0, 0)), child->size());
}

void StaticDialog::redrawChild(int childId) {
    QWidget* child = findChild<QWidget*>(QString::number(childId));
    if (child) child->update();
}

bool StaticDialog::isChecked(int checkControlID) const {
    QCheckBox* cb = findChild<QCheckBox*>(QString::number(checkControlID));
    if (cb) return cb->isChecked();
    // Try to find by object name as int
    for (auto* child : findChildren<QCheckBox*>()) {
        if (child->objectName() == QString::number(checkControlID)) {
            return child->isChecked();
        }
    }
    return false;
}

void StaticDialog::setChecked(int checkControlID, bool checked) const {
    QCheckBox* cb = findChild<QCheckBox*>(QString::number(checkControlID));
    if (cb) cb->setChecked(checked);
    for (auto* child : findChildren<QCheckBox*>()) {
        if (child->objectName() == QString::number(checkControlID)) {
            child->setChecked(checked);
        }
    }
}

void StaticDialog::applyFontSize(int fontSize) {
    Q_UNUSED(fontSize);
    // In Qt, font size is set via QFont
}

// =============================================================================
// RunDlg
// =============================================================================

const QString fullCurrentPathVar = QStringLiteral("$(FULL_CURRENT_PATH)");
const QString currentDirectoryVar = QStringLiteral("$(CURRENT_DIRECTORY)");
const QString onlyFileNameVar = QStringLiteral("$(FILE_NAME)");
const QString currentWordVar = QStringLiteral("$(CURRENT_WORD)");
const QString nppDirectoryVar = QStringLiteral("$(NPP_DIRECTORY)");
const QString currentLineVar = QStringLiteral("$(CURRENT_LINE)");
const QString currentColumnVar = QStringLiteral("$(CURRENT_COLUMN)");

RunDialog::RunDialog()
    : StaticDialog()
{
    setWindowTitle(QStringLiteral("Run"));
    setMinimumWidth(500);

    // Build UI
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    // Command input
    QHBoxLayout* cmdLayout = new QHBoxLayout();
    QLabel* cmdLabel = new QLabel(QStringLiteral("Command:"), this);
    _commandEdit = new QLineEdit(this);
    _commandEdit->setObjectName(QStringLiteral("IDC_COMBO_RUN"));
    _commandEdit->setMinimumWidth(300);
    cmdLayout->addWidget(cmdLabel);
    cmdLayout->addWidget(_commandEdit);
    mainLayout->addLayout(cmdLayout);

    // History combo
    _historyCombo = new QComboBox(this);
    _historyCombo->setObjectName(QStringLiteral("IDC_COMBO_RUNARG"));
    _historyCombo->setEditable(true);
    _historyCombo->setMinimumWidth(300);
    mainLayout->addWidget(_historyCombo);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    _runButton = new QPushButton(QStringLiteral("Run"), this);
    _runButton->setObjectName(QStringLiteral("IDOK"));
    connect(_runButton, &QPushButton::clicked, this, &RunDialog::onRun);
    btnLayout->addWidget(_runButton);

    _browseButton = new QPushButton(QStringLiteral("Browse..."), this);
    connect(_browseButton, &QPushButton::clicked, this, &RunDialog::onBrowse);
    btnLayout->addWidget(_browseButton);

    _cancelButton = new QPushButton(QStringLiteral("Cancel"), this);
    _cancelButton->setObjectName(QStringLiteral("IDCANCEL"));
    connect(_cancelButton, &QPushButton::clicked, this, &QWidget::close);
    btnLayout->addWidget(_cancelButton);

    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);
}

void RunDialog::doDialog(bool isRTL) {
    if (isRTL) {
        setLayoutDirection(Qt::RightToLeft);
        _isRTL = true;
    }
    display(true);
}

QString RunDialog::expandVariables(const QString& input) {
    QString result = input;
    // In a full implementation, these would be substituted with actual values
    // from the main editor state
    result.replace(fullCurrentPathVar, QStringLiteral(""));
    result.replace(currentDirectoryVar, QStringLiteral(""));
    result.replace(onlyFileNameVar, QStringLiteral(""));
    result.replace(currentWordVar, QStringLiteral(""));
    result.replace(nppDirectoryVar, QStringLiteral(""));
    result.replace(currentLineVar, QStringLiteral("1"));
    result.replace(currentColumnVar, QStringLiteral("1"));
    return result;
}

void RunDialog::onRun() {
    QString command = _commandEdit->text();
    if (command.isEmpty()) return;

    command = expandVariables(command);

    // Add to history
    addToHistory(command);

    // Execute (simplified — would use QProcess in full implementation)
    // QProcess::startDetached(command);
    hide();
}

void RunDialog::onBrowse() {
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("Select File"));
    if (!filePath.isEmpty()) {
        _commandEdit->setText(filePath);
    }
}

void RunDialog::addToHistory(const QString& text) {
    if (text.isEmpty()) return;
    _history.removeAll(text);
    _history.prepend(text);
    if (_history.size() > 20) _history.removeLast();
    _historyCombo->clear();
    _historyCombo->addItems(_history);
    _historyCombo->setCurrentIndex(0);
}

void RunDialog::removeFromHistory(const QString& text) {
    _history.removeAll(text);
    _historyCombo->clear();
    _historyCombo->addItems(_history);
}

intptr_t RunDialog::run_dlgProc(unsigned int message, intptr_t wParam, intptr_t lParam) {
    switch (message) {
        case 0x1100 + 1: // WM_INITDIALOG equivalent
            // Dialog initialization
            break;
        case 0x0111: // WM_COMMAND
            switch (wParam & 0xFFFF) {
                case 1: // IDOK (Run)
                    onRun();
                    break;
                case 2: // IDCANCEL (Cancel)
                    close();
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return 0;
}


intptr_t StaticDialog::run_dlgProc(unsigned int, intptr_t, intptr_t) {
    return 0;
}
