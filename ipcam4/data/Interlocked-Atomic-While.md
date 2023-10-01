/* Structure of calls */
1. Program.cpp
- MainThreadImpl


/* This class just opens a socket */
QTcpSocket *MyTcpSocket::OpenSocket(QString ipaddress)
{
	if (firstRun)
	{
		firstRun = false;
		socket = new QTcpSocket;
		socket->connectToHost(host, port);
		lineEnd = "\r\n";
		header.append("GET /video.cgi?.mjpg HTTP/1.1" + lineEnd);
		}
		return socket;
}

/*
 * - This class loops through atomic operators for three kinds of memory operations
 * 1. Costly hardware calls to the network stack
 * 2. Rather costly buffer search and memory copy
 * 3. Slow timer call for collecting stats
 * *	The while loop is superfast and then
 * *	as we add Interlocked Boolean Pointers
 * *	compiler relocates the code to the shortest path possible
 * *	at the cycle nest, and then costly memory calls are mutexed
 * *	not memory fenced at the second nest which operates on the bus.
 * - Then we need a few things,
 * 1. Interlock all operations to atomic pointers inside the while loop
 * 2. Invoke costly memory operations from a different thread working at the bus.
*/
void MyTcpSocket::ConnectAndEmit(QTcpSocket *s)
{
	timer.start();

	while(!_stopping.loadAcquire()){
		// Useless if nesting on a costly initial buffer expectation
		// Rather move it outside the superfast while loop,
		if(s->waitForConnected(2000))
		{
			bool ss = s->waitForReadyRead();
			_data_ready.storeRelease( &ss );

			// Type one error, slow error catching inside Atomic while
			try
			{
				if(_data_ready.loadAcquire())
					// long and costly mem-copy from network socket stack
					// into buffer object. better of without locks
					// and using a memory fence. right?
					buffer +=s->read(bufsize.loadAcquire());
					}
			catch(QException)
			{
				bool x;
				x=true;
				_stopping.storeRelease(&x);
				}

			// Type 2 error: unknown cost of hardware timer call and timer counter evaluation
			// rather slow or non-atomic timer operation
			// improve it adding a interlocked flag
			// which skips cycle-by-cycle costly call.
			if(timer.hasExpired(1000))
				emit export_statistics(frames_per_sec);

			// Type 3 error: unbound string buffer search in Atomic While
			// This is the slowest routine
			// Since it loops a string for a pattern
			// We need a flip-flop handling the current state of buffer reads
			int jfif_start_position = buffer.indexOf("\xff\xd8");
			int jfif_end_position = buffer.indexOf("\xff\xd9");
			jfif_start.storeRelease( jfif_start_position );jfif_end.storeRelease( jfif_end_position );

			if ( jfif_start.loadAcquire() < jfif_end.loadAcquire() )
				buffer_is_locked.storeRelease(&yes);

			// Type 4 error: buffer operation hanging on a Atomic spinlock
			if ( buffer_is_locked.loadAcquire() )
			{
				buffer_is_locked.storeRelease(&yes);

				// Type 5 error: multiple unbound calls to memory copy.
				// This is the actual buffer extraction from A to B
				// This is rather a slower process of buffer slicing and memory copy
				// Could this require a Worker or a Runnable on its own?
				curr_frame = buffer.mid(jfif_start.loadAcquire(), jfif_start.loadAcquire()+jfif_end.loadAcquire()+2);

				// Type 6 error: object emit call without a stack-pointer
				// wait states have to be inserted betwen Interlocks
				// in order to preserve cpu from stalling doing
				// a series of allocation and deallocations of large memory block,
				emit export_current_frame(curr_frame);

				frame_size.storeRelease( jfif_end.loadAcquire() - jfif_start.loadAcquire() );
				jfif_end.storeRelease(0);
				jfif_start.storeRelease(0);

				// Type 7 error: garbage collect of unknown Interlocking condition.
				// rather slow buffer garbage collecting
				// how to simply discard the pointer of buffer
				// instead of doing a costly nested write?
				buffer.clear();
				}
			}
		else
			_stopping.storeRelease(&x);
			} // termina loop while
}
