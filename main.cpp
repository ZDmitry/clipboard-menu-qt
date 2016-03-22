#include <QApplication>

#include <QSystemTrayIcon>

#include "jsonmenu.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QIcon trayIcon(":/images/clipboard.png");
    QSystemTrayIcon systray(trayIcon);

    JsonMenu menu;
    QMenu* trayMenu = menu.build("menu.json");

    systray.setContextMenu(trayMenu);
    systray.show();

    app.setQuitOnLastWindowClosed(false);
    return app.exec();
}
