#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"
#include "esp8266_uart.h"

/* UART收发缓冲大小 */
#define ESP8266_UART_RX_BUF_SIZE            300
#define ESP8266_UART_TX_BUF_SIZE            64

/* 错误代码 */
typedef enum
{ 
	ESP8266_EOK = 0,           /* 没有错误 */
  ESP8266_ERROR ,            /* 通用错误 */
  ESP8266_ETIMEOUT,          /* 超时错误 */
  ESP8266_EINVAL             /* 参数错误 */
}ESP8266_E;

/* 工作模式 */
typedef enum
{
	ESP8266_STA_MODE = 1,  
	ESP8266_AP_MODE,
	ESP8266_STA_AP_MODE
}ESP8266_MODE;

void oled_print_tx(void);

uint8_t esp8266_send_command(uint8_t *cmd, uint8_t *res);

uint8_t esp8266_at_test(void);
uint8_t esp8266_set_mode(uint8_t mode);
uint8_t esp8266_sw_reset(void);
uint8_t esp8266_join_ap(char *ssid, char *pwd);
#endif
