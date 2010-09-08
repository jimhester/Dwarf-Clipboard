#pragma once

#include <QTableView>
#include <QLabel>

class dfCopyPasteTableView : public QTableView
{
    Q_OBJECT

public:
    dfCopyPasteTableView( QWidget * parent = 0 ): QTableView(parent){ editWin = new QLabel; editWin->hide(); };
    bool edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event );
private:
    QLabel * editWin;
};
