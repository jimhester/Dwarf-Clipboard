SOURCES += dfCopyPaste.cpp dfCopyObj.cpp dfCopyPastePng.cpp dfLocationIterator.cpp main.cpp

HEADERS += dfCopyPaste.h dfCopyObj.h dfCopyPastePng.h dfLocationIterator.h

FORMS += dfCopyPaste.ui
RESOURCES += dfCopyPaste.qrc

CONFIG += qt
CONFIG  += qxt
QXT += core gui globalshortcut

CONFIG += debug_and_release

