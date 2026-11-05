#ifndef __Light_h
#define __Light_h

#include "stm32f10x.h"
#include "Delay.h"
#include "Serial.h"

extern uint16_t Data;
extern uint16_t Data_high;
extern uint16_t Data_low;
extern uint16_t Value_light_warning;
extern uint16_t warning_Flag_light;
void Light_checkeven(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
void Light_Init(void);
void Light_write(uint8_t Data);
uint16_t Light_Read(void);
uint8_t Light_Ack(void);
void Light_Sensor_Read(void);
void Light_Sensor_Read_Init(void);
#endif