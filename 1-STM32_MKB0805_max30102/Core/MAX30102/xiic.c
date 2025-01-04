#include "xiic.h"


//��ʼ��IIC
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

//����
void SDA_IN(void)
{
 
   GPIO_InitTypeDef GPIO_InitStruct = {0};
   __HAL_RCC_GPIOB_CLK_ENABLE();
       
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8, GPIO_PIN_SET); 
   
}

//���
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
//����IIC��ʼ�ź�
void IIC_Start(void)
{
    SDA_OUT();     //sda�����
   
    IIC_SDA(GPIO_PIN_SET);
    IIC_SCL(GPIO_PIN_SET);
    delay_us(4);
    IIC_SDA(GPIO_PIN_RESET);//START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL(GPIO_PIN_RESET);//ǯסI2C���ߣ�׼�����ͻ��������
}
//����IICֹͣ�ź�
void IIC_Stop(void)
{
    SDA_OUT();//sda�����
    IIC_SCL(GPIO_PIN_RESET);
    IIC_SDA(GPIO_PIN_RESET);//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL(GPIO_PIN_SET);
    IIC_SDA(GPIO_PIN_SET);//����I2C���߽����ź�
    delay_us(4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime=0;
    SDA_IN();      //SDA����Ϊ����
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
    IIC_SCL(GPIO_PIN_RESET);//ʱ�����0
    return 0;
}
//����ACKӦ��
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
//������ACKӦ��
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL(GPIO_PIN_RESET);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0; t<8; t++)
    {
        IIC_SDA((txd&0x80)>>7);
        txd<<=1;
        delay_us(2);   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL(GPIO_PIN_SET);
        delay_us(2);
        IIC_SCL(GPIO_PIN_RESET);
        delay_us(2);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
uint8_t IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK
    return receive;
}


void IIC_WriteBytes(uint8_t WriteAddr,uint8_t* data,uint8_t dataLength)
{
    uint8_t i;
    IIC_Start();

    IIC_Send_Byte(WriteAddr);	    //����д����
    IIC_Wait_Ack();

    for(i=0; i<dataLength; i++)
    {
        IIC_Send_Byte(data[i]);
        IIC_Wait_Ack();
    }
    IIC_Stop();//����һ��ֹͣ����
    delay_ms(10);
}

void IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr,uint8_t* data,uint8_t dataLength)
{
    uint8_t i;
    IIC_Start();

    IIC_Send_Byte(deviceAddr);	    //����д����
    IIC_Wait_Ack();
    IIC_Send_Byte(writeAddr);
    IIC_Wait_Ack();
    IIC_Send_Byte(deviceAddr|0X01);//�������ģʽ
    IIC_Wait_Ack();

    for(i=0; i<dataLength-1; i++)
    {
        data[i] = IIC_Read_Byte(1);
    }
    data[dataLength-1] = IIC_Read_Byte(0);
    IIC_Stop();//����һ��ֹͣ����
    delay_ms(10);
}

void IIC_Read_One_Byte(uint8_t daddr,uint8_t addr,uint8_t* data)
{
    IIC_Start();

    IIC_Send_Byte(daddr);	   //����д����
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);//���͵�ַ
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(daddr|0X01);//�������ģʽ
    IIC_Wait_Ack();
    *data = IIC_Read_Byte(0);
    IIC_Stop();//����һ��ֹͣ����
}

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data)
{
    IIC_Start();

    IIC_Send_Byte(daddr);	    //����д����
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);      //���͵�ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(data);      //�����ֽ�
    IIC_Wait_Ack();
    IIC_Stop();               //����һ��ֹͣ����
    delay_ms(10);
}
