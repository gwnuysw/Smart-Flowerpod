#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "base_protocol.h"

//#define DEBUG_MODE_PACKET_CONT
//#define DEBUG_MODE_PACKET_SERVER

// init thread mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// thread
void *client_thread(void *arg);
void *server_thread(void *arg);
void *connectServer_thread(void *arg);
void *acceptClient_thread(void *arg);
void *countTime_thread(void *arg);

void error_handling(char *message);
void print_string(char a[]);
void print_n_string (char data[], unsigned int string_num);

// init Variables
void initVariableServer(ServerInfo* stData)
{	
	memset(stData, 0, sizeof(ServerInfo));	
}	
void initVariableControlBoard(ControlboardInfo* stData)
{
	memset(stData, 0, sizeof(ControlboardInfo) * MAX_CLIENT_INFO);	
}	

int main(int argc, char *argv[])
{
	unsigned int base_id;
	
	ServerInfo clntThread;							// Server Connection Info
	ControlboardInfo servThread[MAX_CLIENT_INFO];	// Each ControlBoard Connection Info

	ServerInfo *ptr_clntThread;						// for connecting to Main Server
	ControlboardInfo *ptr_servThread;				// for RaServer to accept ContClient
	
	ptr_clntThread = &clntThread;
	ptr_servThread = ( ControlboardInfo* )servThread;
	
	// init variables
	initVariableServer(ptr_clntThread);
	initVariableControlBoard(ptr_servThread);

	// check port info
	if(argc != NUM_OF_ARGC) {
		printf("Usage: %s <ip address> <hex base id>\n", argv[0]);
		printf("      ex)./ras_server 192.168.10.227 0x2345\n");
		exit(1);
	}
    // argument ip format check 
	struct sockaddr_in sa;
	if (inet_pton(AF_INET,argv[1],&(sa.sin_addr)) < 0)// wrong format
	{
		printf(" IP format:%s is wrong.\n", argv[1]);
		printf("Usage: %s <ip address> <hex base id>\n", argv[0]);
		printf("      ex)./ras_server 192.168.10.227 0x2345\n");
		exit(1);
	}
	char  *stopstring;

    base_id = (unsigned int)strtoul(argv[2],&stopstring, 16);
	printf("base_id:%X\n", base_id);


	
	// argv[1] => main server ip address
	strcpy( ptr_clntThread->clientSock.ipAddr ,argv[1]);

	ptr_clntThread->clientSock.connection = SOCKET_DISCONNECT;


	int serv_sock, clnt_sock;
	
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;

	int option;

	pthread_t pthreadTime;						// countTime_thread

	pthread_t pthreadBaseAcceptControlClient;	// acceptClient_thread , control Cient  acception mange

	pthread_t pthreadBaseClient;				// client_thread  , thread that read  data from server
	pthread_t pthreadBaseConnectServer;			// serverAccess_thred , manage the base to server connection 


	printf("============================================\n");
	printf("TCP/IP Data Transmission Program (Base board)\n");
	printf("============================================\n\n");

	
	// get server socket
	serv_sock =  socket(PF_INET, SOCK_STREAM, PROTOCOL_TYPE);   
	if (serv_sock == -1)
	        error_handling("socket() error");

	//soket option - reuse binded address
	option = 1;
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	// broadcast serv_sock to client
	int i;
	for(i = 0 ; i < MAX_CLIENT_INFO ; i++ )
	{
		ptr_servThread[i].serverSock.serv_sock = serv_sock; 
	}		

	memset(&serv_adr, INIT_VAL_OF_MEM, sizeof(serv_adr));

	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);	// get ip address
	serv_adr.sin_port = htons(RA_SERVER_PORT);		// get port num

	// bind .  for the connection request of control board 
	if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
	        error_handling("bind() error");
	
	// listen
	if (listen(serv_sock, NUM_OF_QUEUE) == -1)
        	error_handling("listen() error");

	// timer thread
	if(pthread_create(&pthreadTime, NULL, countTime_thread, (void*)servThread) == -1)
	{
		error_handling("pthread_create() error");
	}
	else
	{
		printf("time thread start\n");
	}		

	// connect server
	if(pthread_create(&pthreadBaseConnectServer, NULL, connectServer_thread, (void*)&clntThread) == -1)
	{
		error_handling("pthread_create() error\n");
	}
	
	// accept client
	if(pthread_create(&pthreadBaseAcceptControlClient, NULL, acceptClient_thread, (void*)servThread) == -1)
	{
		error_handling("pthread_create() error");
	}

	// main thread loop
	while(1)
	{
		// received from server
		if(ptr_clntThread->flag == FLAG_RX)
		{
//			printf("ptr_clntThread->flag : %x\n", ptr_clntThread->flag);
			ptr_clntThread->flag = FLAG_CLEAR;
			

#ifdef DEBUG_MODE			
			printf("TX:\r\n");
			int temp1 = 0;
			int i;
			for(i = 0; i< sizeof(ptr_clntThread->basePacket) ; i++ )
			{
				printf("[%02X]", *(((unsigned char*)&(ptr_clntThread->basePacket)) + i));
				temp1++;
				if ( temp1 > 9 )
				{
					printf("\n");
					temp1 = 0;
				}					
			}			 
			printf("\r\n");

			printf("lastpacket : %08x\n", ptr_clntThread->lastpacket);
#endif
			// find the control board socket  with the control id received from Server 
			clnt_sock = 0;
			for(i = 0; i < MAX_CLIENT_INFO; i++)
			{
				if ((ptr_clntThread->lastpacket == ptr_servThread[i].contPacket.cont_id) && (ptr_servThread[i].serverSock.clnt_sock != 0 ))// compare  the control id from server  with the connected control board id.
				{
					clnt_sock = ptr_servThread[i].serverSock.clnt_sock;
					printf("control board matching array Index:%d, clnt_sock:%d\n", i, clnt_sock);
					break;
				}
			}
			printf("=======================index:%d,clnt_sock : %d\n",i, clnt_sock);

			usleep(1000*1000); 
			int writeBufSize;
			if ( clnt_sock != 0)
			{
				writeBufSize = write(clnt_sock, (struct packetInfo*)&(ptr_clntThread->basePacket), sizeof(ptr_clntThread->basePacket));
				printf("==== base-> control write.\n");

				if(writeBufSize < 1)
				{
					printf("base BD -> control BD write() error\n");					
				}
			}
			else
			{
				printf("control bd socket not opened\n");
			}
		}			

		//send the data that received from control board  to Server.
		int i;
		for(i = 0; i < MAX_CLIENT_INFO ; i++ )
		{
			if(ptr_servThread[i].flag == FLAG_RX)
			{
				ptr_servThread[i].flag = FLAG_CLEAR;
				int connect_sock = ptr_clntThread->clientSock.connect_sock;

				ptr_servThread[i].contPacket.base_id = base_id ;
	
				int writeBufSize;
				writeBufSize = write(connect_sock, (struct packetInfo*)&(ptr_servThread[i].contPacket), sizeof(ptr_servThread[i].contPacket));
				printf("== base to server write.\n");

				if(writeBufSize < 1)
				{
					printf("base BD -> Server write() error\n");
				}
			}			
		}
				
		if(ptr_clntThread->clientSock.connection == SOCKET_CONNECT)
		{
			if(ptr_clntThread->clientSock.servSocketConnection != SOCKET_DISCONNECTING)
			{
				if(pthread_create(&pthreadBaseClient, NULL, client_thread, (void*)&clntThread) == -1)
				{
					error_handling("pthread_create() error\n");
					break;
				}
				printf("create thread  that read data from server\r\n");
			}
		}
	}
	printf("Transmission terminated.\n");
	printf("Disconnected(IP : %s)\n", inet_ntoa(clnt_adr.sin_addr));
	printf("Port Num : %d\n\n", ntohs(clnt_adr.sin_port));

	close(serv_sock);

	return 0;
}

