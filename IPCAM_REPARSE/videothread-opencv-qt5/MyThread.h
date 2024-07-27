#include <QDebug>
#include <QtCore>
#include <QString>
#include <QObject>
#include <QProcess>
#include <QRunnable>
#include <QtNetwork>
#include <QTcpSocket>
#include <QAbstractSocket>

#include <QMainWindow>
#include <QImage>
#include <QWidget>
#include <QPainter>
#include <QLabel>

#include <opencv2/core/core.hpp>

class MyTcpSocket : public QObject
{
    Q_OBJECT

	public:
		explicit MyTcpSocket(QObject *parent = 0);
		void doConnect();

	signals:

	public slots:

	protected:
		QTcpSocket *socket;

	private:
		int jfif_start = -1;
		int jfif_end = -1;
		QAtomicInt frame_counter = 0;
		QAtomicInt frames_per_sec = 0;
		QElapsedTimer timer1;
		QElapsedTimer timer2;
		QElapsedTimer timer3;

		qint64 bytesReceived, datalen;
};

class MyThread : public QObject, public QRunnable
{
	Q_OBJECT

	public:
		MyThread();
		~MyThread();
		void run();

	private:
		int _socket;
		//QMutex *m_;
		QWaitCondition *_cond;
		QNetworkAccessManager *manager;
		QNetworkReply *reply;
		QNetworkRequest q;
};
