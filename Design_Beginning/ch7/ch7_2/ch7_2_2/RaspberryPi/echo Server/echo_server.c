#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

/*****************************************************************/
// DEFINE
/*****************************************************************/
#define LISTENING_PORT		50001

/*****************************************************************/
// FUNCTION
/*****************************************************************/
// thread
void *listening_thread(void *arg);
void *client_thread(void *arg);
void error_handling(char *message);

int main(int argc, char *argv[])
{

	pthread_t pthreadListening;				// accept client

	printf("============================================\n");
	printf("TCP/IP Echo Server\n");
	printf("============================================\n\n");

	// create listening thread
	if(pthread_create(&pthreadListening, NULL, listening_thread, NULL) == -1)
	{
		error_handling("pthread_create() error\n");
		return -1;
	}
	
	// main thread loop
	while(1)
	{

	}
	printf("Echo Server closed.\n");

	return 0;
}

//control Client  acception manage 
void *listening_thread(void *arg)
{
	pthread_detach(pthread_self());

	int serv_sock, clnt_sock;
	
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	
	pthread_t pthreadClient;	            // threadd receive data 
	
	// get server socket
	serv_sock =  socket(PF_INET, SOCK_STREAM, 0);// tcp => 0   
	if (serv_sock == -1)
	        error_handling("socket() error");

	//soket option - reuse binded address
	int option = 1;
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	memset(&serv_adr, 0, sizeof(serv_adr));

	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);	// get ip address
	serv_adr.sin_port = htons(LISTENING_PORT);		// get port num

	// bind .  for the connection request of control board 
	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
	        error_handling("bind() error");
	
	// listen
	if (listen(serv_sock, 10) == -1)
        	error_handling("listen() error");
		
	int i;
	clnt_adr_sz = sizeof(clnt_adr);
	
	while(1)
	{
		printf("ready to accept client...\n");
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		printf("clnt_sock : %d\n", clnt_sock);

		if(clnt_sock < 0 )
		{
			error_handling("accept() error");
			continue;
		}
		printf("accept() success \n");

		// accept and create object and  create thread
		if(pthread_create(&pthreadClient, NULL , client_thread , (void*)&clnt_sock) == -1)
		{
			error_handling("acceptClient_thread -- pthread_create() error");
			close(clnt_sock);
		}				

		printf("Connected Client IP : %s\n", inet_ntoa(clnt_adr.sin_addr));
		printf("Client Port Num : %d\n\n", ntohs(clnt_adr.sin_port));
	}
	close(serv_sock);
}

#define  RX_DATA_MAX	2048
void *client_thread(void *arg)
{
	pthread_detach(pthread_self());

	int clnt_sock;
	clnt_sock = *(int *)arg;

	char rcv_buf[RX_DATA_MAX];
	int readBufSize,writeBufSize;
	int i, j;
	
	unsigned char sendingErr = 0;

	while(1)
	{
		readBufSize = read(clnt_sock, rcv_buf, RX_DATA_MAX);

		if(readBufSize > 0)
		{
			printf("rx len:%d\n", readBufSize);

			rcv_buf[readBufSize] = 0;
			printf("%s\n", rcv_buf);
			
			// echo TX
			writeBufSize = write(clnt_sock,rcv_buf , readBufSize);
			printf("Send data len: %d\n",writeBufSize);
			if(writeBufSize < 0)
			{
				printf("echo write() error\n");	
				break;
			}			
		}
		else if (readBufSize == 0)
		{
			printf("readBufSize : 0\n");

		}
		else  // readBufSize < 0
		{
			printf("client(%d) write error.\n", clnt_sock);
			break;
		}
	}
	printf("close clnt_sock.:%d\n",clnt_sock);
	close(clnt_sock);
}



void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
//	exit(1);
}

