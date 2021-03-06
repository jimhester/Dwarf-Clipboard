#pragma once

#include <QTableView>
#include <QLabel>
#include <QContextMenuEvent>
#include "inc/DwarfClipboardEditWidget.h"

class DwarfClipboardTableView : public QTableView
{
    Q_OBJECT

public:
    DwarfClipboardTableView( QWidget * parent = 0 ): QTableView(parent){ editWin = new DwarfClipboardEditWidget(this); editWin->hide();}
    bool edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event );
    void contextMenuEvent ( QContextMenuEvent * event );
private:
	DwarfClipboardEditWidget * editWin;
};
