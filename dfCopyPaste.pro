SOURCES += src/*.cpp

HEADERS += inc/*.h

RESOURCES += dfCopyPaste.qrc

FORMS += dfCopyPaste.ui editWidget.ui

CONFIG += qt
CONFIG  += qxt
QXT += core gui globalshortcut
LIBS += dfhack-debug.lib

CONFIG += debug_and_release

