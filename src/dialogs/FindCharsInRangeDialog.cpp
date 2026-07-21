// FindCharsInRangeDialog.cpp - Find Characters in Range dialog implementation
// Copyright (C) 2026 Agent Army
// GPL v3

#include "FindCharsInRangeDialog.h"
#include "editor/ScintillaEditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QTextEdit>
#include <QFont>
#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <QtDebug>

// =============================================================================
// FindCharsWorker - Background thread for scanning document bytes
// =============================================================================

FindCharsWorker::FindCharsWorker(ScintillaEditor* editor, int startHex, int endHex,
                                  int maxResults, QObject* parent)
    : QThread(parent)
    , _editor(editor)
    , _startHex(startHex)
    , _endHex(endHex)
    , _maxResults(maxResults)
{
}

FindCharsWorker::~FindCharsWorker() = default;

void FindCharsWorker::run() {
    if (!_editor) {
        emit finished(0);
        return;
    }

    int foundCount = 0;
    int scannedCount = 0;

    // Get document length
    const int docLength = static_cast<int>(_editor->send(SCI_GETLENGTH));

    // Process in chunks to allow cancellation checks
    const int chunkSize = 4096;
    int pos = 0;

    while (pos < docLength && foundCount < _maxResults) {
        // Check for cancellation
        if (_cancelled.loadAcquire() != 0) {
            qDebug() << "FindCharsWorker: cancelled at position" << pos;
            break;
        }

        // Process a chunk
        int endChunk = qMin(pos + chunkSize, docLength);

        for (int i = pos; i < endChunk && foundCount < _maxResults; ++i) {
            int byteValue = static_cast<int>(_editor->send(SCI_GETCHARAT, i));

            if (byteValue >= _startHex && byteValue <= _endHex) {
                ++foundCount;

                // Get line and column for this position
                int line = static_cast<int>(_editor->send(SCI_LINEFROMPOSITION, i)) + 1;
                int col = i - static_cast<int>(_editor->send(SCI_POSITIONFROMLINE, line - 1));

                emit resultReady(line, col + 1, byteValue);
            }

            ++scannedCount;

            // Emit progress every 1000 positions
            if (scannedCount % 1000 == 0) {
                emit progress(scannedCount, foundCount);
            }
        }

        pos = endChunk;

        // Yield to allow event processing and cancellation checks
        QThread::usleep(100);
    }

    emit finished(foundCount);
    qDebug() << "FindCharsWorker: finished. Scanned" << scannedCount << "bytes, found" << foundCount;
}

void FindCharsWorker::requestCancel() {
    _cancelled.storeRelease(1);
}

// =============================================================================
// FindCharsInRangeDialog
// =============================================================================

FindCharsInRangeDialog::FindCharsInRangeDialog(ScintillaEditor* editor, QWidget* parent)
    : QDialog(parent)
    , _editor(editor)
{
    setWindowTitle("Find Characters in Range");
    setModal(true);
    setMinimumWidth(600);
    setMinimumHeight(400);
    setupUi();

    // Set default range to control characters (0x00-0x1F)
    _startRange->setValue(0x00);
    _endRange->setValue(0x1F);
    _maxResults->setValue(1000);
}

FindCharsInRangeDialog::~FindCharsInRangeDialog() {
    if (_worker && _worker->isRunning()) {
        _worker->requestCancel();
        _worker->wait(1000);
    }
    delete _worker;
}

void FindCharsInRangeDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 12);
    mainLayout->setSpacing(12);

    // Title
    QLabel* titleLabel = new QLabel("Find Characters in Range", this);
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    // Range configuration group
    QGridLayout* rangeGrid = new QGridLayout();
    rangeGrid->setSpacing(8);

    // Start range
    QLabel* startLabel = new QLabel("Start (hex):", this);
    startLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _startRange = new QSpinBox(this);
    _startRange->setObjectName("startRange");
    _startRange->setMinimum(0x00);
    _startRange->setMaximum(0xFF);
    _startRange->setDisplayIntegerBase(16);
    _startRange->setPrefix("0x");
    _startRange->setAlignment(Qt::AlignCenter);
    _startRange->setFixedWidth(80);

    // End range
    QLabel* endLabel = new QLabel("End (hex):", this);
    endLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _endRange = new QSpinBox(this);
    _endRange->setObjectName("endRange");
    _endRange->setMinimum(0x00);
    _endRange->setMaximum(0xFF);
    _endRange->setDisplayIntegerBase(16);
    _endRange->setPrefix("0x");
    _endRange->setAlignment(Qt::AlignCenter);
    _endRange->setFixedWidth(80);

    // Max results
    QLabel* maxLabel = new QLabel("Max results:", this);
    maxLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _maxResults = new QSpinBox(this);
    _maxResults->setObjectName("maxResults");
    _maxResults->setMinimum(1);
    _maxResults->setMaximum(100000);
    _maxResults->setValue(1000);
    _maxResults->setAlignment(Qt::AlignCenter);
    _maxResults->setFixedWidth(100);

    rangeGrid->addWidget(startLabel, 0, 0);
    rangeGrid->addWidget(_startRange, 0, 1);
    rangeGrid->addWidget(endLabel, 0, 2);
    rangeGrid->addWidget(_endRange, 0, 3);
    rangeGrid->addWidget(maxLabel, 1, 0);
    rangeGrid->addWidget(_maxResults, 1, 1);
    rangeGrid->addWidget(new QLabel(this), 1, 2, 1, 2);  // Spacer

    mainLayout->addLayout(rangeGrid);

    // Status label
    _statusLabel = new QLabel("Ready", this);
    _statusLabel->setObjectName("statusLabel");
    mainLayout->addWidget(_statusLabel);

    // Results area
    QLabel* resultsLabel = new QLabel("Results:", this);
    mainLayout->addWidget(resultsLabel);

    _results = new QTextEdit(this);
    _results->setObjectName("results");
    _results->setReadOnly(true);
    _results->setFont(QFont("monospace"));
    _results->setTextInteractionFlags(Qt::TextSelectableByMouse);
    mainLayout->addWidget(_results, 1);

    // Count label
    _countLabel = new QLabel("Found: 0 characters", this);
    _countLabel->setObjectName("countLabel");
    mainLayout->addWidget(_countLabel);

    // Button row
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    _findBtn = new QPushButton("Find", this);
    _findBtn->setObjectName("findBtn");
    _findBtn->setDefault(true);

    _cancelBtn = new QPushButton("Cancel", this);
    _cancelBtn->setObjectName("cancelBtn");
    _cancelBtn->setEnabled(false);

    _closeBtn = new QPushButton("Close", this);
    _closeBtn->setObjectName("closeBtn");

    btnLayout->addWidget(_findBtn);
    btnLayout->addWidget(_cancelBtn);
    btnLayout->addWidget(_closeBtn);

    mainLayout->addLayout(btnLayout);

    // Connect signals
    connect(_findBtn, &QPushButton::clicked, this, &FindCharsInRangeDialog::onFindClicked);
    connect(_cancelBtn, &QPushButton::clicked, this, &FindCharsInRangeDialog::onCancelClicked);
    connect(_closeBtn, &QPushButton::clicked, this, &FindCharsInRangeDialog::onCloseClicked);

    // Allow clicking on results to navigate
    _results->setMouseTracking(true);
    _results->installEventFilter(this);

    applyStyle();
}

