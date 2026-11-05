#ifndef __SERIAL_H
#define __SERIAL_H
#include <stdio.h>
#include "stm32f10x.h"                  // Device header

extern char RXPack[];
extern uint16_t RXPack_Numint[];
extern uint16_t RXPack_Numdeci[];
extern uint16_t CNT1;
extern uint16_t CNT2;
extern uint16_t CNT3;
void Serial_Inint(void);
void Serial_SendBit(uint8_t BIT);
void Serial_SendString(char *String);
void Serial_SendArray(uint16_t *Array,uint16_t Length);
void Serial_SendNum(uint32_t Number,uint16_t Length);
void Serial_SendPacket(uint16_t Serial_Pack[]);
uint16_t USART_GetRXData(void);
uint16_t USART_GetRXFlag(void);
extern uint16_t RXFlag;


#endif 
