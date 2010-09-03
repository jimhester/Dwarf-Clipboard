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

class dfCopyObj
{
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
public:
    dfCopyObj(void);
    dfCopyObj(DFHack::Context *tDF,cursorIdx c1, cursorIdx c2);
    dfCopyObj::dfCopyObj(QImage img,DFHack::Context *tDF);
    void setDF(DFHack::Context *tDF);
    QVector<QVector<QVector<QString > > > getDig(){ return dig; };
    QVector<QVector<QVector<QString > > > getBuild(){ return build; };
    void addPos(cursorIdx pos);
    int getValid ()const;
    QString printDig (QChar sep = ' ')const;
    void clear();
    QImage getImage(int z = 0) const { if( images.size() == 0) return QImage(); else return images.at(z); };
    QString getName()const { return Name;};
    QString getComment()const { return Comment; };
    void setName(QString n);
    void setComment(QString c);
    QList<cursorIdx> getRange()const { return pos; };
    void paste(cursorIdx location);
};
#endif