// pthreadBaseAcceptControlClient  . control Client  acception manage 
void *acceptClient_thread(void *arg)
{
	pthread_detach(pthread_self());

	ControlboardInfo *servThread;
	servThread = ((ControlboardInfo *)arg);

	int serv_sock, clnt_sock;
	
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	
	serv_sock = servThread->serverSock.serv_sock;

	int i;
	
	while(1)
	{
		printf("[CET] ");
		printf("ready to accept client...\n");

		clnt_adr_sz = sizeof(clnt_adr);

		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		printf("clnt_sock : %d\n", servThread->serverSock.clnt_sock);

		if(clnt_sock == -1)
		{
			error_handling("accept() error");
			servThread->serverSock.connection = SOCKET_DISCONNECT;
			servThread->serverSock.servSocketConnection = SOCKET_DISCONNECTING;
		}
		else
		{
			printf("accept() success \n");
		}
		
		ControlboardInfo * tempInfo = servThread;

		// accept and create object and  create thread
		int findSpace = -1;
		
		for(i  = 0; i < MAX_CLIENT_INFO ; i++ )
		{
			if (servThread[i].serverSock.clnt_sock == 0 )
			{
				servThread[i].serverSock.clnt_sock = clnt_sock;
				servThread[i].serverSock.sin_addr = clnt_adr.sin_addr;
				printf("Create clnt_sock : %d, allocated array index:%d\n", servThread[i].serverSock.clnt_sock, i);
				servThread[i].serverSock.connection = SOCKET_CONNECT;
				findSpace = i;
			
				break; 
			}				
		}	
		if ( findSpace == 	-1 )
		{
			close(clnt_sock);
		}
		else
		{
			// remove duplication socket
			for(i  = 0; i < MAX_CLIENT_INFO ; i++ )
			{
				if ((servThread[i].serverSock.clnt_sock != 0) && (i != findSpace) )
				{
					printf("valid socket IP : %s\n", inet_ntoa(servThread[i].serverSock.sin_addr));
					if (!strcmp(inet_ntoa(servThread[i].serverSock.sin_addr),inet_ntoa(clnt_adr.sin_addr)) ) // find duplication 
					{
						close(servThread[i].serverSock.clnt_sock);						
						servThread[i].serverSock.clnt_sock = 0;
						servThread[i].serverSock.connection = SOCKET_DISCONNECT;
						printf("duplication  socket remove array index: %d\n", i );
					}
					
				}				
			}
			
			if(pthread_create(&servThread[findSpace].pthread, NULL , server_thread , (void*)(servThread+findSpace)) == -1)
			{
				error_handling("acceptClient_thread -- pthread_create() error");
				servThread[findSpace].serverSock.clnt_sock = 0;
				servThread[findSpace].serverSock.connection = SOCKET_DISCONNECT;
				close(clnt_sock);
			}				
		}			

		printf("Connected Client IP : %s\n", inet_ntoa(clnt_adr.sin_addr));
		printf("Client Port Num : %d\n\n", ntohs(clnt_adr.sin_port));
	}
	close(clnt_sock);
	servThread->serverSock.connection = SOCKET_DISCONNECT;
	servThread->serverSock.servSocketConnection = SOCKET_DISCONNECTING;
}

