#include "jsonmenu.h"

#include <QApplication>
#include <QMessageBox>
#include <QClipboard>

#include <QAction>
#include <QFile>
#include <QUrl>

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QTimer>
#include <QTime>

#include <QDebug>


#define MSEC          1000

#define DEF_TIMEOUT  (5 * MSEC)
#define MIN_TIMEOUT  100


JsonMenu::JsonMenu(QObject *parent)
    : QObject(parent), m_trayMenu(NULL), m_elapsed(NULL)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

    m_app = QApplication::instance();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(clearClipboard()));

    m_cfg["secure"]  = true;
    m_cfg["timeout"] = DEF_TIMEOUT;

    connect(&m_netman, SIGNAL(finished(QNetworkReply*)), this, SLOT(jsonReceived(QNetworkReply*)));
}

JsonMenu::~JsonMenu()
{
    if (m_trayMenu) delete m_trayMenu;
    if (m_elapsed)  delete m_elapsed;
}

QMenu* JsonMenu::parse(const QString& menuFile)
{
    QString appName = m_app->applicationName();

    if (m_trayMenu) {
        delete m_trayMenu;
    }

    m_trayMenu = new QMenu();

    QFile jsonFile(menuFile);
    if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonBytes  = jsonFile.readAll();
        parseJson(jsonBytes);
    } else {
        QString errTitle = QString("%1 - %2").arg(appName).arg(tr("File open error"));
        QString errMsg   = QString(tr("Can not open file") + ":\n\"%1\"").arg(menuFile);

        QMessageBox::warning(NULL, errTitle, errMsg);
        exit(1);
    }

    return m_trayMenu;
}

QMenu* JsonMenu::parseUrl(const QUrl& remoteJson)
{
    QString appName = m_app->applicationName();

    if (m_trayMenu) {
        delete m_trayMenu;
    }

    m_trayMenu = new QMenu();

    if (remoteJson.isValid()) {
        // download
        QNetworkRequest req(remoteJson);
        m_netman.get(req);
    } else {
        QString errTitle = QString("%1 - %2").arg(appName).arg(tr("Invalid url"));
        QString errMsg   = QString(tr("Invalid url") + ":\n\"%1\"").arg(remoteJson.url());

        QMessageBox::warning(NULL, errTitle, errMsg);
        exit(1);
    }

    return m_trayMenu;
}

QMenu* JsonMenu::parse(QMenu* parent, const QJsonObject& object)
{
    if (!parent) return NULL;

    bool menuExist = false;
    bool confExist = false;

    for (QJsonObject::const_iterator it = object.begin(); it != object.end(); ++it) {
        QJsonValue value = it.value();
        QString    key   = it.key();

        if (!menuExist && value.isObject() && key == "menu") {
            menuExist = buildMenu(parent, value.toObject());
        }

        if (!confExist && value.isObject() && key == "settings") {
            confExist = buildConfig(value.toObject());
        }
    }

    return parent;
}

bool  JsonMenu::buildMenu(QMenu* parent, const QJsonObject& object)
{
    if (!parent) return false;

    for (QJsonObject::const_iterator it = object.begin(); it != object.end(); ++it) {
        QJsonValue value = it.value();
        QString    key   = it.key();

        if (value.isObject()) {
           QMenu* child = parent->addMenu(key);
           buildMenu(child, value.toObject());
        } else if (value.isString()) {
            QAction* action = new QAction(key, parent);
            action->setData(value);

            parent->addAction(action);
            connect(action, SIGNAL(triggered()), this, SLOT(copyClipboard()));
        } else if (value.isNull()) {
            parent->addSeparator();
        }
    }

    return true;
}

bool JsonMenu::buildConfig(const QJsonObject& object)
{
    for (QJsonObject::const_iterator it = object.begin(); it != object.end(); ++it) {
        QJsonValue value = it.value();
        QString    key   = it.key();

        if (m_cfg.contains(key) && !value.isNull()) {
            QVariant& pref  = m_cfg[key];
            if (pref.type() == QVariant::Bool) {
                pref.setValue(value.toBool());
            }
            if (pref.type() == QVariant::Int) {
                pref.setValue(value.toInt());
            }
            if (pref.type() == QVariant::Double) {
                pref.setValue(value.toDouble());
            }
            if (pref.type() == QVariant::String) {
                pref.setValue(value.toString());
            }
        }
    }
    return true;
}

