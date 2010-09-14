#include "inc\DwarfClipboardTableView.h"
#include "inc\DwarfClipboardCopyObj.h"

bool DwarfClipboardTableView::edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event )
{
    if(!index.isValid()){
        return QAbstractItemView::edit(index,trigger,event);
    }
    if(index.column() == 0 && trigger & editTriggers()){
        DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(index.internalPointer());
        QImage img = item->getImage();
        editWin->setCopyObj(item);
        editWin->show();
        return true;
    }
    else{
        if(editWin->isVisible()){
            DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(index.internalPointer());
            QImage img = item->getImage();
       		editWin->setCopyObj(item);
            editWin->show();
        }
        return QAbstractItemView::edit(index,trigger,event);
    }
}