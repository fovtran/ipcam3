#include "MyTcpSocket.h"

//using namespace std;
//extern int parallelism_enabled;

void MyTcpSocket::killSocket() { bool x=true; _dead.storeRelease(&x); }

MyTcpSocket::MyTcpSocket(QObject *parent) : QObject(parent), eventLoop(this){}

QTcpSocket *MyTcpSocket::getSocket(QString ipaddress)
{
	if (firstRun)
	{
		firstRun = false;
		threadCount++;
		//int total = omp_get_num_threads();
		// qDebug() << "ThreadCount: " << threadCount << "IP: " << ipaddress;
		QByteArray ba = ipaddress.toLatin1();
		host = ba.data();
		socket = new QTcpSocket;
		socket->connectToHost(host, port);
		lineEnd = "\r\n";
		header.append("GET /video.cgi?.mjpg HTTP/1.1" + lineEnd);
		header.append("Host: "  + ipaddress + lineEnd);
		header.append("User-Agent: python-requests/2.14.2" + lineEnd);
		//header.append("Accept-Encoding: gzip, deflate" + lineEnd);
		header.append("Accept: */*" + lineEnd);
		header.append("Connection: keep-alive" + lineEnd);
		header.append("\r\r\n\r\n");
		socket->setReadBufferSize(200);
		socket->write((QByteArray)header);
		socket->waitForBytesWritten();
	}
	//eventLoop.processEvents();
		return socket;
}

//#pragma omp parallel if(parallelism_enabled)

void MyTcpSocket::doConnect(QTcpSocket *s)
{
	//bool x; x=false; _dead.storeRelease(&x);
//	qDebug() << "Connecting...";
	timer.start();
	qDebug() << "Timer started...";

	if(s->waitForConnected(1500))
	{
		s->waitForReadyRead();
		s->read(200);
		
	while(!_dead.loadAcquire()){	
		(void*)0;
		while(s->bytesAvailable())
				if(s->bytesAvailable())
				{
					qDebug()<<s->bytesAvailable();
					buffer = s->read();
				}
			//qDebug()<< (QAbstractSocket::ConnectedState == socket->state());
			//qint64 bytesAvail=s->bytesAvailable();
			//if(s->isSequential())	qDebug()<<"SEQ";
			
			char* data;
			qint64 bufz=2048;
			/*
			while(s->waitForReadyRead())
			{
				//qint64 xyz = s->readData(data,bufz);
				buffer+=data;
				buffer.clear();
				//qDebug()<<bytesAvail;	
				//s->flush();
			}				
				try {					
				} catch(std::exception e) { 
					qDebug() << " Connection dropped."; bool x; x=true; _dead.storeRelease(&x); 
				}	*/

			if(!buffer.isNull())
			{
			jfif_start = buffer.indexOf("\xff\xd8");
			jfif_end = buffer.indexOf("\xff\xd9");
			
			if (jfif_start<jfif_end)
			{
				QByteArray xy;
				xy = buffer.mid(jfif_start+1, jfif_start+jfif_end+2);
				
				//sem.acquire(256);
				/*if(timer.hasExpired(1000)){ timer.restart();emit export_statistics(frames_per_sec);frames_per_sec = 0; }*/

				frame_size = jfif_end - jfif_start;				
				jfif_end = 0;
				jfif_start = 0;
				++frames_per_sec; ++frame_counter;				

				if(timer.hasExpired(100))
				{
					buffer.clear();
					if(xy.size()!=frame_size)
					{
						timer.restart();
						qDebug() << "Frames: - " 
							<<frame_counter 
							<< "\t Loops: " 
							<< frames_per_sec
							<< "\t Framesize: " 
							<< xy.size() 
							<< " - " << frame_size;
					}
				}
			}

			}
		}
		}
		else
		{
			bool x; x=true; _dead.storeRelease(&x);
			qDebug() << "Not connected!";
		}
	s->flush();
	s->close();
}
