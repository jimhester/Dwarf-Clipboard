#ifndef dfCopyPaste_H
#define dfCopyPaste_H

#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QDir>
#include "common.h"
#include "dfCopyObj.h"
#include "dfCopyModel.h"
#include "ui_dfCopyPaste.h"

class QListView;
class QTableView;
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QxtGlobalShortcut;

namespace DFHack{
    class ContextManager;
    class Context;
    class Position;
    class Buildings;
    class Maps;
}

class dfCopyPaste : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    dfCopyPaste();

    void setVisible(bool visible);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void setIcon(int index);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();
    void copy();
    void pasteDesignations();
    void pasteBuildings();
    void save();
    void load();
    void deleteSelected();
    void getCopyShortcut();
    void getPasteDesignationShortcut();
	void getTilesetPathUser();
	void getColorPathUser();
	void getTilesetPathDf();
	void getColorPathDf();
    void saveAndQuit();
    void inputDelayChanged();
    void thumbnailSizeChanged();
    void copyToLibrary();
    void saveLibrary(QDir current=QDir("library"), dfCopyObj* parent=0);
    void reloadLibrary();
	void useOriginal(int state);
    void heartbeat();
private:
    void createIconGroupBox();
    void createMessageGroupBox();
    void createActions();
    void createTrayIcon();
	void createConnections();
	void createShortcuts();
    void loadConfig();
    void setupViews();
    void removeDirectory(QDir current);
    void loadDirectory(QString directory=QString("library"),dfCopyObj * parent=0);
    void recalcAllDfCopyObj();
    void recalcDfCopyObj(dfCopyObj* item);
    QString dfCopyPaste::readDFInitFile();
    void setConnected();
    void setDisconnected();
    void loadBuildCommands();

    //DF Stuff
    bool connectToDF();

    QGroupBox *iconGroupBox;
    QLabel *iconLabel;
    QComboBox *iconComboBox;
    QCheckBox *showIconCheckBox;

    QGroupBox *messageGroupBox;
    QLabel *typeLabel;
    QLabel *durationLabel;
    QLabel *durationWarningLabel;
    QLabel *titleLabel;
    QLabel *bodyLabel;
    QComboBox *typeComboBox;
    QSpinBox *durationSpinBox;
    QLineEdit *titleEdit;
    QTextEdit *bodyEdit;
    QPushButton *showMessageButton;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QxtGlobalShortcut * copyShortcut;
    QxtGlobalShortcut * pasteDesignationShortcut;
    int inputDelay;
    int thumbnailSize;

    dfCopyModel * recentModel;
    dfCopyModel * libraryModel;

    //DF things
    DFHack::ContextManager *DFMgr;
    DFHack::Context *DF;
    DFHack::Position *Pos;
    DFHack::Buildings * Bld;
    DFHack::Maps *Maps;

    //Copy things
    cursorIdx prevCursor;
    cursorIdx c3;

    QTimer *heartbeatTimer;
    bool connected;
    QLabel *connectedLabel;
    QPixmap connectedIcon;
    QPixmap disconnectedIcon;
};

#endif