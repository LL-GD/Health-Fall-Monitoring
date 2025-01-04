#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"
#include "esp8266_uart.h"

/* UART�շ������С */
#define ESP8266_UART_RX_BUF_SIZE            300
#define ESP8266_UART_TX_BUF_SIZE            64

/* ������� */
typedef enum
{ 
	ESP8266_EOK = 0,           /* û�д��� */
  ESP8266_ERROR ,            /* ͨ�ô��� */
  ESP8266_ETIMEOUT,          /* ��ʱ���� */
  ESP8266_EINVAL             /* �������� */
}ESP8266_E;

/* ����ģʽ */
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
