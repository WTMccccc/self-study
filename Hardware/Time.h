#ifndef __Time_H
#define __Time_H

#include "stm32f10x.h"
#include "Serial.h"
#include "OLED.h"
#include "Tem_Sensor.h"
#include "Sundires.h"
#include "Key.h"

extern double Value_temp_warning;
extern double Value_humi_warning;
extern uint8_t Menu_Flag;
extern uint8_t sX;
extern uint8_t i;

void Time_Init(void);
void TIM3_Init(void);

#endif