void JsonMenu::parseJson(const QByteArray &jsonData)
{
    Q_ASSERT(m_trayMenu);

    QString appName = m_app->applicationName();

    QJsonParseError  parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error == QJsonParseError::NoError) {
        QJsonObject jsonRoot = jsonDoc.object();
        parse(m_trayMenu, jsonRoot);
    } else {
        long lines;

        QString errline = getErrorLine(jsonData, parseError.offset, lines);
        QString errmsg  = parseError.errorString();

        errline = errline.trimmed();

        QString errMsg   = QString("%1\n" + tr("Line") + " %2 - %3").arg(errline).arg(lines).arg(errmsg);
        QString errTitle = QString("%1 - %2").arg(appName).arg(tr("Parse error"));

        QMessageBox::warning(NULL, errTitle, errMsg);
        exit(1);
    }

    addQuit();
}

void JsonMenu::addQuit()
{
    QAction* quitAction = new QAction(QObject::tr("&Quit"), m_trayMenu);
    connect(quitAction, SIGNAL(triggered()), m_app, SLOT(quit()));

    QAction* clearAction = new QAction(QObject::tr("&Clear"), m_trayMenu);
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearClipboard()));

    m_trayMenu->addSeparator();
    m_trayMenu->addAction(clearAction);
    m_trayMenu->addAction(quitAction);
}

QString JsonMenu::getErrorLine(const QByteArray& data, long offset, long& line)
{
    QTextStream reader(data);

    long lines  = 0;
    long bcount = 0;

    QString errline;
    while(!reader.atEnd()) {
        QString buffer;
        buffer  = reader.readLine();
        bcount += (buffer.length() + 1);

        if (bcount >= offset) {
            break;
        }

        errline = buffer;
        lines++;
    }

    line = lines;
    return errline;
}

void JsonMenu::clearClipboard()
{
    QClipboard* clipboard = QApplication::clipboard();
    if (!!clipboard) {
        clipboard->clear();
    }
}

void JsonMenu::jsonReceived(QNetworkReply* res)
{
    QString appName = m_app->applicationName();

    QString errTitle = QString("%1 - %2").arg(appName).arg(tr("File download error"));
    QString errMsg   = QString(tr("Can not download file") + ":\n\"%1\"");

    if (res && res->error() == QNetworkReply::NoError) {
        int       statusCode = res->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QVariant contentType = res->header(QNetworkRequest::ContentTypeHeader);
        QVariant redirectUrl = res->header(QNetworkRequest::LocationHeader);

        // if redirect - follow location
        if ((statusCode == 301 || statusCode == 302) && redirectUrl.isValid()) {
            QNetworkRequest req(redirectUrl.toUrl());
            m_netman.get(req);
        } else if (statusCode == 200) {
            QByteArray jsonData = res->readAll();
            parseJson(jsonData);
        } else {
            QString statusError = tr("Download failed with status: %1").arg(statusCode);
            QMessageBox::warning(NULL, errTitle, errMsg.arg(statusError));
            exit(1);
        }
    } else {
        QMessageBox::warning(NULL, errTitle, errMsg.arg(res->errorString()));
        exit(1);
    }
}

void JsonMenu::copyClipboard()
{
    if (m_elapsed) delete m_elapsed;

    bool secure  = m_cfg["secure"].toBool();
    int  timeout = m_cfg["timeout"].toInt();

    if (secure) {
        m_elapsed = new QTime();
        m_elapsed->start();
    }

    m_timer->start(timeout > MIN_TIMEOUT ? timeout : DEF_TIMEOUT);

    QClipboard* clipboard = QApplication::clipboard();
    QAction* action = qobject_cast<QAction*>(sender());
    if (!!action && !!clipboard) {
        QString text = action->data().toString();
        clipboard->setText(text);
    }
}


