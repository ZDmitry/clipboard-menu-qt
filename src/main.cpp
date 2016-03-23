#include <QApplication>

#include <QSystemTrayIcon>
#include <QFileInfo>
#include <QDir>

#include "jsonmenu.h"

#define  DEF_MENU_JSON  "menu.json"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set PWD to App Directory
    QString appPath = QFileInfo(argv[0]).absolutePath();
    QDir::setCurrent(appPath);

    QIcon trayIcon(":/images/clipboard.png");
    app.setWindowIcon(trayIcon);

    QStringList args = app.arguments();
    args.removeFirst();

    QString menuFile = DEF_MENU_JSON;
    for(int i = 1; i < argc; i++) {
        QString arg( argv[i] );
        if ( (arg == "--menu" || arg == "-m" ) && (i+1) <= argc ) {
            menuFile = QFileInfo(argv[++i]).absoluteFilePath();
        }
    }

    JsonMenu menu;
    QMenu* trayMenu = menu.parse(menuFile);

    QSystemTrayIcon systray(trayIcon);
    systray.setContextMenu(trayMenu);
    systray.show();

    app.setQuitOnLastWindowClosed(false);
    return app.exec();
}
