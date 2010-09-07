#include "dfCopyPasteTableView.h"
#include "dfCopyObj.h"

bool dfCopyPasteTableView::edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event )
{
    if(index.column() == 0 && trigger & editTriggers()){
        dfCopyObj *item = static_cast<dfCopyObj*>(index.internalPointer());
        QImage img = item->getImage();
        editWin->resize(img.size());
        editWin->setWindowTitle(item->getName());
        editWin->setPixmap(QPixmap::fromImage(img));
        editWin->show();
        return true;
    }
    else{
        if(editWin->isEnabled()){
            dfCopyObj *item = static_cast<dfCopyObj*>(index.internalPointer());
            QImage img = item->getImage();
            editWin->resize(img.size());
            editWin->setWindowTitle(item->getName());
            editWin->setPixmap(QPixmap::fromImage(img));
        }
        return QAbstractItemView::edit(index,trigger,event);
    }
  
}