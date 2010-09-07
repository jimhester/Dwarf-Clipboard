//Base code adapted from Trolltech tutorial at http://doc.trolltech.com/4.2/desktop-dfCopyPaste-main-cpp.html
//Modified and extended for dfCopyPaste by Jim Hester (belal)
/****************************************************************************
**
** Copyright (C) 2006-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/



#include <QtGui>
#include <QtDebug>

#include "dfCopyPaste.h"
#include "dfCopyPastePng.h"
#include "ui_dfCopyPaste.h"
#include <QxtGlobalShortcut>
#include <QTextStream>
#define DFHACK_WANT_MISCUTILS
#define DFHACK_WANT_TILETYPES
#include "DFHack.h"
#include "dfhack/modules/WindowIO.h"

dfCopyPaste::dfCopyPaste()
{

    setupUi(this);
    createActions();
    createTrayIcon();
    connectToDF();

    input_delay = 100;
    thumbnail_size = 64;

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    copy_shortcut = new QxtGlobalShortcut(this);
    connect(copy_shortcut, SIGNAL(activated()), this, SLOT(copy()));
    copy_shortcut->setShortcut(QKeySequence("Ctrl+Shift+C"));

    paste_designation_shortcut = new QxtGlobalShortcut(this);
    connect(paste_designation_shortcut, SIGNAL(activated()), this, SLOT(paste_designations()));
    paste_designation_shortcut->setShortcut(QKeySequence("Ctrl+Shift+D"));

    QShortcut* delete_shortcut = new QShortcut(this);
    delete_shortcut->setKey(QKeySequence(QKeySequence::Delete));
    connect(delete_shortcut, SIGNAL(activated()),this,SLOT(delete_selected()));

    load_config();

    connect(pushButton_recent_save, SIGNAL(clicked()),this,SLOT(save()));
    connect(pushButton_recent_load, SIGNAL(clicked()),this,SLOT(load()));
    connect(pushButton_recent_paste_designations, SIGNAL(clicked()),this,SLOT(paste_designations()));

    connect(copyShortcutLineEdit,SIGNAL(editingFinished()),this,SLOT(copy_shortcut_changed()));
    connect(pasteDesignationShortcutLineEdit,SIGNAL(editingFinished()),this,SLOT(paste_designation_shortcut_changed()));
    connect(thumbnailSizeLineEdit,SIGNAL(editingFinished()),this,SLOT(thumbnail_size_changed()));
    connect(inputDelayMsLineEdit,SIGNAL(editingFinished()),this,SLOT(input_delay_changed()));
    
    recentModel = new dfCopyModel(DF);
    recentModel->setIconSize(QSize(thumbnail_size,thumbnail_size));
 //   libraryModel = new dfCopyModel(&libraryCopyObjs);
    tableView_recent->setModel(recentModel);
    setup_views();
    tableView_recent->verticalHeader()->setDefaultSectionSize(thumbnail_size+1);
    tableView_recent->horizontalHeader()->setDefaultSectionSize(thumbnail_size+7);
    copyShortcutLineEdit->setText(copy_shortcut->shortcut().toString());
    pasteDesignationShortcutLineEdit->setText(paste_designation_shortcut->shortcut().toString());
    thumbnailSizeLineEdit->setText(QString("%1").arg(thumbnail_size));
    inputDelayMsLineEdit->setText(QString("%1").arg(input_delay));

    trayIcon->setIcon(QIcon(":/images/dfCopyPaste.png"));
    trayIcon->show();

    setWindowTitle(tr("dfCopyPaste"));
    prevCursor.x = -30000;
}
void dfCopyPaste::setup_views()
{
    tableView_recent->setColumnWidth(0,thumbnail_size);
    tableView_recent->setMinimumWidth(thumbnail_size);
    tableView_recent->resizeColumnToContents(0);
    tableView_recent->resizeRowsToContents();
}
void dfCopyPaste::paste_designation_shortcut_changed()
{
    QKeySequence seq(pasteDesignationShortcutLineEdit->text());
    if(!seq.isEmpty()){
        paste_designation_shortcut->setShortcut(seq);
    }
}
void dfCopyPaste::copy_shortcut_changed()
{
    QKeySequence seq(copyShortcutLineEdit->text());
    if(!seq.isEmpty()){
        copy_shortcut->setShortcut(seq);
    }
}
void dfCopyPaste::input_delay_changed()
{
    input_delay = inputDelayMsLineEdit->text().toInt();
    dfCopyPastePng::delay = input_delay;
}
void dfCopyPaste::thumbnail_size_changed()
{
    thumbnail_size = thumbnailSizeLineEdit->text().toInt();
    recentModel->setIconSize(QSize(thumbnail_size,thumbnail_size));
    setup_views();
}
void dfCopyPaste::delete_selected()
{
    QModelIndex idx = tableView_recent->currentIndex();
    recentModel->removeRow(idx.row());
    tableView_recent->selectRow(idx.row());
}
void dfCopyPaste::save()
{
    QModelIndex idx = tableView_recent->currentIndex();
	dfCopyObj *item = static_cast<dfCopyObj*>(idx.internalPointer());
    if(idx.isValid()){
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                            ".",
                            tr("Png Images (*.png)"));
        QImage img = item->getImage();
        img.save(fileName);
    }
}
void dfCopyPaste::load()
{
    QImage img;
    QString fileName;
    foreach(fileName,QFileDialog::getOpenFileNames(this, tr("Open File"),
                            ".",
                            tr("Png Images (*.png)")))
    {

        img.load(fileName);
        dfCopyObj* data = new dfCopyObj(img,DF);
        recentModel->prependData(data);
    }
    setup_views();
}
void dfCopyPaste::connectToDF()
{
    bool ok = true;
    try
    {
        DFMgr = new DFHack::ContextManager("Memory.xml");
        DF = DFMgr->getSingleContext(); 
        DF->Attach();
    }
    catch (std::exception& e)
    {
        ok = false;
    }
    if(ok){
        Pos = DF->getPosition();
        Pos->Start();
        DF->Resume();
    }
}
void dfCopyPaste::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QWidget::setVisible(visible);
}

void dfCopyPaste::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("dfCopyPaste"),
                                 tr("dfCopyPaste will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the system tray."));
        hide();
        event->ignore();
    }
}

void dfCopyPaste::setIcon(int index)
{
    QIcon icon = iconComboBox->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip(iconComboBox->itemText(index));
}

void dfCopyPaste::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        setVisible(true);
        setVisible(true);
        setVisible(true);
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}

void dfCopyPaste::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
    trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon,
                          durationSpinBox->value() * 1000);
}

void dfCopyPaste::copy() //this will be ugly, I apoligize to my future self
{
    cursorIdx tempCursor;
    if(prevCursor.x == -30000){
        if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z)){
            trayIcon->showMessage(tr("df Copy"),tr("Please place the df cursor"));
        }
        else
        {
            prevCursor = tempCursor;
            trayIcon->showMessage(tr("df Copy"),tr("df Copy Started, place cursor at second point of region and copy again"));
        }
    }
    else
    {
        if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z))
        {
                trayIcon->showMessage(tr("df Copy"),tr("Please place the df cursor"));
        }
        else
        {
            trayIcon->showMessage(tr("df Copy"),tr("Region Selected, press %1 to paste or %2 to start a new copy").arg(paste_designation_shortcut->shortcut().toString()).arg(copy_shortcut->shortcut().toString()));
            dfCopyObj * newObj = new dfCopyObj(DF,prevCursor,tempCursor);
            recentModel->prependData(newObj);
          //  tableView_recent->selectRow(0);
            setup_views();
            prevCursor.x = -30000;
        }
    }
}

void dfCopyPaste::paste_designations()
{
	QModelIndex idx = tableView_recent->currentIndex();
	if(idx.isValid()){
		cursorIdx tempCursor;
		if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z)){
				trayIcon->showMessage(tr("df Paste"),tr("Please place the df cursor"));
		}
		else
		{
			trayIcon->showMessage( tr("df Paste"),tr("Pasting!"));
			dfCopyObj *item = static_cast<dfCopyObj*>(idx.internalPointer());
			item->paste(tempCursor);
		}
	}
}

void dfCopyPaste::messageClicked()
{
    QMessageBox::information(0, tr("dfCopyPaste"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}

void dfCopyPaste::createIconGroupBox()
{
    iconGroupBox = new QGroupBox(tr("Tray Icon"));

    iconLabel = new QLabel("Icon:");

    iconComboBox = new QComboBox;

    showIconCheckBox = new QCheckBox(tr("Show icon"));
    showIconCheckBox->setChecked(true);

    QHBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(iconLabel);
    iconLayout->addWidget(iconComboBox);
    iconLayout->addStretch();
    iconLayout->addWidget(showIconCheckBox);
    iconGroupBox->setLayout(iconLayout);
}

void dfCopyPaste::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(save_and_quit()));
}
void dfCopyPaste::load_config()
{
    QFile inFile("config.ini");
    if(inFile.exists())
    {
        inFile.open(QIODevice::ReadOnly);
        QTextStream in(&inFile);
        while(!in.atEnd()){
            QStringList list = in.readLine().split(':');
            if (list.at(0) == "copy")
            {
                copy_shortcut->setShortcut(QKeySequence(list.at(1)));
            }
            else if (list.at(0) == "paste_designation")
            {
                paste_designation_shortcut->setShortcut(QKeySequence(list.at(1)));
            }
            else if (list.at(0) == "thumbnail_size")
            {
                thumbnail_size = list.at(1).toInt();
            }
            else if (list.at(0) == "input_delay")
            {
                input_delay = list.at(1).toInt();
            }
        }
        inFile.close();
    }
}

    
void dfCopyPaste::save_and_quit()
{
    QFile outFile("config.ini");
    outFile.open(QIODevice::WriteOnly);
    QTextStream out(&outFile);
    out << "copy:" << copy_shortcut->shortcut().toString() << '\n';
    out << "paste_designation:" << paste_designation_shortcut->shortcut().toString() << '\n';
    out << "thumbnail_size:" << thumbnail_size << '\n';
    out << "input_delay:" << input_delay << '\n';
    outFile.close();
    qApp->quit();
}
void dfCopyPaste::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}