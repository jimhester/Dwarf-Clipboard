#pragma once

class dfCopyObj;
class QGraphicsScene;
#include <QWidget>
#include "ui_editWidget.h"

class dfCopyPasteEditWidget : public QWidget, private Ui::editWidget
{
    Q_OBJECT
private:
	dfCopyObj * obj;
    QGraphicsScene * scene;

private slots:
	void updateImage(int position);

public:
	dfCopyPasteEditWidget(QWidget *parent = 0);
	void setCopyObj(dfCopyObj * newObj);
};