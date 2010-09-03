#include "dfCopyModel.h"
#include <QAbstractItemView>
#include <QAbstractListModel>
#include <QVariant>
#include <QPainter>
#include <QMimeData>
#include <QDebug>
#include <QBuffer>
#include <QUrl>

int dfCopyModel::rowCount(const QModelIndex &parent) const
{
    return list->size();
}
int dfCopyModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}
QVariant dfCopyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
         return QVariant();

     if (index.row() >= list->size())
         return QVariant();
    if(role == Qt::UserRole){
         return list->at(index.row()).getImage();
     }
     if(role == Qt::UserRole+1){
         return list->at(index.row()).getName();
     }
     if(index.column() == 0){
         if(role == Qt::DecorationRole){
             QImage temp(size,QImage::Format_ARGB32);
             QPainter paint(&temp);
             QImage blah = list->at(index.row()).getImage().scaled(size,Qt::KeepAspectRatio);
             paint.fillRect(0,0,size.width(),size.height(),Qt::transparent);
             paint.drawImage(0,0,blah);
             paint.end();
             return temp;
         }
         return QVariant();
     }
     if (index.column() == 1 && (role == Qt::DisplayRole || role == Qt::EditRole))
            return list->at(index.row()).getName();
     if (index.column() == 2 && (role == Qt::DisplayRole || role == Qt::EditRole))
            return list->at(index.row()).getComment();

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
         if(index.column() == 1){
            (*list)[index.row()].setName(value.toString());
         }
         if(index.column() == 2){
            (*list)[index.row()].setComment(value.toString());
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
//            qDebug() << index;
            QImage img = list->at(index.row()).getImage();
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
             prependData(dfCopyObj(img,DF));
             return true;
     }
     if(data->hasFormat("text/uri-list"))
     {
         QString file = data->urls().takeFirst().toLocalFile();
         QImage img;
         img.load(file,"PNG");
         insertDataAtPoint(dfCopyObj(img,DF),row);
         return true;
     }
     return false;
}
bool dfCopyModel::insertDataAtPoint(const dfCopyObj &data,int row)
{
    beginInsertRows(QModelIndex(),row+1,row+1);
    if(row > list->size()){
        list->append(data);
    }
    else{
        list->insert(row,data);
    }
    endInsertRows();
    return true;
}
 bool dfCopyModel::prependData(const dfCopyObj &data)
 {
     beginInsertRows(QModelIndex(),0,0);
     list->prepend(data);
     endInsertRows();
     return true;
 }
bool dfCopyModel::removeRows ( int row, int count, const QModelIndex & parent ) 
{
     beginRemoveRows(QModelIndex(), row, row+count-1);
     for (int itr = 0; itr < count; ++itr) {
         list->removeAt(row);
     }
     endRemoveRows();
     return true;
}