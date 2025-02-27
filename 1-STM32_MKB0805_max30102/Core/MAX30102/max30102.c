#include "max30102.h"
#include "Mis.h"
#include "oled.h"

/**
  ******************************************************************************

  * 管脚    :VIN: 5V
  *          SCL: B6
  *          SDA: B7
  *          INT: B5
  *          IRO: NC
  *          R0 : NC
  *          GND: NC

  * 修改管脚:见lt_myiic.h文件和lt_max30102.h
  * 通信方式:模拟IIC
  * 使用实例:
             int32_t n_heart_rate = 0;
             int32_t n_sp02 = 0;

             max30102_init();

			 while(1)
             {
                 max30102_Read_Data(&n_heart_rate, &n_sp02);            //
                 printf("heart:%d, sp02:%d\r\n", n_heart_rate, n_sp02);
             }

  *
  ******************************************************************************
  */


#define MAX_BRIGHTNESS 255

uint32_t aun_ir_buffer[500];     //IR LED sensor data     //存储红外LED传感器数据
int32_t n_ir_buffer_length;      //data length            //IR存储数据长度
uint32_t aun_red_buffer[500];    //Red LED sensor data    //存储红色LED传感器数据
int32_t n_sp02;                  //SPO2 value             //血氧数值
int8_t ch_spo2_valid;            //indicator to show if the SP02 calculation is valid       //指示符显示SP02计算是否有效
int32_t n_heart_rate;            //heart rate value
int8_t  ch_hr_valid;             //indicator to show if the heart rate calculation is valid //指示符显示heart计算是否有效
uint8_t uch_dummy;
int heart = 0;


//向max30102 地址写入数据（寄存器）
uint8_t max30102_Bus_Write(uint8_t Register_Address, uint8_t Word_Data)
{

    /* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */

    /* 第1步：发起I2C总线启动信号 */
    IIC_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* 此处是写指令 */

    /* 第3步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址 */
    IIC_Send_Byte(Register_Address);
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第5步：开始写入数据 */
    IIC_Send_Byte(Word_Data);

    /* 第6步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 发送I2C总线停止信号 */
    IIC_Stop();
    return 1;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    IIC_Stop();
    return 0;
}




//读取指定地址（寄存器）
uint8_t max30102_Bus_Read(uint8_t Register_Address)
{
    uint8_t  data;


    /* 第1步：发起I2C总线启动信号 */
    IIC_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* 此处是写指令 */

    /* 第3步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址， */
    IIC_Send_Byte((uint8_t)Register_Address);
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }


    /* 第6步：重新启动I2C总线。下面开始读取数据 */
    IIC_Start();

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(max30102_WR_address | I2C_RD);	/* 此处是读指令 */

    /* 第8步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第9步：读取数据 */
    {
        data = IIC_Read_Byte(0);	/* 读1个字节 */

        IIC_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
    }
    /* 发送I2C总线停止信号 */
    IIC_Stop();
    return data;	/* 执行成功 返回data值 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    IIC_Stop();
    return 0;
}


void max30102_FIFO_ReadWords(uint8_t Register_Address,uint16_t Word_Data[][2],uint8_t count)
{
    uint8_t i=0;
    uint8_t no = count;
    uint8_t data1, data2;
    /* 第1步：发起I2C总线启动信号 */
    IIC_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* 此处是写指令 */

    /* 第3步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址， */
    IIC_Send_Byte((uint8_t)Register_Address);
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }


    /* 第6步：重新启动I2C总线。下面开始读取数据 */
    IIC_Start();

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(max30102_WR_address | I2C_RD);	/* 此处是读指令 */

    /* 第8步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第9步：读取数据 */
    while (no)
    {
        data1 = IIC_Read_Byte(0);
        IIC_Ack();
        data2 = IIC_Read_Byte(0);
        IIC_Ack();
        Word_Data[i][0] = (((uint16_t)data1 << 8) | data2);  //


        data1 = IIC_Read_Byte(0);
        IIC_Ack();
        data2 = IIC_Read_Byte(0);
        if(1==no)
            IIC_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        else
            IIC_Ack();
        Word_Data[i][1] = (((uint16_t)data1 << 8) | data2);

        no--;
        i++;
    }
    /* 发送I2C总线停止信号 */
    IIC_Stop();

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    IIC_Stop();
}

