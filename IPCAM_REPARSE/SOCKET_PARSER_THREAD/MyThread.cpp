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
#define LOG_LEVEL 0
#define IMWRITE 0

using namespace cv;
using namespace std;

Viewer::Viewer(QWidget *parent) : QWidget(parent)
{
	this->setFixedSize(640, 480);
	this->show();
	//connect(quit,SIGNAL(triggered()),qApp,SLOT(quit()));
}

Mat Viewer::qImage2Mat(const QImage &src)
{
    Mat res;
    return res;
}

void Viewer::setImage(QByteArray frame)
{
	//const int w=source.width(), h=source.height(), depth = 4, stride = w*depth, W = w, H = h;
	//QImage image(640,480,QImage::Format_RGB32);
	QImage copy = QImage::fromData(frame,"MJPG");  //QImage(frame,W,H,QImage::Format_RGB32);
}

void Viewer::paintEvent(QPaintEvent *event)
{
 QPainter painter;
 painter.begin(this);
 painter.drawImage(QPoint(0, 0), source);
 painter.end();
}

void Viewer::update() { repaint(); }
Viewer::~Viewer() { delete this; }

void createAlphaMat(cv::Mat &mat)
{
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            Vec4b& rgba = mat.at<Vec4b>(i, j);
            //rgba[0] = 200; //UCHAR_MAX;
            rgba[1] = saturate_cast<uchar>((float (mat.cols - j)) / ((float)mat.cols) * UCHAR_MAX);
            //rgba[2] = saturate_cast<uchar>((float (mat.rows - i)) / ((float)mat.rows) * UCHAR_MAX);
            rgba[3] = saturate_cast<uchar>(0.1 * (rgba[1] + rgba[2]));
        }
    }
}

MyTcpSocket::MyTcpSocket(QObject *parent) :  QObject(parent) {}
void MyTcpSocket::doConnect()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("192.168.1.198", 80);

	QByteArray header;
	QByteArray lineEnd("\r\n");
	header.append("GET /video.cgi?.mjpg HTTP/1.1" + lineEnd);
	header.append("Host: 192.168.1.198" + lineEnd);
	header.append("User-Agent: python-requests/2.14.2" + lineEnd);
	header.append("Accept-Encoding: gzip, deflate" + lineEnd);
	header.append("Accept: */*" + lineEnd);
	header.append("Connection: keep-alive" + lineEnd);
	header.append("\r\r\n\r\n");

    if(socket->waitForConnected(1000))
    {
		if(LOG_LEVEL>0)
			qDebug() << "Connected!";
		if(LOG_LEVEL>0)
			qDebug() << "Reading: " << socket->bytesAvailable();

		timer1.start();
		timer2.start();
		timer3.start();

		QByteArray buffer;
		QByteArray curr_frame;
		QImage copy;
		Mat mat;
		Mat edges;
		Mat img_hist_equalized;
		socket->setReadBufferSize(32000);
		socket->write((QByteArray)header);
		socket->waitForBytesWritten();

		while(true){
			if(!socket->waitForReadyRead())
			{
				qDebug()<< "timed out";
				exit(-1);
			}
			//if(socket->bytesAvailable()>1024*2){}
				buffer +=socket->read(1024*8);
			//if(timer3.hasExpired(1500)){timer3.restart();}

			int bbb = waitKey(1);
			if(bbb == 27 ) { exit(0); }

			jfif_start = buffer.indexOf("\xff\xd8");
			jfif_end = buffer.indexOf("\xff\xd9");
			if (jfif_start>0 && jfif_end>0)
			{
				curr_frame = buffer.mid(jfif_start, jfif_start+jfif_end+2);
				jfif_end = -1;
				jfif_start = -1;
				buffer.clear();
				if(LOG_LEVEL>0)
					qDebug() <<" JFIF Found";

				frames_per_sec++;
				frame_counter++;

				if(curr_frame.length()>0){
					//timer1.restart();
					copy = QImage::fromData(curr_frame,"MJPG");  //QImage(frame,W,H,QImage::Format_RGB32);
					int h = copy.height();
					int w = copy.width();
					if (h>100 && w>100){
						uchar *b2 = copy.bits();
						mat = Mat(h, w, CV_8UC4, b2);
						cvtColor(mat, edges, COLOR_RGB2GRAY);
						imshow("OpenCV Live IPcam", edges);
					}
					//socket->flush();
				}

				if(timer2.hasExpired(1000) && IMWRITE)
				{
					string fn2 = QString("sample-" + QString(std::to_string(frame_counter).c_str()) + ".jpg").toStdString();
					char* foo= &fn2[0u];
					if(IMWRITE>0)
						imwrite(foo, edges);
					qDebug() << frames_per_sec;
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
	namedWindow("OpenCV Live IPcam", CV_WINDOW_AUTOSIZE);
	this->run();
}

void MyThread::run()
{
	qDebug() << "Run...";
	MyTcpSocket *s;
	s->doConnect();
}

MyThread::~MyThread()
{
	destroyAllWindows();
	delete this;
}