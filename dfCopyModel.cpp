#include "dfCopyModel.h"
#include <QAbstractItemView>
#include <QAbstractListModel>
#include <QVariant>
#include <QPainter>
#include <QMimeData>
#include <QDebug>
#include <QBuffer>
#include <QUrl>

dfCopyModel::dfCopyModel(DFHack::Context * tDF,QObject *parent)
: DF(tDF),QAbstractItemModel(parent)
{
	size = QSize(64,64);
	rootItem = new dfCopyObj();
}
dfCopyModel::~dfCopyModel()
{
	delete rootItem;
}
QModelIndex dfCopyModel::index(int row, int column,const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	dfCopyObj *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<dfCopyObj*>(parent.internalPointer());

	dfCopyObj *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}
QModelIndex dfCopyModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     dfCopyObj *childItem = static_cast<dfCopyObj*>(index.internalPointer());
     dfCopyObj *parentItem = childItem->parent();

     if (parentItem == rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
 }
int dfCopyModel::rowCount(const QModelIndex &parent) const
{
 dfCopyObj *parentItem;
 if (parent.column() > 0)
     return 0;

 if (!parent.isValid())
     parentItem = rootItem;
 else
     parentItem = static_cast<dfCopyObj*>(parent.internalPointer());

 return parentItem->childCount();
}
//int dfCopyModel::rowCount(const QModelIndex &parent) const
//{
//    return list->size();
//}
int dfCopyModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}
QVariant dfCopyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
         return QVariant();

	dfCopyObj *item = static_cast<dfCopyObj*>(index.internalPointer());
    if(role == Qt::UserRole){
         return item->getImage();
     }
     if(role == Qt::UserRole+1){
         return item->getName();
     }
     if(index.column() == 0){
         if(role == Qt::DecorationRole){
             QImage temp(size,QImage::Format_ARGB32);
             QPainter paint(&temp);
             QImage blah = item->getImage().scaled(size,Qt::KeepAspectRatio);
             paint.fillRect(0,0,size.width(),size.height(),Qt::transparent);
             paint.drawImage(0,0,blah);
             paint.end();
             return temp;
         }
         return QVariant();
     }
     if (index.column() == 1 && (role == Qt::DisplayRole || role == Qt::EditRole))
            return item->getName();
     if (index.column() == 2 && (role == Qt::DisplayRole || role == Qt::EditRole))
            return item->getComment();

    return QVariant();
}
QVariant dfCopyModel::headerData(int section, Qt::Orientation orientation,int role) const
{
    if (role != Qt::DisplayRole)
         return QVariant();

    if (orientation == Qt::Horizontal){
        switch(section){
            case 1: return QString("Name");
            case 2: return QString("Comment");
            default: return QVariant();
        }
    }
    return(QVariant());
}
Qt::ItemFlags dfCopyModel::flags(const QModelIndex &index) const
 {
     Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
     if (!index.isValid())
         return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
     if(index.column() == 0){
         return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
     }
     return defaultFlags | Qt::ItemIsEditable;
 }
bool dfCopyModel::setData(const QModelIndex &index,const QVariant &value, int role)
 {
     if (index.isValid() && role == Qt::EditRole) {
		dfCopyObj *item = static_cast<dfCopyObj*>(index.internalPointer());
         if(index.column() == 1){
            item->setName(value.toString());
         }
         if(index.column() == 2){
            item->setComment(value.toString());
         }
        emit dataChanged(index, index);
        return true;
     }
     return false;
 }
QStringList dfCopyModel::mimeTypes() const
 {
     QStringList types;
     types << "image/png" << "text/uri-list";
     return types;
 }

QMimeData *dfCopyModel::mimeData(const QModelIndexList &indexes) const
 {
     QMimeData *mimeData = new QMimeData();
     QByteArray encodedData;
     QBuffer buffer(&encodedData);
     buffer.open(QIODevice::WriteOnly);
     foreach (QModelIndex index, indexes) {
         if (index.isValid()) {
			 dfCopyObj *item = static_cast<dfCopyObj*>(index.internalPointer());
//            qDebug() << index;
            QImage img = item->getImage();
//            qDebug() << img.text("name") << img.text("comment");
            img.save(&buffer,"PNG");
         }
     }

     mimeData->setData("image/png", encodedData);
     return mimeData;
 }
bool dfCopyModel::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent)
 {
     if (action == Qt::IgnoreAction)
         return true;

     if (data->hasFormat("image/png"))
     {  
             QByteArray encodedData = data->data("image/png");
             QBuffer buffer(&encodedData);
             buffer.open(QIODevice::ReadOnly);
             QImage img;
             img.load(&buffer,"PNG");
			 dfCopyObj* newObj = new dfCopyObj(img,DF);
             prependData(newObj);
             return true;
     }
     if(data->hasFormat("text/uri-list"))
     {
         QString file = data->urls().takeFirst().toLocalFile();
         QImage img;
         img.load(file,"PNG");
		 dfCopyObj * newObj = new dfCopyObj(img,DF);
         insertDataAtPoint(newObj,row);
         return true;
     }
     return false;
}
bool dfCopyModel::insertDataAtPoint(dfCopyObj *data,int row)
{
    beginInsertRows(QModelIndex(),row+1,row+1);
    if(row > rootItem->row()){
        rootItem->appendChild(data);
    }
    else{
		rootItem->insertChild(row,data);
    }
	data->setParent(rootItem);
    endInsertRows();
    return true;
}
 bool dfCopyModel::prependData(dfCopyObj *data)
 {
     beginInsertRows(QModelIndex(),0,0);
	 rootItem->prependChild(data);
	 data->setParent(rootItem);
     endInsertRows();
     return true;
 }
bool dfCopyModel::removeRows ( int row, int count, const QModelIndex & parent ) 
{
     beginRemoveRows(QModelIndex(), row, row+count-1);
     for (int itr = 0; itr < count; ++itr) {
         rootItem->removeChildAt(row);
     }
     endRemoveRows();
     return true;
}