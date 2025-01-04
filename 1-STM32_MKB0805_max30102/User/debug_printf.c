/**
 * @file debug_printf.c
 * @brief printf重定向到串口
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
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(DEBUG_SERIAL, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
 
/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(DEBUG_SERIAL, &ch, 1, 0xffff);
  return ch;
}



