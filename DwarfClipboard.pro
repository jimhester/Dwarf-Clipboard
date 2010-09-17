SOURCES += src/*.cpp

HEADERS += inc/*.h

RESOURCES += DwarfClipboard.qrc

FORMS += DwarfClipboard.ui editWidget.ui

CONFIG += qt
CONFIG  += qxt
CONFIG += debug
QXT += core gui globalshortcut

CONFIG(debug){
     LIBS = dfhack-debug.lib
 } else {
     LIBS = dfhack.lib
 }
RC_FILE = DwarfClipboard.rc