#pragma once

class DwarfClipboardCopyObj;
class QGraphicsScene;
#include <QWidget>
#include "ui_editWidget.h"

class DwarfClipboardEditWidget : public QWidget, private Ui::editWidget
{
    Q_OBJECT
private:
	DwarfClipboardCopyObj * obj;
    QGraphicsScene * scene;

private slots:
	void updateImage(int position);

public:
	DwarfClipboardEditWidget(QWidget *parent = 0);
	void setCopyObj(DwarfClipboardCopyObj * newObj);
};