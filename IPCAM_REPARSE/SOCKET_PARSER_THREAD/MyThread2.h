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
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv/cv.h"

class Viewer : public QWidget {
    Q_OBJECT

public:
    Viewer(QWidget *parent = 0);
	void setImage(QByteArray frame);
	~Viewer();
	void paintEvent(QPaintEvent *event);
	void update();
	cv::Mat qImage2Mat(const QImage &src);

public slots:

private:
	QWidget scene;
    QString path;
	QImage source;
	QLabel view;
    //vec2 translate;
};

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
		int jfif_start = 0;
		int jfif_end = 0;
		int frame_counter = 0;
		int frames_per_sec = 0;
		QElapsedTimer timer;

		qint64 bytesReceived, datalen;
		qint64 next_block_size =0 ;
		int chunk_size = 1024;
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
