#include "stm32f10x.h"                  // Device header

const uint16_t FloodLED_Total = 4;
const uint16_t FloodLED[] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};
const uint16_t LED_Speed_Total = 3;
const uint16_t LED_Speed_Option[] = {500, 1000, 200};//定义三个速度
int Period = 500;//速度初始化
uint8_t LEDStatus;

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}


//向右流水灯
void FloodLED_RightTurn(void)
{
	for (int i = 0; i < FloodLED_Total; i++)
	{
		if (GPIO_ReadInputDataBit(GPIOB, FloodLED[i]) == Bit_RESET)
		{
			GPIO_WriteBit(GPIOB, FloodLED[i], Bit_SET);
			GPIO_WriteBit(GPIOB, FloodLED[(i + 1) % FloodLED_Total], Bit_RESET);
			return ;
		}
	}
	GPIO_WriteBit(GPIOB, FloodLED[0], Bit_RESET);
}
 
//向左流水灯
void FloodLED_LeftTurn(void)
{
	for (int i = FloodLED_Total - 1; i >= 0; i--)
	{
		if (GPIO_ReadInputDataBit(GPIOB, FloodLED[i]) == Bit_RESET)
		{
			GPIO_WriteBit(GPIOB, FloodLED[i], Bit_SET);
			GPIO_WriteBit(GPIOB, FloodLED[(i - 1 + FloodLED_Total) % FloodLED_Total], Bit_RESET);
			return ;
		}
	}
	GPIO_WriteBit(GPIOB, FloodLED[FloodLED_Total - 1], Bit_RESET);
}

//控制灯的亮灭
void LED_Tick(void)
{
	static int Tim = 0;
	Tim++;
	if (Tim >= Period)
	{
		if (LEDStatus == 0)
		{
			FloodLED_RightTurn();
		}
		else
		{
			FloodLED_LeftTurn();
		}
		Tim = 0;
	}
}

//方向控制
void LED_DirSet(int Dir)
{
	LEDStatus = Dir;
}

//速度控制
void LED_SpeedSet(int SpeedOption)
{
	Period = LED_Speed_Option[SpeedOption];
}