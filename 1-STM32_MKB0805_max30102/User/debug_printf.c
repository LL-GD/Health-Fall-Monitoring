/**
 * @file debug_printf.c
 * @brief printf�ض��򵽴���
 * @version 0.1
 * @date 2022-06-21
 * 
 * @copyright Copyright (c) 2022
 * 
 * 
 */
#include "debug_printf.h"
#include "usart.h"
#include <stdarg.h>
#include <string.h>
#include "stdio.h"

#define DEBUG_SERIAL &huart1

struct __FILE
{
    int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}

/**
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(DEBUG_SERIAL, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
 
/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(DEBUG_SERIAL, &ch, 1, 0xffff);
  return ch;
}



