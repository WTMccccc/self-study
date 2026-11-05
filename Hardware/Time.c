#include "Time.h"

double Value_humi_warning = 70;
double Value_temp_warning = 27;
extern uint16_t KeyNum;
uint8_t Menu_Flag = 1;
uint8_t sX = 0;
uint8_t i = 0;

void Time_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_2;
	TIM_2.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_2.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_2. TIM_Period = 2000 - 1;                        //0.2s 200ms
	TIM_2.TIM_Prescaler = 7200 - 1;
	TIM_2.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2,&TIM_2);
	
	TIM_ClearFlag(TIM2,TIM_IT_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	NVIC_InitTypeDef NVIC_TIM2;
	NVIC_TIM2.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_TIM2.NVIC_IRQChannelCmd = ENABLE;
	NVIC_TIM2.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_TIM2.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_TIM2);
	
}
void TIM3_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_3;
	TIM_3.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_3.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_3. TIM_Period = 2000 - 1;
	TIM_3.TIM_Prescaler = 7200 - 1;
	TIM_3.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3,&TIM_3);
	
	TIM_ClearFlag(TIM3,TIM_IT_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	NVIC_InitTypeDef NVIC_TIM3;
	NVIC_TIM3.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_TIM3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_TIM3.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_TIM3.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_TIM3);

	TIM_Cmd(TIM3,ENABLE);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == 1)
	{
		TIM_Cmd(TIM2,DISABLE);
	//*********************************************GPIOB12按钮按下后产生的中断******************************************************************************** *//
		if(KeyNum == 1 && EXTI_GetITStatus(EXTI_Line12) == 0)
		{
		/***当菜单不处于选中状态时，GPIOB12为向上按键********************************************************************* */
			if(Menu_Flag == 1)
			{	
				cursor = -1;          //   向上一个选项
				cursor = Line + cursor;     //读取行数（旧行数+新操作向上（-1））
				if(cursor > current_Menu -> Item_cnt)
				{
					cursor = 1;
				}
				else if(cursor < 1)
				{
					cursor = current_Menu ->Item_cnt;
				}
			}	
		/************************************************************************ */

		/***当菜单处于选中状态，GPIOB12为数值增加按键********************************************************************* */
			if(Button_rise_fall_falg == 1 && Line == 1)
			{
				Value_temp_warning=Value_temp_warning-0.5;
				sX= 1;
			}
			else if(Button_rise_fall_falg == 1 && Line == 2)
			{
				Value_humi_warning=Value_humi_warning-0.5;
				sX= 1;
			}
			else if(Button_rise_fall_falg == 1 && Line == 3)
			{
				Value_light_warning=Value_light_warning-100;
				sX= 1;
			}
		/************************************************************************ */
			KeyNum = 0;
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		}
	//***************************************************************************************************************************** *//
		
	//**********************************************GPIOB14按钮按下后产生的中断******************************************************************************* *//
		else if(KeyNum == 2 && EXTI_GetITStatus(EXTI_Line14) == 0)
		{
			TIM_Cmd(TIM2,DISABLE);
		/***s********************************************************************* */
			if(Menu_Flag == 1)
			{
				cursor = 1;				//   向下一个选项
				cursor = Line + cursor;				//读取行数（旧行数+新操作向上（+1）
				if(cursor > current_Menu -> Item_cnt)
				{
					cursor = 1;
				}
				else if(cursor < 1)
				{
					cursor = current_Menu ->Item_cnt;
				}
			}
		/************************************************************************ */

		/***当菜单处于选中状态，GPIOB14为数值减少按键********************************************************************* */
			if(Button_rise_fall_falg == 1 && Line == 1)
			{
				Value_temp_warning=Value_temp_warning+0.5;
				sX= 1;
			}
			else if(Button_rise_fall_falg == 1 && Line == 2)
			{
				Value_humi_warning=Value_humi_warning+0.5;
				sX= 1;
			}
			else if(Button_rise_fall_falg == 1 && Line == 3)
			{
				Value_light_warning=Value_light_warning+100;
				sX= 1;
			}
		/************************************************************************ */
			KeyNum = 0;
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		}
	//***************************************************************************************************************************** *//
		
	//**********************************************GPIOB10按钮按下后产生的中断******************************************************************************* *//
		/***当菜单不处于选中状态时，GPIOB10为确认按键********************************************************************* */
		else if(KeyNum == 3 && EXTI_GetITStatus(EXTI_Line10) == 0)
		{
			TIM_Cmd(TIM2,DISABLE);
			Key_OKFlag = 1;					//确认按键按下标志位置1
			KeyNum = 0;
			TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		}
		/************************************************************************ */
	//***************************************************************************************************************************** *//
	}
		
}


void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == 1)
	{
	
		i++;
		if(Tem_And_Humi.temp_int+Tem_And_Humi.temp_deci*0.1>= Value_temp_warning)
		{
			warning_Flag_tem = 1;
		}
		else 
		{
			warning_Flag_tem = 0;
		}
		if(Tem_And_Humi.humi_int>= Value_humi_warning)
		{
			warning_Flag_humi = 1;
		}
		else 
		{
			warning_Flag_humi = 0;
		}
		if(Data >= Value_light_warning)
		{
			warning_Flag_light = 1;
		}
		else 
		{
			warning_Flag_light = 0;
		}
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}


