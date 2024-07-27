#include <QDebug>
#include <QtCore>
#include <QObject>
#include <QApplication>
#include <QString>
// #include <QProcess>
// #include <QRunnable>
// #include <QThread>
// #include <QThreadPool>
// #include <QtNetwork>
// #include <QTcpSocket>
// #include <QAbstractSocket>
// #include <QMainWindow>
// #include <QLabel>
// #include <QPainter>
// #include <vector>
// #include <iostream>
// #include <thread>
// #include <stdlib.h>
// #include <math.h>
#include <QStyleFactory>
#include "MainThreadImpl.h"
// #include "extraworker.h"

using namespace std; // λ**=&_

QCoreApplication* createApplication(int &argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (!qstrcmp(argv[i], "-no-gui"))
            return new QCoreApplication(argc, argv);
    }
    return new QApplication(argc, argv);
}

int main(int argc, char *argv[]) {
    Worker *w = new Worker();
    QThread *worker = new QThread();
    worker->moveToThread(w);
    w->start();
    w->wait();
    w->setPriority(QThread::TimeCriticalPriority);
    w->setObjectName("Worker");
    w->setProperty("Worker", 0);

    QThread *thread = QThread::create([]{ Worker().run(); });
    thread->start();

    QScopedPointer<QCoreApplication> app(createApplication(argc, argv));

        if (qobject_cast<QApplication *>(app.data())) {
            // QApplication app(argc, argv, true);
                app->setObjectName("MainQT");
                app->setApplicationName("QTIPCAM");
                app->setApplicationVersion("1.0");
                QIcon q("./resources/thumb.png");
                app.setWindowIcon(q);
                MainThreadImpl* c = new MainThreadImpl();
                app->thread()->moveToThread(c);
                c->start();
                qDebug() << c->objectName();
                app->processEvents();
        } else {
           // start non-GUI version...
        }
    qDebug() << "Application running";

    int p = app->exec();
    return p;
}
