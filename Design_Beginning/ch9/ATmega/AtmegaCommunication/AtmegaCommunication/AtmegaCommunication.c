/*
 * AtmegaCommunication.c
 *
 * Created: 2016-05-02 오전 9:16:31
 *  Author: dogu, seokjae
 */ 

#include <avr/io.h>

#include <avr/interrupt.h>
#include <util/delay.h>
#include "debug.h"
#include "timer.h"
#include "realIotProtocol.h"
#include "sensor.h"
#include "fan.h"
#include "Esp8266.h"

#define SERVER_IP_STR	"192.168.10.227"
#define SERVER_PORT		50001

#define SELECTED_AP_SSID	"CNDI_AES"
#define SELECTED_AP_PASS	"cndi313182246"
//Server connection Info 
#define CNDI_USER_DOMAIN		1111
#define CONTROL_ID				0x00000010

//Server connection Info2 
#define OBJECT_ID_TEMP_MONITOR		0x00000011
#define OBJECT_DESC_TEMP_MONITOR	"Sensing Temp 1,2" // max 20 문자 
#define ENTITY_SORT1_TEMP_MONITOR	"C"
#define ENTITY_SORT2_TEMP_MONITOR	"C"
#define ENTITYID_1_TEMP_MONITOR1	1
#define ENTITYID_2_TEMP_MONITOR2	2

#define OBJECT_ID_FAN_CONTROL		0x00000012
#define OBJECT_DESC_FAN_CONTROL		"Control Fan 1,2"
#define ENTITY_SORT1_FAN_CONTROL	"On"
#define ENTITY_SORT2_FAN_CONTROL	"On"
#define ENTITYID_1_FAN_CONTROL1		3
#define ENTITYID_2_FAN_CONTROL2		4

#define SENSOR_INIT		0

#define FAN_ON	1
#define FAN_OFF	0

void SendSensorData( handler_id handleId, int sensorEntity1Value, int sensorEntity2Value);
void SendActuatorData (handler_id handleId, int actuatorEntity1Value, int actuatorEntity2Value);

volatile int gnFanOnValue;
uint8_t bClientConnected;

static void eventCallback(int eventType,uint8_t* rxBuff, int rxSize)
{
	uint32_t obj_id;
	uint32_t entityValue1;
	uint32_t entityValue2;
	if (eventType == EVENT_RX_DATA) 
	{
		if (pktParcingActuatorControl(rxBuff, rxSize, &obj_id, &entityValue1, &entityValue2) < 0 )
		{
			debugprint("Rx parcing fail.\r\n");
			return;			
		}
		debugprint("value1:%d,value2:%d\r\n",entityValue1,entityValue2);
		switch(obj_id )
		{
			case OBJECT_ID_FAN_CONTROL:
			gnFanOnValue = entityValue1;
			debugprint("gnFanOnValue : %d\r\n",gnFanOnValue);
			break;
			default:
			debugprint("no supported object id.\r\n");
			break;  
		}
		
	}
}