/**
*   作用  ：读取FIFO内的数据
*   名称  ：max30102_FIFO_ReadBytes();
*   参数  ：Register_Address:要读取的寄存器地址
            Data：存储读取到的数据
*   返回值：无
**/

void max30102_FIFO_ReadBytes(uint8_t Register_Address,uint8_t* Data)
{
    max30102_Bus_Read(REG_INTR_STATUS_1);
    max30102_Bus_Read(REG_INTR_STATUS_2);

    /* 第1步：发起I2C总线启动信号 */
    IIC_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* 此处是写指令 */

    /* 第3步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第4步：发送字节地址， */
    IIC_Send_Byte((uint8_t)Register_Address);
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }


    /* 第6步：重新启动I2C总线。下面开始读取数据 */
    IIC_Start();

    /* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    IIC_Send_Byte(max30102_WR_address | I2C_RD);	/* 此处是读指令 */

    /* 第8步：发送ACK */
    if (IIC_Wait_Ack() != 0)
    {
        goto cmd_fail;	/* EEPROM器件无应答 */
    }

    /* 第9步：读取数据 */
    Data[0] = IIC_Read_Byte(1);
    Data[1] = IIC_Read_Byte(1);
    Data[2] = IIC_Read_Byte(1);
    Data[3] = IIC_Read_Byte(1);
    Data[4] = IIC_Read_Byte(1);
    Data[5] = IIC_Read_Byte(0);
    /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
    /* 发送I2C总线停止信号 */
    IIC_Stop();

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
    /* 发送I2C总线停止信号 */
    IIC_Stop();
}


void max_int_init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitStruct.Pin =  GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL ;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
*   作用  ：max30102初始化配置
*   名称  ：max30102_init();
*   参数  ：无
*   返回值：无
**/
void max30102_init(void)
{
    uint32_t   un_min, un_max;   //缓存新信号最小值，最大值
    uint32_t   un_prev_data;     //红色LED器数据缓冲区
    int i;
    uint8_t temp[6];
    IIC_Init();
   max_int_init();
   //max30102模式配置
    max30102_reset();
   
   //寄存器配置
    max30102_Bus_Write(REG_INTR_ENABLE_1,0xc0);	// INTR setting
    max30102_Bus_Write(REG_INTR_ENABLE_2,0x00);
    max30102_Bus_Write(REG_FIFO_WR_PTR,0x00);  	// FIFO_WR_PTR[4:0]
    max30102_Bus_Write(REG_OVF_COUNTER,0x00);  	// OVF_COUNTER[4:0]
    max30102_Bus_Write(REG_FIFO_RD_PTR,0x00);  	// FIFO_RD_PTR[4:0]
    max30102_Bus_Write(REG_FIFO_CONFIG,0x0f);  	// sample avg = 1, fifo rollover=false, fifo almost full = 17
    max30102_Bus_Write(REG_MODE_CONFIG,0x03);  	// 0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
    max30102_Bus_Write(REG_SPO2_CONFIG,0x27);  	// SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)
    max30102_Bus_Write(REG_LED1_PA,0x24);   	   // Choose value for ~ 7mA for LED1
    max30102_Bus_Write(REG_LED2_PA,0x24);   	   // Choose value for ~ 7mA for LED2
    max30102_Bus_Write(REG_PILOT_PA,0x7f);   	// Choose value for ~ 25mA for Pilot LED
   
    un_min=0x3FFFF;
    un_max=0;
   
    n_ir_buffer_length=500;                              //buffer length of 100 stores 5 seconds of samples running at 100sps

    for(i=0; i<n_ir_buffer_length; i++)
    {
       
       //等待，直到中断引脚断言
        while(MAX30102_INT==1)
        {
          //Max30102_Wait_Interface();  //等待检测界面
        }                         
        
        //读取FIFO内的数据
        max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);   
        
        //对从FIFO中读到的数据进行整合
        aun_red_buffer[i] =   (long)((long)((long)temp[0]&0x03)<<16)   | (long)temp[1]<<8  | (long)temp[2];      // Combine values to get the actual number
        aun_ir_buffer [i] =   (long)((long)((long)temp[3] & 0x03)<<16) | (long)temp[4]<<8  | (long)temp[5];      // Combine values to get the actual number

        if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];    //更新信号最小值
        
        if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];    //更新信号最大值
        
    }
      //红色LED器数据缓
      un_prev_data=aun_red_buffer[i];
		un_prev_data=un_prev_data;
    
    
      // 计算心率和SpO2水平
      maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer,
                                                      &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
}

