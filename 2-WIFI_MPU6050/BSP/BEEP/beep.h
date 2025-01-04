#ifndef __BEEP_H
#define __BEEP_H

#include <stm32f10x.h>

#define BEEP_PORT             GPIOB
#define BEEP_PIN              GPIO_Pin_12
#define BEEP_CLK_ENABLE()     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

#define BEEP_ON               GPIO_WriteBit(BEEP_PORT,BEEP_PIN,Bit_RESET)
#define BEEP_OFF              GPIO_WriteBit(BEEP_PORT,BEEP_PIN,Bit_SET)

void beep_init(void);

#endif
