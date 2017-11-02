#include "realIotProtocol.h"
#include <string.h>
#include <stdint.h>

volatile StProtoPkt stPktArr[MAX_HANDLE_NUM];
volatile uint32_t  currentHandlesize = 0; 

handler_id createPktHandle(uint32_t domainId,uint32_t cont_id, uint32_t obj_id, char* strObjDes, unsigned char obj_type,unsigned char entity_num, unsigned char entity_id_1, char* entity_sort_1,
				unsigned char entity_id_2, char* entity_sort_2)
{
	if ( currentHandlesize >= MAX_HANDLE_NUM)
		return -1;
	
	uint32_t temId = 	currentHandlesize;
	
	stPktArr[temId].STX[0] = 0xfd;
	stPktArr[temId].STX[1] = 0xfe;
	
	stPktArr[temId].cmd = CMD_OBJ_UPDATE;
	stPktArr[temId].pkt_len = 0x43;
	stPktArr[temId].domain_id = domainId;
	
	stPktArr[temId].cont_id = cont_id;

	stPktArr[temId].obj_id = obj_id;
	strcpy((char*)stPktArr[temId].domain_pwd,"reg1111");

	uint32_t objDesLen = (uint32_t)strlen(strObjDes);
	
	memset((void*)stPktArr[temId].strObjDes,0,OBJ_DES_MAX_LEN); // set 0 init
	if ( objDesLen >= OBJ_DES_MAX_LEN)
	{
		memcpy((void*)stPktArr[temId].strObjDes,(void*)strObjDes,OBJ_DES_MAX_LEN);
	}
	else
	{
		memcpy((void*)stPktArr[temId].strObjDes,(void*)strObjDes,objDesLen);
	}

	stPktArr[temId].obj_type = obj_type;
	
	if (entity_num > 2 ) 
		stPktArr[temId].entity_num = 2;
	else
		stPktArr[temId].entity_num = entity_num;
	
	#if 1 // for test --------------------------------------------------------------------
//	stPktArr[temId].based_id = 0x00345678;
	stPktArr[temId].based_id = 0x00000000;
	#endif
	
	if ( entity_num > 0)
	{
		stPktArr[temId].stEntity[0].entity_id = entity_id_1;
		memset((void*)stPktArr[temId].stEntity[0].entity_sort,0,MAX_SORT_NUM); // set 0 init
		if (strlen(entity_sort_1) >=  MAX_SORT_NUM)
			memcpy((void*)stPktArr[temId].stEntity[0].entity_sort,(void*)entity_sort_1, MAX_SORT_NUM);
		else
			memcpy((void*)stPktArr[temId].stEntity[0].entity_sort,(void*)entity_sort_1, strlen(entity_sort_1));
			
		
	}
	if ( entity_num > 1)
	{
		stPktArr[temId].stEntity[1].entity_id = entity_id_2;
		memset((void*)stPktArr[temId].stEntity[1].entity_sort,0,MAX_SORT_NUM); // set 0 init
		if (strlen(entity_sort_1) >=  MAX_SORT_NUM)
			memcpy((void*)stPktArr[temId].stEntity[1].entity_sort,(void*)entity_sort_2,MAX_SORT_NUM);
		else
			memcpy((void*)stPktArr[temId].stEntity[1].entity_sort,(void*)entity_sort_2, strlen(entity_sort_2));		
	}
	stPktArr[temId].ETX = 0xff;
	currentHandlesize++;
	return temId;	
}

unsigned char* makePkt(handler_id handleId, uint32_t* resultLen,uint32_t entity_value_1,uint32_t entity_value_2)
{
	if (handleId >= currentHandlesize )
	{
		return NULL;
	}
	
	stPktArr[handleId].stEntity[0].entity_value = entity_value_1;
	stPktArr[handleId].stEntity[1].entity_value = entity_value_2;
	
	*resultLen = sizeof(StProtoPkt);
	return (unsigned char*)(stPktArr+handleId);
}

uint32_t pktParcingActuatorControl(unsigned char* buff, uint32_t len, uint32_t* obj_id, int32_t* entCtlValue1, int32_t* entCtlValue2  )
{
	if (NORMAL_PACK_LEN != len )
	{
		return -1;
	}
	
	if ( (buff[INDEX_STX] != 0xFD) || (buff[INDEX_STX+1] != 0xFE) )
	{
		return -1;		
	}
	
	if (buff[INDEX_ETX] != 0xFF)
	{
		return -1;
	}
	
	if (*(int32_t*)(buff + INDEX_LEN) != (len -7))
	{		
		return -1;
	}
	
	*obj_id = *(unsigned int*)(buff+INDEX_OBJ_ID);
	*entCtlValue1 = *(int32_t*)(buff+INDEX_ENT_VALUE_1);
	*entCtlValue2 = *(int32_t*)(buff+INDEX_ENT_VALUE_2);
	
	
	return len;
	
}

unsigned int getObj_idFromHandler(handler_id handleId)
{
	if (handleId >= currentHandlesize )
	{
		return -1;
	}	
	return stPktArr[handleId].obj_id;
}