void *connectServer_thread(void *arg)// manage the base to server connection  
{
	pthread_detach(pthread_self());

	ServerInfo *clntThread;
	clntThread = ((ServerInfo *)arg);

	char *argv = clntThread->clientSock.ipAddr;

//	server-base board connection
	int connect_sock;
	struct sockaddr_in connect_adr;
	socklen_t connect_adr_sz;

	// init lastpacket
	clntThread->lastpacket = -1;
	
	while(1)
	{
		if(clntThread->clientSock.connection == SOCKET_DISCONNECT)
		{
			connect_sock =  socket(PF_INET, SOCK_STREAM, PROTOCOL_TYPE);
			clntThread->clientSock.connect_sock = connect_sock;

			memset(&connect_adr, INIT_VAL_OF_MEM, sizeof(connect_adr));

			connect_adr.sin_family = AF_INET;
			connect_adr.sin_addr.s_addr = inet_addr(clntThread->clientSock.ipAddr);	// get ip address
			printf("Connecting IP : %s\n", inet_ntoa(connect_adr.sin_addr));
			connect_adr.sin_port = htons(SERVER_PORT);		// get port num

			printf("[SAT] ");
			printf("try to access server...\n");
			
			connect_adr_sz = sizeof (connect_adr);
			if(connect(connect_sock, (struct sockaddr*)&connect_adr, connect_adr_sz) == -1)
			{
				error_handling("connect() error\n");
				clntThread->clientSock.connection = SOCKET_DISCONNECT;
				clntThread->clientSock.servSocketConnection = SOCKET_DISCONNECTING;
				close(connect_sock);
			}
			else
			{
				printf("connect() success \n");
				clntThread->clientSock.servSocketConnection = SOCKET_CONNECTED;		// start to make sock
				clntThread->clientSock.connection = SOCKET_CONNECT;
			}
		}
		sleep(2);
	}
	clntThread->clientSock.connection = SOCKET_DISCONNECT;
	clntThread->clientSock.servSocketConnection = SOCKET_DISCONNECTING;
	close(connect_sock);
}

