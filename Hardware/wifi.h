#ifndef __WIFI_H
#define __WIFI_H

#include "stm32f10x.h"
#include "Delay.h"
#include "string.h"
#include "Serial.h"
#include "stdio.h"
#include "stdlib.h"

extern char Wifi_RXData[100];
extern char Wifi_RXData_Online[512];
extern uint16_t wifi_state;
extern volatile uint8_t wifi_Flag;
extern volatile uint8_t wifi_online_flag;
extern volatile uint8_t wifi_OnlineGetting_Flag;
extern float wifi_Online_Tem;
extern float wifi_Online_Humi;
extern char Current_Online_Time[17];
extern uint8_t state;
extern uint8_t brace_count;
void WiFI_Init(void);
void Wifi_SendBit(uint8_t BIT);
void Wifi_SendString(char *String);
void Wifi_SendArray(uint16_t *Array,uint16_t Length);
uint32_t Wifi_Pow(uint32_t X,uint32_t Y);
void Wifi_SendNum(uint32_t Number,uint16_t Length);
void Wifi_SendPacket(uint16_t Wifi_Pack[]);
uint16_t wifi_flag_check(char *response);
void wifi_Provide(void);
void wifi_connect(void);
void wifi_GetOnlineTemAndHumi(void);

void wifi_Provide_text(void);
#endif 