#include "jsonmenu.h"

#include <QApplication>
#include <QMessageBox>
#include <QClipboard>

#include <QAction>
#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QTimer>
#include <QTime>

#include <QDebug>


#define MSEC 1000


JsonMenu::JsonMenu(QObject *parent)
    : QObject(parent), m_trayMenu(NULL), m_elapsed(NULL)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

    m_app = QApplication::instance();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(clearClipboard()));
}

JsonMenu::~JsonMenu()
{
    if (m_trayMenu) delete m_trayMenu;
    if (m_elapsed)  delete m_elapsed;
}

QMenu* JsonMenu::build(const QString& menuFile)
{
    if (m_trayMenu) {
        delete m_trayMenu;
    }

    m_trayMenu = new QMenu();

    QFile jsonFile(menuFile);
    if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonBytes  = jsonFile.readAll();
        QJsonParseError  parseError;

        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes, &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            QJsonObject jsonRoot = jsonDoc.object();
            build(m_trayMenu, jsonRoot);
        } else {
            long lines;

            QString errline = getErrorLine(jsonBytes, parseError.offset, lines);
            QString errmsg  = parseError.errorString();

            errline = errline.trimmed();
            QString errMsg = QString("%1\nLine %2 - %3").arg(errline).arg(lines).arg(errmsg);

            QMessageBox::warning(NULL, tr("Parse Error"), errMsg);
        }
    }

    addQuit();
    return m_trayMenu;
}

QMenu* JsonMenu::build(QMenu* parent, const QJsonObject& object)
{
    if (!parent) return NULL;

    for (QJsonObject::const_iterator it = object.begin(); it != object.end(); ++it) {
        QJsonValue value = it.value();

        if (value.isObject()) {
           QMenu* child = parent->addMenu(it.key());
           build(child, value.toObject());
        } else if (value.isString()) {
            QAction* action = new QAction(it.key(), parent);
            action->setData(value);

            parent->addAction(action);
            connect(action, SIGNAL(triggered()), this, SLOT(copyClipboard()));
        } else if (value.isNull()) {
            parent->addSeparator();
        }
    }

    return parent;
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
    if (m_elapsed){
        qDebug() << "[T] Elapsed = " << m_elapsed->elapsed();
    }

    QClipboard* clipboard = QApplication::clipboard();
    if (!!clipboard) {
        clipboard->clear();
    }
}

void JsonMenu::copyClipboard()
{
    if (m_elapsed) delete m_elapsed;

    m_elapsed = new QTime();
    m_elapsed->start();

    m_timer->start(5 * MSEC); // clear text after 5 sec

    QClipboard* clipboard = QApplication::clipboard();
    QAction* action = qobject_cast<QAction*>(sender());
    if (!!action && !!clipboard) {
        QString text = action->data().toString();
        clipboard->setText(text);
    }
}


