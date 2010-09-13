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

class dfCopyPaste : public QMainWindow, private Ui::MainWindow{
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
    void paste_designations();
    void save();
    void load();
    void delete_selected();
    void get_copy_shortcut();
    void get_paste_designation_shortcut();
	void get_tileset_path_user();
	void get_color_path_user();
	void get_tileset_path_df();
	void get_color_path_df();
    void save_and_quit();
    void input_delay_changed();
    void thumbnail_size_changed();
    void copy_to_library();
    void save_library(QDir current=QDir("library"), dfCopyObj* parent=0);
    void reload_library();
	void useOriginal(int state);
    void heartbeat();
private:
    void createIconGroupBox();
    void createMessageGroupBox();
    void createActions();
    void createTrayIcon();
	void createConnections();
	void createShortcuts();
    void load_config();
    void setup_views();
    void remove_directory(QDir current);
    void load_directory(QString directory=QString("library"),dfCopyObj * parent=0);
    void recalcAllDfCopyObj();
    void recalcDfCopyObj(dfCopyObj* item);
    QString dfCopyPaste::readDFInitFile();

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

    QxtGlobalShortcut * copy_shortcut;
    QxtGlobalShortcut * paste_designation_shortcut;
    int input_delay;
    int thumbnail_size;

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