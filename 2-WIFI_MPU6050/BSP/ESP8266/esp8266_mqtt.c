#include "esp8266_mqtt.h"
#include <stdio.h>

// 连接
uint8_t esp8266_connect_MQTTUSERCFG(char *usename, char *pwd)
{
		uint8_t cmd[140];
		//sprintf((char *)cmd,"AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"", usename, pwd);
		sprintf((char *)cmd,"AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n",usename,pwd);
		return esp8266_send_command(cmd,(uint8_t *)"OK");
}

uint8_t esp8266_connect_MQTTCLIENTID(char *clientld)
{

    uint8_t cmd[140];
		sprintf((char *)cmd, "AT+MQTTCLIENTID=0,\"%s\"\r\n",clientld);
		return esp8266_send_command(cmd,(uint8_t *)"OK");
}

uint8_t esp8266_connect_MQTTCONN(char *mqttHostUrl,uint32_t port)
{
    uint8_t cmd[140];
		sprintf((char *)cmd, "AT+MQTTCONN=0,\"%s\",%d,1\r\n", mqttHostUrl,port);
		printf("%s",cmd);

	
	return esp8266_send_command(cmd,(uint8_t *)"+");

}

// 上报数据

uint8_t atk_mw8266d_connect_MQTTPUB_oneData(char *data1,char *data2)
{
//AT+MQTTPUB=0,"/sys/k1no3E41ggi/hr_spo2/thing/event/property/post","{\"method\":\"thing.event.property.post\"\,\"id\":\"1231\"\,\"params\":{\"diedao\":1}\,\"version\":\"1.0.0\"}",1,0
		uint8_t cmd[300];
		
	
		sprintf((char *)cmd,"AT+MQTTPUB=0,\"/sys/k1no3E41ggi/hr_spo2/thing/event/property/post\",\"{\\\"method\\\":\\\"thing.event.property.post\\\"\\,\\\"id\\\":\\\"1231\\\"\\,\\\"params\\\":{\\\"%s\\\":%s}\\,\\\"version\\\":\\\"1.0.0\\\"}\",1,0\r\n",data1,data2);

		return esp8266_send_command(cmd,(uint8_t *)"OK");

}

uint8_t atk_mw8266d_connect_MQTTPUB(uint8_t hr,uint8_t spo2,uint8_t xy_high,uint8_t xy_low)
{
//AT+MQTTPUB=0,"/sys/k1no3E41ggi/hr_spo2/thing/event/property/post","{\"method\":\"thing.event.property.post\"\,\"id\":\"1231\"\,\"params\":{\"diedao\":1}\,\"version\":\"1.0.0\"}",1,0
		uint8_t cmd[300];

		sprintf((char *)cmd,"AT+MQTTPUB=0,\"/sys/k1no3E41ggi/hr_spo2/thing/event/property/post\",\"{\\\"method\\\":\\\"thing.event.property.post\\\"\\,\\\"id\\\":\\\"1231\\\"\\,\\\"params\\\":{\\\"HeartRate\\\":%d\\,\\\"spo2\\\":%d\\,\\\"xueya_high\\\":%d\\,\\\"xueya_low\\\":%d}\\,\\\"version\\\":\\\"1.0.0\\\"}\",1,0\r\n",hr,spo2,xy_high,xy_low);
			printf("%s",cmd);
		return esp8266_send_command(cmd,(uint8_t *)"OK");

}

// 断开连接
uint8_t atk_mw8266d_connect_MQTTCLEAN(void)
{
		return esp8266_send_command((uint8_t *)"AT+MQTTCLEAN=0\r\n",(uint8_t *)"OK");
}
