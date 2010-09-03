#include "dfCopyPasteView.h"
#include <QTableView>

bool dfCopyPasteView::edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event )
{
    if(index.column() == 0 && trigger & editTriggers()){
        QImage img = model()->data(index,Qt::UserRole).value<QImage>();
        editWin->resize(img.size());
        editWin->setWindowTitle(model()->data(index,Qt::UserRole+1).value<QString>());
        editWin->setPixmap(QPixmap::fromImage(img));
        editWin->show();
        return true;
    }
    else{
        if(editWin->isEnabled()){
            QImage img = model()->data(index,Qt::UserRole).value<QImage>();
            editWin->resize(img.size());
            editWin->setWindowTitle(model()->data(index,Qt::UserRole+1).value<QString>());
            editWin->setPixmap(QPixmap::fromImage(img));
        }
        return QTableView::edit(index,trigger,event);
    }
  
}
