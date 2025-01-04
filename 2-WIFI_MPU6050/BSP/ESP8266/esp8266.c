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
	if(USART_GetITStatus(USART2,USART_IT_RXNE))	//�жϴ���״̬
	{
		res=USART_ReceiveData(USART2);	//���ն���������
		

		if(esp8266_cnt >= sizeof(g_uart_rx_buf))
						esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
		g_uart_rx_buf[esp8266_cnt++] = res;
	}
}


/**
 * @brief       ESP8266 ��մ��ڽ��ջ���
 * @param       ��
 * @retval      ��
 */
void esp8266_clear(void)
{
	memset(g_uart_rx_buf, 0, sizeof(g_uart_rx_buf));
	esp8266_cnt = 0;
}

/**
 * @brief       ESP8266 ѭ�����ü���Ƿ�������
 * @param       ��
 * @retval      ESP8266_EOK-�������		ESP8266_ERROR-���ճ�ʱδ���
 */
uint8_t esp8266_wait_receive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return ESP8266_ERROR;
		
	if(esp8266_cnt == esp8266_cntPre) {				//�����һ�ε�ֵ�������ͬ����˵���������
		esp8266_cnt = 0;							//��0���ռ���
		return ESP8266_EOK;							//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	return ESP8266_ERROR;							//���ؽ���δ��ɱ�־
}

/**
 * @brief       ESP8266 ��������
 * @param       cmd-�����͵�AT���res-�ڴ����յ����ַ���
 * @retval      ESP8266_EOK-�ɹ�  ESP8266_ERROR-ʧ��
 */
uint8_t esp8266_send_command(uint8_t *cmd, uint8_t *res)
{
	
	uint16_t timeOut = 500;

    esp8266_clear();

	USART_SendStr(ESP8266_UART_INTERFACE, cmd);

	while(timeOut--) {
		if(esp8266_wait_receive() == ESP8266_EOK) {						//����յ�����
//			printf("%s\r\n",g_uart_rx_buf);
			if(strstr((const char *)g_uart_rx_buf, (const char *)res) != NULL)		//����������ؼ���
				return ESP8266_EOK;
		}
		delay_ms(10);
	}

	return ESP8266_ERROR;

}

/**
 * @brief       ESP8266 ATָ�����
 * @param       ��
 * @retval      ESP8266_EOK  : ATָ����Գɹ�
 *              ESP8266_ERROR: ATָ�����ʧ��
 */
uint8_t esp8266_at_test(void)
{
    return esp8266_send_command((uint8_t *)"AT\r\n", (uint8_t *)"OK");
}

/**
 * @brief       ����ESP8266����ģʽ
 * @param       mode: 1��Stationģʽ
 *                    2��APģʽ
 *                    3��AP+Stationģʽ
 * @retval      ESP8266_EOK   : ����ģʽ���óɹ�
 *              ESP8266_ERROR : ����ģʽ����ʧ��
 *              ESP8266_EINVAL: mode�������󣬹���ģʽ����ʧ��
 */
uint8_t esp8266_set_mode(uint8_t mode)
{
    switch (mode) {
        case ESP8266_STA_MODE:
            return esp8266_send_command((uint8_t *)"AT+CWMODE=1\r\n", (uint8_t *)"OK");    /* Stationģʽ */
        
        case ESP8266_AP_MODE:
            return esp8266_send_command((uint8_t *)"AT+CWMODE=2\r\n", (uint8_t *)"OK");    /* APģʽ */
        
        case ESP8266_STA_AP_MODE:
            return esp8266_send_command((uint8_t *)"AT+CWMODE=3\r\n", (uint8_t *)"OK");    /* AP+Stationģʽ */
        
        default:
            return ESP8266_EINVAL;
    }
}

/**
 * @brief       ESP8266�����λ
 * @param       ��
 * @retval      ESP8266_EOK  : �����λ�ɹ�
 *              ESP8266_ERROR: �����λʧ��
 */
uint8_t esp8266_sw_reset(void)
{
    return esp8266_send_command((uint8_t *)"AT+RST\r\n", (uint8_t *)"OK");
}

/**
 * @brief       ESP8266����WIFI
 * @param       ssid: WIFI����
 *              pwd : WIFI����
 * @retval      ESP8266_EOK  : WIFI���ӳɹ�
 *              ESP8266_ERROR: WIFI����ʧ��
 */
uint8_t esp8266_join_ap(char *ssid, char *pwd)
{
    uint8_t cmd[64];
    
    sprintf((char *)cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    
    return esp8266_send_command(cmd, (uint8_t *)"WIFI GOT IP");
}
