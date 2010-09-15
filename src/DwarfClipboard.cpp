//Base code adapted from Trolltech tutorial at http://doc.trolltech.com/4.2/desktop-DwarfClipboard-main-cpp.html
//Modified and extended for DwarfClipboard by Jim Hester (belal)
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

#include "inc/DwarfClipboard.h"
#include "inc/DwarfClipboardPng.h"
#include "ui_DwarfClipboard.h"
#include <QxtGlobalShortcut>
#include <QTextStream>
#define DFHACK_WANT_MISCUTILS
#define DFHACK_WANT_TILETYPES
#include "inc/DFHack.h"
#include "dfhack/modules/WindowIO.h"
#include "inc/getShortcutDialog.h"

DwarfClipboard::DwarfClipboard()
{

    setupUi(this);
    createActions();
    createTrayIcon();
    trayIcon->setIcon(QIcon(":/icons/images/DwarfClipboard.png"));
    trayIcon->show();
    setWindowTitle(tr("DwarfClipboard"));
    inputDelay = 100;
    thumbnailSize = 64;
    prevCursor.x = -30000;
    createShortcuts();
    heartbeatTimer = new QTimer(this);
    connectedLabel = new QLabel();
	createConnections();
	loadConfig();
    recentModel = new DwarfClipboardModel();
    tableViewRecent->setModel(recentModel);
    libraryModel = new DwarfClipboardModel();
    treeViewLibrary->setModel(libraryModel);

    connected = connectToDF();
    thumbnailSizeLineEdit->setText(QString("%1").arg(thumbnailSize));
    inputDelayMsLineEdit->setText(QString("%1").arg(inputDelay));

	tilesetPathButton->setText(DwarfClipboardPng::getTileSetPath());
	colorPathButton->setText(DwarfClipboardPng::getColorPath());

    copyShortcutButton->setText(copyShortcut->shortcut());
    pasteDesignationShortcutButton->setText(pasteDesignationShortcut->shortcut());
    pasteBuildingShortcutButton->setText(pasteBuildingShortcut->shortcut());
    setCursorToPreviousPasteShortcutButton->setText(setCursorToPreviousPasteShortcut->shortcut());
    
    heartbeatTimer->start(1000);

    Ui_MainWindow::statusBar->addPermanentWidget(connectedLabel);
    
    connectedIcon = QPixmap(":/icons/images/connect.png");
    
    //This mess just converts the disconnected icon to look the same as a disabled one
    //It is the same code used in the icon object
    disconnectedIcon = QPixmap(":/icons/images/disconnect.png");
    QStyleOption opt(0);
    opt.palette = QApplication::palette();
    QPixmap generated = QApplication::style()->generatedIconPixmap(QIcon::Disabled, disconnectedIcon, &opt);
    disconnectedIcon = generated;
    loadDirectory();
    loadBuildCommands();
    if(connected){
        connected = false; //this is ugly, but just ensures everything will be set proprly
        setConnected();
    }
    else{
        connected = true;
        setDisconnected();
    }
    setupViews();   
}
void DwarfClipboard::heartbeat()
{
    bool result = true;
    if(!DF || !DF->isValid()){
        try{
            DFMgr->Refresh();
            DF = DFMgr->getSingleContext();
        }
        catch(std::exception& e){};
        result = false;
    }
    else{

        try{
            result = DF->getMaps()->Start();
        }
        catch (std::exception& e){
            result = false;
        }
    }

    if(!result){
        setDisconnected();
    }
    else{
        setConnected();
    }
}
void DwarfClipboard::setConnected()
{
    if(!connected){ //coming from a disconnected state, so load the library
        recentModel->setDF(DF);
        libraryModel->setDF(DF);
        DwarfClipboardCopyObj::setDF(DF);
        DwarfClipboardPng::setDF(DF);
        connectedLabel->setPixmap(connectedIcon);
        Ui_MainWindow::statusBar->showMessage("Connected");
        connected = true;
    }
}
void DwarfClipboard::setDisconnected()
{
    if(connected){ //coming from a connected state, change icon and DF pointers
        DF = 0;
        recentModel->setDF(DF);
        libraryModel->setDF(DF);
        DwarfClipboardCopyObj::setDF(DF);
        DwarfClipboardPng::setDF(DF);
        connectedLabel->setPixmap(disconnectedIcon);
        Ui_MainWindow::statusBar->showMessage("Disconnected");
        connected = false;
    }
}
void DwarfClipboard::createShortcuts()
{
    copyShortcut = new QxtGlobalShortcut(this);
    connect(copyShortcut, SIGNAL(activated()), this, SLOT(copy()));
    copyShortcut->setShortcut(QKeySequence("Ctrl+Shift+C"));

    pasteDesignationShortcut = new QxtGlobalShortcut(this);
    connect(pasteDesignationShortcut, SIGNAL(activated()), this, SLOT(pasteDesignations()));
    pasteDesignationShortcut->setShortcut(QKeySequence("Ctrl+Shift+D"));

    pasteBuildingShortcut = new QxtGlobalShortcut(this);
    connect(pasteBuildingShortcut, SIGNAL(activated()), this, SLOT(pasteBuildings()));
    pasteBuildingShortcut->setShortcut(QKeySequence("Ctrl+Shift+B"));

    QShortcut* deleteShortcut = new QShortcut(this);
    deleteShortcut->setKey(QKeySequence(QKeySequence::Delete));
    connect(deleteShortcut, SIGNAL(activated()),this,SLOT(deleteSelected()));

    setCursorToPreviousPasteShortcut = new QxtGlobalShortcut(this);
    connect(setCursorToPreviousPasteShortcut,SIGNAL(activated()),this,SLOT(setPreviousPos()));
    setCursorToPreviousPasteShortcut->setShortcut(QKeySequence("Ctrl+Shift+P"));
}

