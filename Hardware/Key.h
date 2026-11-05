#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Serial.h"
#include "Time.h"
#include "sundires.h"
#include "OLED.h"


void Key_Init(void);
uint16_t Get_KeyNum(void);
void Key_OKButton(void);
extern uint8_t Key_OKFlag;

#endif
