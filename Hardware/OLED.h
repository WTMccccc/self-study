#ifndef __OLED_H
#define __OLED_H
#include "stm32f10x.h"
#include "Tem_Sensor.h"
#include "Time.h"
#include "Sundires.h"
#include "Light.h"

typedef struct
{
    uint8_t Menu_Page;		//菜单页码
    uint8_t Item_cnt;		//选项数量

} OLED_MenuTypeDef;

extern OLED_MenuTypeDef *current_Menu;
extern OLED_MenuTypeDef OLED_Menu_Page1;
extern OLED_MenuTypeDef OLED_Menu_Moresetting;
extern OLED_MenuTypeDef OLED_Menu_Page2;
extern uint8_t Voice_control;
extern uint8_t Line;
extern int8_t cursor;
extern uint8_t Button_rise_fall_falg;
extern uint8_t Voice_Flag;



void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowDeciNum(uint8_t Line, uint8_t Column, float Number, uint8_t Length_int,uint8_t Length_Deci);
void OLED_Menu_Init(void);                     //菜单初始化
void OLED_Begin_Mainmuen(void);                                        //主菜单begin
void OLED_Begin_Moresettingmuen(void);                                     //更多设置菜单begin
void OLED_Begin_Page2(void);                                           //页面2 begin
uint8_t Cursor(void);                                  //调用后清零
void OLED_ShowMenu(void);                                               //显示菜单
void OLED_ShowChoose(uint8_t Line, uint8_t Column, char *String);	//显示选项

#endif
