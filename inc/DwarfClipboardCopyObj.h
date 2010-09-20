#ifndef DwarfClipboardCopyObj_H
#define DwarfClipboardCopyObj_H

#include "common.h"
#include "dfLocationIterator.h"
#include "DwarfClipboardPng.h"
#include <QList>
#include <QString>
#include <QVariant>
#include <QImage>
#include <QColor>

class DwarfClipboardCopyObj;

class DwarfClipboardCopyObj
{
private:
    int idx;
    DFHack::Maps *Maps;
    QList <cursorIdx> pos;
    void setupVectors();
    void getDataFromDF();
    void writeImages();
    void setTextForImages();
    void checkDig(int32_t tileType, cursorIdx current, cursorIdx begin);
    void changeDesignation(DFHack::designations40d *ptr, dfLocationIterator * itr,const cursorIdx &begin);
    QVector<QVector<QVector<QString > > >dig;
    QVector<QVector<QVector<QString > > >build;
    QList<QImage> images;
    QString Name;
    QString Comment;
	QList<DwarfClipboardCopyObj*> childItems;
	DwarfClipboardCopyObj *parentItem;
    QList<QImage> imageListFromTiledImages(const QImage &image);
    int defaultIndex;
    static bool useOriginal;
    static DFHack::Context *DF;
	QList<QImage> originalImages;
    static QMap<QString,QString>buildCommands;
    cursorIdx pasteLocation;
    void moveToPoint(int x, int y,int z);
    void moveToPoint(cursorIdx location);
    static int delay;
    QList< int > getBorders(cursorIdx cur);
public:
    DwarfClipboardCopyObj(DwarfClipboardCopyObj *parent = 0);
    DwarfClipboardCopyObj(cursorIdx c1, cursorIdx c2,DwarfClipboardCopyObj *parent = 0);
    DwarfClipboardCopyObj(QImage img,DwarfClipboardCopyObj *parent = 0);
	DwarfClipboardCopyObj::~DwarfClipboardCopyObj();
	void appendChild(DwarfClipboardCopyObj *child);
	void prependChild(DwarfClipboardCopyObj *child);
	void insertChild(int position,DwarfClipboardCopyObj *child);
	void removeChildAt(int position);
	DwarfClipboardCopyObj *child(int row);
	int childCount() const;
	int row() const;
	DwarfClipboardCopyObj *parent();
	static void setOriginalState(int state);
    QVector<QVector<QVector<QString > > > getDig() const { return dig; };
    QVector<QVector<QVector<QString > > > getBuild() const { return build; } ;
    void addPos(cursorIdx pos);
    int getValid ()const;
    QString printDig (QChar sep = ' ')const;
    void clear();
    void setDefaultIndex(int newIdx) { defaultIndex = newIdx;};
    int getDefaultIndex() { return defaultIndex; };
    QImage getImage(int z = -1) const;
    QImage getTiledImages() const;
	int getImageCount() const { return images.size(); };
    QString getName()const { return Name;};
    QString getComment()const { return Comment; };
    void setName(QString n);
    void setComment(QString c);
	void setParent(DwarfClipboardCopyObj *parent){ parentItem = parent; };
    QList<cursorIdx> getRange()const { return pos; };
    void pasteDesignations(cursorIdx location);
    void pasteBuildings(cursorIdx location);
    void recalcImages();
    static void setDF(DFHack::Context *tDF);
    static void setBuildCommands(QMap<QString,QString> commands);
    static void setDelay(int newDelay);
    cursorIdx getPrevPastePoint();
    void setOrignialToCurrent();
};
#endif
