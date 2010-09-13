#pragma once

#include <QTreeView>
#include <QLabel>
#include "inc/dfCopyPasteEditWidget.h"

class dfCopyPasteTreeView : public QTreeView
{
    Q_OBJECT

public:
    dfCopyPasteTreeView( QWidget * parent = 0 ): QTreeView(parent){editWin = new dfCopyPasteEditWidget(this); editWin->hide();};
    bool edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event );
    void dfCopyPasteTreeView::mouseDoubleClickEvent(QMouseEvent *event);

private:
    dfCopyPasteEditWidget * editWin;
};