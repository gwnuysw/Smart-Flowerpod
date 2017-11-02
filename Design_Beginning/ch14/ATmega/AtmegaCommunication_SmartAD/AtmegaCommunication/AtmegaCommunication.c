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
#include "Esp8266.h"
#include "btHm11.h"		// read Bluetooth Mac
#include "gpio.h"

//#include "sensor.h"
//#include "cds.h"
//#include "fan.h"
//#include "led.h"
//#include "motor.h"
#include "lcd.h"

//#define TEMP_HUMI
//#define CDS
//#define FAN
//#define LED
//#define MOTOR
#define TEXTLCD

#define SERVER_IP_STR	"192.168.10.141"
#define SERVER_PORT		50001

#define SELECTED_AP_SSID	"CNDI_AES"
#define SELECTED_AP_PASS	"cndi313182246"

//Server connection Info 
#define CNDI_USER_DOMAIN		1111
#define CONTROL_ID				0x00000010

//Server connection Info2 
#define OBJECT_ID_TEMP_MONITOR		0x00000011
#define OBJECT_DESC_TEMP_MONITOR	"Sensing Temp 1"		// max 20 문자 
#define ENTITY_SORT1_TEMP_MONITOR	"C"
#define ENTITY_SORT2_TEMP_MONITOR	""
#define ENTITYID_1_TEMP_MONITOR1	1
#define ENTITYID_2_TEMP_MONITOR2	2

#define OBJECT_ID_FAN_CONTROL		0x00000012
#define OBJECT_DESC_FAN_CONTROL		"Control Fan 1"
#define ENTITY_SORT1_FAN_CONTROL	"On"
#define ENTITY_SORT2_FAN_CONTROL	""
#define ENTITYID_1_FAN_CONTROL1		1
#define ENTITYID_2_FAN_CONTROL2		2

#define OBJECT_ID_CDS_MONITOR		0x00000013
#define OBJECT_DESC_CDS_MONITOR		"Sensing CDS 1"
#define ENTITY_SORT1_CDS_MONITOR	"Lux"
#define ENTITY_SORT2_CDS_MONITOR	""
#define ENTITYID_1_CDS_MONITOR1		1
#define ENTITYID_2_CDS_MONITOR2		2

#define OBJECT_ID_LED_CONTROL		0x00000014
#define OBJECT_DESC_LED_CONTROL		"Control LED 1"
#define ENTITY_SORT1_LED_CONTROL	"On"
#define ENTITY_SORT2_LED_CONTROL	""
#define ENTITYID_1_LED_CONTROL1		1
#define ENTITYID_2_LED_CONTROL2		2

#define OBJECT_ID_MOTOR_CONTROL			0x00000015
#define OBJECT_DESC_MOTOR_CONTROL		"Control Motor 1"
#define ENTITY_SORT1_MOTOR_CONTROL		"On"
#define ENTITY_SORT2_MOTOR_CONTROL		""
#define ENTITYID_1_MOTOR_CONTROL1		1
#define ENTITYID_2_MOTOR_CONTROL2		2

#define OBJECT_ID_TEXTLCD_CONTROL		0x00000016
#define OBJECT_DESC_TEXTLCD_CONTROL		"Control TextLCD 1"
#define ENTITY_SORT1_TEXTLCD_CONTROL	"On"
#define ENTITY_SORT2_TEXTLCD_CONTROL	""
#define ENTITYID_1_TEXTLCD_CONTROL1		1
#define ENTITYID_2_TEXTLCD_CONTROL2		2


#define SENSOR_INIT		0

#define FAN_ON	1
#define FAN_OFF	0

#define LED_ON	1
#define LED_OFF	0

#define TEXTLCD_INIT	-1

#define MAKEUP		1
#define SHOES		2
#define SPORT		3

void SendSensorData( handler_id handleId, int sensorEntity1Value, int sensorEntity2Value);
void SendActuatorData (handler_id handleId, int actuatorEntity1Value, int actuatorEntity2Value);

volatile int gnFanOnValue;
volatile int gnLedOnValue;
volatile int gnMotorValue;
volatile int gnTextLcdValue;

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
			case OBJECT_ID_LED_CONTROL:
				gnLedOnValue = entityValue1;
				debugprint("gnLedOnValue : %d\r\n",gnLedOnValue);
				break;
			case OBJECT_ID_MOTOR_CONTROL:
				gnMotorValue = entityValue1;
				debugprint("gnMotorValue : %d\r\n",gnMotorValue);
				break;
			case OBJECT_ID_TEXTLCD_CONTROL:
				gnTextLcdValue = entityValue1;
				debugprint("gnTextLcdValue : %d\r\n",gnTextLcdValue);
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
	wifiInit();			// init wifi
	btInit();			// init bluetooth
	
	sei();				// set interrupt
	
	
	debugprint("\r\nread Bluetooth Mac Address \r\n");
	btprint("AT+ADDR?");

	_delay_ms(1000);

	btReadMac();
	_delay_ms(4000);
	
	
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

