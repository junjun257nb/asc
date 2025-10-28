#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "KEY.h"
#include "MENU.h"
#include "Encoder.h"
#include "Timer.h"
#include "LED.h"

int main(void)
{
    // 系统初始化
    OLED_Init();
    Key_Init();
    init_menus();
	Encoder_Init();
	Timer_Init();
	LED_Init();
    
    // 初始显示
    display_menu();
    
    while(1)
    {
		Menu_Tick();//菜单运行
        uint8_t key = Key_GetNum();//读取按键返回值
        if(key != 0) {
            menu_process(key);// 菜单处理主函数
        }
		LED_DirSet(Menu_GetDir());//流水灯方向
		LED_SpeedSet(Menu_GetSpeed());//流水灯速度
    }
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		LED_Tick();//流水灯运行
		Key_Tick();//按钮运行
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

