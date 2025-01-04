#ifndef __XIIC_H__
#define __XIIC_H__
#include "stm32f1xx_hal.h"
#include "delay.h"

#define MAX30102_SCL_GPIO_PORT    	 GPIOB		                /* MAX30102 SCL�˿� */
#define MAX30102_SCL_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* MAX30102 SCL�˿�ʱ�� */
#define MAX30102_SCL_GPIO_PIN		    GPIO_Pin_7			        /* MAX30102 SCL�ܽ� */

#define MAX30102_SDA_GPIO_PORT    	 GPIOB		                /* MAX30102 SDA�˿� */
#define MAX30102_SDA_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* MAX30102 SDA�˿�ʱ�� */
#define MAX30102_SDA_GPIO_PIN		    GPIO_Pin_8			        /* MAX30102 SDA�ܽ� */

#define IIC_SCL(a)                         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,a)                  //SCL
#define IIC_SDA(a)                         HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,a)              //SDA	 
#define READ_SDA                           HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)                  //����SDA 




void IIC_Init(void);                     //��ʼ��IIC��IO��
void IIC_Start(void);				      //����IIC��ʼ�ź�
void IIC_Stop(void);	  			         //����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					        //IIC����ACK�ź�
void IIC_NAck(void);				             //IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
void IIC_Read_One_Byte(uint8_t daddr,uint8_t addr,uint8_t* data);

void IIC_WriteBytes(uint8_t WriteAddr,uint8_t* data,uint8_t dataLength);
void IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr,uint8_t* data,uint8_t dataLength);
void SDA_IN(void);
void SDA_OUT(void);

#endif
