int main(void)
{
	debugInit();
	
	wifiInit();
	
	sei();
	
	debugprint("wifi start\r\n");
	_delay_ms(1000);

	wificlearUartBuff();
	// UART echo disable 
	wifiEchoDisable();
	wificlearUartBuff();
	
	// register callback function 
	wifiSetEventCallback(eventCallback);
	
	// Auto connection disable
	wifiAutoConnectDisable();
	wificlearUartBuff();
	
	wifiModeSet();
	
	wifiEnableDHCP();
	
	debugprint("\r\nAP list display:\r\n");
	wifiDisplayAPlist();
	
	// connect  AP 
	debugprint("\r\n");
	if ( !wifiConnectAP(SELECTED_AP_SSID, SELECTED_AP_PASS))
	{
		debugprint("AP connected.\r\n");
	}
	else
	{
		debugprint("AP connection fail.\r\n");
	}
	// display local allocated IP 
	debugprint("\r\n Allocated local IP:\r\n");
	wifiDisplayLocalIP();
	debugprint("\r\n");
	
	// TCP remote server connection
	if( !wifiConnectTCPServer(SERVER_IP_STR,SERVER_PORT))
	{
		bClientConnected = 1;
		debugprint("Connected Server.\r\n");
	}
	else
	{
		bClientConnected = 0;
		debugprint("Server connection fail.\r\n");
	}				
	debugprint("\r\n");
	wificlearUartBuff();

	// 두개의 객체
	handler_id  handleIdSensor = -1;	// 온도 센서
	handler_id  handleIdActuator = -1;	// FAN 액츄에이터
	
	char  obj_des[] = OBJECT_DESC_TEMP_MONITOR;
	char  entity_sort1[] = ENTITY_SORT1_TEMP_MONITOR;
	char  entity_sort2[] = ENTITY_SORT2_TEMP_MONITOR;
	
	handleIdSensor = createPktHandle((uint32_t)CNDI_USER_DOMAIN,(uint32_t)CONTROL_ID, (uint32_t)OBJECT_ID_TEMP_MONITOR, obj_des, (uint8_t)CMD_OBJ_TYPE_SENSOR,1, 
										(uint8_t)ENTITYID_1_TEMP_MONITOR1,entity_sort1,(uint8_t)ENTITYID_2_TEMP_MONITOR2, entity_sort2); 


	char  obj_des_ac[] = OBJECT_DESC_FAN_CONTROL;
	char  entity_sort1_ac[] = ENTITY_SORT1_FAN_CONTROL;
	char  entity_sort2_ac[] = ENTITY_SORT2_FAN_CONTROL;
	
	handleIdActuator = createPktHandle((uint32_t)CNDI_USER_DOMAIN,(uint32_t)CONTROL_ID, (uint32_t)OBJECT_ID_FAN_CONTROL, obj_des_ac, (uint8_t)CMD_OBJ_TYPE_ACTUATOR,1,
			(uint8_t)ENTITYID_1_FAN_CONTROL1,entity_sort1_ac,(uint8_t)ENTITYID_2_FAN_CONTROL2, entity_sort2_ac);
	
	SendSensorData(handleIdSensor, SENSOR_INIT, 0);
	
	_delay_ms(1000);
	
	SendActuatorData(handleIdActuator, FAN_OFF, 0);
	
	TIMER_100mSInit ();
	setElapsedTime100mSUnit(100);	// 1 Sec

/*************** Sensor and Actuator Initialization ****************/
	// temp, humi sensor
	int tempData;
	SHT11_Init();
	
	// fan actuator
	gnFanOnValue = 0; // default disable
	initFan();

	debugprint("INDEX_ENT_VALUE_1:%d\r\n",INDEX_ENT_VALUE_1);

	while(1)
	{
		tempData = get_SHT11_data(TEMP);
		debugprint("temp = %d [C], fan value = %d \r\n", tempData, gnFanOnValue);

		FAN_Action(gnFanOnValue);
			
		wifiMain();
		
		if ( isElapsed())
		{
			SendSensorData(handleIdSensor, tempData, 0);
		}
	}
}

void SendSensorData( handler_id handleId, int sensorEntity1Value, int sensorEntity2Value)
{
	uint32_t len;
	uint8_t* buff;
	buff = makePkt(handleId,&len,sensorEntity1Value,sensorEntity2Value); // make packet
	if ( buff == 0)
	{
		debugprint("SendSensor Data -- makePkt error.\n");
		return;
	}
	if ( bClientConnected)
	{
		wifiSendData(buff, len);
	}
	else
	{
		debugprint("disconnected, tx error\r\n");
	}				    
}

void SendActuatorData (handler_id handleId, int actuatorEntity1Value, int actuatorEntity2Value)
{
	uint32_t len;
	uint8_t* buff;
	buff = makePkt(handleId,&len,actuatorEntity1Value,actuatorEntity2Value); // make packet
	if ( buff == 0)
	{
		debugprint("actuator Data -- makePkt error.\n");
		return;
	}
	if ( bClientConnected)
	{
		wifiSendData(buff, len);
	}
	else
	{
		debugprint("disconnected, tx error\r\n");
	}		
}


