#include "esp8266_uart.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


/**
 * @brief       ATK-MW8266D UART初始化
 * @param       baudrate: UART通讯波特率
 * @retval      无
 */
void esp8266_uart_init(uint32_t baudrate)
{
		GPIO_InitTypeDef gpio_init_struct;
		USART_InitTypeDef USART_InitStruct;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);               /* 使能UART时钟 */
		
		gpio_init_struct.GPIO_Pin    = ESP8266_UART_TX_GPIO_PIN;        /* UART TX引脚 */
		gpio_init_struct.GPIO_Mode   = GPIO_Mode_AF_PP;                     /* 复用推挽输出 */                           
		gpio_init_struct.GPIO_Speed  = GPIO_Speed_50MHz;                    /* 高速 */
		GPIO_Init(ESP8266_UART_TX_GPIO_PORT, &gpio_init_struct);        /* 初始化UART TX引脚 */
		
		gpio_init_struct.GPIO_Pin    = ESP8266_UART_RX_GPIO_PIN;        /* UART RX引脚 */
		gpio_init_struct.GPIO_Mode   = GPIO_Mode_IN_FLOATING;               /* 输入 */      
		gpio_init_struct.GPIO_Speed  = GPIO_Speed_50MHz;                    /* 高速 */
		GPIO_Init(ESP8266_UART_RX_GPIO_PORT, &gpio_init_struct);        /* 初始化UART RX引脚 */
		
		USART_InitStruct.USART_BaudRate = baudrate;
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;
		USART_InitStruct .USART_StopBits = USART_StopBits_1;
		USART_InitStruct.USART_Parity = USART_Parity_No;
		USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(ESP8266_UART_INTERFACE,&USART_InitStruct);
		
		USART_Cmd(ESP8266_UART_INTERFACE,ENABLE);
	
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		// 配置USART2中断通道
		NVIC_InitStructure.NVIC_IRQChannel = ESP8266_UART_IRQn;
		// 配置抢占优先级，这里设置为1
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		// 配置子优先级，这里设置为1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		// 使能中断通道
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		// 初始化NVIC通道
		NVIC_Init(&NVIC_InitStructure);
		
		// 使能UART接收中断（对应标准库中的USART_IT_RXNE）
    USART_ITConfig(ESP8266_UART_INTERFACE, USART_IT_RXNE, ENABLE);

}

//发送一个字符
void uasrt_sendbyte(USART_TypeDef *pUSART,uint8_t data)
{
	USART_SendData(pUSART,data);
	while(USART_GetFlagStatus(pUSART,USART_FLAG_TXE) == RESET);
}
//发送字符串
void USART_SendStr(USART_TypeDef *pUSART,uint8_t *str)
{
	while(*str != '\0')
	{
		uasrt_sendbyte(pUSART,*str++);
	}
}

