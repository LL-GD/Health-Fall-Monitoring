#ifndef __ESP8266_UART_H
#define __ESP8266_UART_H

#include "stm32f10x.h"

/* Òý½Å¶¨Òå */
#define ESP8266_UART_TX_GPIO_PORT           GPIOA
#define ESP8266_UART_TX_GPIO_PIN            GPIO_Pin_2


#define ESP8266_UART_RX_GPIO_PORT           GPIOA
#define ESP8266_UART_RX_GPIO_PIN            GPIO_Pin_3

#define ESP8266_UART_INTERFACE              USART2
#define ESP8266_UART_IRQn                   USART2_IRQn
#define ESP8266_UART_IRQHandler             USART2_IRQHandler





void esp8266_uart_init(uint32_t baudrate);
void uasrt_sendbyte(USART_TypeDef *pUSART,uint8_t data);
void USART_SendStr(USART_TypeDef *pUSART,uint8_t *str);

#endif
