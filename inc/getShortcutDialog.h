#pragma once
#include <QDialog>

class QLabel;

class getShortcutDialog :public QDialog
{
private:
	void keyPressEvent ( QKeyEvent * event );
	QKeySequence * sequence;
	QLabel * label;
public:
	getShortcutDialog(QWidget* parent = 0);
	QKeySequence getSequence();

static QKeySequence getKeySequence(QWidget *parent = 0);
};