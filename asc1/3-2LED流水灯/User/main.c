#include "stm32f10x.h"
#include "Delay.h"// Device header

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	while (1)
	{
		GPIO_Write(GPIOB,~0x1000);
		Delay_ms(500);
		GPIO_Write(GPIOB,~0x2000);
		Delay_ms(500);
		GPIO_Write(GPIOB,~0x4000);
		Delay_ms(500);
		GPIO_Write(GPIOB,(uint16_t)~0x8000);
		Delay_ms(500);
	}
}
