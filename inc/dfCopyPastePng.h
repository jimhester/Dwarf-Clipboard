#pragma once

#include "common.h"

#include <QList>
#include <QImage>
#include <QMap>

class dfCopyObj;

class dfCopyPastePng
{
public:
    dfCopyPastePng();
    QImage getTileSet(){ return *tileSet; };
    QList<QImage> getImagesForRange(QList<cursorIdx> range);
    QList<QImage> regenerateImages(dfCopyObj& obj);
    QVector<QVector<QVector<QString > > > digFromString(QString str);
    QString stringFrom3dVector(const QVector<QVector<QVector<QString> > > &vec);
    QVector<QVector<QVector<QString> > > ThreeDVectorFromString(QString str);
    QList<QImage> ImagesFromString(QString str);
	static void setDelay(int newDelay);
	static void setTileSetPath(QString newPath);
	static void setColorPath(QString newPath);
	static int getDelay();
	static QString getTileSetPath();
	static QString getColorPath();
    static void setDF(DFHack::Context* DFPtr);
private:
    static DFHack::Context *DF;
    void setTransparency(QImage* toSet);
    QColor getForegroundColorFromScreen(DFHack::t_screen tile);
    QColor getBackgroundColorFromScreen(DFHack::t_screen tile);
    QImage getTileFromScreen(DFHack::t_screen screen);
    void waitTillScreenPosition(cursorIdx check);
    QString stringFromScreen(DFHack::t_screen tile);
    void waitTillDigMenuClear();
    QList< int > getBorders(const QVector<QVector<QVector<QString > > > &dig,cursorIdx cur);
	static void readConfig(QString FileName);
	static void processLine(QString line);
	static QMap <QString,QColor> colorMap;
    static QImage *tileSet;
    static int tileWidth;
    static int tileHeight;
	static int delay;
	static QString tileSetPath;
	static QString colorPath;
};