/*************** Sensor and Actuator ****************/

/**********		temp, humi sensor		**********/
#ifdef TEMP_HUMI
	handler_id  handleIdSensor = -1;		// Temp sensor
	
	char  obj_des[] = OBJECT_DESC_TEMP_MONITOR;
	char  entity_sort1[] = ENTITY_SORT1_TEMP_MONITOR;
	char  entity_sort2[] = ENTITY_SORT2_TEMP_MONITOR;
	
	handleIdSensor = createPktHandle((uint32_t)CNDI_USER_DOMAIN,(uint32_t)CONTROL_ID, (uint32_t)OBJECT_ID_TEMP_MONITOR, obj_des, (uint8_t)CMD_OBJ_TYPE_SENSOR,1, 
										(uint8_t)ENTITYID_1_TEMP_MONITOR1,entity_sort1,(uint8_t)ENTITYID_2_TEMP_MONITOR2, entity_sort2); 

	int tempData;
	SHT11_Init();
	
	SendSensorData(handleIdSensor, SENSOR_INIT, 0);
#endif

/**********		cds sensor		**********/
#ifdef CDS
	handler_id  handleIdSensor1 = -1;		// CDS sensor
	
	char  obj_des_s1[] = OBJECT_DESC_CDS_MONITOR;
	char  entity_sort1_s1[] = ENTITY_SORT1_CDS_MONITOR;
	char  entity_sort2_s1[] = ENTITY_SORT2_CDS_MONITOR;
	
	handleIdSensor1 = createPktHandle((uint32_t)CNDI_USER_DOMAIN,(uint32_t)CONTROL_ID, (uint32_t)OBJECT_ID_CDS_MONITOR, obj_des_s1, (uint8_t)CMD_OBJ_TYPE_SENSOR,1,
										(uint8_t)ENTITYID_1_CDS_MONITOR1,entity_sort1_s1,(uint8_t)ENTITYID_2_CDS_MONITOR2, entity_sort2_s1);

	int cdsData;
	CDS_Init(CDS_CHANNEL);		// CDS_CHANNEL = ADC channel
	
	SendSensorData(handleIdSensor1, SENSOR_INIT, 0);
#endif

	_delay_ms(1000);

/**********		fan actuator		**********/
#ifdef FAN
	handler_id  handleIdActuator = -1;		// Fan Actuator
	
	char  obj_des_ac[] = OBJECT_DESC_FAN_CONTROL;
	char  entity_sort1_ac[] = ENTITY_SORT1_FAN_CONTROL;
	char  entity_sort2_ac[] = ENTITY_SORT2_FAN_CONTROL;
	
	handleIdActuator = createPktHandle((uint32_t)CNDI_USER_DOMAIN,(uint32_t)CONTROL_ID, (uint32_t)OBJECT_ID_FAN_CONTROL, obj_des_ac, (uint8_t)CMD_OBJ_TYPE_ACTUATOR,1,
											(uint8_t)ENTITYID_1_FAN_CONTROL1,entity_sort1_ac,(uint8_t)ENTITYID_2_FAN_CONTROL2, entity_sort2_ac);

	gnFanOnValue = 0;			// default disable
	initFan();
	
	SendActuatorData(handleIdActuator, FAN_OFF, 0);
#endif

/**********		led actuator		**********/
#ifdef LED
	handler_id  handleIdActuator1 = -1;		// LED Actuator
	
	char  obj_des_ac1[] = OBJECT_DESC_LED_CONTROL;
	char  entity_sort1_ac1[] = ENTITY_SORT1_LED_CONTROL;
	char  entity_sort2_ac1[] = ENTITY_SORT2_LED_CONTROL;
	
	handleIdActuator1 = createPktHandle((uint32_t)CNDI_USER_DOMAIN,(uint32_t)CONTROL_ID, (uint32_t)OBJECT_ID_LED_CONTROL, obj_des_ac1, (uint8_t)CMD_OBJ_TYPE_ACTUATOR,1,
											(uint8_t)ENTITYID_1_LED_CONTROL1,entity_sort1_ac1,(uint8_t)ENTITYID_2_LED_CONTROL2, entity_sort2_ac1);

	gnLedOnValue = 0;			// default disable
	LED_Init (LED_PORT, LED_PIN);
	
	SendActuatorData(handleIdActuator1, LED_OFF, 0);
#endif

