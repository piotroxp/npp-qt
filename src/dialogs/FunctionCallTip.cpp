// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Port of FunctionCallTip.cpp — function calltip display during typing
// Win32→Qt6: TCM_* → removed (Scintilla handles this natively via SCI_CALLTIP*)
#include <QByteArray>
#include <QColor>
#include <QApplication>
#include <cctype>
#include <cstring>
#include <sstream>
#include <cstdio>
#include "FunctionCallTip.h"
#include "../core/LanguageManager.h"
#include "../editor/ScintillaEditor.h"

// ─── Scintilla calltip colour constants ───────────────────────────────────────
// Scintilla uses 0-based colour indices for calltips.
// colourBG = 0, colourFG = 1, colourHL = 2 (highlight current param)
// See SCI_CALLTIPSET* messages.

FunctionCallTip::FunctionCallTip(ScintillaComponent* pEditView)
    : _pEditView(pEditView)
{
    applyStyling();
}

void FunctionCallTip::setLanguageXML(void* xmlKeyword)
{
    Q_UNUSED(xmlKeyword);
    if (isVisible()) close();
    reset();
    _funcName.clear();
    _overloadList.clear();
    _additionalWordChar.clear();

    // Load additional word characters from the active language.
    // In production this would parse the API XML keyword definitions.
    // For now we keep defaults (underscore is always valid).
}