/**
*   作用  ：max30102模式配置
*   名称  ：max30102_reset();
*   参数  ：无
*   返回值：无
**/
void max30102_reset(void)
{
    max30102_Bus_Write(REG_MODE_CONFIG,0x40);
    max30102_Bus_Write(REG_MODE_CONFIG,0x40);
}


/**
*   作用  ：向max30102写
*   名称  ：max30102_reset();
*   参数  ：无
*   返回值：无
**/
void maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data)
{

    IIC_Write_One_Byte(I2C_WRITE_ADDR,uch_addr,uch_data);
}




void maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data)
{

    IIC_Read_One_Byte(I2C_WRITE_ADDR,uch_addr,puch_data);
}



/**
*   作用  ：读max30102FIFO
*   名称  ：max30102_reset();
*   参数  ：无
*   返回值：无
**/
void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)
{
    uint32_t un_temp;
    unsigned char uch_temp;
    char ach_i2c_data[6];
    *pun_red_led=0;
    *pun_ir_led=0;


    //read and clear status register   读取和清除状态寄存器
    maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_temp);
    maxim_max30102_read_reg(REG_INTR_STATUS_2, &uch_temp);

    IIC_ReadBytes(I2C_WRITE_ADDR,REG_FIFO_DATA,(uint8_t *)ach_i2c_data,6);

     //红色led
    un_temp=(unsigned char) ach_i2c_data[0];
    un_temp<<=16;
    *pun_red_led+=un_temp;
    un_temp=(unsigned char) ach_i2c_data[1];
    un_temp<<=8;
    *pun_red_led+=un_temp;
    un_temp=(unsigned char) ach_i2c_data[2];
    *pun_red_led+=un_temp;

    //红外LED
    un_temp=(unsigned char) ach_i2c_data[3];
    un_temp<<=16;
    *pun_ir_led+=un_temp;
    un_temp=(unsigned char) ach_i2c_data[4];
    un_temp<<=8;
    *pun_ir_led+=un_temp;
    un_temp=(unsigned char) ach_i2c_data[5];
    *pun_ir_led+=un_temp;
    
    //18~23位不是想要数据
    *pun_red_led&=0x03FFFF;      //Mask MSB [23:18]
    *pun_ir_led&= 0x03FFFF;      //Mask MSB [23:18]
}

/**
*   作用  ：max30102读取心率血氧
*   名称  ：max30102_Read_Data();
*   参数  ：heart_rate：存储心率
            sp02：存血氧
*   返回值：无
**/

uint8_t  max30102_FLAG=0;
uint8_t  max30102_Tem=1;  //用来清oled屏

uint8_t heart_Data[3]={0};

uint8_t heart_Picture[100]={0},heart_count=0;

