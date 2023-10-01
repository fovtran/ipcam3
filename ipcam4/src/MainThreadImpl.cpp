#include <QDebug>
#include <QtCore>
#include <QObject>
#include <QThread>
#include <QString>

#include "ThreadsController.h"
#include "XmlConfigReader.h"
#include "MainThreadImpl.h"

using namespace std;

MainThreadImpl::MainThreadImpl(QObject *parent) : eventLoop(this)
{
    this->setParent(parent);
    this->setObjectName("MainThread");
    //this->start(QThread::LowPriority);
    eventLoop.processEvents();
    qDebug() << "NEVER SHOULD REACH THIS";
}

void MainThreadImpl::run()
{
    eventLoop.processEvents();
    // QThreadPool* pool2 = new QThreadPool();
    // pool2 ->setMaxThreadCount (1);
    // pool2->start(networkthread); // its a locking event

    // using QThread(this) solves the parent connection of slots
    networkthread = new NetworkThread(this);
    videowidgetthread = new WidgetThread(this);

    QThread *networkpool = new QThread();
    networkpool->moveToThread(networkthread);
    networkthread->start();
    networkthread->setPriority(QThread::LowPriority);
    //networkthread->setPriority(QThread::TimeCriticalPriority);
    networkthread->setObjectName("Network");
    networkthread->setProperty("Network", 0);

    QThread *renderpool = new QThread();
    renderpool->moveToThread(videowidgetthread);
    videowidgetthread->start();
    videowidgetthread->setPriority(QThread::LowPriority);
    //videowidgetthread->setPriority(QThread::TimeCriticalPriority);
    videowidgetthread->setObjectName("Canvas");
    videowidgetthread->setProperty("Canvas", 0);

    connect(networkthread, &NetworkThread::statistics_ready, this, &MainThreadImpl::display_stats);

    if (renderpool->isRunning() and networkpool->isRunning())
        qDebug() << "MainLoop reached";

    //videowidgetthread->wait();
}

// Signal Slots
void MainThreadImpl::emitToCanvas(QByteArray dataframe)
{
    current_frame.loadFromData(dataframe);
    emit videowidgetthread->ImageReady(current_frame);
}
void MainThreadImpl::display_stats(int stats, int counts)
{
    qDebug() << "stats - " << stats << " counts " << counts;
}

// Disposals
MainThreadImpl::~MainThreadImpl()
{
    qDebug() << "MainLoop disposal";
    delete this;
}
void MainThreadImpl::stop() {}
