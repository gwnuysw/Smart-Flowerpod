#ifndef __BASE_PROTOCOL_H__
#define __BASE_PROTOCOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define  BUF_SIZE		256
#define  INIT_VAL_OF_MEM	0

#define  PROTOCOL_TYPE	0
#define  NUM_OF_QUEUE	5

#define  PKT_STX1	0xfd
#define  PKT_STX2	0xfe

#define  PKT_ETX			0xff

#define  PKT_LEN		74

#define  RA_SERVER_PORT		50001
#define  SERVER_PORT		50002

// packet index 
#define  INDEX_STX				0
#define  INDEX_LEN				(INDEX_STX + 2)
#define  INDEX_CMD				(INDEX_LEN + 4)
#define  INDEX_OBJ_ID			(INDEX_CMD + 1)
#define  INDEX_CONT_ID			(INDEX_OBJ_ID + 4)
#define  INDEX_BASE_ID			(INDEX_CONT_ID + 4)
#define  INDEX_OBJ_DES			(INDEX_BASE_ID +4)
#define  INDEX_LOGINID			(INDEX_OBJ_DES + 20)
#define  INDEX_LOGINPWD			(INDEX_LOGINID + 4)
#define  INDEX_OBJ_TYPE			(INDEX_LOGINPWD + 10)
#define  INDEX_ENT_NUM			(INDEX_OBJ_TYPE + 1)
#define  INDEX_ENT_ID_1			(INDEX_ENT_NUM + 1)
#define  INDEX_ENT_SORT_1		(INDEX_ENT_ID_1 + 1)
#define  INDEX_ENT_VALUE_1		(INDEX_ENT_SORT_1+4)

#define  INDEX_ENT_ID_2			(INDEX_ENT_VALUE_1 + 4)
#define  INDEX_ENT_SORT_2		(INDEX_ENT_ID_2 + 1)
#define  INDEX_ENT_VALUE_2		(INDEX_ENT_SORT_2+4)
#define  INDEX_ETX				(INDEX_ENT_VALUE_2 +4)

#define  NORMAL_PACK_LEN  		(INDEX_ETX +1)

#define  NUM_OF_ARGC	3

#define  MAX_CLIENT_INFO   18

#define  IP_MAX_LEN		16

#define  FLAG_RX		1
#define  FLAG_CLEAR		0

#define  COUNT_CLEAR	0
#define  COUNT_1SEC		1
#define  COUNT_5SEC		5

// packet status
#define  PACKET_CORRECT		0
#define  PACKET_WRONG		1

// socket status 
#define  SOCKET_DISCONNECT	0
#define  SOCKET_CONNECT		1

// server socket status 
#define  SOCKET_DISCONNECTING	0
#define  SOCKET_CONNECTED		1

#define  STX_LEN			2
#define  OBJ_DES_MAX_LEN	20
#define  DOMAIN_PWD_LEN		10
#define  MAX_ENTITY_NUM		2

#define  MAX_SORT_NUM		4

//#define  DEBUG_MODE

typedef struct __attribute__((packed)) {
	unsigned char	stx[STX_LEN];
	unsigned int	packetlen;
	unsigned char	cmd;
	unsigned int	object_id;
	unsigned int	cont_id;
	unsigned int	base_id;
	char			object_des[OBJ_DES_MAX_LEN];
	unsigned int	domain_id;
	char			domain_pw[DOMAIN_PWD_LEN];
	unsigned char	object_type;
	unsigned char	entity_num;
	unsigned char	entity_id1;
	char			entity_sort1[MAX_SORT_NUM];
	int				entity_val1;
	unsigned char	entity_id2;
	char			entity_sort2[MAX_SORT_NUM];
	int				entity_val2;
	unsigned char	etx;
} packetInfo;

// Socket
typedef struct {
	char			flag;
	char			ipAddr[IP_MAX_LEN];		// ip address
	int				connection;
	int				connect_sock;
	int				serv_sock;
	int				clnt_sock;
	int				servSocketConnection;	// server socket is connected & enable to accept client socket
	int				port;
	struct in_addr   sin_addr;
	
	char			countflag;				// cout 1sec
	char			wrongPacket;
} socketInfo;

typedef struct {
	char			flag;
	socketInfo		serverSock;				// RaServer socket Info accepted ContBoard 
	packetInfo		contPacket;
	pthread_t		pthread;
} ControlboardInfo;

typedef struct {
	char			flag;
	unsigned int	lastpacket;
	socketInfo		clientSock;
	packetInfo		basePacket;
} ServerInfo;


void *client_thread(void *arg);
void *server_thread(void *arg);
void *serverAccess_thread(void *arg);
void *clientEnable_thread(void *arg);
int getData(void);
void error_handling(char *message);
int uptime(void);


#endif	// __BASE_PROTOCOL_H__