void DwarfClipboard::createConnections()
{
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(pushButtonRecentSave, SIGNAL(clicked()),this,SLOT(save()));
    connect(pushButtonRecentLoad, SIGNAL(clicked()),this,SLOT(load()));
    connect(pushButtonLibrarySaveSelected, SIGNAL(clicked()),this,SLOT(save()));
    connect(pushButtonLibrarySave,SIGNAL(clicked()),this,SLOT(saveLibrary()));
    connect(pushButtonLibraryLoad, SIGNAL(clicked()),this,SLOT(load()));
    connect(pushButtonRecentPasteDesignations, SIGNAL(clicked()),this,SLOT(pasteDesignations()));
    connect(pushButtonLibraryPasteDesignations, SIGNAL(clicked()),this,SLOT(pasteDesignations()));
    connect(pushButtonRecentPasteBuildings,SIGNAL(clicked()),this,SLOT(pasteBuildings()));
    connect(pushButtonLibraryPasteBuildings,SIGNAL(clicked()),this,SLOT(pasteBuildings()));
    connect(pushButtonLibraryReload,SIGNAL(clicked()),this,SLOT(reloadLibrary()));
    connect(pushButtonRecentSetCursorPrev,SIGNAL(clicked()),this,SLOT(setPreviousPos()));
    connect(pushButtonLibrarySetCursorPrev,SIGNAL(clicked()),this,SLOT(setPreviousPos()));

	connect(copyShortcutButton,SIGNAL(clicked()),this,SLOT(getCopyShortcut()));
	connect(pasteDesignationShortcutButton,SIGNAL(clicked()),this,SLOT(getPasteDesignationShortcut()));
    connect(pasteBuildingShortcutButton,SIGNAL(clicked()),this,SLOT(getPasteBuildingShortcut()));
    connect(setCursorToPreviousPasteShortcutButton,SIGNAL(clicked()),this,SLOT(getSetCursorToPreviousPasteShortcut()));

    connect(thumbnailSizeLineEdit,SIGNAL(editingFinished()),this,SLOT(thumbnailSizeChanged()));
    connect(inputDelayMsLineEdit,SIGNAL(editingFinished()),this,SLOT(inputDelayChanged()));
	connect(tilesetPathButton,SIGNAL(clicked()),this,SLOT(getTilesetPathUser()));
	connect(colorPathButton,SIGNAL(clicked()),this,SLOT(getColorPathUser()));
	connect(tilesetPathToolButton,SIGNAL(clicked()),this,SLOT(getTilesetPathDf()));
	connect(colorPathToolButton,SIGNAL(clicked()),this,SLOT(getColorPathDf()));

    connect(pushButtonToLibrary,SIGNAL(clicked()),this,SLOT(copyToLibrary()));
	connect(useOriginalTilesetImagesCheckBox,SIGNAL(stateChanged(int)),this,SLOT(useOriginal(int)));
    connect(heartbeatTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));
}
void DwarfClipboard::useOriginal(int state)
{
	DwarfClipboardCopyObj::setOriginalState(state);
}
void DwarfClipboard::copyToLibrary()
{
    QModelIndex idx;   
    foreach(idx, tableViewRecent->selectionModel()->selectedRows())
    {
        if(idx.isValid())
        {
            DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(idx.internalPointer());
            DwarfClipboardCopyObj *item2 = new DwarfClipboardCopyObj(*item);
            libraryModel->appendData(item2);
        }
    }
    setupViews();
}
void DwarfClipboard::removeDirectory(QDir current)
{
    QImage img;
    QString fileName;
    QFileInfo file;
    foreach(file, current.entryInfoList(QDir::NoFilter,QDir::DirsFirst | QDir::Name))
    {
        if(file.isDir() && file.fileName()!= "." && file.fileName() != "..")
        {
            removeDirectory(QDir(file.filePath()));
            current.rmdir(file.absoluteFilePath());
        }
        else{
            current.remove(file.absoluteFilePath());
        }
    }
}    

