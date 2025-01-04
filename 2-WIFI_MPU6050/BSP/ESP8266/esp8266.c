#include "esp8266.h"
#include "delay.h"
#include "oled.h"
#include <string.h>
#include <stdio.h>

uint8_t g_uart_rx_buf[ESP8266_UART_RX_BUF_SIZE];
uint8_t g_uart_tx_buf[ESP8266_UART_TX_BUF_SIZE];
uint16_t esp8266_cnt = 0, esp8266_cntPre = 0;


void ESP8266_UART_IRQHandler()
{
	uint8_t res;
	if(USART_GetITStatus(USART2,USART_IT_RXNE))	//判断串口状态
	{
		res=USART_ReceiveData(USART2);	//接收读到的数据
		

		if(esp8266_cnt >= sizeof(g_uart_rx_buf))
						esp8266_cnt = 0; //防止串口被刷爆
		g_uart_rx_buf[esp8266_cnt++] = res;
	}
}


/**
 * @brief       ESP8266 清空串口接收缓存
 * @param       无
 * @retval      无
 */
void esp8266_clear(void)
{
	memset(g_uart_rx_buf, 0, sizeof(g_uart_rx_buf));
	esp8266_cnt = 0;
}

/**
 * @brief       ESP8266 循环调用检测是否接收完成
 * @param       无
 * @retval      ESP8266_EOK-接收完成		ESP8266_ERROR-接收超时未完成
 */
uint8_t esp8266_wait_receive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return ESP8266_ERROR;
		
	if(esp8266_cnt == esp8266_cntPre) {				//如果上一次的值和这次相同，则说明接收完毕
		esp8266_cnt = 0;							//清0接收计数
		return ESP8266_EOK;							//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	return ESP8266_ERROR;							//返回接收未完成标志
}

/**
 * @brief       ESP8266 发送命令
 * @param       cmd-待发送的AT命令，res-期待接收到的字符串
 * @retval      ESP8266_EOK-成功  ESP8266_ERROR-失败
 */
uint8_t esp8266_send_command(uint8_t *cmd, uint8_t *res)
{
	
	uint16_t timeOut = 500;

    esp8266_clear();

	USART_SendStr(ESP8266_UART_INTERFACE, cmd);

	while(timeOut--) {
		if(esp8266_wait_receive() == ESP8266_EOK) {						//如果收到数据
//			printf("%s\r\n",g_uart_rx_buf);
			if(strstr((const char *)g_uart_rx_buf, (const char *)res) != NULL)		//如果检索到关键词
				return ESP8266_EOK;
		}
		delay_ms(10);
	}

	return ESP8266_ERROR;

}

/**
 * @brief       ESP8266 AT指令测试
 * @param       无
 * @retval      ESP8266_EOK  : AT指令测试成功
 *              ESP8266_ERROR: AT指令测试失败
 */
uint8_t esp8266_at_test(void)
{
    return esp8266_send_command((uint8_t *)"AT\r\n", (uint8_t *)"OK");
}

/**
 * @brief       设置ESP8266工作模式
 * @param       mode: 1，Station模式
 *                    2，AP模式
 *                    3，AP+Station模式
 * @retval      ESP8266_EOK   : 工作模式设置成功
 *              ESP8266_ERROR : 工作模式设置失败
 *              ESP8266_EINVAL: mode参数错误，工作模式设置失败
 */
uint8_t esp8266_set_mode(uint8_t mode)
{
    switch (mode) {
        case ESP8266_STA_MODE:
            return esp8266_send_command((uint8_t *)"AT+CWMODE=1\r\n", (uint8_t *)"OK");    /* Station模式 */
        
        case ESP8266_AP_MODE:
            return esp8266_send_command((uint8_t *)"AT+CWMODE=2\r\n", (uint8_t *)"OK");    /* AP模式 */
        
        case ESP8266_STA_AP_MODE:
            return esp8266_send_command((uint8_t *)"AT+CWMODE=3\r\n", (uint8_t *)"OK");    /* AP+Station模式 */
        
        default:
            return ESP8266_EINVAL;
    }
}

/**
 * @brief       ESP8266软件复位
 * @param       无
 * @retval      ESP8266_EOK  : 软件复位成功
 *              ESP8266_ERROR: 软件复位失败
 */
uint8_t esp8266_sw_reset(void)
{
    return esp8266_send_command((uint8_t *)"AT+RST\r\n", (uint8_t *)"OK");
}

/**
 * @brief       ESP8266连接WIFI
 * @param       ssid: WIFI名称
 *              pwd : WIFI密码
 * @retval      ESP8266_EOK  : WIFI连接成功
 *              ESP8266_ERROR: WIFI连接失败
 */
uint8_t esp8266_join_ap(char *ssid, char *pwd)
{
    uint8_t cmd[64];
    
    sprintf((char *)cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    
    return esp8266_send_command(cmd, (uint8_t *)"WIFI GOT IP");
}
