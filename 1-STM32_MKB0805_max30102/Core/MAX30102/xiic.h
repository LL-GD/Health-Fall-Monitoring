#ifndef __XIIC_H__
#define __XIIC_H__
#include "stm32f1xx_hal.h"
#include "delay.h"

#define MAX30102_SCL_GPIO_PORT    	 GPIOB		                /* MAX30102 SCL端口 */
#define MAX30102_SCL_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* MAX30102 SCL端口时钟 */
#define MAX30102_SCL_GPIO_PIN		    GPIO_Pin_7			        /* MAX30102 SCL管脚 */

#define MAX30102_SDA_GPIO_PORT    	 GPIOB		                /* MAX30102 SDA端口 */
#define MAX30102_SDA_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* MAX30102 SDA端口时钟 */
#define MAX30102_SDA_GPIO_PIN		    GPIO_Pin_8			        /* MAX30102 SDA管脚 */

#define IIC_SCL(a)                         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,a)                  //SCL
#define IIC_SDA(a)                         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,a)              //SDA	 
#define READ_SDA                           HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)                  //输入SDA 




void IIC_Init(void);                     //初始化IIC的IO口
void IIC_Start(void);				      //发送IIC开始信号
void IIC_Stop(void);	  			         //发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					        //IIC发送ACK信号
void IIC_NAck(void);				             //IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
void IIC_Read_One_Byte(uint8_t daddr,uint8_t addr,uint8_t* data);

void IIC_WriteBytes(uint8_t WriteAddr,uint8_t* data,uint8_t dataLength);
void IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr,uint8_t* data,uint8_t dataLength);
void SDA_IN(void);
void SDA_OUT(void);

#endif
















