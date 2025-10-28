#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "OLED.h"
#include "Key.h"
#include "Encoder.h"

// 菜单类型定义
typedef enum {
    MAIN_MENU,      //0
    LED_CONTROL,    //1
    PID,            //2
    IMAGE,          //3
    ANGLE           //4
} MenuType;

// 菜单项类型定义
typedef enum {
    SUBMENU_ITEM,    // 子菜单项
    PARAMETER_ITEM,  // 参数项
    FUNCTION_ITEM    // 功能项
} ItemType;

// 菜单项结构
typedef struct {
    char name[30];//菜单显示的名字
    ItemType type;
    MenuType target_menu;  // 对于子菜单项，指向目标菜单
    int *param_ptr;        // 对于参数项，指向参数变量
} MenuItem;

// 菜单结构
typedef struct {
    char name[20];//储存菜单名字
    int item_count;//储存当前菜单个数
    MenuItem items[10];//跳转到子菜单项
} Menu;

// 全局变量
MenuType current_menu = MAIN_MENU;//初始位置
int current_selection = 0;//标记箭头位置
int edit_mode = 0;  // 0=导航模式, 1=编辑模式

// 参数变量
int led_speed;
int led_dir;
int kp;
int ki;
int kd;

// 菜单数据初始化
Menu menus[5];

// 初始化菜单数据
void init_menus() {
    // 主菜单
    menus[MAIN_MENU].item_count = 4;//该位置有四项
    sprintf(menus[MAIN_MENU].name, "Main Menu");
    sprintf(menus[MAIN_MENU].items[0].name, "LED Control");
    menus[MAIN_MENU].items[0].type = SUBMENU_ITEM;//定义菜单类型// 子菜单项
    menus[MAIN_MENU].items[0].target_menu = LED_CONTROL;//指向子菜单的定位
    sprintf(menus[MAIN_MENU].items[1].name, "PID");
    menus[MAIN_MENU].items[1].type = SUBMENU_ITEM;// 子菜单项
    menus[MAIN_MENU].items[1].target_menu = PID;
    sprintf(menus[MAIN_MENU].items[2].name, "Image");
    menus[MAIN_MENU].items[2].type = SUBMENU_ITEM;// 子菜单项
    menus[MAIN_MENU].items[2].target_menu = IMAGE;
    sprintf(menus[MAIN_MENU].items[3].name, "Angle");
    menus[MAIN_MENU].items[3].type = SUBMENU_ITEM;// 子菜单项
    menus[MAIN_MENU].items[3].target_menu = ANGLE;
    
    // LED Control菜单
    menus[LED_CONTROL].item_count = 2;//该位置有二项
    sprintf(menus[LED_CONTROL].name, "LED Control");
    sprintf(menus[LED_CONTROL].items[0].name, "LED_Speed");
    menus[LED_CONTROL].items[0].type = PARAMETER_ITEM;// 参数项
    menus[LED_CONTROL].items[0].param_ptr = &led_speed;
    sprintf(menus[LED_CONTROL].items[1].name, "LED_Dir");
    menus[LED_CONTROL].items[1].type = PARAMETER_ITEM;// 参数项
    menus[LED_CONTROL].items[1].param_ptr = &led_dir;
    
    // PID菜单
    menus[PID].item_count = 3;
    sprintf(menus[PID].name, "PID");
    sprintf(menus[PID].items[0].name, "kp");
    menus[PID].items[0].type = PARAMETER_ITEM;// 参数项
    menus[PID].items[0].param_ptr = &kp;
    sprintf(menus[PID].items[1].name, "ki");
    menus[PID].items[1].type = PARAMETER_ITEM;// 参数项
    menus[PID].items[1].param_ptr = &ki;
    sprintf(menus[PID].items[2].name, "kd");
    menus[PID].items[2].type = PARAMETER_ITEM;// 参数项
    menus[PID].items[2].param_ptr = &kd;
    
    // Image菜单
    menus[IMAGE].item_count = 1;
    sprintf(menus[IMAGE].name, "Image");
    sprintf(menus[IMAGE].items[0].name, "Image");
    menus[IMAGE].items[0].type = FUNCTION_ITEM;// 功能项
    
    //Angle菜单
    menus[ANGLE].item_count = 1;
    sprintf(menus[ANGLE].name,"Angle");
    sprintf(menus[ANGLE].items[0].name,"Angle");
    menus[ANGLE].items[0].type = FUNCTION_ITEM;// 功能项
}

