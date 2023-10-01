#include <QDebug>
#include <QtCore>
#include <QObject>
#include <QApplication>
#include <QString>
#include <QStyleFactory>
#include <QStandardItemModel>
#include <Qt>
#include "MainThreadImpl.h"

using namespace std; // λ**=&_

// #define qApp QCoreApplication::instance()
// QSessionManager
// QFileSystemModel
// QFileSystemWatcher
// QGenericPluginFactory
// QGenericPlugin
// QStyleHints

QCoreApplication* createApplication(int &argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (!qstrcmp(argv[i], "-no-threads"))
            return new QCoreApplication(argc, argv);
    }
    return new QApplication(argc, argv);
}

int main(int argc, char *argv[]) {
    QScopedPointer<QCoreApplication> app(createApplication(argc, argv));
    // int r,c;
    // QStandardItemModel* model = new QStandardItemModel(r,c,app.data());

        QIcon q("./resources/thumb.png");
        //qApp->setWindowIcon(q);
        //app.setStyle(QStyleFactory::create("Fusion"));

        if (qobject_cast<QApplication *>(app.data())) {
                app->setObjectName("MainQT");
                app->setApplicationName("QTIPCAM");
                app->setApplicationVersion("1.0");
                Qt::ApplicationAttribute attribute;
                app->setAttribute(attribute, false);
                MainThreadImpl* c = new MainThreadImpl();
                app->thread()->moveToThread(c);
                c->start();
                qDebug() << c->objectName();
                app->processEvents();
        } else {
           // start non-GUI version...
           qDebug() << "Non-threaded run";
        }

    qDebug() << "Application running";

// #include <QThread>
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QPushButton>
// #include <QPainter>
// #include <QDateTime>
// #include <QEventLoop>
// #include <QStyleFactory>
// #include <QScopedPointer>
// #include <QByteArray>
// #include <QPixmap>
// #include <qendian.h>
// #include <QDateTime>
// #include <QRunnable>
    //qDebug() << "MainWindowWidget reached";
    // QScopedPointer<RenderArea> window = new RenderArea);
    //QWidget *const windowPtr = window->take(); // ownership transferred
    //QScopedPointer<QVBoxLayout> layout(new QVBoxLayout);
    //QScopedPointer<QAction> save();
    //QScopedPointer<QPushButton> button1(new QPushButton());
    //QScopedPointer<QHBoxLayout> bar(new QHBoxLayout);
    //QPushButton *button1(new QPushButton());
    //button1->setMinimumSize(50,50);
    //bar->addWidget(button1);
    //window->setLayout(bar.data());
    //bar.take();
    //layout->addWidget(renderArea);
    // renderArea->update();
    //window->setLayout(layout.data());
    // layout.take(); // ownership transferred
    //setCentralWidget(window.data());
    //qDebug() << "MainWindowWidget show";
    //windowPtr->show();
    /*
    //QThreadPool::globalInstance()->setMaxThreadCount (1);
    //QThreadPool::globalInstance()->start(c);
    //QThread *pool = new QThread();
    //pool->moveToThread(c);
    //app.sync();
    //for(int i = 0; i < 100000000; ++i) app.processEvents();
    */
    // QObject::connect(&app, &QApplication::aboutToQuit, c->get, &QWidget::deleteLater);
    int p = app->exec();
    return p;
}
