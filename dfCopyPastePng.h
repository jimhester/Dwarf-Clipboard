#pragma once

#include "common.h"

#include <QVector>
#include <QImage>
#include <QMap>

class dfCopyObj;

class dfCopyPastePng
{
public:
    dfCopyPastePng(DFHack::Context * DFPtr);
    QImage getTileSet(){ return *tileSet; };
    QVector<QImage> getPicture(QVector<cursorIdx> range);
    QMap <QString,QColor> colorMap;
private:
    DFHack::Context *DF;
    QImage *tileSet;
    int tileWidth;
    int tileHeight;
    void setTransparency(QImage* toSet);
    void dfCopyPastePng::readConfig(QString FileName);
    void dfCopyPastePng::processLine(QString line);
    QColor getForegroundColorFromScreen(DFHack::t_screen tile);
    QColor getBackgroundColorFromScreen(DFHack::t_screen tile);
    QImage getTileFromScreen(DFHack::t_screen screen);
    void waitTillScreenPosition(cursorIdx check);
    QString dfCopyPastePng::stringFromScreen(DFHack::t_screen tile);
    void dfCopyPastePng::waitTillDigMenuClear();
};
