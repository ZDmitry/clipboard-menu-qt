#ifndef JSONMENU_H
#define JSONMENU_H

#include <QNetworkAccessManager>
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
    QMenu*   parseUrl(const QUrl& remoteJson);
    QMenu*   parse(QMenu* parent, const QJsonObject& object);

protected:
    void     addQuit();
    QString  getErrorLine(const QByteArray& data, long offset, long& line);

protected slots:
    void     copyClipboard();
    void     clearClipboard();

    void     jsonReceived(QNetworkReply* res);

protected:
    bool     buildMenu(QMenu* parent, const QJsonObject& object);
    bool     buildConfig(const QJsonObject& object);
    void     parseJson(const QByteArray& jsonData);

private:
    QMenu*   m_trayMenu;
    QTimer*  m_timer;
    QTime*   m_elapsed;

    QVariantMap           m_cfg;
    QCoreApplication*     m_app;
    QNetworkAccessManager m_netman;
};

#endif // JSONMENU_H
