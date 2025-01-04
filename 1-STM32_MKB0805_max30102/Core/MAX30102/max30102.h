#ifndef __MAX30102_H__
#define __MAX30102_H__
#include "stm32f1xx_hal.h"
#include "algorithm.h"
#include "xiic.h"
#include "delay.h"
#include "stdio.h"


////////////////////////////////////////////////////////////////////////////////// 	  

#define MAX30102_INT   HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)      //MAX30102中断标志位


#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */

#define max30102_WR_address 0xAE

#define I2C_WRITE_ADDR 0xAE     //写地址
#define I2C_READ_ADDR 0xAF      //读地址

//register addresses
#define REG_INTR_STATUS_1 0x00   //中断状态 0X00~0X01
#define REG_INTR_STATUS_2 0x01
#define REG_INTR_ENABLE_1 0x02   //中断使   0X02~0X03
#define REG_INTR_ENABLE_2 0x03
#define REG_FIFO_WR_PTR 0x04     //FIFO     0X04~0X07
#define REG_OVF_COUNTER 0x05
#define REG_FIFO_RD_PTR 0x06
#define REG_FIFO_DATA 0x07        
#define REG_FIFO_CONFIG 0x08      //FIFO配置
#define REG_MODE_CONFIG 0x09      //模式配置
#define REG_SPO2_CONFIG 0x0A      //SpO2配置
#define REG_LED1_PA 0x0C          //LED脉冲幅度              0X0C~0X10
#define REG_LED2_PA 0x0D
#define REG_PILOT_PA 0x10
#define REG_MULTI_LED_CTRL1 0x11     //多led模式控制寄存器     0X11~0X12
#define REG_MULTI_LED_CTRL2 0x12
#define REG_TEMP_INTR 0x1F           // 温度数据              0X1F~0X21
#define REG_TEMP_FRAC 0x20
#define REG_TEMP_CONFIG 0x21
#define REG_PROX_INT_THRESH 0x30     //接近模式中断阈值
#define REG_REV_ID 0xFE
#define REG_PART_ID 0xFF

void max30102_init(void);  
void max30102_reset(void);
uint8_t max30102_Bus_Write(uint8_t Register_Address, uint8_t Word_Data);
uint8_t max30102_Bus_Read(uint8_t Register_Address);
void max30102_FIFO_ReadWords(uint8_t Register_Address,uint16_t  Word_Data[][2],uint8_t count);
void max30102_FIFO_ReadBytes(uint8_t Register_Address,uint8_t* Data);

void maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data);
void maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data);
void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led);
void max30102_Read_Data(int32_t* n_heart_rate, int32_t* n_sp02);
#endif
