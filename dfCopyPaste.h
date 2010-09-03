#ifndef dfCopyPaste_H
#define dfCopyPaste_H

#include <QSystemTrayIcon>
#include <QWidget>
#include "common.h"
#include "dfCopyObj.h"
#include "dfCopyModel.h"
#include "dfCopyDelegate.h"
#include "ui_dfCopyPaste.h"

class Ui_TabWidget;
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

class dfCopyPaste : public QTabWidget, private Ui::TabWidget
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
    void paste_designations();
    void save();
    void load();
    void delete_selected();
    void copy_shortcut_changed();
    void paste_designation_shortcut_changed();
    void save_and_quit();
    void input_delay_changed();
    void thumbnail_size_changed();
private:
    void createIconGroupBox();
    void createMessageGroupBox();
    void createActions();
    void createTrayIcon();
    void load_config();
    void setup_views();


    //DF Stuff
    void connectToDF();

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

    Ui_TabWidget * testWindow;
    dfCopyModel * recentModel;

    //DF things
    DFHack::ContextManager *DFMgr;
    DFHack::Context *DF;
    DFHack::Position *Pos;
    DFHack::Buildings * Bld;
    DFHack::Maps *Maps;

    //Copy things
    QList<dfCopyObj> recentCopyObjs;
    cursorIdx prevCursor;
    QList<dfCopyObj> copyLibrary;
    cursorIdx c3;

};

#endif