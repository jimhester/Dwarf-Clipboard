#include "inc\DwarfClipboardModel.h"
#include <QAbstractItemView>
#include <QAbstractListModel>
#include <QVariant>
#include <QPainter>
#include <QMimeData>
#include <QDebug>
#include <QBuffer>
#include <QUrl>

DwarfClipboardModel::DwarfClipboardModel(DFHack::Context * tDF,QObject *parent)
: DF(tDF),QAbstractItemModel(parent){
	rootItem = new DwarfClipboardCopyObj();
}
DwarfClipboardModel::~DwarfClipboardModel()
{
	delete rootItem;
}
QModelIndex DwarfClipboardModel::index(int row, int column,const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	DwarfClipboardCopyObj *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<DwarfClipboardCopyObj*>(parent.internalPointer());

	DwarfClipboardCopyObj *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}
QModelIndex DwarfClipboardModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     DwarfClipboardCopyObj *childItem = static_cast<DwarfClipboardCopyObj*>(index.internalPointer());
     DwarfClipboardCopyObj *parentItem = childItem->parent();

     if (parentItem == rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
 }
int DwarfClipboardModel::rowCount(const QModelIndex &parent) const
{
 DwarfClipboardCopyObj *parentItem;
 if (parent.column() > 0)
     return 0;

 if (!parent.isValid())
     parentItem = rootItem;
 else
     parentItem = static_cast<DwarfClipboardCopyObj*>(parent.internalPointer());

 return parentItem->childCount();
}
int DwarfClipboardModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}
QVariant DwarfClipboardModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
         return QVariant();

    DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(index.internalPointer());
    if(item->getImage().isNull()){
        if(index.column() == 0 && (role == Qt::DisplayRole || role == Qt::EditRole))
        {
            return item->getName();
        }
        return QVariant();
     }
     if(index.column() == 0){
         if(role == Qt::DecorationRole){           
             return QIcon(QPixmap::fromImage(item->getImage()));
         }
         return QVariant();
     }
     if (index.column() == 1 && (role == Qt::DisplayRole || role == Qt::EditRole))
            return item->getName();
     if (index.column() == 2 && (role == Qt::DisplayRole || role == Qt::EditRole))
            return item->getComment();
    return QVariant();
}
QVariant DwarfClipboardModel::headerData(int section, Qt::Orientation orientation,int role) const
{
    if (role != Qt::DisplayRole)
         return QVariant();

    if (orientation == Qt::Horizontal)
{
        switch(section){
            case 1: return QString("Name");
            case 2: return QString("Comment");
            default: return QVariant();
        }
    }
    return(QVariant());
}
Qt::ItemFlags DwarfClipboardModel::flags(const QModelIndex &index) const
 {

     Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
     if (!index.isValid())
         return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	 DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(index.internalPointer());
	 if(item->getImage().isNull()){
         return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	 }
     if(index.column() == 0){
         return defaultFlags | Qt::ItemIsDragEnabled; // | Qt::ItemIsDropEnabled;
     }
	 return defaultFlags;
 }
bool DwarfClipboardModel::setData(const QModelIndex &index,const QVariant &value, int role)
 {
     if (index.isValid() && role == Qt::EditRole) 
{
		DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(index.internalPointer());
        if(item->getImage().isNull()){
            if(index.column() == 0){
                item->setName(value.toString());
            }
            emit dataChanged(index, index);
            return true;
        }
        else{
             if(index.column() == 1){
                item->setName(value.toString());
             }
             if(index.column() == 2){
                item->setComment(value.toString());
             }
            emit dataChanged(index, index);
            return true;
        }
     }
     return false;
 }
QStringList DwarfClipboardModel::mimeTypes() const
 {
  //   return QAbstractItemModel::mimeTypes();
     QStringList types;
     types << "image/png" << "text/uri-list";
     return types;
 }

QMimeData *DwarfClipboardModel::mimeData(const QModelIndexList &indexes) const
 {
//     return QAbstractItemModel::mimeData(indexes);
     QMimeData *mimeData = new QMimeData();
     QByteArray encodedData;
     QBuffer buffer(&encodedData);
     QByteArray encodedData2;
     QBuffer buffer2(&encodedData2);
     buffer.open(QIODevice::WriteOnly);
     buffer2.open(QIODevice::WriteOnly);
     QDataStream out(&buffer2);
     foreach (QModelIndex index, indexes) {
         if (index.isValid()) {
			 DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(index.internalPointer());
            QImage img = item->getImage();
            if(!img.isNull()){
                img.save(&buffer,"PNG");
            }
            out.writeRawData((const char *)&item,sizeof(item));
         }
     }
     buffer.close();
    buffer2.close();
     mimeData->setData("image/png", encodedData);
     mimeData->setData("pointer",encodedData2);
     return mimeData;
 }
