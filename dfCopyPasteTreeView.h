#pragma once

#include <QTreeView>
#include <QLabel>

class dfCopyPasteTreeView : public QTreeView
{
    Q_OBJECT

public:
    dfCopyPasteTreeView( QWidget * parent = 0 ): QTreeView(parent){editWin = new QLabel; editWin->hide();};
    bool edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event );
private:
    QLabel * editWin;
};