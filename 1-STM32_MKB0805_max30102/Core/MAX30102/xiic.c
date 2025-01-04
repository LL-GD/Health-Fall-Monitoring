#include "xiic.h"


//初始化IIC
void IIC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pins : PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


}

//输入
void SDA_IN(void)
{
 
   GPIO_InitTypeDef GPIO_InitStruct = {0};
   __HAL_RCC_GPIOB_CLK_ENABLE();
       
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, GPIO_PIN_SET); 
   
}

//输出
void SDA_OUT(void)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};
   __HAL_RCC_GPIOB_CLK_ENABLE();
       
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, GPIO_PIN_SET);   
       

}
//产生IIC起始信号
void IIC_Start(void)
{
    SDA_OUT();     //sda线输出
   
    IIC_SDA(GPIO_PIN_SET);
    IIC_SCL(GPIO_PIN_SET);
    delay_us(4);
    IIC_SDA(GPIO_PIN_RESET);//START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL(GPIO_PIN_RESET);//钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC_Stop(void)
{
    SDA_OUT();//sda线输出
    IIC_SCL(GPIO_PIN_RESET);
    IIC_SDA(GPIO_PIN_RESET);//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL(GPIO_PIN_SET);
    IIC_SDA(GPIO_PIN_SET);//发送I2C总线结束信号
    delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime=0;
    SDA_IN();      //SDA设置为输入
    IIC_SDA(GPIO_PIN_SET);
    delay_us(1);
    IIC_SCL(GPIO_PIN_SET);
    delay_us(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL(GPIO_PIN_RESET);//时钟输出0
    return 0;
}
//产生ACK应答
void IIC_Ack(void)
{
    IIC_SCL(GPIO_PIN_RESET);
    SDA_OUT();
    IIC_SDA(GPIO_PIN_RESET);
    delay_us(2);
    IIC_SCL(GPIO_PIN_SET);
    delay_us(2);
    IIC_SCL(GPIO_PIN_RESET);
}
//不产生ACK应答
void IIC_NAck(void)
{
    IIC_SCL(GPIO_PIN_RESET);
    SDA_OUT();
    IIC_SDA(GPIO_PIN_SET);
    delay_us(2);
    IIC_SCL(GPIO_PIN_SET);
    delay_us(2);
    IIC_SCL(GPIO_PIN_RESET);
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL(GPIO_PIN_RESET);//拉低时钟开始数据传输
    for(t=0; t<8; t++)
    {
        IIC_SDA((txd&0x80)>>7);
        txd<<=1;
        delay_us(2);   //对TEA5767这三个延时都是必须的
        IIC_SCL(GPIO_PIN_SET);
        delay_us(2);
        IIC_SCL(GPIO_PIN_RESET);
        delay_us(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA设置为输入
    for(i=0; i<8; i++ )
    {
        IIC_SCL(GPIO_PIN_RESET);
        delay_us(2);
        IIC_SCL(GPIO_PIN_SET);
        receive<<=1;
        if(READ_SDA)receive++;
        delay_us(1);
    }
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}


void IIC_WriteBytes(uint8_t WriteAddr,uint8_t* data,uint8_t dataLength)
{
    uint8_t i;
    IIC_Start();

    IIC_Send_Byte(WriteAddr);	    //发送写命令
    IIC_Wait_Ack();

    for(i=0; i<dataLength; i++)
    {
        IIC_Send_Byte(data[i]);
        IIC_Wait_Ack();
    }
    IIC_Stop();//产生一个停止条件
    delay_ms(10);
}

void IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr,uint8_t* data,uint8_t dataLength)
{
    uint8_t i;
    IIC_Start();

    IIC_Send_Byte(deviceAddr);	    //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(writeAddr);
    IIC_Wait_Ack();
    IIC_Send_Byte(deviceAddr|0X01);//进入接收模式
    IIC_Wait_Ack();

    for(i=0; i<dataLength-1; i++)
    {
        data[i] = IIC_Read_Byte(1);
    }
    data[dataLength-1] = IIC_Read_Byte(0);
    IIC_Stop();//产生一个停止条件
    delay_ms(10);
}

void IIC_Read_One_Byte(uint8_t daddr,uint8_t addr,uint8_t* data)
{
    IIC_Start();

    IIC_Send_Byte(daddr);	   //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);//发送地址
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(daddr|0X01);//进入接收模式
    IIC_Wait_Ack();
    *data = IIC_Read_Byte(0);
    IIC_Stop();//产生一个停止条件
}

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data)
{
    IIC_Start();

    IIC_Send_Byte(daddr);	    //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);      //发送地址
    IIC_Wait_Ack();
    IIC_Send_Byte(data);      //发送字节
    IIC_Wait_Ack();
    IIC_Stop();               //产生一个停止条件
    delay_ms(10);
}
