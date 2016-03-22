QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NoteKeeper
TEMPLATE = app


SOURCES += \
    main.cpp \
    jsonmenu.cpp

HEADERS += \
    jsonmenu.h

RESOURCES += \
    resources.qrc

DISTFILES += \
    menu.json

