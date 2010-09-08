#include "inc\dfCopyPaste.h"
#include <QtGui/QApplication>
#include <QtGui>

int main(int argc, char *argv[])
 {
    QApplication a(argc, argv);
    if (!QSystemTrayIcon::isSystemTrayAvailable()) 
    {
        QMessageBox::critical(0, QObject::tr("dfCopyPaste"),
        QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }
    dfCopyPaste w;
    w.show();
    return a.exec();
}
