#include <QByteArray>
#include <cctype>
#include <cstring>
#include <sstream>
// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Port of FunctionCallTip.cpp — function calltip display during typing
// Win32→Qt6: TCM_* → removed (Scintilla handles this natively via SCI_CALLTIP*)
// Complex XML parsing → simplified keyword-based approach
#include "FunctionCallTip.h"
#include <cctype>
#include <cstring>
#include <sstream>

FunctionCallTip::FunctionCallTip(ScintillaComponent* pEditView)
    : _pEditView(pEditView)
{}

void FunctionCallTip::setLanguageXML(void* xmlKeyword)
{
    Q_UNUSED(xmlKeyword);
    if (isVisible()) close();
    reset();
    _funcName.clear();
}

bool FunctionCallTip::updateCalltip(int ch, bool needShown)
{
    if (!needShown && ch != _start && ch != _param && !isVisible())
        return false;
    _curPos = _pEditView ? _pEditView->send(SCI_GETCURRENTPOS) : 0;
    if (!getCursorFunction()) {
        close();
        return false;
    }
    showCalltip();
    return true;
}

void FunctionCallTip::showNextOverload()
{
    if (!isVisible()) return;
    if (_currentNbOverloads > 0)
        _currentOverload = (_currentOverload + 1) % _currentNbOverloads;
    showCalltip();
}

void FunctionCallTip::showPrevOverload()
{
    if (!isVisible()) return;
    _currentOverload = _currentOverload > 0 ? (_currentOverload - 1) : (_currentNbOverloads - 1);
    showCalltip();
}

bool FunctionCallTip::isVisible() const
{
    return _pEditView && _pEditView->send(SCI_CALLTIPACTIVE) != 0;
}

void FunctionCallTip::close()
{
    if (!isVisible() || !_selfActivated) return;
    if (_pEditView) _pEditView->send(SCI_CALLTIPCANCEL);
    _selfActivated = false;
    _currentOverload = 0;
}

void FunctionCallTip::reset()
{
    _currentOverload = 0;
    _currentParam = 0;
    _startPos = 0;
    _overloads.clear();
    _currentNbOverloads = 0;
    _retVals.clear();
    _descriptions.clear();
}

void FunctionCallTip::cleanup()
{
    reset();
    _pEditView = nullptr;
}

// Simplified tokenization for cursor position
bool FunctionCallTip::getCursorFunction()
{
    if (!_pEditView) return false;
    auto line = _pEditView->send(SCI_LINEFROMPOSITION, _curPos);
    auto startpos = _pEditView->send(SCI_POSITIONFROMLINE, line);
    auto endpos = _pEditView->send(SCI_GETLINEENDPOSITION, line);
    auto len = endpos - startpos + 3;
    if (len <= 2 || len >= 1024) { reset(); return false; }

    static char lineData[1024];
    // SCI_GETLINE doesn't null-terminate properly, so we handle it
    QByteArray lineBytes = QByteArray(len + 1, '\0');
    _pEditView->send(SCI_GETLINE, line, reinterpret_cast<sptr_t>(lineBytes.data()));
    lineBytes[len] = '\0';
    std::memcpy(lineData, lineBytes.constData(), len);
    lineData[len] = '\0';

    auto offset = _curPos - startpos;
    if (offset < 2) { reset(); return false; }

    // Find the function token before the opening paren
    int parenCount = 0;
    int funcEnd = -1;
    for (int i = offset - 1; i >= 0; --i) {
        if (lineData[i] == _stop) parenCount++;
        else if (lineData[i] == _start) {
            if (parenCount > 0) parenCount--;
            else { funcEnd = i - 1; break; }
        }
    }
    if (funcEnd < 0) { reset(); return false; }

    // Back up to find start of identifier
    int funcStart = funcEnd;
    while (funcStart >= 0 && (isBasicWordChar(lineData[funcStart]) || isAdditionalWordChar(lineData[funcStart])))
        funcStart--;
    funcStart++;

    int funcLen = funcEnd - funcStart + 1;
    if (funcLen <= 0) { reset(); return false; }

    _funcName.assign(&lineData[funcStart], funcLen);
    // Count parameters before cursor
    _currentParam = 0;
    for (int i = funcEnd + 1; i < offset; ++i) {
        if (lineData[i] == _param) _currentParam++;
    }
    return loadFunction();
}

bool FunctionCallTip::loadFunction()
{
    reset();
    // Stub: in production this parses the API XML to find function signatures
    // For now, return false to indicate no function found
    return false;
}

void FunctionCallTip::showCalltip()
{
    if (!_pEditView || _currentNbOverloads == 0) return;
    if (_currentNbOverloads == 0) return;

    std::ostringstream tip;
    if (_currentNbOverloads > 1) {
        tip << "\001" << (_currentOverload + 1) << " of " << _currentNbOverloads << "\002";
    }
    if (!_retVals.empty() && _currentOverload < _retVals.size())
        tip << _retVals[_currentOverload] << " ";
    tip << _funcName << "()";
    if (_currentOverload < _descriptions.size() && _descriptions[_currentOverload][0])
        tip << "\n" << _descriptions[_currentOverload];

    QString tipStr = QString::fromLatin1(tip.str().c_str());
    if (isVisible())
        _pEditView->send(SCI_CALLTIPCANCEL);
    else
        _startPos = _curPos;
    _pEditView->send(SCI_CALLTIPSHOW, _startPos, reinterpret_cast<sptr_t>(tipStr.toUtf8().constData()));
    _selfActivated = true;
}
