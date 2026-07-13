// Semantic Lift: Win32 StatusBar → Qt6 QStatusBar Implementation
// Original: PowerEditor/src/WinControls/StatusBar/StatusBar.cpp
// Target: npp-qt/src/WinControls/StatusBar.cpp

#include "StatusBar.h"
#include <QLabel>
#include <QMouseEvent>
#include <QTimer>
#include <QStyle>

StatusBar::StatusBar(QWidget* parent) : QStatusBar(parent)
{
    _pFlashTimer = new QTimer(this);
    _pFlashTimer->setSingleShot(true);
    connect(_pFlashTimer, &QTimer::timeout, this, &StatusBar::onFlashTimeout);

    QLabel* readyLabel = new QLabel(tr("Ready"), this);
    readyLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    addPermanentWidget(readyLabel);

    for (int i = 1; i < 8; ++i) {
        QLabel* label = new QLabel(this);
        label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        label->setMinimumWidth(50);
        addPermanentWidget(label);
    }
}

StatusBar::~StatusBar() = default;

void StatusBar::init(QApplication* app, QWidget* parent)
{
    Q_UNUSED(app);
    Q_UNUSED(parent);
}

void StatusBar::destroy() { _parts.clear(); }

void StatusBar::setText(const QString& text, int partIndex)
{
    if (partIndex == 0) {
        showMessage(text);
    } else {
        QLabel* label = getOrCreatePart(partIndex);
        if (label) label->setText(text);
    }
}

QString StatusBar::text(int partIndex) const
{
    if (partIndex == 0) return currentMessage();
    auto it = _parts.find(partIndex);
    if (it != _parts.end() && it.value().label) return it.value().label->text();
    return QString();
}

void StatusBar::setInt(int value, int partIndex)
{
    setText(QString::number(value), partIndex);
}

void StatusBar::flash(const QString& message, int durationMs)
{
    showMessage(message, durationMs);
    _lastText = message;
    if (durationMs > 0) _pFlashTimer->start(durationMs);
}

void StatusBar::showAnimate(bool show)
{
    if (show) {
        if (!_pAnimateLabel) {
            _pAnimateLabel = new QLabel("●", this);
            _pAnimateLabel->setStyleSheet("color: green;");
            addPermanentWidget(_pAnimateLabel);
        }
        _pAnimateLabel->show();
    } else {
        hideAnimate();
    }
}

void StatusBar::hideAnimate()
{
    if (_pAnimateLabel) _pAnimateLabel->hide();
}

void StatusBar::setEncoding(const QString& encoding)
{
    _lastEncoding = encoding;
    setText(encoding, static_cast<int>(StatusBarPart::Encoding));
}

void StatusBar::setEol(const QString& eol)
{
    _lastEol = eol;
    setText(eol, static_cast<int>(StatusBarPart::EolFormat));
}

void StatusBar::setLanguage(const QString& language)
{
    _lastLanguage = language;
    setText(language, static_cast<int>(StatusBarPart::Language));
}

void StatusBar::setFileName(const QString& fileName)
{
    setToolTip(fileName);
}

void StatusBar::setDocType(const QString& docType)
{
    setText(docType, static_cast<int>(StatusBarPart::FileSize));
}

void StatusBar::display(int which, const QString& str, bool isImportant)
{
    if (which == 0) {
        // Part 0: main status area
        if (str.isEmpty())
            clearMessage();
        else if (isImportant)
            flash(str, 0);  // 0 = indefinite (clear explicitly)
        else
            showMessage(str);
    } else {
        setText(str, which);
    }
}

void StatusBar::setPartWidth(int partIndex, int width)
{
    getOrCreatePart(partIndex);
    QLabel* part = getOrCreatePart(partIndex);
    if (part) part->setMinimumWidth(width);
}

void StatusBar::setProgress(int percent)
{
    if (percent < 0) clearProgress();
    else setText(QString(" %1% ").arg(percent), static_cast<int>(StatusBarPart::PollInterval));
}

void StatusBar::clearProgress()
{
    setText("", static_cast<int>(StatusBarPart::PollInterval));
}

void StatusBar::onDocModified()
{
    flash(tr("Modified"), 2000);
}

void StatusBar::updateStatus()
{
    update();
}

void StatusBar::onFlashTimeout()
{
    showMessage(_lastText);
}

QLabel* StatusBar::getOrCreatePart(int partIndex)
{
    auto it = _parts.find(partIndex);
    if (it != _parts.end() && it.value().label) return it.value().label;

    QLabel* label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    label->setMinimumWidth(50);
    label->installEventFilter(this);

    PartInfo info;
    info.label = label;
    _parts.insert(partIndex, info);

    addPermanentWidget(label);
    return label;
}