void DwarfClipboard::saveLibrary(QDir current, DwarfClipboardCopyObj* parent)
{

    if(parent == NULL){ //start of the save
        parent = libraryModel->getRoot();
        removeDirectory(current);
    }
    DwarfClipboardCopyObj* child;
    for(int i = 0;i < parent->childCount();i++){
        child = parent->child(i);
        if(child->getImage().isNull()){
            current.mkpath(current.absolutePath() + "/" + child->getName());
            QDir newDir(current.absolutePath() + "/" + child->getName());
            saveLibrary(newDir,child);
        }
        else{
            QImage img = child->getTiledImages();
            img.save(current.absolutePath() +"/"+ child->getName() + ".png");
        }
    }
}         

void DwarfClipboard::setupViews()
{
    tableViewRecent->setIconSize(QSize(thumbnailSize,thumbnailSize));
    tableViewRecent->resizeColumnToContents(0);
    tableViewRecent->resizeRowsToContents();
    treeViewLibrary->setIconSize(QSize(thumbnailSize,thumbnailSize));
    treeViewLibrary->resizeColumnToContents(0);
}
void DwarfClipboard::getPasteDesignationShortcut()
{
	QKeySequence seq = getShortcutDialog::getKeySequence(this);
    if(!seq.isEmpty()){
        pasteDesignationShortcut->setShortcut(seq);
		pasteDesignationShortcutButton->setText(pasteDesignationShortcut->shortcut().toString());
    }
}
void DwarfClipboard::getPasteBuildingShortcut()
{
	QKeySequence seq = getShortcutDialog::getKeySequence(this);
    if(!seq.isEmpty()){
        pasteBuildingShortcut->setShortcut(seq);
		pasteBuildingShortcutButton->setText(pasteBuildingShortcut->shortcut().toString());
    }
}
void DwarfClipboard::getCopyShortcut()
{
	QKeySequence seq = getShortcutDialog::getKeySequence(this);
    if(!seq.isEmpty()){
        copyShortcut->setShortcut(seq);
		copyShortcutButton->setText(copyShortcut->shortcut().toString());
    }
}
void DwarfClipboard::getSetCursorToPreviousPasteShortcut()
{
    QKeySequence seq = getShortcutDialog::getKeySequence(this);
    if(!seq.isEmpty()){
        setCursorToPreviousPasteShortcut->setShortcut(seq);
		setCursorToPreviousPasteShortcutButton->setText(setCursorToPreviousPasteShortcut->shortcut().toString());
    }
}
void DwarfClipboard::getTilesetPathUser()
{
	QString newPath = QFileDialog::getOpenFileName(this,"Select Tileset to Use",DwarfClipboardPng::getTileSetPath(),"Images (*.png *.bmp)");
    if(!newPath.isEmpty())
    {
    	DwarfClipboardPng::setTileSetPath(newPath);
	    tilesetPathButton->setText(DwarfClipboardPng::getTileSetPath());
        recalcAllDwarfClipboardCopyObj();
    }
}
void DwarfClipboard::getColorPathUser()
{
	QString newPath = QFileDialog::getOpenFileName(this,"Select Color file",DwarfClipboardPng::getColorPath(),"Color Files (*.txt)");
    if(!newPath.isEmpty())
    {
	    DwarfClipboardPng::setColorPath(newPath);
	    colorPathButton->setText(DwarfClipboardPng::getColorPath());
        recalcAllDwarfClipboardCopyObj();
    }
}
void DwarfClipboard::recalcAllDwarfClipboardCopyObj()
{
    recalcDwarfClipboardCopyObj(recentModel->getRoot());
    recalcDwarfClipboardCopyObj(libraryModel->getRoot());
}
void DwarfClipboard::recalcDwarfClipboardCopyObj(DwarfClipboardCopyObj* item)
{
    item->recalcImages();
    for(int i =0;i<item->childCount();++i)
    {
        recalcDwarfClipboardCopyObj(item->child(i));
    }
}
void DwarfClipboard::getTilesetPathDf()
{
    if(!connected)
        return;
    QString tileSetPath = readDFInitFile();
    QFile DFExe(DF->getProcess()->getPath().c_str());
    QString newPath = DFExe.fileName().left(DFExe.fileName().lastIndexOf('/')) + "/data/art/" + tileSetPath;
    DwarfClipboardPng::setTileSetPath(newPath);
	tilesetPathButton->setText(DwarfClipboardPng::getTileSetPath());
    recalcAllDwarfClipboardCopyObj();
}
QString DwarfClipboard::readDFInitFile()
{
    bool windowed = false;
    bool graphics = false;
    QFile DFExe(DF->getProcess()->getPath().c_str());
    QString initPath = DFExe.fileName().left(DFExe.fileName().lastIndexOf('/')) + "/data/init/init.txt";
    QString windowedFont,fullFont,graphicsFont,graphicsFullFont;
    
    QFile inFile(initPath);
    if(inFile.exists())
    {
        inFile.open(QIODevice::ReadOnly);
        QTextStream in(&inFile);
        while(!in.atEnd()){
            QString line = in.readLine();
            if(line.startsWith('[') && line.endsWith(']'))
            {
                QStringList list = line.right(line.length()-1).left(line.length()-2).split(':');

                if (list.at(0) == "WINDOWED" && list.at(1) == "YES")
                {
                    windowed = true;
                }
                else if(list.at(0) == "FONT")
                {
                    windowedFont = list.at(1);
                }
                else if(list.at(0) == "FULLFONT")
                {
                    fullFont = list.at(1);
                }
                else if(list.at(0) == "GRAPHICS" && list.at(1) == "YES")
                {
                    graphics = true;
                }
                else if(list.at(0) == "GRAPHICS_FONT")
                {
                    graphicsFont = list.at(1);
                }
                else if(list.at(0) == "GRAPHICS_FULLFONT")
                {
                    graphicsFullFont = list.at(1);
                }
            }
        }
    }
    if(windowed)
    {
        if(graphics)
            return graphicsFont;
        return windowedFont;
    }
    else{
        if(graphics)
            return graphicsFullFont;
        return fullFont;
    }
}


