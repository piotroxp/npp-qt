// UserDefineDialog.h - Qt port
#pragma once
#include <QDialog>
#include <QTreeWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include "ScintillaEditView.h"
#include "lexerG mane\NppStyle.h"
class UserDefineDialog : public QDialog {
    Q_OBJECT
public:
    UserDefineDialog() : QDialog() {}
    void init(HINSTANCE hInst, HWND hParent, ScintillaEditView** pEditView) { Q_UNUSED(hInst); Q_UNUSED(hParent); _pEditView = pEditView; }
    void initWithoutScintilla();
    void createDialog(bool isRTL = true);
    bool isDialogStarted() const { return _started; }
    void destroy() { hide(); closed(); }
    void setScintilla(ScintillaEditView* pScintilla);
    void doDialog(bool toShow, bool isRTL);
    void styleChanged();
    void styleUpdated(const wchar_t* styleName);
private:
    int _currentIndex = 0;
    int _currentTheme = 0;
    bool _started = false;
    enum { WORDList1 = 0, WORDList2, WORDList3, WORDList4, WORDList5, WORDList6, WORDList7, WORDList8 };
};
HeaderEOF
echo "UserDefineDialog created"

cat > /workspace/piotro/npp-qt/src/ScintillaComponent/xmlMatchedTagsHighlighter.h << 'HEADEREOF'
// xmlMatchedTagsHighlighter.h - Qt port (already cross-platform)
#pragma once
#include <vector>
#include <string>
#include <regex>
class xmlMatchedTagsHighlighter {
public:
    xmlMatchedTagsHighlighter() : _tagHighlighted(-1), _currentTagStart(-1), _currentTagEnd(-1) {}
    void init(ScintillaEditView* pEditView) { _pEditView = pEditView; }
    void highlight(intPtr_t currentPos, bool isInsertion);
    void clear();
    void removeHighlightTag(intPtr_t startPos, intPtr_t endPos);
    void removeCanceledTag(intPtr_t startPos, intPtr_t endPos);
private:
    static const wchar_t* const TAGRX;
    bool getTagRange(std::wsmatch& match, intPtr_t startPos);
    void highlightClosingTag(std::wsmatch& match);
    void highlightOpenTag(std::wsmatch& match);
    void highlightSelfClosingTag(std::wsmatch& match);
    void highlightOpenTagOnly(std::wsmatch& match);
    void _highlightClosingTagOnly(std::wsmatch& match);
    ScintillaEditView* _pEditView = nullptr;
    intPtr_t _tagHighlighted = 0;
    intPtr_t _currentTagStart = 0;
    intPtr_t _currentTagEnd = 0;
    std::vector<intPtr_t> _matchedStarts;
    std::vector<intPtr_t> _matchedEnds;
};
HeaderEOF
echo "xmlMatchedTagsHighlighter created"