void max30102_Read_Data(int32_t *heart_rate, int32_t *sp02)
{
    //variables to calculate the on-board LED brightness that reflects the heartbeats
    //变量来计算反映心跳的板载LED亮度
    uint32_t un_min, un_max, un_prev_data;
    int i;
    uint8_t temp[6];
    float f_temp;
    int32_t n_brightness;
    
    while(1)
    {
        i=0;
        un_min=0x3FFFF;
        un_max=0;
        for(i=100; i<500; i++)
        {
            aun_red_buffer[i-100]  =  aun_red_buffer[i];
            aun_ir_buffer [i-100]  =  aun_ir_buffer[i];

            //update the signal min and max   更新信号最小值和最大值
            if(un_min>aun_red_buffer[i])
                un_min=aun_red_buffer[i];
            if(un_max<aun_red_buffer[i])
                un_max=aun_red_buffer[i];
        }
        
        //take 100 sets of samples before calculating the heart rate.
        //在计算心率前，取100组样本
        for(i=400; i<500; i++)
        {
             
           un_prev_data=aun_red_buffer[i-1];
           
//           printf("un_prev_data=%d\r\n", un_prev_data);  //串口测试点
           
           //等待，直到中断引脚断言
           while(MAX30102_INT==1)
            {
                 if(max30102_FLAG==0)
                 {
                     //Max30102_Wait_Interface();  //等待检测界面
                 }
            }  
            //读取FIFO内的数据
            max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
           
              // 将红色led值组合以获得实际数字
            aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];   
             // 将红外led值组合以获得实际数字               
            aun_ir_buffer[i] =   (long)((long)((long)temp[3] & 0x03)<<16) | (long)temp[4]<<8 | (long)temp[5];      

            //对取到的样本进行处理
            if(aun_red_buffer[i]>un_prev_data)
            {
                f_temp=aun_red_buffer[i]-un_prev_data;
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness-=(int)f_temp;
                if(n_brightness<0)
                    n_brightness=0;
            }
            else
            {
                f_temp=un_prev_data-aun_red_buffer[i];
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness+=(int)f_temp;
                if(n_brightness>MAX_BRIGHTNESS)
                    n_brightness=MAX_BRIGHTNESS;
            }
            
//            printf("n_brightness=%d\r\n", n_brightness);  //串口测试点
            
            //心率图 
//            if(max30102_FLAG==1)
//            {
//              if((n_brightness>=30)&&(n_brightness<=250))
//                 {
//                      n_brightness-=30;
//                      n_brightness/= 5;
//                      heart_count++;
//                      if(heart_count>100)
//                      {
//                        heart_count=0;
//                      }
//                      heart_Picture[heart_count]=n_brightness;
//                      //oled显示心率图
//                      OLED_heart_Picture(heart_Picture[heart_count-1],heart_Picture[heart_count]);
//                 }  
//           }
//            //send samples and calculation result to terminal program through UART
            //通过UART将采样和计算结果发送到终端程序
            if(ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<150 && n_sp02<101)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
            {
                heart = n_heart_rate;

            } 
            else
            {
                n_heart_rate = 0;
                heart = 0;
            }
            if(ch_hr_valid == 0)
            {
                n_heart_rate = 0;
                heart = 0;
            }

        }

        //计算心率和SpO2水平
        maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
        
        //对采集到的数据进行筛选
        if((ch_hr_valid == 1) && (ch_spo2_valid ==1) && (n_heart_rate>50) && (n_heart_rate<150) && (n_sp02<101) && (n_sp02>80))//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
        {
               *heart_rate = n_heart_rate;
               *sp02 = n_sp02;
              
               // 心率和血氧浓度初始数据打印
               //printf("HR=%d, spo2:%d%%\r\n", n_heart_rate, n_sp02);
     
              //清除oled旧界面
              if(max30102_Tem==1)
              {
//                  OLED_Clear();
//								OLED_Update();
                  max30102_Tem=0;
              }
              
               max30102_FLAG=1;

               break;
        }
        else
        {
               //清除oled旧界面
              if(max30102_Tem==0)
              {
//                  OLED_Clear();
//								OLED_Update();
              }
               max30102_Tem=1;
               max30102_FLAG=0;
        }
    }
}