void DwarfClipboard::getColorPathDf()
{
    if(!connected)
        return;
    QFile DFExe(DF->getProcess()->getPath().c_str());
    QString newPath = DFExe.fileName().left(DFExe.fileName().lastIndexOf('/')) + "/data/init/colors.txt";
    DwarfClipboardPng::setColorPath(newPath);
	colorPathButton->setText(DwarfClipboardPng::getColorPath());
    recalcAllDwarfClipboardCopyObj();
}
void DwarfClipboard::inputDelayChanged()
{
    inputDelay = inputDelayMsLineEdit->text().toInt();
    DwarfClipboardPng::setDelay(inputDelay);
}
void DwarfClipboard::thumbnailSizeChanged()
{
    thumbnailSize = thumbnailSizeLineEdit->text().toInt();
    tableViewRecent->setIconSize(QSize(thumbnailSize,thumbnailSize));
    treeViewLibrary->setIconSize(QSize(thumbnailSize,thumbnailSize));
    setupViews();
}
void DwarfClipboard::deleteSelected()
{
    QItemSelectionModel* selectModel;
    QModelIndex idx;
    DwarfClipboardModel* model;
    if(TabWidget->currentIndex() > 1 || TabWidget->currentIndex() < 0)
    {
        return;
    }
    if(TabWidget->currentIndex() == 0)
    {
        selectModel = tableViewRecent->selectionModel();
        model = recentModel;
    }
    if(TabWidget->currentIndex() == 1)
    {
        selectModel = treeViewLibrary->selectionModel();
        model = libraryModel;
    }
    QModelIndexList selected = selectModel->selectedRows();
    while(!selected.empty()){
        idx = selected.takeFirst();
        model->removeRow(idx.row(),idx.parent());
        selected = selectModel->selectedRows();
    }
}
void DwarfClipboard::save()
{
    QItemSelectionModel* selectModel;
    QModelIndex idx;
    if(TabWidget->currentIndex() > 1 || TabWidget->currentIndex() < 0)
    {
        return;
    }
    if(TabWidget->currentIndex() == 0)
    {
        selectModel = tableViewRecent->selectionModel();
    }
    if(TabWidget->currentIndex() == 1)
    {
        selectModel = treeViewLibrary->selectionModel();
    }
    
    foreach(idx, selectModel->selectedRows())
    {
	    DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(idx.internalPointer());
        if(idx.isValid()){
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                ".",
                                tr("Png Images (*.png)"));
            QImage img = item->getTiledImages();
            img.save(fileName);
        }
    }
}
void DwarfClipboard::load()
{
    QImage img;
    QString fileName;
    DwarfClipboardModel* model;
    if(TabWidget->currentIndex() > 1 || TabWidget->currentIndex() < 0){
        return;
    }
    if(TabWidget->currentIndex() == 0){
        model = recentModel;
    }
    if(TabWidget->currentIndex() == 1){
        model = libraryModel;
    }
    foreach(fileName,QFileDialog::getOpenFileNames(this, tr("Open File"),
                            ".",
                            tr("Png Images (*.png)")))
    {

        img.load(fileName);
        DwarfClipboardCopyObj* data = new DwarfClipboardCopyObj(img);
            QModelIndex idx;
    
        model->prependData(data);
    }
    setupViews();
}
void DwarfClipboard::reloadLibrary()
{
    libraryModel->clear();
    loadDirectory("library");
    setupViews();
}
void DwarfClipboard::loadDirectory(QString directory,DwarfClipboardCopyObj * parent)
{
    QImage img;
    QString fileName;
    QFileInfo file;
    foreach(file, QDir(directory).entryInfoList(QDir::NoFilter,QDir::DirsFirst | QDir::Name)){
        if(file.isDir() && file.fileName()!= "." && file.fileName() != ".."){
            DwarfClipboardCopyObj* dir = new DwarfClipboardCopyObj(parent);
            dir->setName(file.fileName());
            libraryModel->appendData(dir,parent);
            loadDirectory(file.filePath(),dir);
        }
        else if(file.suffix() == "png"){
            img.load(file.absoluteFilePath());
            DwarfClipboardCopyObj* data = new DwarfClipboardCopyObj(img,parent);
            libraryModel->appendData(data,parent);
        }
    }
}
        
