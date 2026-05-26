// ScintillaCtrls.cpp - Qt port
#include "ScintillaCtrls.h"
#include <QVBoxLayout>
HWND ScintillaCtrls::createSintilla(HWND hParent) {
    auto* sci = new ScintillaEditView();
    if (hParent) {
        QWidget* parentWidget = qobject_cast<QWidget*>(hParent);
        if (parentWidget) sci->setParent(parentWidget);
    }
    QVBoxLayout* layout = new QVBoxLayout(sci);
    layout->setContentsMargins(0, 0, 0, 0);
    sci->show();
    _scintVector.push_back(sci);
    return sci->getHSelf();
}
ScintillaEditView* ScintillaCtrls::getScintillaEditViewFrom(HWND handle2Find) {
    int idx = getIndexFrom(handle2Find);
    return (idx >= 0 && idx < static_cast<int>(_scintVector.size())) ? _scintVector[idx] : nullptr;
}
void ScintillaCtrls::destroy() {
    for (auto* sci : _scintVector) delete sci;
    _scintVector.clear();
}
int ScintillaCtrls::getIndexFrom(HWND handle2Find) {
    for (int i = 0; i < static_cast<int>(_scintVector.size()); ++i) {
        if (_scintVector[i]->getHSelf() == handle2Find) return i;
    }
    return -1;
}
