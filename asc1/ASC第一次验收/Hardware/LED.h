#ifndef __LED_H
#define __LED_H

void LED_Init(void);
void FloodLED_RightTurn(void);
void FloodLED_LeftTurn(void);
void LED_Tick(void);
void LED_DirSet(int);
void LED_SpeedSet(int);

#endif