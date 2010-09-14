#include "inc/getShortcutDialog.h"
#include <QKeySequence>
#include <QKeyEvent>
#include <QLabel>
#include <QBoxLayout>

getShortcutDialog::getShortcutDialog(QWidget* parent) : QDialog(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	label = new QLabel();
	setWindowFlags(Qt::Popup);
	layout = new QHBoxLayout(this);
	label->setText("Press Keys For New Shortcut");
	layout->addWidget(label);
//	setGeometry(0,0,64,64);
	setLayout(layout);
}
QKeySequence getShortcutDialog::getKeySequence(QWidget *parent)
{
	getShortcutDialog dialog(parent);
    if (dialog.exec() == QDialog::Accepted) 
{
        return dialog.getSequence();
    }
    return QKeySequence();
}
QKeySequence getShortcutDialog::getSequence()
{
	return(*sequence);
}

void getShortcutDialog::keyPressEvent ( QKeyEvent * event )
{
	int key = event->key();
	if(key >= Qt::Key_Shift && key <= Qt::Key_Alt || key == Qt::Key_AltGr)
{
		return QWidget::keyPressEvent(event);
	}
	sequence = new QKeySequence(event->key() + event->modifiers());
	done(QDialog::Accepted);
}
