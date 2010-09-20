SOURCES += src/*.cpp

HEADERS += inc/*.h

INCLUDEPATH += inc dfhack/library/include

RESOURCES += DwarfClipboard.qrc

FORMS += DwarfClipboard.ui editWidget.ui

CONFIG += qt
CONFIG  += qxt
CONFIG += debug
QXT += core gui globalshortcut
win32{
CONFIG(debug){
     LIBS += dfhack-debug.lib
 } else {
     LIBS += dfhack.lib
 }
}
unix{
    DEFINES+=LINUX_BUILD
    LIBS += -L./dfhack/output
    CONFIG(debug){
        LIBS += -ldfhack-debug
    }
    else{
        LIBS += -ldfhack
    }
}
RC_FILE = DwarfClipboard.rc