bool DwarfClipboard::connectToDF()
{
    try
    {
        DFMgr = new DFHack::ContextManager("Memory.xml");
        DF = DFMgr->getSingleContext(); 
        DF->Attach();
        DF->Resume();
        Pos = DF->getPosition();
        Pos->Start();
        Maps = DF->getMaps();
        if(!DF->getMaps()->Start())
        {
            return false;
        }
    }
    catch (std::exception& e)
    {
        DF=0;
        return false;
    }
    return true;
}
void DwarfClipboard::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QWidget::setVisible(visible);
}

void DwarfClipboard::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("DwarfClipboard"),
                                 tr("DwarfClipboard will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the system tray."));
        hide();
        event->ignore();
    }
}

void DwarfClipboard::setIcon(int index){    
    QIcon icon = iconComboBox->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip(iconComboBox->itemText(index));
}

void DwarfClipboard::iconActivated(QSystemTrayIcon::ActivationReason reason)
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

void DwarfClipboard::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
    trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon,
                          durationSpinBox->value() * 1000);
}

void DwarfClipboard::copy()
{
    if(!connected)
        return;
    cursorIdx tempCursor;
    if(prevCursor.x == -30000){
        if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z)){
            trayIcon->showMessage(tr("df Copy"),tr("Please place the df cursor"));
        }
        else{
            prevCursor = tempCursor;
            trayIcon->showMessage(tr("df Copy"),tr("df Copy Started, place cursor at second point of region and copy again"));
        }
    }
    else{
        if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z)){
                trayIcon->showMessage(tr("df Copy"),tr("Please place the df cursor"));
        }
        else{
            trayIcon->showMessage(tr("df Copy"),tr("Region Selected, press %1 to paste designation, %2 to paste buildings or %3 to start a new copy").arg(pasteDesignationShortcut->shortcut().toString()).arg(pasteBuildingShortcut->shortcut().toString()).arg(copyShortcut->shortcut().toString()));
            DwarfClipboardCopyObj * newObj = new DwarfClipboardCopyObj(prevCursor,tempCursor);
            recentModel->prependData(newObj);
            tableViewRecent->selectRow(0);
            setupViews();
            prevCursor.x = -30000;
        }
    }
}
void DwarfClipboard::setPreviousPos()
{
    if(!connected)
        return;
    QModelIndex idx;
    if(TabWidget->currentIndex() > 1 || TabWidget->currentIndex() < 0){
        return;
    }
    if(TabWidget->currentIndex() == 0){
        idx = tableViewRecent->currentIndex();
    }
    if(TabWidget->currentIndex() == 1){
        idx = treeViewLibrary->currentIndex();
    }
	if(idx.isValid()){
        DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(idx.internalPointer());
        cursorIdx prev = item->getPrevPastePoint();
        Pos->setCursorCoords(prev.x,prev.y,prev.z);
    }
}
void DwarfClipboard::pasteDesignations()
{
    if(!connected)
        return;
    QModelIndex idx;
    if(TabWidget->currentIndex() > 1 || TabWidget->currentIndex() < 0){
        return;
    }
    if(TabWidget->currentIndex() == 0){
        idx = tableViewRecent->currentIndex();
    }
    if(TabWidget->currentIndex() == 1){
        idx = treeViewLibrary->currentIndex();
    }
	if(idx.isValid()){
		cursorIdx tempCursor;
		if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z)){
				trayIcon->showMessage(tr("df Paste"),tr("Please place the df cursor"));
		}
		else
		{
			trayIcon->showMessage( tr("df Paste"),tr("Pasting!"));
			DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(idx.internalPointer());
			item->pasteDesignations(tempCursor);
		}
	}
}
void DwarfClipboard::pasteBuildings()
{
    if(!connected)
        return;
    QModelIndex idx;
    if(TabWidget->currentIndex() > 1 || TabWidget->currentIndex() < 0){
        return;
    }
    if(TabWidget->currentIndex() == 0){
        idx = tableViewRecent->currentIndex();
    }
    if(TabWidget->currentIndex() == 1){
        idx = treeViewLibrary->currentIndex();
    }
	if(idx.isValid()){
		cursorIdx tempCursor;
		if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z)){
				trayIcon->showMessage(tr("df Paste"),tr("Please place the df cursor"));
		}
		else
		{
			trayIcon->showMessage( tr("df Paste"),tr("Pasting!"));
			DwarfClipboardCopyObj *item = static_cast<DwarfClipboardCopyObj*>(idx.internalPointer());
			item->pasteBuildings(tempCursor);
		}
	}
}
void DwarfClipboard::messageClicked()
{
    QMessageBox::information(0, tr("DwarfClipboard"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}

void DwarfClipboard::createIconGroupBox()
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

void DwarfClipboard::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(saveAndQuit()));
}
void DwarfClipboard::loadConfig()
{
    QFile inFile("config.ini");
    if(inFile.exists())
    {
        inFile.open(QIODevice::ReadOnly);
        QTextStream in(&inFile);
        while(!in.atEnd()){
            QStringList list = in.readLine().split(';');
            if (list.at(0) == "copy"){
                copyShortcut->setShortcut(QKeySequence(list.at(1)));
            }
            else if (list.at(0) == "pasteDesignation"){
                pasteDesignationShortcut->setShortcut(QKeySequence(list.at(1)));
            }
            else if (list.at(0) == "pasteBuilding"){
                pasteBuildingShortcut->setShortcut(QKeySequence(list.at(1)));
            }
            else if (list.at(0) == "setCursorPreviousPaste"){
                setCursorToPreviousPasteShortcut->setShortcut(QKeySequence(list.at(1)));
            }
            else if (list.at(0) == "thumbnailSize"){
                thumbnailSize = list.at(1).toInt();
            }
            else if (list.at(0) == "inputDelay"){
                inputDelay = list.at(1).toInt();
            }
			else if(list.at(0) == "tilesetPath"){
				DwarfClipboardPng::setTileSetPath(list.at(1));
			}
			else if(list.at(0) == "colorPath"){
				DwarfClipboardPng::setColorPath(list.at(1));
			}
        }
        inFile.close();
    }
}