void FindCharsInRangeDialog::applyStyle() {
    setStyleSheet(R"(
        QDialog {
            background-color: #2b2b2b;
            border-radius: 6px;
        }
        #titleLabel {
            color: #ffffff;
            font-size: 16px;
            font-weight: bold;
        }
        QLabel {
            color: #cccccc;
            font-size: 12px;
        }
        QSpinBox {
            background-color: #404040;
            color: #e0e0e0;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 4px 8px;
            font-size: 13px;
        }
        QSpinBox:focus {
            border: 1px solid #007acc;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #505050;
            border-radius: 2px;
            width: 16px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background-color: #606060;
        }
        #results {
            background-color: #1e1e1e;
            color: #d4d4d4;
            border: 1px solid #3c3c3c;
            border-radius: 4px;
            padding: 8px;
            font-size: 12px;
            selection-background-color: #264f78;
        }
        #statusLabel {
            color: #888888;
            font-size: 11px;
            padding: 4px;
        }
        #countLabel {
            color: #00aa00;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton {
            background-color: #404040;
            color: #e0e0e0;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 6px 16px;
            font-size: 12px;
            min-width: 70px;
        }
        QPushButton:hover {
            background-color: #505050;
            border-color: #666666;
        }
        QPushButton:pressed {
            background-color: #353535;
        }
        #findBtn {
            background-color: #007acc;
            border-color: #007acc;
            font-weight: bold;
        }
        #findBtn:hover {
            background-color: #0098dd;
            border-color: #0098dd;
        }
        #findBtn:pressed {
            background-color: #006699;
        }
        #findBtn:disabled {
            background-color: #333333;
            color: #666666;
            border-color: #444444;
        }
        #cancelBtn {
            background-color: #cc5c00;
            border-color: #cc5c00;
        }
        #cancelBtn:hover {
            background-color: #dd6d00;
            border-color: #dd6d00;
        }
        #cancelBtn:pressed {
            background-color: #aa4a00;
        }
        #cancelBtn:disabled {
            background-color: #333333;
            color: #666666;
            border-color: #444444;
        }
        #closeBtn {
            background-color: #353535;
        }
        #closeBtn:hover {
            background-color: #454545;
        }
    )");
}

void FindCharsInRangeDialog::setControlsEnabled(bool enabled) {
    _startRange->setEnabled(enabled);
    _endRange->setEnabled(enabled);
    _maxResults->setEnabled(enabled);
    _findBtn->setEnabled(enabled);
    _cancelBtn->setEnabled(!enabled);
}

void FindCharsInRangeDialog::updateCountLabel() {
    _countLabel->setText(QString("Found: %1 character%2")
        .arg(_allResults.size())
        .arg(_allResults.size() == 1 ? "" : "s"));
}

QString FindCharsInRangeDialog::formatCharacter(int byteValue) {
    // Format a single character for display
    if (byteValue == 0x00) return "<NUL>";
    if (byteValue == 0x01) return "<SOH>";
    if (byteValue == 0x02) return "<STX>";
    if (byteValue == 0x03) return "<ETX>";
    if (byteValue == 0x04) return "<EOT>";
    if (byteValue == 0x05) return "<ENQ>";
    if (byteValue == 0x06) return "<ACK>";
    if (byteValue == 0x07) return "<BEL>";
    if (byteValue == 0x08) return "<BS>";
    if (byteValue == 0x09) return "<TAB>";
    if (byteValue == 0x0A) return "<LF>";
    if (byteValue == 0x0B) return "<VT>";
    if (byteValue == 0x0C) return "<FF>";
    if (byteValue == 0x0D) return "<CR>";
    if (byteValue == 0x0E) return "<SO>";
    if (byteValue == 0x0F) return "<SI>";
    if (byteValue == 0x10) return "<DLE>";
    if (byteValue == 0x11) return "<DC1>";
    if (byteValue == 0x12) return "<DC2>";
    if (byteValue == 0x13) return "<DC3>";
    if (byteValue == 0x14) return "<DC4>";
    if (byteValue == 0x15) return "<NAK>";
    if (byteValue == 0x16) return "<SYN>";
    if (byteValue == 0x17) return "<ETB>";
    if (byteValue == 0x18) return "<CAN>";
    if (byteValue == 0x19) return "<EM>";
    if (byteValue == 0x1A) return "<SUB>";
    if (byteValue == 0x1B) return "<ESC>";
    if (byteValue == 0x1C) return "<FS>";
    if (byteValue == 0x1D) return "<GS>";
    if (byteValue == 0x1E) return "<RS>";
    if (byteValue == 0x1F) return "<US>";
    if (byteValue == 0x7F) return "<DEL>";

    // Printable ASCII
    if (byteValue >= 0x20 && byteValue <= 0x7E) {
        QChar c(byteValue);
        return QString("'%1'").arg(c);
    }

    // Other bytes
    return QString("0x%1").arg(byteValue, 2, 16, QChar('0')).toUpper();
}

