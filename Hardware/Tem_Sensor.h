#ifndef __Tem_Sensor_H
#define __Tem_Sensor_H
#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/************************** DHT11 数据类型定义********************************/
typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
		                 
} DHT11_Data_TypeDef;
extern DHT11_Data_TypeDef Tem_And_Humi;
/*****************************************************************************/




uint8_t Tem_GPIORead(void);
void Tem_Init(void);
void Tem_Mod_IPU(void);
void Tem_Mod_OutPP(void);

static uint8_t Tem_Recive(void);
uint8_t Tem_Inf(DHT11_Data_TypeDef *DHT11_Data);
extern uint8_t warning_Flag_tem;
extern uint8_t warning_Flag_humi;
#endif
