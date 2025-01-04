#ifndef __ESP8266_MQTT_H
#define __ESP8266_MQTT_H

#include "stm32f10x.h"
#include "esp8266_uart.h"
#include "esp8266.h"

#define MQTT_USERNAME      "hr_spo2&k1no3E41ggi"
#define MQTT_PWD           "5cef4a177e72dff632e0ba0535b8b02b5539535014895cf042a83a54fda9487a"
#define MQTT_CLIENTLD      "k1no3E41ggi.hr_spo2|securemode=2\\,signmethod=hmacsha256\\,timestamp=1733300636201|"
#define MQTT_HOSTURL       "iot-06z00cuu985oxx1.mqtt.iothub.aliyuncs.com"
#define MQTT_PORT          1883


uint8_t esp8266_connect_MQTTUSERCFG(char *usename, char *pwd);
uint8_t esp8266_connect_MQTTCLIENTID(char *clientld);
uint8_t esp8266_connect_MQTTCONN(char *mqttHostUrl,uint32_t port);

// 上报数据
uint8_t atk_mw8266d_connect_MQTTPUB_oneData(char *data1,char *data2);
uint8_t atk_mw8266d_connect_MQTTPUB(uint8_t hr,uint8_t spo2,uint8_t xy_high,uint8_t xy_low);

// 断开连接
uint8_t atk_mw8266d_connect_MQTTCLEAN(void);

#endif
