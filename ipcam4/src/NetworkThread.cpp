#include <QDebug>
#include <QtCore>
#include <QObject>
#include <QtNetwork>
#include <QString>
// #include <omp.h>
extern int parallelism_enabled;

#include "NetworkThread.h"

using namespace std::chrono_literals;

// QObject(nullptr) is defined
NetworkSocket::NetworkSocket(QObject *) : QObject(nullptr), eventLoop(this)
{
}

/* This class just opens a insecure socket shared all across the thread */
void NetworkSocket::OpenSocket(QString ipaddress, QString url)
{
    std::this_thread::sleep_for(1000ms);
    if (firstRun)
    {
        firstRun = false;
        // threadCount.operator++();
        // qDebug() << "Thread count: " << threadCount.load();

        qDebug() << "Target ip address: " << ipaddress;
        qDebug() << "Target url: " << url;

        QElapsedTimer *etim = new QElapsedTimer();
        etim->start();

        QByteArray ba = ipaddress.toLatin1();
        host = ba.data();
        socket = new QTcpSocket;
        socket->connectToHost(host, port);
        *header+="GET " + url + " HTTP/1.1" + *lineEnd;
        *header+="Host: "  + ipaddress + *lineEnd;
        *header+="User-Agent: python-requests/2.14.2" + *lineEnd;
        *header+="Accept-Encoding: gzip, deflate" + *lineEnd;
        *header+="Accept: */*" + *lineEnd;
        *header+="Connection: keep-alive" + *lineEnd;
        *header+="\r\r\n\r\n";
        socket->setReadBufferSize( bufsize.loadAcquire() );

        //socket->write(static_cast<QByteArray>(header));
        socket->write(*header);
        socket->waitForBytesWritten();

        qint64 timed = etim->msecsSinceReference();
        qDebug() << "OpenSocket timed " << timed;
    }
    //eventLoop.processEvents();
}

/*
 * This class loops through atomic operators for three kinds of memory operations
 * 1. Costly hardware calls to the network stack
 * 2. Rather costly buffer search and memory copy
 * 3. Slow timer call for collecting stats
 * The while loop is superfast and then
 * as we add Interlocked Boolean Pointers
 * compiler relocates the code to the shortest path possible
 * at the cycle nest, and then costly memory calls are mutexed
 * not memory fenced at the second nest which operates on the bus.
 * Then we need a few things,
 * 1. Interlock all operations to atomic pointers inside the while loop
 * 2. Invoke costly memory operations from a different thread working at the bus.
*/
void NetworkSocket::ReadSocketEmitFrame()
{
    if (socket->isOpen())
        emit export_statistics(1,1);
    timer.start(); // its a high resolution timer...

    jfif_end_position.storeRelease(nullptr);
    jfif_start_position.storeRelease(nullptr);

    while(!_stopping.loadAcquire()){
        // Useless if nesting on a costly initial buffer expectation
        // Rather move it outside the superfast while loop,
        if(socket->waitForConnected(2400))
        {
            bool isSocketReady = socket->waitForReadyRead();
            _data_ready.storeRelease( &isSocketReady );

            // Type one error, slow error catching inside Atomic while
            try
            {
                if(_data_ready.loadAcquire())
                    // long and costly mem-copy from network socket stack
                    // into buffer object. better of without locks
                    // and using a memory fence. right?
                    buffer +=socket->read(bufsize.loadAcquire());
            }
            catch(QException)
            {
                qDebug() << "Error: No data.";
                bool x;
                x=true;
                _stopping.storeRelease(&x);
            }

            /*
            * Type 3 error: unbound string buffer search in Atomic While
            * This is the slowest routine
            * Since it loops a string for a pattern
            * We need a flip-flop handling the current state of buffer reads
            * TODO: Options for buffer are QDataStrean and QTextStream
            */

            int _jfifstart = buffer.indexOf("\xff\xd8");
            int _jfifend = buffer.indexOf("\xff\xd9");

            jfif_start_position.storeRelease( &_jfifstart );
            jfif_end_position.storeRelease( &_jfifend );

            jfif_start.storeRelease( jfif_start_position );
            jfif_end.storeRelease( jfif_end_position );


            if ( *jfif_start.loadAcquire()==-1 or *jfif_end.loadAcquire()==-1)
                continue;

            if ( jfif_start.loadAcquire() < jfif_end.loadAcquire() ) // bad comparison
            {
                bool yes;
                yes = true;
                buffer_is_locked.storeRelease(&yes);
            }
            else
            {
                bool no;
                no = false;
                buffer_is_locked.storeRelease(&no);
            }

            // Type 4 error: buffer operation hanging on a Atomic spinlock
            if ( buffer_is_locked.loadAcquire() )
            {
                // Type 5 error: multiple unbound calls to memory copy.
                // This is the actual buffer extraction from A to B
                // This is rather a slower process of buffer slicing and memory copy
                // Could this require a Worker or a Runnable on its own?

                curr_frame = buffer.mid(*jfif_start.loadAcquire(), *jfif_start.loadAcquire()+*jfif_end.loadAcquire()+2);
            }

            // Type 4 error: buffer operation hanging on a Atomic spinlock
            if ( buffer_is_locked.loadAcquire() )
            {
                // Type 6 error: object emit call without a stack-pointer
                // wait states have to be inserted betwen Interlocks
                // in order to preserve cpu from stalling doing
                // a series of allocation and deallocations of large memory block,
                //QByteArray x = *curr_frame.loadAcquire();
                emit export_current_frame( curr_frame );

                buffer.clear();

                bool yes;
                yes= true;
                buffer_is_locked.storeRelease(&yes);

                frame_size.storeRelease( *jfif_end.loadAcquire() - *jfif_start.loadAcquire() );
                jfif_end.storeRelease(nullptr);
                jfif_start.storeRelease(nullptr);
                ++frames_per_sec;
                ++frame_counter;

                // Type 2 error: unknown cost of hardware timer call and timer counter evaluation
                // rather slow or non-atomic timer operation
                // improve it adding a interlocked flag which skips cycle-by-cycle costly call.
                /*
                if(timer.hasExpired(1000))
                {
                    emit export_statistics(frames_per_sec, frame_counter);
                    frames_per_sec.storeRelease( 0 );
                    timer.restart();
                }
                */
                // Type 7 error: garbage collect of unknown Interlocking condition.
                // rather slow buffer garbage collecting
                // how to simply discard the pointer of buffer
                // instead of doing a costly nested write?
            }
        }
        else
        {
            qDebug() << "Packets dropped.";
            bool x;
            x=true;
            _stopping.storeRelease(&x);
        }
    } // termina loop while

    socket->flush();
    socket->close();
}

// Disposals
void NetworkSocket::killSocket()
{
    bool x=true;
    _stopping.storeRelease(&x);
}
