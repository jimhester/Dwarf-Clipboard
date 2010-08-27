#ifndef dfCopyObj_H
#define dfCopyObj_H

#include "common.h"
#include "dfLocationIterator.h"
#include <QVector>
#include <QString>

class dfCopyObj
{
private:
    int idx;
    DFHack::Context *DF;
    DFHack::Maps *Maps;
    QVector <cursorIdx> pos;
    void setupVectors();
    void getDataFromDF();
    void checkDig(int32_t tileType, cursorIdx current, cursorIdx begin);
    void dfCopyObj::changeDesignation(DFHack::designations40d *ptr, QChar desig, cursorIdx blockIdx);
    QVector<QVector<QVector<QString > > >dig;
    QVector<QVector<QVector<QString > > >build;
public:
    dfCopyObj(void);
    dfCopyObj(DFHack::Context *tDF,cursorIdx c1, cursorIdx c2);
  //  dfCopyObj(const dfCopyObj & oldObj);
    void setDF(DFHack::Context *tDF);
    void addPos(cursorIdx pos);
    int getValid();
    QString printDig(QChar sep = ' ');
    void clear();
    QVector<cursorIdx> getRange(){ return pos; };
    void paste(cursorIdx location);
    ~dfCopyObj(void);
};
#endif
