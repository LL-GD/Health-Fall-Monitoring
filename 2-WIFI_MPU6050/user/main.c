#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "esp8266_uart.h"
#include "esp8266.h"
#include "esp8266_mqtt.h"
#include "mpu6050.h"
#include "beep.h"
#include <math.h>
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "uart3.h"
#include "timer.h"


u8 mpu_flag = 0;      				//人体跌倒检测标志位 标志位1、2任意一个异常 该标志位为1 	【1：跌倒，0：正常】
_Bool mpu_1_flag = 0;      		//人体跌倒检测标志位1	角度异常标志		【1：异常，0：正常】
_Bool mpu_2_flag = 0;      		//人体跌倒检测标志位2	加速度异常标志	【1：异常，0：正常】
_Bool flag_old = 0;
int SVM;											//人体加速度向量幅值SVM和微分加速度幅值的绝对平均值DSVM是区分人体运动状态的重要参量。SVM通过计算加速度幅度表征人体运动的剧烈程度，其值越大表明运动越剧烈。

uint8_t buf[4];
uint8_t flag = 0,len = 0,rx_flag = 0;

_Bool esp8266_flag = 0;


void esp8266_test(void)
{
	while(esp8266_at_test());
	printf("at success\r\n");
	while(esp8266_set_mode(1));
	printf("set mode success\r\n");
	while(esp8266_sw_reset());
	printf("RST success\r\n");
	//while(esp8266_join_ap("sleep","12345677"));
	printf("wifi lj success\r\n");
	while(esp8266_connect_MQTTUSERCFG(MQTT_USERNAME,MQTT_PWD));
	printf("MQTTUSERCFG success\r\n");
	while(esp8266_connect_MQTTCLIENTID(MQTT_CLIENTLD));
	printf("MQTTCLIENTID success\r\n");
	while(esp8266_connect_MQTTCONN(MQTT_HOSTURL,MQTT_PORT));
	printf("MQTTCONN success\r\n");
	
}



int main(void)
{
	float pitch,roll,yaw; 	//欧拉角
	short aacx,aacy,aacz;		//加速度传感器原始数据
	
    sys_stm32_clock_init(9);    /* 设置时钟, 72Mhz */
    delay_init(72);             /* 延时初始化 */
    usart_init(72, 115200);     /* 串口初始化为115200 */
	OLED_Init();
	esp8266_uart_init(115200);
	uart3_init(115200);
	beep_init();
	printf("OK\r\n");
	
	MPU_Init();					//初始化MPU6050
	delay_ms(1000);			//等待初始化稳定
	printf("OK\r\n");
	while(mpu_dmp_init())//DMP初始化
 	{
 		delay_ms(200);
	}
	printf("OK\r\n");
	
	//esp8266_test();

	OLED_ShowString(0,0,"HR:   /sp02:",OLED_8X16);
	OLED_ShowChinese(0,16,"收缩压");
	OLED_ShowChinese(0,16*2,"舒张压");
	OLED_ShowChinese(0,16*3,"跌倒");
	OLED_Update();
	Timer_init();
	
		while ( 1 )
		{
			if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
			{ 
				MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据		
				SVM = sqrt(pow(aacx,2)+  pow(aacy,2) + pow(aacz,2));	
				//printf("pitch:%0.1f   roll:%0.1f   yaw:%0.1f   SVM:%u\r\n",fabs(pitch),fabs(roll),fabs(yaw),SVM);
				
				//分析x、y、z角度的异常判断
				if( fabs(pitch)>40 || fabs(roll)>40 || fabs(yaw)>40 )//倾斜角度的 【绝对值】 大于40°SVM大于设定的阈值时，即认为摔倒
					mpu_1_flag = 1;	
				else 
					mpu_1_flag = 0;
				
				//分析加速度SVM的异常判断
				if( SVM>23000 || SVM<12000 )
					mpu_2_flag = 1;
				else
					mpu_2_flag = 0;
				
				//综合欧拉角、SVM异常判断异常	
				if( mpu_2_flag || mpu_1_flag )
				{
					mpu_flag = 1;
					//show_mpu(1);
					BEEP_ON;
				}
				else
				{
					mpu_flag = 0;
					BEEP_OFF;
				}
			}
			OLED_ShowNum(16*3,16*3,mpu_flag,1,OLED_8X16);

			if(flag_old != mpu_flag)
			{
				if(flag_old == 0)
				{
					// 发送1
					while(atk_mw8266d_connect_MQTTPUB_oneData("diedao","1"));
				}
				else
				{
					// 发送0
					while(atk_mw8266d_connect_MQTTPUB_oneData("diedao","0"));
				}
			}
			flag_old = mpu_flag;
			
			// OLED
			if(rx_flag == 1)
			{
				rx_flag = 0;
				len = 0;
				OLED_ShowNum(8*3,0,buf[0],3,OLED_8X16);
				OLED_ShowNum(8*4,16,buf[1],3,OLED_8X16);
				OLED_ShowNum(8*4,16*2,buf[2],3,OLED_8X16);
				OLED_ShowNum(8*12,0,buf[3],2,OLED_8X16);
				
			}
			
			if(esp8266_flag == 1)
			{
				esp8266_flag = 0;
				while(atk_mw8266d_connect_MQTTPUB(buf[0],buf[3],buf[1],buf[2]));
			}
			OLED_Update();
		}
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{
		uint8_t res;
		res = USART_ReceiveData(USART3);
		if(res == 0xfe)
		{
			rx_flag = 1;
			flag = 0;
		}
		if(len == 4)
			len = 0;
		if(flag == 1)
		{
			buf[len++] = res;
		}
		if(res == 0xfd)
			flag = 1;
	}
}


void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{
		esp8266_flag = 1;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		
	}
}