// 显示菜单

int Numlen(int num) {
	if (num < 0) num = -num;
	if (num >=0 && num <10) return 1;
	int res = -1;
	while(num > 0) {
		num /= 10;
		res++;
	}
	return res;
}
void display_menu() {
    static MenuType last_menu = MAIN_MENU;
    // 只在菜单切换时清屏
    if (last_menu != current_menu) {
        OLED_Clear();
        last_menu = current_menu;
    }
    
    int x=0;//用于确定是否为主菜单
    
    if(current_menu){
        OLED_ShowString(1,1, menus[current_menu].name);
        x=1;
    }
    
    // 显示模式提示（如果处于编辑模式）
    if(edit_mode) {
        OLED_ShowString(1,15, "E");
    } else {
        OLED_ShowChar(1,15,' ');  // 清除编辑模式提示
    }
    
    for (int i = 0; i < menus[current_menu].item_count; i++) {
        if (i == current_selection) {
            OLED_ShowChar(i+x+1,1,'>');
        } else {
            OLED_ShowChar(i+x+1,1,' ');
        }
        
        MenuItem *item = &menus[current_menu].items[i];
        if (item->type == PARAMETER_ITEM) {
            OLED_ShowString(i+x+1,2,item->name);
            // 显示参数值
            char temp[16];
            if(current_menu == LED_CONTROL) {
                // LED参数显示为整数
                sprintf(temp, "%d", (*(item->param_ptr))/10);  // 强制转换为整数
            } else if(current_menu == PID) {
                // PID参数显示为1位小数
                sprintf(temp, "%.1f", (float)(*(item->param_ptr))/10);  // 显示1位小数
            }
			OLED_ShowString(i+x+1,16-1-(*(item->param_ptr)<0)-Numlen(*(item->param_ptr)), temp);
        } else {
            OLED_ShowString(i+x+1,2,item->name);
        }
    }
}

// 处理向上移动
void move_up() {
    if(!edit_mode) {
        current_selection--;
        if (current_selection < 0) {
            current_selection = menus[current_menu].item_count - 1;
        }
    } else {
        // 编辑模式下，上键增加参数值
        MenuItem *item = &menus[current_menu].items[current_selection];
        if(item->type == PARAMETER_ITEM) {
            if(current_menu == LED_CONTROL) {
                // LED参数每次加1（整数）
                (*(item->param_ptr)) += 10;
				if(current_selection == 0) { // LED_Speed
                    if(*(item->param_ptr) > 20) {
                        *(item->param_ptr) = 0; // 速度不能为负
                    }
                } else if(current_selection == 1) { // LED_Dir
                    if(*(item->param_ptr) > 10) {
                        *(item->param_ptr) = 0; // 方向不能为负
                    }
				}
            } else if(current_menu == PID) {
                // PID参数每次加0.1（小数）
                (*(item->param_ptr)) += 1;
            }
			OLED_Clear();
        }
    }
}

// 处理向下移动
void move_down() {
    if(!edit_mode) {
        current_selection++;
        if (current_selection >= menus[current_menu].item_count) {
            current_selection = 0;
        }
    } else {
        // 编辑模式下，下键减少参数值
        MenuItem *item = &menus[current_menu].items[current_selection];
        if(item->type == PARAMETER_ITEM) {
            if(current_menu == LED_CONTROL) {
                // LED参数每次减1（整数）
                (*(item->param_ptr)) -= 10;
                
                // LED参数最小限制
                if(current_selection == 0) { // LED_Speed
                    if(*(item->param_ptr) < 0) {
                        *(item->param_ptr) = 20; // 速度不能为负
                    }
                } else if(current_selection == 1) { // LED_Dir
                    if(*(item->param_ptr) < 0) {
                        *(item->param_ptr) = 10; // 方向不能为负
                    }
                }
            } else if(current_menu == PID) {
                // PID参数每次减0.1（小数）
                (*(item->param_ptr)) -= 1;
            }
			OLED_Clear();
        }
    }
}

