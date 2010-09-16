SOURCES += src/*.cpp

HEADERS += inc/*.h

RESOURCES += DwarfClipboard.qrc

FORMS += DwarfClipboard.ui editWidget.ui

CONFIG += qt
CONFIG  += qxt
CONFIG += debug
QXT += core gui globalshortcut

LIBS+=dfhack-debug.lib


RC_FILE = DwarfClipboard.rc