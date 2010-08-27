//Base code adapted from Trolltech tutorial at http://doc.trolltech.com/4.2/desktop-systray-main-cpp.html
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
#include <QxtGlobalShortcut>
#define DFHACK_WANT_MISCUTILS
#define DFHACK_WANT_TILETYPES
#include "DFHack.h"
#include "dfhack/modules/WindowIO.h"

dfCopyPaste::dfCopyPaste()
{
 //   createIconGroupBox();
 //   createMessageGroupBox();

//    iconLabel->setMinimumWidth(durationLabel->sizeHint().width());

    createActions();
    createTrayIcon();

  //  connect(showMessageButton, SIGNAL(clicked()), this, SLOT(showMessage()));
  /*  connect(showIconCheckBox, SIGNAL(toggled(bool)),
            trayIcon, SLOT(setVisible(bool)));
    connect(iconComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setIcon(int)));
            */
   // connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    QxtGlobalShortcut* copy_shortcut = new QxtGlobalShortcut(this);
    connect(copy_shortcut, SIGNAL(activated()), this, SLOT(copy()));
    copy_shortcut->setShortcut(QKeySequence("Ctrl+Shift+C"));

    QxtGlobalShortcut* paste_shortcut = new QxtGlobalShortcut(this);
    connect(paste_shortcut, SIGNAL(activated()), this, SLOT(paste()));
    paste_shortcut->setShortcut(QKeySequence("Ctrl+Shift+D"));
   
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    //mainLayout->addWidget(iconGroupBox);
    //mainLayout->addWidget(messageGroupBox);
    setLayout(mainLayout);

  //  iconComboBox->setCurrentIndex(1);
    trayIcon->setIcon(QIcon(":/images/dfCopyPaste.png"));
    trayIcon->show();

    setWindowTitle(tr("Systray"));
    resize(400, 300);
    connectToDF();
    copyObj = new dfCopyObj();
    copyObj->setDF(DF);
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
        QMessageBox::information(this, tr("Systray"),
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
    if(copyObj->getValid() == 0){
        if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z)){
            trayIcon->showMessage(tr("df Copy"),tr("Please place the df cursor"));
        }
        else{
            copyObj->addPos(tempCursor);
            trayIcon->showMessage(tr("df Copy"),tr("df Copy Started, place cursor at second point of region and copy again"));
        }
    }
    else{
        if(copyObj->getValid() == 1){
            if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z)){
                trayIcon->showMessage(tr("df Copy"),tr("Please place the df cursor"));
            }
            else{
                copyObj->addPos(tempCursor);
                trayIcon->showMessage(tr("df Copy"),tr("Region Selected, press CTRL-SHIFT-D to paste, or CTRL-SHIFT-C to start a new copy"));
                dfCopyPastePng testPng(DF);
                QImage pictures = testPng.getPicture(copyObj->getRange())[0];
                QLabel * test = new QLabel();
                test->setPixmap(QPixmap::fromImage(pictures));
                this->layout()->addWidget(test);
            }
        }
        else{ // both values set, restart the copy
            copyObj->clear();
            copy();
            return;
        }
    }
    return;
}

void dfCopyPaste::paste()
{
    if(copyObj->getValid() == 2){
        cursorIdx tempCursor;
        if(!Pos->getCursorCoords(tempCursor.x,tempCursor.y,tempCursor.z)){
                trayIcon->showMessage(tr("df Paste"),tr("Please place the df cursor"));
            }
            else{
                trayIcon->showMessage( tr("df Paste"),
                         tr("Pasting!"));
                copyObj->paste(tempCursor);
            }
    }
}

void dfCopyPaste::messageClicked()
{
    QMessageBox::information(0, tr("Systray"),
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

void dfCopyPaste::createMessageGroupBox()
{
    messageGroupBox = new QGroupBox(tr("Balloon Message"));

    typeLabel = new QLabel(tr("Type:"));



    typeComboBox = new QComboBox;
    typeComboBox->addItem(tr("None"), QSystemTrayIcon::NoIcon);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxInformation), tr("Information"),
            QSystemTrayIcon::Information);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxWarning), tr("Warning"),
            QSystemTrayIcon::Warning);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxCritical), tr("Critical"),
            QSystemTrayIcon::Critical);
    typeComboBox->setCurrentIndex(1);

    durationLabel = new QLabel(tr("Duration:"));

    durationSpinBox = new QSpinBox;
    durationSpinBox->setRange(5, 60);
    durationSpinBox->setSuffix(" s");
    durationSpinBox->setValue(15);

    durationWarningLabel = new QLabel(tr("(some systems might ignore this "
                                         "hint)"));
    durationWarningLabel->setIndent(10);

    titleLabel = new QLabel(tr("Title:"));

    titleEdit = new QLineEdit(tr("Cannot connect to network"));

    bodyLabel = new QLabel(tr("Body:"));

    bodyEdit = new QTextEdit;
    bodyEdit->setPlainText(tr("Don't believe me. Honestly, I don't have a "
                              "clue.\nClick this balloon for details."));

    showMessageButton = new QPushButton(tr("Show Message"));
    showMessageButton->setDefault(true);

    QGridLayout *messageLayout = new QGridLayout;
    messageLayout->addWidget(typeLabel, 0, 0);
    messageLayout->addWidget(typeComboBox, 0, 1, 1, 2);
    messageLayout->addWidget(durationLabel, 1, 0);
    messageLayout->addWidget(durationSpinBox, 1, 1);
    messageLayout->addWidget(durationWarningLabel, 1, 2, 1, 3);
    messageLayout->addWidget(titleLabel, 2, 0);
    messageLayout->addWidget(titleEdit, 2, 1, 1, 4);
    messageLayout->addWidget(bodyLabel, 3, 0);
    messageLayout->addWidget(bodyEdit, 3, 1, 2, 4);
    messageLayout->addWidget(showMessageButton, 5, 4);
    messageLayout->setColumnStretch(3, 1);
    messageLayout->setRowStretch(4, 1);
    messageGroupBox->setLayout(messageLayout);
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
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
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