// 进入下一级菜单
void handle_a() {
    MenuItem *item = &menus[current_menu].items[current_selection];
    
    switch (item->type) {
        case SUBMENU_ITEM:
            current_menu = item->target_menu;
            current_selection = 0;
            break;
        case PARAMETER_ITEM:
            edit_mode = 1;  // 进入编辑模式
            break;
        case FUNCTION_ITEM:
            // 执行功能时不在这里清屏，让display_menu统一处理
            // 功能显示会在下一次display_menu调用时更新
            break;
    }
}

// 返回上级菜单
void go_back() {
    if(edit_mode) {
        // 在编辑模式下，返回键退出编辑模式
        edit_mode = 0;
		OLED_ShowChar(1,15,' ');
    } else {
        // 普通模式下返回主菜单
        if (current_menu != MAIN_MENU) {
            // 根据当前菜单返回主菜单并设置正确的选中项
            switch (current_menu) {
                case LED_CONTROL:
                    current_menu = MAIN_MENU;
                    current_selection = 0;
                    break;
                case PID:
                    current_menu = MAIN_MENU;
                    current_selection = 1;
                    break;
                case IMAGE:
                    current_menu = MAIN_MENU;
                    current_selection = 2;
                    break;
                case ANGLE:
                    current_menu = MAIN_MENU;
                    current_selection = 3;
                    break;
                case MAIN_MENU:
                    // 已经在主菜单，无需操作
                    break;
            }
        }
    }
}

// 菜单处理主函数
void menu_process(uint8_t key_value) {
    // 根据按键值执行相应操作
    switch(key_value) {
        case 1:  // 上
            move_up();
            break;
        case 2:  // 下
            move_down();
            break;
        case 3:  // 确认
            handle_a();
            break;
        case 4:  // 返回
            go_back();
            break;
    }
    // 统一在最后刷新显示，避免多次清屏
    display_menu();
}

//返回方向
int Menu_GetDir()
{
	return led_dir / 10;
}

//返回速度
int Menu_GetSpeed()
{
	return led_speed / 10;
}

//旋钮操控
void Menu_Tick()
{
    int16_t tmp = Encoder_Get();
    
    if(tmp != 0) {
        if(edit_mode) {
            // 编辑模式下调整参数值
            MenuItem *item = &menus[current_menu].items[current_selection];
            if(item->type == PARAMETER_ITEM) {
                int increment = 0;
                
                if(current_menu == LED_CONTROL) {
                    increment = tmp * 10;  // LED参数步进
                } else if(current_menu == PID) {
                    increment = tmp;  // PID参数步进
                }
                
                *(item->param_ptr) += increment;
                
                // 参数范围限制
                if(current_menu == LED_CONTROL) {
                    if(current_selection == 0) { // LED_Speed
                        if(*(item->param_ptr) < 0) *(item->param_ptr) = 0;
                    } else if(current_selection == 1) { // LED_Dir
                        if(*(item->param_ptr) < 0) *(item->param_ptr) = 0;
                        if(*(item->param_ptr) > 10) *(item->param_ptr) = 10; // 方向限制
                    }
                }
                OLED_Clear(); // 刷新显示
				display_menu();
            }
        } else {
            // 导航模式下用编码器选择菜单项
            if(tmp > 0) {
                current_selection++;
                if(current_selection >= menus[current_menu].item_count) {
                    current_selection = 0;
                }
            } else if(tmp < 0) {
                current_selection--;
                if(current_selection < 0) {
                    current_selection = menus[current_menu].item_count - 1;
                }
            }
            OLED_Clear(); // 刷新显示
			display_menu();
        }
    }
	if (edit_mode && current_menu == PID) {
		MenuItem *item = &menus[current_menu].items[current_selection];
		if (Key_Check(0, KEY_REPEAT)) {
			(*item->param_ptr)++;
			OLED_Clear(); // 刷新显示
			display_menu();
		}
		if (Key_Check(1, KEY_REPEAT)) {
			(*item->param_ptr)--;
			OLED_Clear(); // 刷新显示
			display_menu();
		}
			
	}
}