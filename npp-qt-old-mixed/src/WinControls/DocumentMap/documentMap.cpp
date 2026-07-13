// Semantic Lift: DocumentMap — Win32 Scintilla margin + custom draw → Qt6 mini-text view
// Original: PowerEditor/src/WinControls/DocumentMap/documentMap.cpp (626 lines)
// Win32 → Qt6: ScintillaMargin + WM_PAINT → QScrollArea + QLabel mini-view

#include "documentMap.h"
#include "ScintillaEdit.h"
#include <QScrollArea>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QTimer>

DocumentMap::DocumentMap(QWidget* parent)
    : QWidget(parent)
    , _pScintilla(nullptr)
    , _miniViewLabel(new QLabel(this))
    , _syncTimer(new QTimer(this))
{
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    _miniViewLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    _miniViewLabel->setScaledContents(false);

    connect(_syncTimer, &QTimer::timeout, this, &DocumentMap::syncScrollBar);
    _syncTimer->setInterval(50);
}

DocumentMap::~DocumentMap()
{
}

void DocumentMap::init(ScintillaEdit* pScintilla, QWidget* view, int nbChar)
{
    _pScintilla = pScintilla;
    _pView = view;
    _nbChar = nbChar;

    updateMiniView();
    _syncTimer->start();
}

void DocumentMap::setScintilla(ScintillaEdit* pScintilla)
{
    _pScintilla = pScintilla;
    if (_pScintilla)
        connect(_pScintilla, &ScintillaEdit::updateRequest, this, &DocumentMap::onScintillaUpdate);
    updateMiniView();
}

void DocumentMap::setCurrentPosition(int pos)
{
    _currentPos = pos;
    updateCurrentIndicator();
}

void DocumentMap::drawDocumentMap()
{
    updateMiniView();
}

void DocumentMap::updateMiniView()
{
    if (!_pScintilla)
        return;

    // Get document text (or a sampled version)
    QString text = _pScintilla->text();
    if (text.length() > 10000) {
        // Sample: take every Nth character for the mini view
        int step = text.length() / 2000;
        QString sampled;
        for (int i = 0; i < text.length(); i += step) {
            sampled += text[i];
        }
        text = sampled;
    }

    // Render mini text to a pixmap
    int viewWidth = width();
    int viewHeight = height();

    QPixmap pixmap(viewWidth, viewHeight);
    pixmap.fill(Qt::white);

    QPainter p(&pixmap);
    p.setFont(QFont(QStringLiteral("monospace"), 4));

    int lineHeight = 5;
    int x = 2;
    int y = 2;
    int lineNum = 0;
    int maxX = viewWidth - 4;

    for (QChar c : text) {
        if (c == QLatin1Char('\n')) {
            x = 2;
            y += lineHeight;
            ++lineNum;
            if (y > viewHeight)
                break;
        } else {
            p.drawText(x, y + lineHeight, QString(c));
            x += 4;
            if (x > maxX) {
                x = 2;
                y += lineHeight;
                ++lineNum;
            }
        }
    }

    _miniViewLabel->setPixmap(pixmap);
    updateCurrentIndicator();
}

void DocumentMap::updateCurrentIndicator()
{
    if (!_pScintilla)
        return;

    // Calculate position of current line in mini view
    int totalLines = _pScintilla->lineCount();
    if (totalLines == 0)
        return;

    int currentLine = _pScintilla->lineFromPosition(_currentPos);
    int visibleHeight = height() - 4;
    int lineY = (currentLine * visibleHeight) / totalLines;

    update();
}

void DocumentMap::onScintillaUpdate(int, int, void*)
{
    updateMiniView();
}

void DocumentMap::syncScrollBar()
{
    if (!_pScintilla || !_pView)
        return;

    // Sync scroll position between main view and document map
    int mainScrollValue = _pScintilla->firstVisibleLine();
    int totalLines = _pScintilla->lineCount();

    if (totalLines == 0)
        return;

    int mapScrollLine = (mainScrollValue * _miniViewLabel->height()) / height();
    _miniViewLabel->update();
}

void DocumentMap::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateMiniView();
}
