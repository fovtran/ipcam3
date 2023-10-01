#ifndef MAINTHREADIMPL_H
#define MAINTHREADIMPL_H

#include <QtCore>
#include <QString>
#include <QObject>
#include <QThread>
#include <QImage>

#include "ThreadsController.h"

class MainThreadImpl: public QThread
{
    Q_OBJECT
    Q_COMPILER_THREAD_LOCAL

    public:
        explicit MainThreadImpl(QObject* parent = nullptr);
        ~MainThreadImpl();
        void run();
        void stop();
        WidgetThread *videowidgetthread;
        NetworkThread *networkthread;
        QImage current_frame;

    signals:
        void enableCanvas();
        void frame_ready(QImage);

    public slots:
        void display_stats(int,int);
        void emitToCanvas(QByteArray);

    protected:
        int counter;

    private:
        QEventLoop eventLoop;
};

#endif // MAINTHREADIMPL_H
