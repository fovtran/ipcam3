#ifndef MYTCPSOCKET_H_
#define MYTCPSOCKET_H_

#include <QtCore>
#include <QString>
#include <QObject>
#include <QTcpSocket>

#define LOG_LEVEL 0

class NetworkSocket : public QObject
{
    char X0=0;
    Q_OBJECT;
    Q_COMPILER_THREAD_LOCAL;

signals:
    void export_statistics(int, int);
    void export_current_frame(QByteArray);
    void enable();

public slots:
    void killSocket();

    public:
        explicit NetworkSocket(QObject* parent=nullptr);
        void OpenSocket(QString,QString);
        void ReadSocketEmitFrame();
        char x1,x2,x3,x4,x5,x6,x7=0;
        QTcpSocket *socket;

    protected:
        QByteArray buffer;
        QByteArray curr_frame;
        QElapsedTimer timer;
        QByteArray *header = new QByteArray("");
        QByteArray *lineEnd = new QByteArray("\r\n");

    private:
        //QByteArray *lineEnd = new QByteArray("\r\n");
        QEventLoop eventLoop;
        bool firstRun = true;
        char a,b,c,d,e,f,g = 0; // padding 7

        QAtomicPointer<bool> _stopping;
        QAtomicPointer<bool> _data_ready;
        QAtomicPointer<bool> buffer_is_locked;
        QAtomicPointer<int> jfif_start_position;
        QAtomicPointer<int> jfif_end_position;
        QAtomicPointer<int> jfif_start;
        QAtomicPointer<int> jfif_end;

        QAtomicInt threadCount = 0;
        QAtomicInt frame_size = 0;
        QAtomicInt frame_counter = 0;
        QAtomicInt frames_per_sec = 0;
        QAtomicInt chunk_size = 1448;
        QAtomicInt buffer_count = 1;
        QAtomicInt bufsize = chunk_size*buffer_count;

        char h,i,j,k=0; // padding 4
        const char* host;
        quint16 port = 80;
};

#endif
