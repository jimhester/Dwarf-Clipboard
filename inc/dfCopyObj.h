#ifndef dfCopyObj_H
#define dfCopyObj_H

#include "common.h"
#include "dfLocationIterator.h"
#include "dfCopyPastePng.h"
#include <QList>
#include <QString>
#include <QVariant>
#include <QImage>
#include <QColor>

class dfCopyObj;

class dfCopyObj{
private:
    int idx;
    DFHack::Context *DF;
    DFHack::Maps *Maps;
    QList <cursorIdx> pos;
    void setupVectors();
    void getDataFromDF();
    void writeImages();
    void setTextForImages();
    void checkDig(int32_t tileType, cursorIdx current, cursorIdx begin);
    void changeDesignation(DFHack::designations40d *ptr, QChar desig, cursorIdx blockIdx);
    QVector<QVector<QVector<QString > > >dig;
    QVector<QVector<QVector<QString > > >build;
    QList<QImage> images;
    QString Name;
    QString Comment;
	QList<dfCopyObj*> childItems;
	dfCopyObj *parentItem;
    QList<QImage> imageListFromTiledImages(const QImage &image);
    int defaultIndex;
    static bool useOriginal;
	QList<QImage> originalImages;
public:
	dfCopyObj::~dfCopyObj();
	void appendChild(dfCopyObj *child);
	void prependChild(dfCopyObj *child);
	void insertChild(int position,dfCopyObj *child);
	void removeChildAt(int position);
	dfCopyObj *child(int row);
	int childCount() const;
	int row() const;
	dfCopyObj *parent();
    dfCopyObj(DFHack::Context *tDF, dfCopyObj *parent = 0);
    dfCopyObj(DFHack::Context *tDF, cursorIdx c1, cursorIdx c2,dfCopyObj *parent = 0);
    dfCopyObj(DFHack::Context *tDF, QImage img,dfCopyObj *parent = 0);
	static void setOriginalState(int state);
    void setDF(DFHack::Context *tDF);
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
	void setParent(dfCopyObj *parent){ parentItem = parent; };
    QList<cursorIdx> getRange()const { return pos; };
    void paste(cursorIdx location);
    void recalcImages();
};
#endif
