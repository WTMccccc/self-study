#ifndef __SUNDIRES_H
#define __SUNDIRES_H

#include "stm32f10x.h"         
#include "OLED.h"         
extern uint8_t Menu_Flag;
void Limit(uint8_t *x, uint16_t a, uint16_t b);
uint8_t Once_ues(uint8_t *x, uint8_t *y);
void Data_clear(int8_t *x, uint8_t *y);
void Buzzer_Init(void);
void Buzzer_ON(void);
void Buzzer_OFF(void);
void Buzzer_Turn(void);
float Combination_Pack(uint16_t *RXPack_Numint ,uint16_t *RXPack_Numdeci);

#endif