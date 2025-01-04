#include "esp8266_uart.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


/**
 * @brief       ATK-MW8266D UART��ʼ��
 * @param       baudrate: UARTͨѶ������
 * @retval      ��
 */
void esp8266_uart_init(uint32_t baudrate)
{
		GPIO_InitTypeDef gpio_init_struct;
		USART_InitTypeDef USART_InitStruct;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);               /* ʹ��UARTʱ�� */
		
		gpio_init_struct.GPIO_Pin    = ESP8266_UART_TX_GPIO_PIN;        /* UART TX���� */
		gpio_init_struct.GPIO_Mode   = GPIO_Mode_AF_PP;                     /* ����������� */                           
		gpio_init_struct.GPIO_Speed  = GPIO_Speed_50MHz;                    /* ���� */
		GPIO_Init(ESP8266_UART_TX_GPIO_PORT, &gpio_init_struct);        /* ��ʼ��UART TX���� */
		
		gpio_init_struct.GPIO_Pin    = ESP8266_UART_RX_GPIO_PIN;        /* UART RX���� */
		gpio_init_struct.GPIO_Mode   = GPIO_Mode_IN_FLOATING;               /* ���� */      
		gpio_init_struct.GPIO_Speed  = GPIO_Speed_50MHz;                    /* ���� */
		GPIO_Init(ESP8266_UART_RX_GPIO_PORT, &gpio_init_struct);        /* ��ʼ��UART RX���� */
		
		USART_InitStruct.USART_BaudRate = baudrate;
		USART_InitStruct.USART_WordLength = USART_WordLength_8b;
		USART_InitStruct .USART_StopBits = USART_StopBits_1;
		USART_InitStruct.USART_Parity = USART_Parity_No;
		USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
		USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(ESP8266_UART_INTERFACE,&USART_InitStruct);
		
		USART_Cmd(ESP8266_UART_INTERFACE,ENABLE);
	
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		// ����USART2�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannel = ESP8266_UART_IRQn;
		// ������ռ���ȼ�����������Ϊ1
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		// ���������ȼ�����������Ϊ1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		// ʹ���ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		// ��ʼ��NVICͨ��
		NVIC_Init(&NVIC_InitStructure);
		
		// ʹ��UART�����жϣ���Ӧ��׼���е�USART_IT_RXNE��
    USART_ITConfig(ESP8266_UART_INTERFACE, USART_IT_RXNE, ENABLE);

}

//����һ���ַ�
void uasrt_sendbyte(USART_TypeDef *pUSART,uint8_t data)
{
	USART_SendData(pUSART,data);
	while(USART_GetFlagStatus(pUSART,USART_FLAG_TXE) == RESET);
}
//�����ַ���
void USART_SendStr(USART_TypeDef *pUSART,uint8_t *str)
{
	while(*str != '\0')
	{
		uasrt_sendbyte(pUSART,*str++);
	}
}

