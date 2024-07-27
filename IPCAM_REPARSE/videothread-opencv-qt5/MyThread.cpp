//#include <iostream>
//#include <sstream>
//#include <string>
//#include <thread>
//#include <stdio.h>
#include <vector>

#include <QDebug>
#include <QtCore>
#include <QString>
#include <QObject>
#include <QProcess>
#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <QtNetwork>
#include <QMainWindow>
#include <QImage>
#include <QWidget>
#include <QPainter>
#include <QLabel>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "MyThread.h"
#define LOG_LEVEL 1
#define IMWRITE 1

using namespace cv;
using namespace std;

MyTcpSocket::MyTcpSocket(QObject *parent) :  QObject(parent) {}
void MyTcpSocket::doConnect()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.168.2.198", 80);

	QByteArray header;
	QByteArray lineEnd("\r\n");
	header.append("GET /video.cgi?.mjpg HTTP/1.1" + lineEnd);
	header.append("Host: 192.168.2.198" + lineEnd);
	header.append("User-Agent: python-requests/2.14.2" + lineEnd);
	header.append("Accept-Encoding: gzip, deflate" + lineEnd);
	header.append("Accept: */*" + lineEnd);
	header.append("Connection: keep-alive" + lineEnd);
	header.append("\r\r\n\r\n");

    if(socket->waitForConnected(2000))
    {
		if(LOG_LEVEL>0)
			qDebug() << "Connected!";
		if(LOG_LEVEL>0)
			qDebug() << "Reading: " << socket->bytesAvailable();

		timer3.start();
		timer1.start();

		QByteArray buffer;
		QByteArray curr_frame;
		Mat img_hist_equalized;
		socket->setReadBufferSize(32000);
		socket->write((QByteArray)header);
		socket->waitForBytesWritten();

		while(true){

			if(socket->waitForReadyRead()){}
			if(socket->bytesAvailable()>1024)
			{
				buffer +=socket->read(2048);
			}
			jfif_start = buffer.indexOf("\xff\xd8");
			jfif_end = buffer.indexOf("\xff\xd9");
			if (jfif_start>0 && jfif_end>0)
			{
				curr_frame.clear();
				curr_frame = buffer.mid(jfif_start, jfif_start+jfif_end+1);				
				jfif_end = -1;
				jfif_start = -1;

				QImage copy;
				copy = QImage::fromData(curr_frame, "MJPG");  //QImage(frame,W,H,QImage::Format_RGB32);
				int w = copy.width();
				int h = copy.height();
				//qDebug() << "Frame: " << w << "x" << h;
				
				// err: loop faster than curr_frame refresh.

				if (h>400 && w>400){
					int bbb = waitKey(1);
					if(bbb == 27 ) { exit(0); }					
					uchar *b2 = copy.bits();
					Mat mat;					
					mat = Mat(h, w, CV_8UC4, b2);
					Mat edges;
					cvtColor(mat, edges, COLOR_RGB2GRAY);						
					imshow("OpenCV Live IPcam", edges);
				}				
				// socket->flush();
				
				if(timer1.hasExpired(150))
				{
					frames_per_sec++;
					//buffer.clear();	
					timer1.restart();
				}

				if(timer2.hasExpired(2000) && IMWRITE)
				{
					//string fn2 = QString("sample-" + QString(std::to_string(frame_counter).c_str()) + ".jpg").toStdString();
					//char* foo= &fn2[0u];
					//if(IMWRITE>0)
					//	imwrite(foo, edges);
					//qDebug() << frames_per_sec;
					frames_per_sec = 0;
					timer2.restart();
				}				
			}
			
		}

		socket->close();
    }
    else
    {
        qDebug() << "Not connected!";
    }
}

MyThread::MyThread()
{
	qDebug() << "MyThread - " << QThread::currentThread();
	namedWindow("OpenCV Live IPcam", WINDOW_AUTOSIZE);
	this->run();
}

void MyThread::run()
{
	qDebug() << "Run...";
	MyTcpSocket *s = new MyTcpSocket();
	s->doConnect();
}

MyThread::~MyThread()
{
	destroyAllWindows();
	delete this;
}
