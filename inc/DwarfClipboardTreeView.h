#pragma once

#include <QTreeView>
#include <QLabel>
#include "inc/DwarfClipboardEditWidget.h"

class DwarfClipboardTreeView : public QTreeView
{
    Q_OBJECT

public:
    DwarfClipboardTreeView( QWidget * parent = 0 ): QTreeView(parent){editWin = new DwarfClipboardEditWidget(this); editWin->hide();};
    bool edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event );
    void DwarfClipboardTreeView::mouseDoubleClickEvent(QMouseEvent *event);

private:
    DwarfClipboardEditWidget * editWin;
};