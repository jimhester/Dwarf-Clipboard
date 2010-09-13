#include "inc\dfCopyPasteTableView.h"
#include "inc\dfCopyObj.h"

bool dfCopyPasteTableView::edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event ){
    if(!index.isValid()){
        return QAbstractItemView::edit(index,trigger,event);
    }
    if(index.column() == 0 && trigger & editTriggers()){
        dfCopyObj *item = static_cast<dfCopyObj*>(index.internalPointer());
        QImage img = item->getImage();
        editWin->setCopyObj(item);
        editWin->show();
        return true;
    }
    else{
        if(editWin->isVisible()){
            dfCopyObj *item = static_cast<dfCopyObj*>(index.internalPointer());
            QImage img = item->getImage();
       		editWin->setCopyObj(item);
            editWin->show();
        }
        return QAbstractItemView::edit(index,trigger,event);
    }
}