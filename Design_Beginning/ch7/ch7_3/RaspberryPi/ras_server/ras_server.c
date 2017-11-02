#include <stdio.h>
#include <stdlib.h>			// exit()
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define	BUF_SIZE			256
#define	PROTOCOL_TYPE		0
#define	NUM_OF_QUEUE		5
#define	INIT_VAL_OF_MEM		0
#define	RA_SERVER_PORT		50001
#define	NUM_OF_ARGC			2

//#define  DEBUG_MODE

int main(int argc, char **argv)
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	int readBufSize;
	int writeBufSize;
	
	char rcv_buf[BUF_SIZE];

	// check port info
	if(argc != NUM_OF_ARGC) {
		printf("Usage: %s <port number>\n", argv[0]);
		printf("      ex)./ras_server 50001\n");
		exit(1);
	}
	
	// get server socket
	serv_sock = socket (PF_INET, SOCK_STREAM, PROTOCOL_TYPE);
	if (serv_sock == -1) {
		perror("socket() error : ");
		exit(1);
	}		

	memset(&serv_adr, INIT_VAL_OF_MEM, sizeof(serv_adr));

	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	// bind
	if (bind (serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
		perror("bind() error : ");

	// listen
	if (listen(serv_sock, NUM_OF_QUEUE) == -1)
		perror("listen() error : ");

	clnt_adr_sz = sizeof(clnt_adr);
		
	while(1)
	{
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
		if (clnt_sock == -1) {
			perror("accept() error : ");
			exit(1);
		}
		printf("Connected Client IP : %s\n", inet_ntoa(clnt_adr.sin_addr));
		printf("Client Port Num : %d\n\n", ntohs(clnt_adr.sin_port));
		
		while(1)
		{
			memset(rcv_buf, INIT_VAL_OF_MEM, BUF_SIZE);
			readBufSize = read(clnt_sock, rcv_buf, BUF_SIZE);
//			printf("== read from client.Rx len:%d\n",readBufSize );
			if (readBufSize <= 0)
			{
				perror("read error : ");
				close(clnt_sock);
				break;
			}
			rcv_buf[readBufSize] = 0;	// add null
			printf("read : %s", rcv_buf);

#ifdef DEBUG_MODE
			printf("RX:\r\n");
			int temp = 0;
			int k;
			for( k=0; k<BUF_SIZE; k++ )	{
				printf("[%02x]", rcv_buf[k]);
				temp++;
				if ( temp > 9 )
				{
					printf("\n");
					temp = 0;
				}
			}
			printf("\n");
#endif
			// Echo Server
			writeBufSize = write(clnt_sock, rcv_buf, readBufSize);
			if (writeBufSize <= 0)
			{
				perror("write error : ");
				close(clnt_sock);
				break;
			}
		}
		close(clnt_sock);
	}
}
