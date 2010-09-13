#pragma once

#include <QTableView>
#include <QLabel>
#include "inc/dfCopyPasteEditWidget.h"

class dfCopyPasteTableView : public QTableView{
    Q_OBJECT

public:
    dfCopyPasteTableView( QWidget * parent = 0 ): QTableView(parent){ editWin = new dfCopyPasteEditWidget(this); editWin->hide(); };
    bool edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event );
private:
	dfCopyPasteEditWidget * editWin;
};
