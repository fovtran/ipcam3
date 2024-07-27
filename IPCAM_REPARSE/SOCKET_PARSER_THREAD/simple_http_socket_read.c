// g++ -g -O2 simple_http_socket_read.c && ./a.out
#include <arpa/inet.h>
// #include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <cstring>

// HTTP HEADER STRING
const char *http_header = "GET /IMAGE.JPG? HTTP/1.1\r\nhost: 192.168.2.45\r\nUser-agent: Mozilla/4.0\r\nAccept: */*\r\nConnection: keep-alive\r\nConnection: close\r\r\n\r\n";
const char* dst_addr = "192.168.2.198";
const int dst_port = 80;
const char* thats_what_it_takes_to_love_you = "/tmp/f3.jpg";

int main(){
	int clientSocket;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	/*---- Create the socket. The three arguments are: ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	serverAddr.sin_family = AF_INET;		/* Address family = Internet */
	serverAddr.sin_port = htons(dst_port);		/* Set port number, using htons function to use proper byte order */
	serverAddr.sin_addr.s_addr = inet_addr(dst_addr);	/* Set the IP address to desired host to connect to */
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  		/* Set all bits of the padding field to 0 */

	addr_size = sizeof serverAddr;		/*---- Connect the socket to the server using the address struct ----*/
	connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

	int status = send(clientSocket, http_header, strlen(http_header), 0);

	switch(status){
		case 0:
			printf("Error openning socket");
			break;
	}

	if (status>0)
	{
		printf("Header response size: %i \n", status);
		ssize_t bReceived;
		ssize_t bsize = 1024;
		char buffer[bsize];

		bReceived = recv(clientSocket, buffer, bsize, 0);

		//char *t2 = strstr(buffer, "Content-Length: "); //! why doesn't it handle the motherfucking whitespace
		char *t2 = "Content-Length: ";
		for(int n=0; n<=strlen(t2);n++)
			printf("`%c", t2[n]);

		//char *t3 = strstr(buffer, "\r\n"); 	//t2 = t2 + 16; t3 = t3 + 2;
		char *t3 = "\r\n";

		//* Vectorize string search to the fullest...
		//* Check if cpu does nested vectorization.??
		int nests=2;
		int distances[nests];
		int misses[nests];
		int len_of_ring0 = strlen(t2);  //* volatile.. why?
		int len_of_ring1 = strlen(t3); //* What the fuck is strlen doing?
		int bufferlen = strlen(buffer);
		printf("\nBuffer len strlen() is %i\n", strlen(buffer));

		for (int i = 0; i<=bufferlen; i++) //* i is defaulted to an atomic int
		{			
			//! Long fetch barrier; this will be our heap. if i'm correct,
			int buf0 = buffer[i];
			int buf1 = buffer[i+1];
			int buf2 = buffer[i+2];
			int cmp0 = t2[0];
			int cmp1 = t2[1];
			int cmp2 = t2[2];
			int distance = 0;
			int misses = 0;
			int begin_distance=0;
			int final_distance=0;
			int endline_distance=0;
			int cmprn1 = t3[0];
			int cmprn2 = t3[1];		
			
			//! lock short register ops. register vectorization may be smooth;
			if(buf0==cmp0)
			{
				if(buf1==cmp1)
				{					
					if(buf2==cmp2)
					{					
						//! Three level match from i
						//* What counters should approach better
						printf("Three match at %i\n", i);
						
						for(int n2=0; n2<len_of_ring0+1; n2++) //! violent and funky
						{							
							//! TODO: atomized counters are not intended to work this way
							//* TODO: atomized counters are not intended to work this way
							if(buffer[i+n2]==t2[n2])
							{
								//* MATCHES
								printf("-"); // crap debug
								//! barrier distance for atom store
								if(n2==len_of_ring0)
								{
									//! WARN: i'm, hot,  simplify number of variables or die!				
									printf(">Fully matches from %i to %i\n", i, i+n2); // crap debug
									begin_distance = i;
									for(int n3=i; n3<=i+len_of_ring0; n3++ )
									{
										printf("intermediate buffer %i is %c\n", n3, buffer[n3]);
										if(buffer[n3]==cmprn1)
											if(buffer[n3+1]==cmprn2)
												{
												endline_distance=n3+n2;
												break;
												}
									}
									final_distance=endline_distance; //! atom!!!
									break;
								}
							}
							else
							{
								//! barrier distance
								distance=0;
								misses++;
								break;
							}
						}
					}
				}
			}
			//! Another barrier for long stores before restarting search
			//! free after use volatiles; barriers can indeed be interrupted by hardware;
			//* Content matches, then motherfucking what, motherfucker.
			if (begin_distance+distance<final_distance)
			{
				printf("warp: %i final: %i \n", begin_distance+distance, final_distance); //* maomeno
				for (int n=begin_distance+distance; n<final_distance; n++)
					printf("`%c", buffer[n]);
			}

		}

		char *t = strstr(buffer, "\r\n\r\n");

		int bodySize = 0;
		t = t + 4;
		for (auto it = buffer; it != t; ++it) {
			++bodySize;
		}

		bodySize = bReceived - bodySize;

		FILE *fp;
		fp = fopen(thats_what_it_takes_to_love_you,"w");
		if(fp == NULL)
		{
			printf("Error!");   
			exit(1);             
		}
		
		printf("bytes after header: %i\n", strlen(t));
		//fprintf(fp,"%s",t);
		//fwrite(t, sizeof t[0], bodySize, fp);
		memset(buffer, 0, bsize); // clears HTTP header buffer

		bsize = 1024;

		int loopsTotal=0;
		ssize_t bytesRead=0;
		
		//char bigbuffer[bsize*100];
		char* bigbuffer = (char*)malloc(bsize*60000);

		//! TODO: FIX concatenation
		//! TODO: FIX buffer size after wrrite
		//* DATALEAK!!!! strcat reads environment variables.
		//* Still not working
		while(bReceived = recv(clientSocket, buffer, bsize, 0) > 0)
		{
			printf("%i, ", strlen(buffer));
			loopsTotal++;
			bytesRead+= strlen(buffer);
			char str[strlen(buffer)];
			strcat(bigbuffer, buffer); //! concatenates sequentially and the buffer is filled backwards since ABI
			memset(buffer, 0, bsize);
		}
		
		printf("\nData ready after %i loops\n", loopsTotal);
		printf("Bytes Read %i\n", bytesRead);
		
		char imagebytes[bytesRead];
		memset(imagebytes, 0, bytesRead); // clear after initialize
		//char* imagebytes = (char*)malloc(bytesRead+bodySize);
		printf("Data Size: %i (%i * %i) \n", bytesRead, loopsTotal, bsize);   

		strcpy(imagebytes, bigbuffer);
		// printf("Data received: %s",imagebytes);
		//fprintf(fp,"%s",imagebytes);
		fwrite(imagebytes, strlen(imagebytes), bytesRead, fp);
   		fclose(fp);
	}

	int status2 = shutdown(clientSocket, 2);

	switch(status2)
	{
		case ENOTCONN:
			printf("Socket was already closed");
			exit(-1);
		case ENOTSOCK:
			printf("Socket was damaged");
			exit(-2);
		case EBADF:
			printf("Socket is invalid");
			exit(-3);
	}
	
	//free(buffer2);
	return 0;
}