/****************************************************************************
** Meta object code from reading C++ file 'QDEAdapter.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QDEAdapter.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QDEAdapter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.0. It"
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
struct qt_meta_tag_ZN10QDEAdapterE_t {};
} // unnamed namespace

template <> constexpr inline auto QDEAdapter::qt_create_metaobjectdata<qt_meta_tag_ZN10QDEAdapterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QDEAdapter",
        "oneEvolutionStepped",
        "",
        "wasReset",
        "stoppingCriteriaReached",
        "setup",
        "de::DifferentialEvolution::SetupState",
        "de::SolutionStrategy*",
        "solution",
        "setEngineParameters",
        "de::EngineParameters",
        "parameters",
        "stepEvolution",
        "reset"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'oneEvolutionStepped'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wasReset'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stoppingCriteriaReached'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setup'
        QtMocHelpers::SlotData<de::DifferentialEvolution::SetupState(de::SolutionStrategy *)>(5, 2, QMC::AccessPublic, 0x80000000 | 6, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'setEngineParameters'
        QtMocHelpers::SlotData<void(de::EngineParameters const &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Slot 'stepEvolution'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'reset'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QDEAdapter, qt_meta_tag_ZN10QDEAdapterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QDEAdapter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10QDEAdapterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10QDEAdapterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10QDEAdapterE_t>.metaTypes,
    nullptr
} };

void QDEAdapter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QDEAdapter *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->oneEvolutionStepped(); break;
        case 1: _t->wasReset(); break;
        case 2: _t->stoppingCriteriaReached(); break;
        case 3: { de::DifferentialEvolution::SetupState _r = _t->setup((*reinterpret_cast<std::add_pointer_t<de::SolutionStrategy*>>(_a[1])));
            if (_a[0]) *reinterpret_cast<de::DifferentialEvolution::SetupState*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->setEngineParameters((*reinterpret_cast<std::add_pointer_t<de::EngineParameters>>(_a[1]))); break;
        case 5: _t->stepEvolution(); break;
        case 6: _t->reset(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QDEAdapter::*)()>(_a, &QDEAdapter::oneEvolutionStepped, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QDEAdapter::*)()>(_a, &QDEAdapter::wasReset, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (QDEAdapter::*)()>(_a, &QDEAdapter::stoppingCriteriaReached, 2))
            return;
    }
}

const QMetaObject *QDEAdapter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QDEAdapter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10QDEAdapterE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QDEAdapter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QDEAdapter::oneEvolutionStepped()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void QDEAdapter::wasReset()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void QDEAdapter::stoppingCriteriaReached()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
