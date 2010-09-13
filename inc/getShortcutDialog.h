#pragma once
#include <QDialog>

class QLabel;
class QHBoxLayout;

class getShortcutDialog :public QDialog{
private:
	void keyPressEvent ( QKeyEvent * event );
	QKeySequence * sequence;
	QLabel * label;
	QHBoxLayout * layout;
public:
	getShortcutDialog(QWidget* parent = 0);
	QKeySequence getSequence();

static QKeySequence getKeySequence(QWidget *parent = 0);
};