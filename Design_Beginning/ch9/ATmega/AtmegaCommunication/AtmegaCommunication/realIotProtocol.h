#ifndef __REALIOT_PROTOCOL_H__
#define __REALIOT_PROTOCOL_H__

#include <stdint.h>

#define  OBJ_DES_MAX_LEN		20
#define  DOMAIN_PWD_LEN			10
#define  MAX_ENTITY_NUM			2

#define  CMD_OBJ_UPDATE			0x10
#define  CMD_CONTROL_ACTUATOR	0x11

#define  CMD_OBJ_TYPE_SENSOR	0
#define  CMD_OBJ_TYPE_ACTUATOR  1

#define  MAX_HANDLE_NUM		8
#define  MAX_SORT_NUM		4

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




typedef struct ENTITY_tag {
	unsigned char	entity_id; 
	char			entity_sort[MAX_SORT_NUM];
	uint32_t		entity_value;
}StEntity,*pStEntity;

typedef struct PROTOCL_PKT_tag {
	unsigned char STX[2];
	uint32_t			  pkt_len;
	unsigned char cmd;
	uint32_t			  obj_id;
	uint32_t			  cont_id;
	uint32_t			  based_id;
	char		  strObjDes[OBJ_DES_MAX_LEN]; 	
	uint32_t			  domain_id; 
	char		  domain_pwd[DOMAIN_PWD_LEN];
	unsigned char obj_type;
	unsigned char entity_num; 
	StEntity	  stEntity[MAX_ENTITY_NUM];
	unsigned char ETX;
}StProtoPkt, *pStProtoPkt;

typedef uint32_t handler_id;


/*
  return value => handler_id
                  if return value < 0  , it is error
*/
handler_id createPktHandle(uint32_t domainId,uint32_t cont_id, uint32_t obj_id, char* strObjDes, unsigned char obj_type,unsigned char entity_num, unsigned char entity_id_1, char* entity_sort_1,
							unsigned char entity_id_2, char* entity_sort_2);
/*
  return value => buffer pointer  ,
                  if return value  == -1 , error 
  resultLen(OUTPUT) => return buff length 				   
*/
unsigned char* makePkt(handler_id handleId, uint32_t* resultLen,uint32_t entity_value_1,uint32_t entity_value_2);
						 
						 
uint32_t pktParcingActuatorControl(unsigned char* buff, uint32_t len,  uint32_t* obj_id, int32_t* entCtlValue1, int32_t* entCtlValue2  );

unsigned int getObj_idFromHandler(handler_id handleId);
#endif 