void *client_thread(void *arg) // thread that read  data from server 
{
	pthread_detach(pthread_self());

	ServerInfo *clntThread;
	clntThread = ((ServerInfo *)arg);
	
//	packetInfo basePacket = clntThread->basePacket;

	clntThread->clientSock.servSocketConnection = SOCKET_DISCONNECTING;		// stop to make sock

	int connect_sock = clntThread->clientSock.connect_sock;
//	printf("connect_sock_th : %d\n", clntThread->clientSock.connect_sock);

	char* rcv_buf = (char *) &clntThread->basePacket;
	int readBufSize;
	
	int i, j;
	// monitor pthread
	int status;
	
	while(1)
	{					
		packetInfo* clntPacket;

		readBufSize = read(connect_sock, rcv_buf, sizeof(packetInfo));

		clntPacket = (packetInfo*)rcv_buf;

		if(readBufSize > 0)
		{
			printf("== read from server.Rx len:%d\n",readBufSize );

#ifdef DEBUG_MODE
			printf("RX:\r\n");
			int temp = 0;
			int k;
			for( k=0; k<sizeof(packetInfo); k++ )	{
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

			if(rcv_buf[INDEX_STX] == PKT_STX1 && rcv_buf[INDEX_STX+1] == PKT_STX2)
			{
#ifdef DEBUG_MODE_PACKET_SERVER
				printf(" stx : %02x %02x\n", rcv_buf[INDEX_STX], rcv_buf[INDEX_STX+1]);
#endif
				if(rcv_buf[INDEX_LEN] == (NORMAL_PACK_LEN-7))
				{
#ifdef DEBUG_MODE_PACKET_SERVER
					printf(" packetlen : %02x\n", rcv_buf[INDEX_LEN]);
#endif
					if(rcv_buf[INDEX_ETX] == PKT_ETX)
					{
						clntThread->lastpacket = clntPacket->cont_id;
						
						clntThread->flag = FLAG_RX;
			
#ifdef DEBUG_MODE_PACKET_SERVER
						printf(" cmd : %02x\n", clntPacket->cmd);

						printf(" object_id : %08x\n", clntPacket->object_id);
						printf(" cont_id : %08x\n", clntPacket->cont_id);
						printf(" base_id : %08x\n", clntPacket->base_id);

						printf(" object_des : ");
						print_n_string (clntPacket->object_des, OBJ_DES_MAX_LEN);

						printf(" domain_id : %08x\n", clntPacket->domain_id);
						printf(" domain_pw : ");
						print_n_string (clntPacket->domain_pw, DOMAIN_PWD_LEN);

						printf(" object_type : %02x\n", clntPacket->object_type);
						
						printf(" entity_num : %02x\n", clntPacket->entity_num);
						printf(" entity_id1 : %02x\n", clntPacket->entity_id1);
						printf(" entity_sort1 : ");
						print_n_string (clntPacket->entity_sort1, MAX_SORT_NUM);
						printf(" entity_val1 : %08x\n", clntPacket->entity_val1);
						printf(" entity_id2 : %02x\n", clntPacket->entity_id2);
						printf(" entity_sort2 : ");
						print_n_string (clntPacket->entity_sort2, MAX_SORT_NUM);
						printf(" entity_val2 : %08x\n", clntPacket->entity_val2);

						printf(" etx : %02x\n", rcv_buf[INDEX_ETX]);
#endif
					}
					else
					{
						printf("etx is wrong.\n");
					}						
				}
				else
				{
					printf("packet length is wrong.\n");
				}					
			}
			else	//when app is stopped?
			{
				printf("stx is wrong.\n");
			}
		}
		else if (readBufSize == 0)
		{
			printf("readBufSize : 0\n");

		}
		else
		{
			printf("Server read() error \n");
			break;
		}
	}
	printf("close connect_sock.\n");
	
	close(connect_sock);
	clntThread->clientSock.connection = SOCKET_DISCONNECT;
	clntThread->clientSock.servSocketConnection = SOCKET_DISCONNECTING;
}

// read data from control board
void *server_thread(void *arg)
{
	pthread_detach(pthread_self());

	ControlboardInfo *servThread;
	servThread = ((ControlboardInfo *)arg);

	packetInfo contPacket = servThread->contPacket;

	servThread->serverSock.servSocketConnection = SOCKET_DISCONNECTING;		// stop to make sock

	int clnt_sock = servThread->serverSock.clnt_sock;

	char* rcv_buf;
	int readBufSize;
	int i, j;
	// monitor pthread
	int status;

	// check wrong packet
	int pre_uptime;
	int taketime;
	
	unsigned char sendingErr = 0;

	while(1)
	{
		packetInfo* clntPacket;

		readBufSize = read(clnt_sock, (char*)&servThread->contPacket, sizeof(packetInfo));

		rcv_buf = (char *)&servThread->contPacket;
		if(readBufSize > 0)
		{
			printf("==== read from control. rx len:%d\n", readBufSize);

			// 패킷 길이 계산하여 읽은 packetlen 값과 비교
			
#ifdef DEBUG_MODE
			printf("RX:\r\n");
			int temp = 0;
			int k;
			for( k=0; k<sizeof(packetInfo); k++ )	{
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
			if(rcv_buf[INDEX_STX] == PKT_STX1 && rcv_buf[INDEX_STX+1] == PKT_STX2)
			{
#ifdef DEBUG_MODE_PACKET_CONT
				printf(" stx : %x %x\n", rcv_buf[INDEX_STX], rcv_buf[INDEX_STX+1]);
#endif
				if(rcv_buf[INDEX_LEN] == (NORMAL_PACK_LEN - 7))
				{
#ifdef DEBUG_MODE_PACKET_CONT
					printf(" packetlen : %x\n", rcv_buf[INDEX_LEN]);
#endif
					if(rcv_buf[INDEX_ETX] == PKT_ETX)
					{
						servThread->flag = FLAG_RX;
			
						servThread->serverSock.wrongPacket = PACKET_CORRECT;		// check wrong packet
#ifdef DEBUG_MODE_PACKET_CONT
						printf(" cmd : %02x\n", servThread->contPacket.cmd);

						printf(" object_id : %08x\n", servThread->contPacket.object_id);
						printf(" cont_id : %08x\n", servThread->contPacket.cont_id);
						printf(" base_id : %08x\n", servThread->contPacket.base_id);

						printf(" object_des : ");
						print_n_string (servThread->contPacket.object_des, OBJ_DES_MAX_LEN);

						printf(" domain_id : %08x\n", servThread->contPacket.domain_id);
						printf(" domain_pw : ");
						print_n_string (servThread->contPacket.domain_pw, DOMAIN_PWD_LEN);

						printf(" object_type : %02x\n", servThread->contPacket.object_type);

						printf(" entity_num : %02x\n", servThread->contPacket.entity_num);
						printf(" entity_id1 : %02x\n", servThread->contPacket.entity_id1);
						
						printf(" entity_sort1 : ");
						print_n_string (servThread->contPacket.entity_sort1, MAX_SORT_NUM);
						
						printf(" entity_val1 : %08x\n", servThread->contPacket.entity_val1);
						printf(" entity_id2 : %02x\n", servThread->contPacket.entity_id2);
						
						printf(" entity_sort2 : ");
						print_n_string (servThread->contPacket.entity_sort2, MAX_SORT_NUM);

						printf(" entity_val2 : %08x\n", servThread->contPacket.entity_val2);
#endif
						sendingErr = 0;
					}
					else
					{
						printf("etx is wrong.\n");
						sendingErr = 1;
					}						
				}
				else
				{
					printf("packet length is wrong.\n");
					sendingErr = 1;
				}					
			}
			else	//when app is stopped?
			{
				printf("stx is wrong.\n");
				sendingErr = 1;
			}
			
			if(sendingErr == 1)
			{
				if(servThread->serverSock.wrongPacket == PACKET_WRONG)
				{
					printf("taketime : %d\n", servThread->serverSock.countflag);				
					if(servThread->serverSock.countflag > COUNT_5SEC)	// delete packet in 5sec
					{
						printf("close clnt_sock.\n");
						close(clnt_sock);
						servThread->serverSock.connection = SOCKET_CONNECT;
						servThread->serverSock.servSocketConnection = SOCKET_DISCONNECTING;
						servThread->serverSock.wrongPacket = PACKET_CORRECT;
						servThread->serverSock.countflag = COUNT_CLEAR;
					}					
				}
				else
				{
					servThread->serverSock.countflag = COUNT_CLEAR;
					servThread->serverSock.wrongPacket = PACKET_WRONG;
				}
			}
			else
			{
				printf("control=>base Rx OK\n");
			}				
		}
		else if (readBufSize == 0)
		{
			printf("readBufSize : 0\n");

		}
		else  // readBufSize < 0
		{
			printf("control BD read() error , readBufSize:%d\n", readBufSize);
			break;
		}
	}
	printf("close clnt_sock.\n");
	close(clnt_sock);
	servThread->serverSock.clnt_sock =0; // dklee add
	servThread->serverSock.connection = SOCKET_DISCONNECT;
	servThread->serverSock.servSocketConnection = SOCKET_DISCONNECTING;

}


void *countTime_thread(void *arg)
{
	pthread_detach(pthread_self());

	ControlboardInfo *servThread;
	servThread = ((ControlboardInfo *)arg);

	int i;
	while(1)
	{
		sleep(1);
		for ( i = 0; i < MAX_CLIENT_INFO ; i++ )
		{
			servThread[i].serverSock.countflag += COUNT_1SEC;
		}			
	}
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
}

int uptime(void)
{
	FILE *fp;
	char buf[36];
	double stime;
	double idletime;

	if((fp = fopen("/proc/uptime", "r")) == NULL)
	{
		return -1;
	}
	fgets(buf, 36, fp);
	sscanf(buf, "%lf %lf", &stime, &idletime);
	fclose(fp);

	return(int)stime;
}

void print_string(char a[])
{
    int i=0;
    while(a[i] != '\0')
    {
        printf("%c", a[i]);
        i++;
    }
	printf("\n");
}

void print_n_string (char data[], unsigned int string_num)
{
	char stringArr[string_num+1];
	strncpy(stringArr, data, string_num);
	stringArr[string_num] = 0;
	printf("%s\n", stringArr);
}
