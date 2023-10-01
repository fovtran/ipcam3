#ifndef MYTHREAD_H_
#define MYTHREAD_H_

#include <QtCore>
#include <QString>
#include <QObject>

#include "MainWindowWidget.h"
#include "NetworkThread.h"

class WidgetThread : public QThread
{
    Q_OBJECT;
    Q_COMPILER_THREAD_LOCAL;

    public:
        explicit WidgetThread(QObject *parent = nullptr);
        ~WidgetThread();
        void run();
        void stop();

    signals:
        void ImageReady(QImage);

    protected:
        MainWindoWidget* videowidget;

    private:
        QEventLoop eventLoop;
};

class NetworkThread : public QThread//public QObject, public QRunnable
{
    Q_OBJECT

    public:
        explicit NetworkThread(QObject *parent = nullptr);
        ~NetworkThread();
        void run();
        void stop();
        NetworkSocket* httpStreamer;

    signals:
        void frame_ready(QByteArray);
        void statistics_ready(int,int);

    public slots:
        void network_statistics(int,int);
        void send_frame(QByteArray);

    protected:
        int counter2;

    private:
        QEventLoop eventLoop;
};

#endif