/**********		motor actuator		**********/
#ifdef MOTOR
	handler_id  handleIdActuator2 = -1;		// Motor Actuator
	
	char  obj_des_ac2[] = OBJECT_DESC_MOTOR_CONTROL;
	char  entity_sort1_ac2[] = ENTITY_SORT1_MOTOR_CONTROL;
	char  entity_sort2_ac2[] = ENTITY_SORT2_MOTOR_CONTROL;
	
	handleIdActuator2 = createPktHandle((uint32_t)CNDI_USER_DOMAIN,(uint32_t)CONTROL_ID, (uint32_t)OBJECT_ID_MOTOR_CONTROL, obj_des_ac2, (uint8_t)CMD_OBJ_TYPE_ACTUATOR,1,
										(uint8_t)ENTITYID_1_MOTOR_CONTROL1,entity_sort1_ac2,(uint8_t)ENTITYID_2_MOTOR_CONTROL2, entity_sort2_ac2);

	gnMotorValue = 0;			// default disable
	Motor_Init();
	
	SendActuatorData(handleIdActuator2, MOTOR_OFF, 0);
#endif

/**********		textlcd actuator		**********/
#ifdef TEXTLCD
	handler_id  handleIdActuator3 = -1;		// TextLCD Actuator
	
	char  obj_des_ac3[] = OBJECT_DESC_TEXTLCD_CONTROL;
	char  entity_sort1_ac3[] = ENTITY_SORT1_TEXTLCD_CONTROL;
	char  entity_sort2_ac3[] = ENTITY_SORT2_TEXTLCD_CONTROL;
	
	handleIdActuator3 = createPktHandle((uint32_t)CNDI_USER_DOMAIN,(uint32_t)CONTROL_ID, (uint32_t)OBJECT_ID_TEXTLCD_CONTROL, obj_des_ac3, (uint8_t)CMD_OBJ_TYPE_ACTUATOR,1,
	(uint8_t)ENTITYID_1_TEXTLCD_CONTROL1,entity_sort1_ac3,(uint8_t)ENTITYID_2_TEXTLCD_CONTROL2, entity_sort2_ac3);

	gnTextLcdValue = -1;		// default disable
	LCD_Init(LCD_CD_PORT, RS_PIN, RW_PIN, E_PIN, LCD_DB_PORT);

	write_Command(0x01);			/* Clear Display */
	_delay_ms(9);
	write_Command(0x80);			/* Set DDRAM Address */
	_delay_us(220);
	
	SendActuatorData(handleIdActuator3, TEXTLCD_INIT, 0);
#endif
	
	
	TIMER_100mSInit ();
	setElapsedTime100mSUnit(100);	// 1 Sec


	debugprint("INDEX_ENT_VALUE_1:%d\r\n",INDEX_ENT_VALUE_1);

	while(1)
	{
		
#ifdef TEMP_HUMI
		tempData = get_SHT11_data(TEMP);
		debugprint("temp = %d [C] \r\n", tempData);
#endif

#ifdef CDS
		cdsData = AdcRead();
		debugprint("cds = %d [lux] \r\n", cdsData);
#endif

#ifdef FAN
		debugprint("fan value = %d \r\n", gnFanOnValue);
		FAN_Action(gnFanOnValue);
#endif

#ifdef LED
		debugprint("led value = %d \r\n", gnLedOnValue);
		controlLED(LED_PORT, LED_PIN, gnLedOnValue);
#endif

#ifdef MOTOR
		debugprint("motor value = %d \r\n", gnMotorValue);
		Motor_Action(gnMotorValue, gnMotorValue);
#endif

#ifdef TEXTLCD
	debugprint("textlcd value = %d \r\n", gnTextLcdValue);

	if (gnTextLcdValue == MAKEUP)
	{
		printString ( "Everyday New    " );
		write_Command(0xC0);			/* Move into 2nd Line */
		_delay_us(220);
		printString ( "Face! -CN Lotion" );
		write_Command(0x02);			/* return Home */
		_delay_us(220);		
	}
	else if(gnTextLcdValue == SHOES)
	{
		printString ( "Your Perfect    " );
		write_Command(0xC0);			/* Move into 2nd Line */
		_delay_us(220);
		printString ( "Running Partner!" );
		write_Command(0x02);			/* return Home */
		_delay_us(220);
	}	
	else	// (gnTextLcdValue == SPORT)
	{
		printString ( "Sound mind,sound" );
		write_Command(0xC0);			/* Move into 2nd Line */
		_delay_us(220);
		printString ( "body! -CN Sports" );
		write_Command(0x02);			/* return Home */
		_delay_us(220);
	}
#endif

		wifiMain();
		
		if ( isElapsed())
		{
#ifdef TEMP_HUMI
			SendSensorData(handleIdSensor, tempData, 0);
#endif
#ifdef CDS
			SendSensorData(handleIdSensor1, cdsData, 0);
#endif
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


