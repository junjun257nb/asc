#ifndef __MENU_H
#define __MENU_H

// 函数声明
void init_menus(void);
void display_menu(void);
void move_up(void);
void move_down(void);
void handle_a(void);
void go_back(void);
void menu_process(uint8_t key_value);
int Menu_GetDir();
int Menu_GetSpeed();
void Menu_Tick();

#endif
