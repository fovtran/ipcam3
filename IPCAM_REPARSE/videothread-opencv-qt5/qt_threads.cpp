#define _WIN32_DCOM
// #include <comdef.h>
#include <iostream>
#include <thread>
#include <vector>
#include <QDebug>
#include <QtCore>
#include <QString>
#include <QObject>
#include <QProcess>
#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <QApplication>

#include "MyThread.h"

using namespace std;

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	qDebug() << "Starting";
	QThreadPool *pool = new QThreadPool();
	pool->setMaxThreadCount (1);
	MyThread *thread = new MyThread;
		pool->start(thread);
		

	return app.exec();
}
