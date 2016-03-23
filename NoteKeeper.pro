QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = NoteKeeper
TEMPLATE = app

SOURCES += \
    $$PWD/src/main.cpp \
    $$PWD/src/jsonmenu.cpp

HEADERS += \
    $$PWD/src/jsonmenu.h

RESOURCES += \
    resources.qrc

mac {
    QMAKE_INFO_PLIST  += $$PWD/res/Info.plist
}
