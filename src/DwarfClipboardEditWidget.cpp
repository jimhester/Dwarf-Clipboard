#include "inc/DwarfClipboardEditWidget.h"
#include "inc/DwarfClipboardCopyObj.h"
#include <QDebug>

DwarfClipboardEditWidget::DwarfClipboardEditWidget(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
    scene = new QGraphicsScene();
    graphicsView->setScene(scene);
    setWindowFlags(Qt::Window);
	connect(verticalSlider,SIGNAL(valueChanged(int)),this,SLOT(updateImage(int)));
}
void DwarfClipboardEditWidget::setCopyObj(DwarfClipboardCopyObj * newObj)
{
	obj = newObj;
    scene->clear();
    QImage img = obj->getImage(obj->getDefaultIndex());
    scene->addPixmap(QPixmap::fromImage(img));
	verticalSlider->setMaximum(obj->getImageCount()-1);
	verticalSlider->setMinimum(0);
    verticalSlider->setValue(obj->getImageCount()-obj->getDefaultIndex()-1);
    setWindowTitle(obj->getName());
    graphicsView->setSceneRect(QRect(QPoint(0,0),img.size()));
    graphicsView->updateGeometry();
}
void DwarfClipboardEditWidget::updateImage(int position)
{
    QImage img = obj->getImage(obj->getImageCount()-position-1);
    graphicsView->setSceneRect(QRect(QPoint(0,0),img.size()));
    scene->addPixmap(QPixmap::fromImage(img));
    graphicsView->updateGeometry();
    obj->setDefaultIndex(obj->getImageCount()-position-1);
}