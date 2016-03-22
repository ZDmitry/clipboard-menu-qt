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

    QMenu*   parse(const QString& menuFile);
    QMenu*   parse(QMenu* parent, const QJsonObject& object);

protected:
    void     addQuit();
    QString  getErrorLine(const QByteArray& data, long offset, long& line);

protected slots:
    void     copyClipboard();
    void     clearClipboard();

protected:
    bool     buildMenu(QMenu* parent, const QJsonObject& object);
    bool     buildConfig(const QJsonObject& object);

private:
    QMenu*   m_trayMenu;
    QTimer*  m_timer;
    QTime*   m_elapsed;

    QVariantMap        m_cfg;
    QCoreApplication*  m_app;
};

#endif // JSONMENU_H
