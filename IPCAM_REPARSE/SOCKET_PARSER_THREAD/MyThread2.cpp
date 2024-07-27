//#include <iostream>
//#include <sstream>
//#include <string>
//#include <thread>

#include <QDebug>
#include <QtCore>
#include <QString>
#include <QObject>
#include <QProcess>
#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <QtNetwork>

#include <vector>
//#include <stdio.h>
#include <QMainWindow>
#include <QImage>
#include <QWidget>
#include <QPainter>
#include <QLabel>

//#include <opencv2/opencv.hpp>
#include <vector>
//#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
//#include "opencv/cv.h"

#include "MyThread.h"

#define LOG_LEVEL 0

using namespace cv;
using namespace std;

Viewer::Viewer(QWidget *parent) : QWidget(parent)
{
	//QMainWindow *q = new QMainWindow(parent);
	//scene = new QWidget(q);
	//parent->setWindowTitle("IPCam Blaster");
	this->setFixedSize(640, 480);
	this->show();
	//q->setCentralWidget(&scene);
	//scene->show();
	//q->show();	
	//connect(quit,SIGNAL(triggered()),qApp,SLOT(quit()));
}

Mat Viewer::qImage2Mat(const QImage &src)
{
    //cv::Mat tmp(640,480, CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
    Mat res;
    //cv::cvtColor(tmp, res,CV_RGB2GRAY);
    return res;
}

void Viewer::setImage(QByteArray frame)
{
	//const int w=source.width(), h=source.height(), depth = 4, stride = w*depth, W = w, H = h;
	//QImage image(640,480,QImage::Format_RGB32);	
	
	QImage copy = QImage::fromData(frame,"MJPG");  //QImage(frame,W,H,QImage::Format_RGB32);
	//uchar* const src = copy.bits();
	//uchar* const dst = image.bits();	
	//source = QImage(reinterpret_cast<unsigned char*>(dst),W,H,QImage::Format_RGB32);
	//source = copy;
	//cv::Mat mat;
	//mat = cv::imdecode(qImage2Mat(copy), 1);
	//cv::cvNamedWindow("hello");
	//cv::imshow("hello",mat);
	//cvWaitKey();
}

void Viewer::paintEvent(QPaintEvent *event)
{
 QPainter painter;
 painter.begin(this);
 painter.drawImage(QPoint(0, 0), source);
 painter.end();
}

void Viewer::update() 
{
	//view.setPixmap(QPixmap::fromImage(source));
	repaint();
}

Viewer::~Viewer() { delete this; }

MyTcpSocket::MyTcpSocket(QObject *parent) :  QObject(parent) 
{
}

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

void MyTcpSocket::doConnect()
{

    socket = new QTcpSocket(this);
    socket->connectToHost("192.168.1.198", 80);

	QByteArray buffer;
	QByteArray curr_frame;
	QByteArray header;
	QByteArray lineEnd("\r\n");
	header.append("GET /video.cgi?.mjpg HTTP/1.1" + lineEnd);	
	header.append("Host: 192.168.1.198" + lineEnd);	
	header.append("User-Agent: python-requests/2.14.2" + lineEnd);			
	header.append("Accept-Encoding: gzip, deflate" + lineEnd);	
	header.append("Accept: */*" + lineEnd);		
	header.append("Connection: keep-alive" + lineEnd);	
	header.append("\r\r\n\r\n");		

    if(socket->waitForConnected(5000))
    {
		if(LOG_LEVEL>0)
			qDebug() << "Connected!";
		socket->setReadBufferSize(1024*16);
		socket->write((QByteArray)header);
		socket->waitForBytesWritten();
        
		if(LOG_LEVEL>0)
			qDebug() << "Reading: " << socket->bytesAvailable();

		timer.start();		
		Mat mat;		
		Mat edges;
		Mat img_hist_equalized;
		QImage copy;
		while(true){ 
			socket->waitForReadyRead(); 

			buffer +=socket->read(1024*16); 

			jfif_start = buffer.indexOf("\xff\xd8");
			jfif_end = buffer.indexOf("\xff\xd9");
			if (jfif_start>0 && jfif_end>0)
			{
				if(LOG_LEVEL>0)
					qDebug() <<" JFIF Found";

				curr_frame = buffer.mid(jfif_start, jfif_start+jfif_end+2);
				jfif_end = 0;
				
				buffer.clear();
				frames_per_sec +=1 ;
				frame_counter +=1;
				
				int bbb = waitKey(1);

				if(timer.hasExpired(50)){
				if(bbb != 27 ) {					
					copy = QImage::fromData(curr_frame,"MJPG");  //QImage(frame,W,H,QImage::Format_RGB32);
					mat = Mat(480, 640, CV_8UC4, copy.bits());							
					//createAlphaMat(mat);					
					//AdaptiveThreshold(im, im, 255, cv.CV_ADAPTIVE_THRESH_MEAN_C, cv.CV_THRESH_BINARY, blockSize=31, param1=15)	
					//VideoWriter::VideoWriter(const String& filename, int fourcc, double fps, Size frameSize, bool isColor=true)¶				
					cvtColor(mat, edges, COLOR_RGB2GRAY);
					//equalizeHist(edges, img_hist_equalized); //equalize the histogram
					//GaussianBlur(edges, edges, Size(5,5), 1.4, 1.4);
					//Canny(edges, edges, 5, 100, 3);
					imshow("OpenCV Live IPcam", edges);	
				}
				else { break; }
				}

				if(timer.hasExpired(1000))
				{					
					string fn2 = QString("sample-" + QString(std::to_string(frame_counter).c_str()) + ".jpg").toStdString();
					char* foo= &fn2[0u];
					imwrite(foo, edges);
					qDebug() << frames_per_sec;
					frames_per_sec = 0;
					timer.restart();
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