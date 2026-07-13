/****************************************************************************
** Meta object code from reading C++ file 'WordStyleDlg.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../npp-qt/src/WinControls/WordStyleDlg.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WordStyleDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12WordStyleDlgE_t {};
} // unnamed namespace

template <> constexpr inline auto WordStyleDlg::qt_create_metaobjectdata<qt_meta_tag_ZN12WordStyleDlgE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WordStyleDlg",
        "styleApplied",
        "",
        "applyCurrentSelectedThemeAndUpdateUI",
        "updateColour",
        "which",
        "updateFontStyleStatus",
        "fontStyleType",
        "whichStyle",
        "updateExtension",
        "updateFontName",
        "updateFontSize",
        "updateUserKeywords",
        "switchToTheme",
        "loadLangListFromNppParam",
        "enableFontStyle",
        "isEnable",
        "showGlobalOverrideCtrls",
        "show",
        "updateGlobalOverrideCtrls",
        "setStyleListFromLexer",
        "index",
        "setVisualFromStyleList"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'styleApplied'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'applyCurrentSelectedThemeAndUpdateUI'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateColour'
        QtMocHelpers::SlotData<void(bool)>(4, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Slot 'updateFontStyleStatus'
        QtMocHelpers::SlotData<void(fontStyleType)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'updateExtension'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateFontName'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateFontSize'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateUserKeywords'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'switchToTheme'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'loadLangListFromNppParam'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'enableFontStyle'
        QtMocHelpers::SlotData<void(bool) const>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 16 },
        }}),
        // Slot 'showGlobalOverrideCtrls'
        QtMocHelpers::SlotData<void(bool)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 18 },
        }}),
        // Slot 'updateGlobalOverrideCtrls'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'setStyleListFromLexer'
        QtMocHelpers::SlotData<void(int)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 21 },
        }}),
        // Slot 'setVisualFromStyleList'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WordStyleDlg, qt_meta_tag_ZN12WordStyleDlgE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WordStyleDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<StaticDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12WordStyleDlgE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12WordStyleDlgE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12WordStyleDlgE_t>.metaTypes,
    nullptr
} };

void WordStyleDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WordStyleDlg *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->styleApplied(); break;
        case 1: _t->applyCurrentSelectedThemeAndUpdateUI(); break;
        case 2: _t->updateColour((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->updateFontStyleStatus((*reinterpret_cast<std::add_pointer_t<fontStyleType>>(_a[1]))); break;
        case 4: _t->updateExtension(); break;
        case 5: _t->updateFontName(); break;
        case 6: _t->updateFontSize(); break;
        case 7: _t->updateUserKeywords(); break;
        case 8: _t->switchToTheme(); break;
        case 9: _t->loadLangListFromNppParam(); break;
        case 10: _t->enableFontStyle((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->showGlobalOverrideCtrls((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->updateGlobalOverrideCtrls(); break;
        case 13: _t->setStyleListFromLexer((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->setVisualFromStyleList(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WordStyleDlg::*)()>(_a, &WordStyleDlg::styleApplied, 0))
            return;
    }
}

const QMetaObject *WordStyleDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WordStyleDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12WordStyleDlgE_t>.strings))
        return static_cast<void*>(this);
    return StaticDialog::qt_metacast(_clname);
}

int WordStyleDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StaticDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void WordStyleDlg::styleApplied()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
