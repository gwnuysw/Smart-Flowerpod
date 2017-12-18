#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
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
	struct tm *t;
	time_t timer;

	timer = time(NULL);
	t = localtime(&timer);

	pthread_detach(pthread_self());
	int clnt_sock;
	clnt_sock = *(int *)arg;
	char rcv_buf[RX_DATA_MAX];
	int readBufSize,writeBufSize;	
	unsigned char sendingErr = 0;

	char filename[20] = "filetest0.txt";		//파일 이름 -------------------수정 v

	FILE* stream = fopen(filename, "w");		//파일 오픈 
	if(stream == NULL)							//오픈 확인 
	{
		printf("file open error \n");
		exit(1);
	}										//=------------------------수정 ^
	fprintf(stream,"** %d year %d mon %d day **\n", t->tm_year + 1900, t->tm_mon + 1,t-> tm_mday); 
	while(1)
	{
		if(ftell(stream) > 1000)				//각 파일의 크기 검사 -----------수정 v
		{
			printf("new file opened \n");	
			if(fclose(stream) == EOF)			//크기를 벗어나면 파일닫음 
			{
				printf("file close error \n");
				exit(1);
			}
			
			filename[8]++;						//새로운 파일을 열기위한 단계 
			
			stream = fopen(filename, "w");		//새 파일 오픈 
			
			if(stream ==NULL)		//오픈 확인 
			{
				printf("file open error \n");
				exit(1);
			} 
		}							//------------------수정 ^
		
		readBufSize = read(clnt_sock, rcv_buf, RX_DATA_MAX);  //소켓이로부터 문자를 읽어서 rcv_buf에 저장 하는데 그 크기는  최대 크기는 2048BYTE다 

		if(readBufSize > 0)
		{

			timer = time(NULL);
			t = localtime(&timer);

			printf("rx len:%d\n", readBufSize); //read()함수는 정상적으로 읽었다면 읽은 BYTE수를 반환 한다. 

			rcv_buf[readBufSize] = 0;	//맨 뒷자리를 0으로 셋팅  
			printf("%s\n", rcv_buf);	//읽어온 대로 출력 
			 
			fprintf(stream," %d hour %d min %d sec \t",t-> tm_hour, t->tm_min, t->tm_sec); 
			fprintf(stream,"%s", rcv_buf);	//파일 입력 ----------------------------수정 ============
			
			// echo TX
			writeBufSize = write(clnt_sock,rcv_buf , readBufSize);  //받은 데이터를 clnt_sock으로 되돌려 줍니다. 
			printf("Send data len: %d\n",writeBufSize); //되돌려준 데이터의 길이를 알려줍니다. 
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
