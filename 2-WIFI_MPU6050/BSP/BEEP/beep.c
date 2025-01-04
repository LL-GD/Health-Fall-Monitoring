#include "beep.h"

void beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	BEEP_CLK_ENABLE();
	
	GPIO_InitStruct.GPIO_Pin = BEEP_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BEEP_PORT,&GPIO_InitStruct);
	
	BEEP_OFF;
}
