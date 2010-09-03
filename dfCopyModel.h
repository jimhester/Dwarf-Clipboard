#pragma once

//
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QLabel>
#include "dfCopyObj.h"

class dfCopyModel : public QAbstractTableModel
{
     Q_OBJECT

public:
    dfCopyModel(DFHack::Context * tDF,QList<dfCopyObj> *nlist, QObject *parent = 0)
         : DF(tDF),QAbstractTableModel(parent),list(nlist){ size = QSize(64,64);};

     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;
     QVariant data(const QModelIndex &index, int role) const;
     QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;
     void setIconSize(const QSize &s){ size = s; };
     bool setData(const QModelIndex &index,const QVariant &value, int role);
     bool prependData(const dfCopyObj &data);
     Qt::ItemFlags flags(const QModelIndex &index) const;
     QStringList mimeTypes() const;
     QMimeData *mimeData(const QModelIndexList &indexes) const;
     bool dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent);
     bool insertDataAtPoint(const dfCopyObj &data,int row);
     bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() ) ;
     void mouseDoubleClickEvent ( QMouseEvent * event )  ;

 private:
     QList<dfCopyObj> *list;
     QSize size;
     DFHack::Context * DF;
};