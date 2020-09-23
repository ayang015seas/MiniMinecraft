/****************************************************************************
** Meta object code from reading C++ file 'playerinfo.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../assignment_package/src/playerinfo.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playerinfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PlayerInfo_t {
    QByteArrayData data[8];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlayerInfo_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlayerInfo_t qt_meta_stringdata_PlayerInfo = {
    {
QT_MOC_LITERAL(0, 0, 10), // "PlayerInfo"
QT_MOC_LITERAL(1, 11, 15), // "slot_setPosText"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 15), // "slot_setVelText"
QT_MOC_LITERAL(4, 44, 15), // "slot_setAccText"
QT_MOC_LITERAL(5, 60, 16), // "slot_setLookText"
QT_MOC_LITERAL(6, 77, 17), // "slot_setChunkText"
QT_MOC_LITERAL(7, 95, 16) // "slot_setZoneText"

    },
    "PlayerInfo\0slot_setPosText\0\0slot_setVelText\0"
    "slot_setAccText\0slot_setLookText\0"
    "slot_setChunkText\0slot_setZoneText"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlayerInfo[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x0a /* Public */,
       3,    1,   47,    2, 0x0a /* Public */,
       4,    1,   50,    2, 0x0a /* Public */,
       5,    1,   53,    2, 0x0a /* Public */,
       6,    1,   56,    2, 0x0a /* Public */,
       7,    1,   59,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

       0        // eod
};

void PlayerInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlayerInfo *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slot_setPosText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->slot_setVelText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->slot_setAccText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->slot_setLookText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->slot_setChunkText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->slot_setZoneText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PlayerInfo::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_PlayerInfo.data,
    qt_meta_data_PlayerInfo,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PlayerInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlayerInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlayerInfo.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PlayerInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
