/****************************************************************************
** Meta object code from reading C++ file 'mpv_windows_player.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../window_mpv/mpv_windows_player.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mpv_windows_player.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_mpv_windows_player_t {
    QByteArrayData data[17];
    char stringdata0[177];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mpv_windows_player_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mpv_windows_player_t qt_meta_stringdata_mpv_windows_player = {
    {
QT_MOC_LITERAL(0, 0, 18), // "mpv_windows_player"
QT_MOC_LITERAL(1, 19, 10), // "mpv_events"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 13), // "changeCurrent"
QT_MOC_LITERAL(4, 45, 14), // "changeDuration"
QT_MOC_LITERAL(5, 60, 16), // "create_mvpPlayer"
QT_MOC_LITERAL(6, 77, 3), // "WId"
QT_MOC_LITERAL(7, 81, 3), // "wid"
QT_MOC_LITERAL(8, 85, 16), // "handle_mpv_event"
QT_MOC_LITERAL(9, 102, 10), // "mpv_event*"
QT_MOC_LITERAL(10, 113, 5), // "event"
QT_MOC_LITERAL(11, 119, 15), // "command_variant"
QT_MOC_LITERAL(12, 135, 4), // "args"
QT_MOC_LITERAL(13, 140, 11), // "setProperty"
QT_MOC_LITERAL(14, 152, 4), // "name"
QT_MOC_LITERAL(15, 157, 5), // "value"
QT_MOC_LITERAL(16, 163, 13) // "on_mpv_events"

    },
    "mpv_windows_player\0mpv_events\0\0"
    "changeCurrent\0changeDuration\0"
    "create_mvpPlayer\0WId\0wid\0handle_mpv_event\0"
    "mpv_event*\0event\0command_variant\0args\0"
    "setProperty\0name\0value\0on_mpv_events"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mpv_windows_player[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    1,   55,    2, 0x06 /* Public */,
       4,    1,   58,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   61,    2, 0x08 /* Private */,
       8,    1,   64,    2, 0x08 /* Private */,
      11,    1,   67,    2, 0x08 /* Private */,
      13,    2,   70,    2, 0x08 /* Private */,
      16,    0,   75,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, QMetaType::QVariant,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   14,   15,
    QMetaType::Void,

       0        // eod
};

void mpv_windows_player::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        mpv_windows_player *_t = static_cast<mpv_windows_player *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->mpv_events(); break;
        case 1: _t->changeCurrent((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->changeDuration((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->create_mvpPlayer((*reinterpret_cast< WId(*)>(_a[1]))); break;
        case 4: _t->handle_mpv_event((*reinterpret_cast< mpv_event*(*)>(_a[1]))); break;
        case 5: _t->command_variant((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 6: _t->setProperty((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: _t->on_mpv_events(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (mpv_windows_player::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mpv_windows_player::mpv_events)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (mpv_windows_player::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mpv_windows_player::changeCurrent)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (mpv_windows_player::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&mpv_windows_player::changeDuration)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject mpv_windows_player::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_mpv_windows_player.data,
      qt_meta_data_mpv_windows_player,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *mpv_windows_player::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mpv_windows_player::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_mpv_windows_player.stringdata0))
        return static_cast<void*>(const_cast< mpv_windows_player*>(this));
    return QObject::qt_metacast(_clname);
}

int mpv_windows_player::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void mpv_windows_player::mpv_events()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void mpv_windows_player::changeCurrent(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void mpv_windows_player::changeDuration(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