void DwarfClipboard::loadBuildCommands()
{
    QMap<QString, QString> commands;
    QFile inFile("buildCommands.ini");
    if(inFile.exists())
    {
        inFile.open(QIODevice::ReadOnly);
        QTextStream in(&inFile);
        while(!in.atEnd()){
            QStringList list = in.readLine().split('=');
            commands[list.at(0)]=list.at(1);
        }
    }
    DwarfClipboardCopyObj::setBuildCommands(commands);
}
    
void DwarfClipboard::saveAndQuit()
{
    QFile outFile("config.ini");
    outFile.open(QIODevice::WriteOnly);
    QTextStream out(&outFile);
    out << "copy;" << copyShortcut->shortcut().toString() << '\n';
    out << "pasteDesignation;" << pasteDesignationShortcut->shortcut().toString() << '\n';
    out << "pasteBuilding;" << pasteBuildingShortcut->shortcut().toString() << '\n';
    out << "setCursorPreviousPaste;" << setCursorToPreviousPasteShortcut->shortcut().toString() << '\n';
    out << "thumbnailSize;" << thumbnailSize << '\n';
    out << "inputDelay;" << inputDelay << '\n';
	out << "tilesetPath;" << DwarfClipboardPng::getTileSetPath() << '\n';
	out << "colorPath;" << DwarfClipboardPng::getColorPath() << '\n';
    outFile.close();
    qApp->quit();
}
void DwarfClipboard::createTrayIcon()
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