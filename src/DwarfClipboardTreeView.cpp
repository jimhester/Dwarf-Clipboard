#include "../inc/DwarfClipboardTreeView.h"
#include "../inc/DwarfClipboardCopyObj.h"
#include "../inc/DwarfClipboardModel.h"
#include <QMouseEvent>
#include <QAction>
#include <QMenu>

bool DwarfClipboardTreeView::edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event )
{
    if(!index.isValid())
    {
        return QAbstractItemView::edit(index,trigger,event);
    }
    DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(index.internalPointer());
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
void DwarfClipboardTreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if(index.isValid())
    {
        return QAbstractItemView::mouseDoubleClickEvent(event);
    }
    DwarfClipboardModel *dfModel = static_cast<DwarfClipboardModel*>(model());
    DwarfClipboardCopyObj *item = new DwarfClipboardCopyObj();
    item->setName("New Category");
    dfModel->appendData(item);
    resizeColumnToContents(0);
//    DwarfClipboardTreeView::edit(idx2,QAbstractItemView::DoubleClicked,event);
}
void DwarfClipboardTreeView::contextMenuEvent ( QContextMenuEvent * event ) 
{
    QMenu menu(this);
    QAction * changeTileset = menu.addAction("Set Original Tileset to Current");
    QAction * result = menu.exec(event->globalPos());
    if(result == changeTileset){
        QModelIndex idx = indexAt(event->pos());
        DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(idx.internalPointer());
        item->setOrignialToCurrent();
    }    
}
