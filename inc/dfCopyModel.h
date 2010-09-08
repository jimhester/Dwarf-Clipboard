#pragma once

//
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QLabel>
#include "dfCopyObj.h"

class dfCopyModel : public QAbstractItemModel
{
     Q_OBJECT

public:
    dfCopyModel(DFHack::Context * tDF, QObject *parent = 0);
	dfCopyModel::~dfCopyModel();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
					 int role = Qt::DisplayRole) const;
	//void setIconSize(const QSize &s){ size = s; };
	bool setData(const QModelIndex &index,const QVariant &value, int role);
	bool prependData(dfCopyObj *data,dfCopyObj *parent = 0);
    bool appendData(dfCopyObj *data, dfCopyObj *parent =0);
    dfCopyObj* getRoot() const { return rootItem; };
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data,
	Qt::DropAction action, int row, int column, const QModelIndex &parent);
	bool insertDataAtPoint(dfCopyObj *data,int row,dfCopyObj *parent = 0);
	bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() ) ;
	void mouseDoubleClickEvent ( QMouseEvent * event )  ;
	QModelIndex index(int row, int column,
				   const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

 private:
	 dfCopyObj *rootItem;
  //   QSize size;
     DFHack::Context * DF;
};