bool DwarfClipboardModel::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent)
 {
     if (action == Qt::IgnoreAction)
         return true;
     DwarfClipboardCopyObj *parentItem;
     if(!parent.isValid()){
         parentItem = rootItem;
     }
     else
     {
        parentItem = static_cast<DwarfClipboardCopyObj*>(parent.internalPointer());
     }
	 int insertPt;
	 if(row == -1){
		 insertPt = parentItem->childCount();
	 }
	 else{
		 insertPt = row;
	 }
     if(data->hasFormat("pointer"))
     {
        QByteArray encodedData = data->data("pointer");
        QBuffer buffer(&encodedData);
        buffer.open(QIODevice::ReadOnly);
        QDataStream in(&buffer);
        while(!in.atEnd())
        {
            DwarfClipboardCopyObj *item;
            in.readRawData((char *)&item,sizeof(item));
            QModelIndex index = createIndex(item->parent()->row(),0,item->parent());
            if(beginMoveRows(index,item->row(),item->row(),parent,insertPt)){
                item->parent()->removeChildAt(item->row());
                item->setParent(parentItem);
                parentItem->insertChild(insertPt,item);
                endMoveRows();
            }
        }
        buffer.close();
        return true;
     }
     if (data->hasFormat("image/png"))
     {  
         QByteArray encodedData = data->data("image/png");
         QBuffer buffer(&encodedData);
         buffer.open(QIODevice::ReadOnly);
         QImage img;
         img.load(&buffer,"PNG");
		 DwarfClipboardCopyObj* newObj = new DwarfClipboardCopyObj(img);
         insertDataAtPoint(newObj,insertPt,parent);
         return true;
     }
     if(data->hasFormat("text/uri-list"))
     {
         QUrl url;
         foreach(url, data->urls()){
             QString file = url.toLocalFile();
             if(file.endsWith("png",Qt::CaseInsensitive))
{
                QImage img;
                 img.load(file,"PNG");
		         DwarfClipboardCopyObj * newObj = new DwarfClipboardCopyObj(img);
                 insertDataAtPoint(newObj,insertPt,parent);
            }
         }
         return true;
     }
     return false;
}
bool DwarfClipboardModel::insertDataAtPoint(DwarfClipboardCopyObj *data,int row, const QModelIndex &parent)
{
    DwarfClipboardCopyObj* parentItem;
    if(!parent.isValid()){
        parentItem = rootItem;
    }
    else{
        parentItem = static_cast<DwarfClipboardCopyObj*>(parent.internalPointer());
    }
    beginInsertRows(parent,row,row);
    parentItem->insertChild(row,data);
	data->setParent(parentItem);
    endInsertRows();
    return true;
}
 bool DwarfClipboardModel::prependData(DwarfClipboardCopyObj *data, DwarfClipboardCopyObj *parent)
 {
     if(parent == NULL){
         parent = rootItem;
     }
     beginInsertRows(QModelIndex(),0,0);
	 parent->prependChild(data);
	 data->setParent(parent);
     endInsertRows();
     return true;
 }
 void DwarfClipboardModel::clear()
{
    for (int i = 0; i < rootItem->childCount(); ++i) {
        DwarfClipboardCopyObj* item = rootItem->child(i);
        delete item;
    }
    rootItem->clear();
    reset();
}
 bool DwarfClipboardModel::appendData(DwarfClipboardCopyObj *data, DwarfClipboardCopyObj *parent)
 {
     if(parent == NULL){
         parent = rootItem;
     }
     beginInsertRows(QModelIndex(),parent->childCount(),parent->childCount());
     parent->appendChild(data);
     data->setParent(parent);
     endInsertRows();
     return true;
 }
bool DwarfClipboardModel::removeRows ( int row, int count, const QModelIndex & parent ) 
{
    DwarfClipboardCopyObj* parentItem;
    if(!parent.isValid()){
        parentItem = rootItem;
    }
    else{
        parentItem= static_cast<DwarfClipboardCopyObj*>(parent.internalPointer());
    }
    
    beginRemoveRows(parent, row, row+count-1); 
    for (int itr = 0; itr < count; ++itr) {
        //DwarfClipboardCopyObj* childItem = parentItem->child(row);
        parentItem->removeChildAt(row);
        //delete childItem;
    }
    endRemoveRows();
    return true;
}