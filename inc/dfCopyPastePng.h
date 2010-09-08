#pragma once

#include "common.h"

#include <QList>
#include <QImage>
#include <QMap>

class dfCopyObj;

class dfCopyPastePng
{
public:
    dfCopyPastePng(DFHack::Context * DFPtr);
    QImage getTileSet(){ return *tileSet; };
    QList<QImage> getImagesForRange(QList<cursorIdx> range);
    QMap <QString,QColor> colorMap;
    QList<QImage> regenerateImages(dfCopyObj& obj);
    QVector<QVector<QVector<QString > > > digFromString(QString str);
    QString stringFrom3dVector(const QVector<QVector<QVector<QString> > > &vec);
    QVector<QVector<QVector<QString> > > ThreeDVectorFromString(QString str);
    QList<QImage> ImagesFromString(QString str);
    static int delay;
private:
    DFHack::Context *DF;
    QImage *tileSet;
    int tileWidth;
    int tileHeight;
    void setTransparency(QImage* toSet);
    void readConfig(QString FileName);
    void processLine(QString line);
    QColor getForegroundColorFromScreen(DFHack::t_screen tile);
    QColor getBackgroundColorFromScreen(DFHack::t_screen tile);
    QImage getTileFromScreen(DFHack::t_screen screen);
    void waitTillScreenPosition(cursorIdx check);
    QString stringFromScreen(DFHack::t_screen tile);
    void waitTillDigMenuClear();
    QList< int > getBorders(const QVector<QVector<QVector<QString > > > &dig,cursorIdx cur);
};