bool FunctionCallTip::updateCalltip(int ch, bool needShown)
{
    if (!needShown && ch != _start && ch != _param && !isVisible())
        return false;

    _curPos = _pEditView ? _pEditView->send(SCI_GETCURRENTPOS) : 0;

    if (!getCursorFunction()) {
        if (isVisible()) close();
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
    _currentOverload = (_currentOverload > 0) ? (_currentOverload - 1)
                                               : (_currentNbOverloads - 1);
    showCalltip();
}

void FunctionCallTip::showForFunction(const QString& funcName,
                                      const std::vector<FunctionOverload>& overloads)
{
    if (!_pEditView || overloads.empty()) return;

    _funcName = funcName.toStdString();
    _overloadList = overloads;
    _currentOverload = 0;
    _currentParam = 0;
    _currentNbOverloads = overloads.size();

    // Build return values and descriptions vectors from overload list.
    _retVals.clear();
    _descriptions.clear();
    for (const auto& ov : overloads) {
        _retVals.push_back(ov.returnType.c_str());
        _descriptions.push_back(ov.documentation.c_str());
    }

    _selfActivated = true;
    showCalltip();
}

void FunctionCallTip::highlightParam(int paramIndex)
{
    if (!_pEditView || !isVisible()) return;

    // Scintilla: highlight a parameter by setting its colour style.
    // SCI_CALLTIPSETHLT starts highlighting at the given parameter index.
    _pEditView->send(SCI_CALLTIPSETHLT, paramIndex);
    emit paramHighlightChanged(paramIndex);
}

void FunctionCallTip::setForeground(const QColor& c)
{
    _fgColor = c;
    if (isVisible()) {
        _pEditView->send(SCI_CALLTIPSETFORE, 0, static_cast<int>(qRgb(c.red(), c.green(), c.blue())));
    }
}

void FunctionCallTip::setBackground(const QColor& c)
{
    _bgColor = c;
    if (isVisible()) {
        _pEditView->send(SCI_CALLTIPSETBACK, 0, static_cast<int>(qRgb(c.red(), c.green(), c.blue())));
    }
}

void FunctionCallTip::setHighlightColor(const QColor& c)
{
    _hlColor = c;
    if (isVisible()) {
        // Scintilla highlight uses a fixed colour index (2), set via
        // SCI_CALLTIPSETFOREHLT.
        _pEditView->send(SCI_CALLTIPSETFOREHLT, 0, static_cast<int>(qRgb(c.red(), c.green(), c.blue())));
    }
}

bool FunctionCallTip::isVisible() const
{
    return _pEditView && (_pEditView->send(SCI_CALLTIPACTIVE) != 0);
}

void FunctionCallTip::close()
{
    if (!_selfActivated) return;
    if (isVisible() && _pEditView)
        _pEditView->send(SCI_CALLTIPCANCEL);

    _selfActivated = false;
    _currentOverload = 0;
    _sticky = false;
    emit dismissed();
}

void FunctionCallTip::reset()
{
    _currentOverload = 0;
    _currentParam = 0;
    _startPos = 0;
    _overloads.clear();
    _overloadList.clear();
    _currentNbOverloads = 0;
    _retVals.clear();
    _descriptions.clear();
    _fullSignature.clear();
}

void FunctionCallTip::cleanup()
{
    close();
    reset();
    _pEditView = nullptr;
}

// ─── Styling ──────────────────────────────────────────────────────────────────

void FunctionCallTip::applyStyling()
{
    if (!_pEditView) return;

    // Set calltip appearance via Scintilla messages.
    _pEditView->send(SCI_CALLTIPSETFORE, 0, static_cast<int>(qRgb(_fgColor.red(), _fgColor.green(), _fgColor.blue())));
    _pEditView->send(SCI_CALLTIPSETBACK, 0, static_cast<int>(qRgb(_bgColor.red(),   _bgColor.green(),   _bgColor.blue())));
    _pEditView->send(SCI_CALLTIPSETFOREHLT, 0, static_cast<int>(qRgb(_hlColor.red(), _hlColor.green(), _hlColor.blue())));

    // Enable hover/click handling for calltips.
    _pEditView->send(SCI_CALLTIPSETOPTIONS, 0);
}

// ─── Cursor function detection ─────────────────────────────────────────────────

bool FunctionCallTip::getCursorFunction()
{
    if (!_pEditView) return false;

    auto line = static_cast<int>(_pEditView->send(SCI_LINEFROMPOSITION, _curPos));
    auto lineStart = static_cast<int>(_pEditView->send(SCI_POSITIONFROMLINE, line));
    auto lineEnd   = static_cast<int>(_pEditView->send(SCI_GETLINEENDPOSITION, line));
    auto len = lineEnd - lineStart + 3;

    if (len <= 2 || len >= 4096) { reset(); return false; }

    // Get the line text.
    QByteArray lineBytes(len + 1, '\0');
    _pEditView->send(SCI_GETLINE, line, reinterpret_cast<sptr_t>(lineBytes.data()));
    lineBytes.data()[len] = '\0';

    auto offset = _curPos - lineStart;
    if (offset < 2) { reset(); return false; }

    // Find the opening paren matching the function being called.
    // Scan backwards counting nesting level.
    int parenCount = 0;
    int funcEnd = -1;

    // Start scanning from the char just before cursor.
    for (int i = offset - 1; i >= 0; --i) {
        char c = lineBytes.data()[i];
        if (c == _stop) {
            ++parenCount;          // closing paren: entering deeper nest
        } else if (c == _start) {
            if (parenCount > 0) {
                --parenCount;      // closing a nested pair
            } else {
                funcEnd = i - 1;   // this is the function's opening paren
                break;
            }
        }
    }

    if (funcEnd < 0) { reset(); return false; }

    // Back up to find the start of the function name identifier.
    int funcStart = funcEnd;
    while (funcStart >= 0 &&
           (isBasicWordChar(lineBytes.data()[funcStart]) ||
            isAdditionalWordChar(lineBytes.data()[funcStart]) ||
            lineBytes.data()[funcStart] == ':' ||    // handle :: (C++ namespaces/methods)
            lineBytes.data()[funcStart] == '.'))     // handle . (C++/Python dot access)
    {
        --funcStart;
    }
    ++funcStart;

    int funcLen = funcEnd - funcStart + 1;
    if (funcLen <= 0) { reset(); return false; }

    _funcName.assign(lineBytes.data() + funcStart, static_cast<size_t>(funcLen));

    // Count commas between the function name and cursor to know which param we're on.
    _currentParam = 0;
    for (int i = funcEnd + 1; i < offset; ++i) {
        if (lineBytes.data()[i] == _param) ++_currentParam;
    }

    _startPos = lineStart + funcStart;
    return loadFunction();
}

// ─── Function lookup ──────────────────────────────────────────────────────────

bool FunctionCallTip::loadFunction()
{
    reset();
    if (_funcName.empty()) return false;

    // In production this queries the LanguageManager's API database
    // (parsed from .api / .xml files) for the function name and populates
    // _overloadList, _retVals, _descriptions.
    //
    // For now, return false so no calltip is shown — the caller is responsible
    // for populating overloads via showForFunction() or by providing a richer
    // API XML parser integration.
    return false;
}

// ─── Calltip rendering ─────────────────────────────────────────────────────────

void FunctionCallTip::showCalltip()
{
    if (!_pEditView) return;

    // If we have explicit overloads (populated via showForFunction), use those.
    // Otherwise use the legacy _retVals/_descriptions vectors.
    QString tipStr;

    if (!_overloadList.empty()) {
        const auto& ov = _overloadList[_currentOverload % _overloadList.size()];

        std::ostringstream oss;
        if (_overloadList.size() > 1) {
            // Scintilla uses \001 to start a section, \002 to end it (bold/indicator).
            oss << "\001" << (_currentOverload + 1) << " of "
                << _overloadList.size() << "\002\n";
        }

        // Return type + function name (shown in bold via \001…\002).
        if (!ov.returnType.empty()) {
            oss << "\001" << ov.returnType << " \002";
        }
        oss << _funcName << "\001(" << ov.signature << ")\002";

        // Parameter highlight: colour the current parameter.
        // Scintilla uses \003 to mark the start of a highlighted section,
        // \004 for the end.
        if (_currentParam > 0 && _currentParam <= ov.paramCount) {
            // Count parameters in signature to insert highlight markers.
            // A simplified approach: wrap the N-th parameter in \003…\004.
            int paramIdx = 0;
            std::string sig = ov.signature;
            size_t openParen = sig.find('(');
            if (openParen != std::string::npos) {
                std::string params = sig.substr(openParen + 1);
                // Find the N-th comma-separated parameter.
                int commasSeen = 0;
                size_t pStart = 0;
                for (size_t i = 0; i < params.size(); ++i) {
                    if (params[i] == ',') {
                        ++commasSeen;
                        if (commasSeen == static_cast<int>(_currentParam)) {
                            // End of target parameter
                            pStart = i + 1;
                            while (pStart < params.size() &&
                                   (params[pStart] == ' ' || params[pStart] == '\t'))
                                ++pStart;
                            size_t pEnd = pStart;
                            while (pEnd < params.size() &&
                                   params[pEnd] != ',' && params[pEnd] != ')')
                                ++pEnd;
                            // Insert highlight markers
                            std::string before = sig.substr(0, openParen + 1 + pStart);
                            std::string highlighted = sig.substr(pStart, pEnd - pStart);
                            std::string after = sig.substr(openParen + 1 + pEnd);
                            oss.str("");
                            oss.clear();
                            if (!ov.returnType.empty())
                                oss << "\001" << ov.returnType << " \002";
                            oss << _funcName << "\001(" << before << "\003"
                                << highlighted << "\004" << after << ")\002";
                            break;
                        }
                    }
                }
            }
        }

        if (!ov.documentation.empty())
            oss << "\n\001" << ov.documentation << "\002";

        tipStr = QString::fromLatin1(oss.str().c_str());
    } else {
        // Legacy path: use _retVals and _descriptions.
        if (_currentNbOverloads == 0) return;

        std::ostringstream oss;
        if (_currentNbOverloads > 1) {
            oss << "\001" << (_currentOverload + 1) << " of "
                << _currentNbOverloads << "\002\n";
        }
        if (!_retVals.empty() && _currentOverload < _retVals.size())
            oss << "\001" << _retVals[_currentOverload] << " \002";
        oss << _funcName << "()";
        if (_currentOverload < _descriptions.size() && _descriptions[_currentOverload][0])
            oss << "\n" << _descriptions[_currentOverload];
        tipStr = QString::fromLatin1(oss.str().c_str());
    }

    if (isVisible()) {
        _pEditView->send(SCI_CALLTIPCANCEL);
    } else {
        _startPos = _curPos;
    }

    // Show calltip at current position.
    // Scintilla will automatically position it above or below the cursor
    // depending on available space.
    _pEditView->send(SCI_CALLTIPSHOW, _startPos,
                     reinterpret_cast<sptr_t>(tipStr.toUtf8().constData()));
    _selfActivated = true;

    // Highlight the current parameter if any.
    if (_currentParam > 0) {
        highlightParam(static_cast<int>(_currentParam));
    }
}

void FunctionCallTip::updateParamHighlight()
{
    if (!isVisible()) return;
    highlightParam(static_cast<int>(_currentParam));
}

// paramHighlightChanged is called by highlightParam() to notify listeners
void FunctionCallTip::paramHighlightChanged(int /*paramIndex*/) {
    // Called directly from highlightParam - no-op since this was a signal stub
}

void FunctionCallTip::dismissed() {
    // Called from close() - no-op since this was a signal stub
}