void FindCharsInRangeDialog::onFindClicked() {
    if (!_editor) {
        _statusLabel->setText("Error: No editor");
        return;
    }

    // Validate range
    int startHex = _startRange->value();
    int endHex = _endRange->value();

    if (startHex > endHex) {
        _statusLabel->setText("Error: Start must be <= End");
        return;
    }

    int maxResults = _maxResults->value();

    // Clear previous results
    _allResults.clear();
    _results->clear();
    _totalScanned = 0;

    // Update UI
    setControlsEnabled(false);
    _statusLabel->setText(QString("Scanning... (0x%1 to 0x%2)")
        .arg(startHex, 2, 16, QChar('0')).arg(endHex, 2, 16, QChar('0')).toUpper());

    // Create and start worker
    _worker = new FindCharsWorker(_editor, startHex, endHex, maxResults, this);

    connect(_worker, &FindCharsWorker::resultReady,
            this, &FindCharsInRangeDialog::onWorkerResult,
            Qt::QueuedConnection);
    connect(_worker, &FindCharsWorker::progress,
            this, &FindCharsInRangeDialog::onWorkerProgress,
            Qt::QueuedConnection);
    connect(_worker, &FindCharsWorker::finished,
            this, &FindCharsInRangeDialog::onWorkerFinished,
            Qt::QueuedConnection);

    _worker->start();
}

void FindCharsInRangeDialog::onWorkerResult(int line, int col, int byteValue) {
    // Store result
    QString resultLine = QString("L%1:%2  %3  %4")
        .arg(line, 6)
        .arg(col, 4)
        .arg(QString("0x%1").arg(byteValue, 2, 16, QChar('0')).toUpper(), 6)
        .arg(formatCharacter(byteValue));

    _allResults.append(qMakePair(line, resultLine));

    // Append to results display
    _results->append(resultLine);

    // Scroll to bottom
    QScrollBar* scrollBar = _results->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void FindCharsInRangeDialog::onWorkerProgress(int scanned, int found) {
    _totalScanned = scanned;
    _statusLabel->setText(QString("Scanning... %1 bytes, %2 found")
        .arg(scanned)
        .arg(found));
    updateCountLabel();
}

void FindCharsInRangeDialog::onWorkerFinished(int totalFound) {
    setControlsEnabled(true);
    _statusLabel->setText(QString("Complete. Scanned %1 bytes, found %2 characters")
        .arg(_totalScanned)
        .arg(totalFound));
    updateCountLabel();

    // Cleanup worker
    if (_worker) {
        _worker->deleteLater();
        _worker = nullptr;
    }

    // Emit signal
    emit charactersFound(_allResults);
}

void FindCharsInRangeDialog::onCancelClicked() {
    if (_worker && _worker->isRunning()) {
        _worker->requestCancel();
        _statusLabel->setText("Cancelling...");
    }
}

void FindCharsInRangeDialog::onCloseClicked() {
    // Cancel any running search
    if (_worker && _worker->isRunning()) {
        _worker->requestCancel();
        _worker->wait(1000);
    }

    accept();
}

void FindCharsInRangeDialog::onResultClicked() {
    // Double-click on a result row — for now, no-op.
    // Intended to jump to the selected position in the editor.
}

void FindCharsInRangeDialog::populateResults(const QVector<QPair<int, QString>>& results) {
    _results->clear();

    for (const auto& result : results) {
        _results->append(result.second);
    }

    updateCountLabel();
}
