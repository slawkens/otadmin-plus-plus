TARGET = OTAdmin
TEMPLATE = app

#include(qtsingleapplication/src/qtsingleapplication.pri)

SOURCES += main.cpp \
    mainwindow.cpp \
    rsa.cpp \
    networkmessage.cpp \
    client.cpp \
    settings.cpp \
    clientstab.cpp \
    clientgui.cpp \
    commandexecutor.cpp \
    codeeditor.cpp \
    aboutwindow.cpp \
    favorites_window.cpp \
    highlighter.cpp \
    players_window.cpp \
    splashscreen.cpp
HEADERS += mainwindow.h \
    networkmessage.h \
    definitions.h \
    rsa.h \
    client.h \
    clientstab.h \
    statusreader.h \
    clientgui.h \
    commandexecutor.h \
    codeeditor.h \
    aboutwindow.h \
    favorites_window.h \
    highlighter.h \
    players_window.h \
    splashscreen.h

RC_FILE = resources.rc
RESOURCES += OTAdmin.qrc
QT += xml network widgets core

#LIBS += -lgmp
win32:LIBS += -lwsock32

win32:CONFIG(release, debug|release): LIBS += -lgmp
else:win32:CONFIG(debug, debug|release): LIBS += -lgmp

#win32:INCLUDEPATH += C:/MinGW/include
#win32:DEPENDPATH += C:/MinGW/include
