#ifndef JSONMENU_H
#define JSONMENU_H

#include <QCoreApplication>
#include <QObject>
#include <QMenu>


class JsonMenu : public QObject
{
    Q_OBJECT

public:
    JsonMenu(QObject *parent = 0);
    ~JsonMenu();

    QMenu*  build(const QString& menuFile);
    QMenu*  build(QMenu* parent, const QJsonObject& object);

protected:
    void    addQuit();


protected slots:
    void    copyClipboard();
    void    clearClipboard();


private:
    QMenu*  m_trayMenu;
    QTimer* m_timer;
    QTime*  m_elapsed;

    QCoreApplication*  m_app;
};

#endif // JSONMENU_H
