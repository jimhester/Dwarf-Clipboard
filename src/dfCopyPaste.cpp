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

#include "inc\dfCopyPaste.h"
#include "inc\dfCopyPastePng.h"
#include "ui_dfCopyPaste.h"
#include <QxtGlobalShortcut>
#include <QTextStream>
#define DFHACK_WANT_MISCUTILS
#define DFHACK_WANT_TILETYPES
#include "inc\DFHack.h"
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
    connect(pushButton_library_save_selected, SIGNAL(clicked()),this,SLOT(save()));
    connect(pushButton_library_save,SIGNAL(clicked()),this,SLOT(save_library()));
    connect(pushButton_library_load, SIGNAL(clicked()),this,SLOT(load()));
    connect(pushButton_recent_paste_designations, SIGNAL(clicked()),this,SLOT(paste_designations()));
    connect(pushButton_library_paste_designations, SIGNAL(clicked()),this,SLOT(paste_designations()));

    connect(copyShortcutLineEdit,SIGNAL(editingFinished()),this,SLOT(copy_shortcut_changed()));
    connect(pasteDesignationShortcutLineEdit,SIGNAL(editingFinished()),this,SLOT(paste_designation_shortcut_changed()));
    connect(thumbnailSizeLineEdit,SIGNAL(editingFinished()),this,SLOT(thumbnail_size_changed()));
    connect(inputDelayMsLineEdit,SIGNAL(editingFinished()),this,SLOT(input_delay_changed()));

    connect(pushButton_to_library,SIGNAL(clicked()),this,SLOT(copy_to_library()));
    
    recentModel = new dfCopyModel(DF);
    tableView_recent->setIconSize(QSize(thumbnail_size,thumbnail_size));
    tableView_recent->setModel(recentModel);
    libraryModel = new dfCopyModel(DF);
    treeView_library->setModel(libraryModel);
    tableView_recent->setIconSize(QSize(thumbnail_size,thumbnail_size));
    load_directory("library");
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
void dfCopyPaste::copy_to_library()
{
    QModelIndex idx;   
    foreach(idx, tableView_recent->selectionModel()->selectedRows())
    {
        if(idx.isValid())
        {
            dfCopyObj *item = static_cast<dfCopyObj*>(idx.internalPointer());
            dfCopyObj *item2 = new dfCopyObj(*item);
            libraryModel->appendData(item2);
        }
    }
    setup_views();
}

void dfCopyPaste::save_library(QDir current, dfCopyObj* parent)
{
    if(parent == NULL){
        parent = libraryModel->getRoot();
    }
    dfCopyObj* child;
    for(int i = 0;i < parent->childCount();i++){
        child = parent->child(i);
        if(child->getImage().isNull()){
            current.mkpath(current.absolutePath() + "/" + child->getName());
         //   current.cd(current.absolutePath() + "/" + child->getName());
            save_library(current,child);
        }
        else{
            QImage img = child->getImage();
            img.save(current.absolutePath() + child->getName() + ".png");
        }
    }
}         

void dfCopyPaste::setup_views()
{
    tableView_recent->setIconSize(QSize(thumbnail_size,thumbnail_size));
    tableView_recent->resizeColumnToContents(0);
    tableView_recent->resizeRowsToContents();
    treeView_library->setIconSize(QSize(thumbnail_size,thumbnail_size));
    treeView_library->resizeColumnToContents(0);
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
    tableView_recent->setIconSize(QSize(thumbnail_size,thumbnail_size));
    treeView_library->setIconSize(QSize(thumbnail_size,thumbnail_size));
    setup_views();
}
void dfCopyPaste::delete_selected()
{
    QItemSelectionModel* select_model;
    QModelIndex idx;
    dfCopyModel* model;
    if(currentIndex() > 1 || currentIndex() < 0)
    {
        return;
    }
    if(currentIndex() == 0)
    {
        select_model = tableView_recent->selectionModel();
        model = recentModel;
    }
    if(currentIndex() == 1)
    {
        select_model = treeView_library->selectionModel();
        model = libraryModel;
    }
    
    foreach(idx, select_model->selectedRows())
    {
        model->removeRow(idx.row());
//        selectRow(idx.row());
    }
}
void dfCopyPaste::save()
{
    QItemSelectionModel* select_model;
    QModelIndex idx;
    if(currentIndex() > 1 || currentIndex() < 0)
    {
        return;
    }
    if(currentIndex() == 0)
    {
        select_model = tableView_recent->selectionModel();
    }
    if(currentIndex() == 1)
    {
        select_model = treeView_library->selectionModel();
    }
    
    foreach(idx, select_model->selectedRows())
    {
	    dfCopyObj *item = static_cast<dfCopyObj*>(idx.internalPointer());
        if(idx.isValid()){
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                ".",
                                tr("Png Images (*.png)"));
            QImage img = item->getImage();
            img.save(fileName);
        }
    }
}
void dfCopyPaste::load()
{
    QImage img;
    QString fileName;
    dfCopyModel* model;
    if(currentIndex() > 1 || currentIndex() < 0){
        return;
    }
    if(currentIndex() == 0){
        model = recentModel;
    }
    if(currentIndex() == 1){
        model = libraryModel;
    }
    foreach(fileName,QFileDialog::getOpenFileNames(this, tr("Open File"),
                            ".",
                            tr("Png Images (*.png)")))
    {

        img.load(fileName);
        dfCopyObj* data = new dfCopyObj(DF,img);
            QModelIndex idx;
    
        model->prependData(data);
    }
    setup_views();
}
void dfCopyPaste::load_directory(QString directory,dfCopyObj * parent)
{
    QImage img;
    QString fileName;
    QFileInfo file;
    foreach(file, QDir(directory).entryInfoList(QDir::NoFilter,QDir::DirsFirst | QDir::Name))
    {
        if(file.isDir() && file.fileName()!= "." && file.fileName() != "..")
        {
            dfCopyObj* dir = new dfCopyObj(DF,parent);
            dir->setName(file.fileName());
            libraryModel->appendData(dir,parent);
            load_directory(file.filePath(),dir);
        }
        else if(file.suffix() == "png")
        {
            img.load(file.absoluteFilePath());
            dfCopyObj* data = new dfCopyObj(DF,img,parent);
            libraryModel->appendData(data,parent);
        }
    }
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

void dfCopyPaste::copy()
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
            tableView_recent->selectRow(0);
            setup_views();
            prevCursor.x = -30000;
        }
    }
}

void dfCopyPaste::paste_designations()
{

    QModelIndex idx;
    if(currentIndex() > 1 || currentIndex() < 0){
        return;
    }
    if(currentIndex() == 0){
        idx = tableView_recent->currentIndex();
    }
    if(currentIndex() == 1){
        idx = treeView_library->currentIndex();
    }
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