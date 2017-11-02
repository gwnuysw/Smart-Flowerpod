#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/socket.h>

#define	BUF_SIZE		256
#define	PROTOCOL_TYPE	0
#define	INIT_VAL_OF_MEM	0
#define	RA_SERVER_PORT	50001
#define	NUM_OF_ARGC		3
#define IP_MAX_LEN		16
#define STD_INPUT_SOCK	0

//#define  DEBUG_MODE

int main(int argc, char **argv)
{
	int connect_sock;
	struct sockaddr_in connect_adr;
	socklen_t connect_adr_sz;
	int readBufSize;
	int writeBufSize;
	
	char send_buf[BUF_SIZE];
	char rcv_buf[BUF_SIZE];
	char ipAddr[IP_MAX_LEN];

	// check port info
	if(argc != NUM_OF_ARGC) {
		printf("Usage: %s <ip address> <port number>\n", argv[0]);
		printf("      ex)./ras_server 192.168.10.141 50001\n");
		exit(1);
	}
	
	// argv[1] => main server ip address
	strcpy( ipAddr ,argv[1]);
	
	// get server socket
	connect_sock = socket(PF_INET, SOCK_STREAM, PROTOCOL_TYPE);
	if (connect_sock == -1) {
		perror("error :");
		exit(1);
	}

	memset(&connect_adr, INIT_VAL_OF_MEM, sizeof(connect_adr));
	
	connect_adr.sin_family = AF_INET;
	connect_adr.sin_addr.s_addr = inet_addr(ipAddr);
	connect_adr.sin_port = htons(atoi(argv[2]));

	connect_adr_sz = sizeof(connect_adr);
		
	// connect
	if (connect(connect_sock, (struct sockaddr *)&connect_adr, connect_adr_sz) < 0)
	{
		perror("connect error :");
		exit(1);
	}
	
	while(1)
	{
		memset(send_buf, INIT_VAL_OF_MEM, BUF_SIZE);
		// fd = 0 : Standard Input - Keyboard
		readBufSize = read(STD_INPUT_SOCK, send_buf, BUF_SIZE);	
		if (readBufSize <= 0)
		{
			perror("read error : ");
			break;
		}
		
		writeBufSize = write(connect_sock, send_buf, readBufSize);
		if (writeBufSize <= 0)
		{
			perror("write error : ");
			break;
		}
		
		memset(rcv_buf, INIT_VAL_OF_MEM, BUF_SIZE);
		readBufSize = read(connect_sock, rcv_buf, BUF_SIZE);
//		printf("== read from server.Rx len:%d\n",readBufSize );
		if (readBufSize <= 0)
		{
			perror("read error : ");
			break;
		}
		rcv_buf[readBufSize] = 0;	//add null
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

	}
	close(connect_sock);
	return 0;
}