#include "inc\dfCopyPasteTreeView.h"
#include "inc\dfCopyObj.h"
#include "inc\dfCopyModel.h"
#include <QMouseEvent>

bool dfCopyPasteTreeView::edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event ){
    if(!index.isValid())
    {
        return QAbstractItemView::edit(index,trigger,event);
    }
    dfCopyObj *item = static_cast<dfCopyObj*>(index.internalPointer());
    QImage img = item->getImage();
    if(img.isNull())
    {
        if(index.column() == 1 || index.column() == 2)
        {
            return false;
        }
        return QAbstractItemView::edit(index,trigger,event);
    }
    if(index.column() == 0 && trigger & editTriggers())
    {
		editWin->setCopyObj(item);
        editWin->show();
        return true;
    }
    if(editWin->isVisible())
    {
		editWin->setCopyObj(item);
        editWin->show();
    }
    return QAbstractItemView::edit(index,trigger,event);
}
void dfCopyPasteTreeView::mouseDoubleClickEvent(QMouseEvent *event){
    QModelIndex index = indexAt(event->pos());
    if(index.isValid())
    {
        return QAbstractItemView::mouseDoubleClickEvent(event);
    }
    dfCopyModel *dfModel = static_cast<dfCopyModel*>(model());
    dfCopyObj *item = new dfCopyObj(0,0);
    item->setName("New Category");
    dfModel->appendData(item);
    resizeColumnToContents(0);
//    dfCopyPasteTreeView::edit(idx2,QAbstractItemView::DoubleClicked,event);
}