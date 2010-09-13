#pragma once

//
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QLabel>
#include "dfCopyObj.h"

class dfCopyModel : public QAbstractItemModel{
     Q_OBJECT

public:
    dfCopyModel(DFHack::Context * tDF=0, QObject *parent = 0);
	dfCopyModel::~dfCopyModel();
    void clear();
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
    bool dfCopyModel::insertDataAtPoint(dfCopyObj *data,int row, const QModelIndex &parent = QModelIndex());
	bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() ) ;
	void mouseDoubleClickEvent ( QMouseEvent * event )  ;
	QModelIndex index(int row, int column,
				   const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
    void setDF(DFHack::Context * tDF){ DF = tDF; };

 private:
	 dfCopyObj *rootItem;
  //   QSize size;
     DFHack::Context * DF;
};