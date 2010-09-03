SOURCES += dfCopyModel.cpp dfCopyObj.cpp dfCopyPaste.cpp dfCopyPastePng.cpp dfLocationIterator.cpp main.cpp dfCopyPasteView.cpp

HEADERS += dfCopyModel.h dfCopyObj.h dfCopyPaste.h dfCopyPastePng.h dfLocationIterator.h common.h dfCopyPasteView.h

RESOURCES += dfCopyPaste.qrc

FORMS += dfCopyPaste.ui

CONFIG += qt
CONFIG  += qxt
LIBS += dfhack-debug.lib
QXT += core gui globalshortcut

CONFIG += debug_and_release

