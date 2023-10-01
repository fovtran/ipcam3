#include <QDebug>
#include <QtCore>
#include <QObject>
#include <QThread>
#include <QString>

#include "ThreadsController.h"
#include "MainWindowWidget.h"
#include "XmlConfigReader.h"

using namespace std;

// Runs in a QThread
WidgetThread::WidgetThread(QObject *parent) : eventLoop(this)
{
    videowidget = new MainWindoWidget(parent);
    //connect(videowidget, SIGNAL(_ShowVideoCanvas()), videowidget, SLOT(show()) );
    connect(this, &WidgetThread::ImageReady, videowidget, &MainWindoWidget::setImage, Qt::DirectConnection);

    qDebug() << "WidgetThread reached" << QThread::currentThread();
}

// Runs in a QThread
NetworkThread::NetworkThread(QObject* parent)  : eventLoop(this)
{
    httpStreamer = new NetworkSocket(this);
    connect(httpStreamer, &NetworkSocket::export_statistics, this, &NetworkThread::network_statistics );
    connect(httpStreamer, SIGNAL(export_current_frame(QByteArray)), parent, SLOT(emitToCanvas(QByteArray)), Qt::QueuedConnection);

    qDebug() << "NetworkThread reached" << QThread::currentThread();
    //eventLoop.processEvents();
}

void WidgetThread::run()
{
    qDebug() << "WidgetThread::run" << QThread::currentThread();
    //emit videowidget->Show();
    //emit videowidget->Enabled(true);
    videowidget->setEnabled(true);
    videowidget->setVisible(true);
}

/* NetworkThread setups the socket thread stream */
void NetworkThread::run()
{
    qDebug() << "NetworkThread run" << QThread::currentThread();
    // eventLoop.processEvents();
    /* OpenSocket and ConnectEmit should be made stateful here */
    XmlConfigReader conf;
    conf.ReadConfigFile("config/config.xml");
    httpStreamer->OpenSocket(conf.ipaddress, conf.url);
    httpStreamer->ReadSocketEmitFrame();
    delete &conf;
}

// Signal Slots
void NetworkThread::send_frame(QByteArray frame)
{
    emit frame_ready(frame);
}

void NetworkThread::network_statistics(int stats, int counts)
{
    qDebug() << "Statistics: ";
    emit statistics_ready(stats,counts);
}

// Disposals
void WidgetThread::stop() { }
void NetworkThread::stop() { }
WidgetThread::~WidgetThread() { delete this;}
NetworkThread::~NetworkThread() { delete this; }
