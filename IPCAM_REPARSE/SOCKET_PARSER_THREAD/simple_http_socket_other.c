#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <cstring>

int main()
{
    struct sockaddr_in source, destination = {};  //two sockets declared as previously
    int sock = 0;
    int datalen = 0;
    int pkt = 0;

    uint8_t *send_buffer, *recv_buffer;

    struct sockaddr_storage fromAddr;   // same as the previous entity struct sockaddr_storage serverStorage;
    unsigned int addrlen;  //in the previous example socklen_t addr_size;
    struct timeval tv;
    tv.tv_sec = 3;  /* 3 Seconds Time-out */
    tv.tv_usec = 0;

    /* creating the socket */         
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
        printf("Failed to create socket\n");

    /*set the socket options*/
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

    /*Inititalize source to zero*/
    memset(&source, 0, sizeof(source));       //source is an instance of sockaddr_in. Initialization to zero
    /*Inititalize destinaton to zero*/
    memset(&destination, 0, sizeof(destination));


    /*---- Configure settings of the source address struct, WHERE THE PACKET IS COMING FROM ----*/
    /* Address family = Internet */
    source.sin_family = AF_INET;    
    /* Set IP address to localhost */   
    source.sin_addr.s_addr = INADDR_ANY;  //INADDR_ANY = 0.0.0.0
    /* Set port number, using htons function to use proper byte order */
    source.sin_port = htons(7005); 
    /* Set all bits of the padding field to 0 */
    memset(source.sin_zero, '\0', sizeof source.sin_zero); //optional


    /*bind socket to the source WHERE THE PACKET IS COMING FROM*/
    if (bind(sock, (struct sockaddr *) &source, sizeof(source)) < 0) 
        printf("Failed to bind socket");

    /* setting the destination, i.e our OWN IP ADDRESS AND PORT */
    destination.sin_family = AF_INET;                 
    destination.sin_addr.s_addr = inet_addr("127.0.0.1");  
    destination.sin_port = htons(7005); 

    //Creating a Buffer;
    send_buffer=(uint8_t *) malloc(350);
    recv_buffer=(uint8_t *) malloc(250);

    addrlen=sizeof(fromAddr);

    memset((void *) recv_buffer, 0, 250);
    memset((void *) send_buffer, 0, 350);

    sendto(sock, send_buffer, 20, 0,(struct sockaddr *) &destination, sizeof(destination));

    pkt=recvfrom(sock, recv_buffer, 98,0,(struct sockaddr *)&destination, &addrlen);
    if(pkt > 0)
        printf("%u bytes received\n", pkt);

	
}

/*
SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
SOCKADDR_IN SockAddr;
SockAddr.sin_port = 80;
SockAddr.sin_family = AF_INET;
SockAddr.sin_addr.s_addr = *((unsigned long*)gethostbyname("SomeName.000webhost.com")->h_addr);
*/
/*
socket tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
raw_socket = socket(AF_INET, SOCK_RAW, protocol);
int main2()
{		
	struct sockaddr_in saddr;
	int sockfd;
	unsigned short port = 8080;

	if((sockfd=socket(AF_INET, SOCK_STREAM, 0) < 0)) 
	{
		printf("Error creating socket\n");
	}

	memset(&saddr, '\0', sizeof(saddr)); // zero structure out
	saddr.sin_family = AF_INET; // match the socket() call
	saddr.sin_addr.s_addr = htonl(INADDR_ANY); // bind to any local address
	saddr.sin_port = htons(port); // specify port to listen on

	if((bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)) { // bind!
		printf("Error binding\n");
	}
}
/**
int main2(){
	int clientSocket;
	char buffer[1024];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	serverAddr.sin_family = AF_INET;		/* Address family = Internet */
	serverAddr.sin_port = htons(80);		/* Set port number, using htons function to use proper byte order */
	serverAddr.sin_addr.s_addr = inet_addr("192.168.2.198");	/* Set the IP address to desired host to connect to */
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  		/* Set all bits of the padding field to 0 */

	addr_size = sizeof serverAddr;		/*---- Connect the socket to the server using the address struct ----*/
	connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

	/*---- Read the message from the server into the buffer ----*/
	char buffer2[200] = "GET /IMAGE.JPG? HTTP/1.1\r\nhost: 192.168.2.45\r\nUser-agent: Mozilla/4.0\r\nAccept: */*\r\nConnection: keep-alive\r\nConnection: close\r\r\n\r\n";

	int status = send(clientSocket, buffer2, strlen(buffer2), 0);
	if (status>0)
	{
		printf("Bytes to read: %i \n", status);
		ssize_t bReceived;
		int bTotal;

		recv(clientSocket, buffer, status, 0);
		printf(buffer);

		while(bReceived = recv(clientSocket, buffer, 512, 0) > 0)
		{
			bTotal++;
			memset(buffer, 0, 512);
		}
		printf("Data received: %s",buffer);
		printf("Data Size: %i",bTotal);   
	}

	return 0;
}

*/
