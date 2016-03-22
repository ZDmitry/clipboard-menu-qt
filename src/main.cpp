#include <QApplication>

#include <QSystemTrayIcon>
#include <QFileInfo>
#include <QDir>

#include "jsonmenu.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set PWD to App Directory
    QString appPath = QFileInfo(argv[0]).absolutePath();
    QDir::setCurrent(appPath);

    QIcon trayIcon(":/images/clipboard.png");
    app.setWindowIcon(trayIcon);

    JsonMenu menu;
    QMenu* trayMenu = menu.parse("menu.json");

    QSystemTrayIcon systray(trayIcon);
    systray.setContextMenu(trayMenu);
    systray.show();

    app.setQuitOnLastWindowClosed(false);
    return app.exec();
}
