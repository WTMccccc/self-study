#include "key.h"

uint16_t KeyNum = 0;
uint8_t Key_OKFlag = 0;
/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB1和PB11引脚初始化为上拉输入
	

	/*EXTI外部中断初始化*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
	
	EXTI_InitTypeDef EXTI_GPIOB12;
	EXTI_GPIOB12.EXTI_Line = EXTI_Line12;
	EXTI_GPIOB12.EXTI_LineCmd = ENABLE;
	EXTI_GPIOB12.EXTI_Mode =EXTI_Mode_Interrupt;
	EXTI_GPIOB12.EXTI_Trigger = EXTI_Trigger_Falling;
	
	EXTI_Init(&EXTI_GPIOB12);
	
	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);
	
	EXTI_InitTypeDef EXTI_GPIOB13;
	EXTI_GPIOB13.EXTI_Line = EXTI_Line13;
	EXTI_GPIOB13.EXTI_LineCmd = ENABLE;
	EXTI_GPIOB13.EXTI_Mode =EXTI_Mode_Interrupt;
	EXTI_GPIOB13.EXTI_Trigger = EXTI_Trigger_Falling;
	
	EXTI_Init(&EXTI_GPIOB13);
	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	
	EXTI_InitTypeDef EXTI_GPIOB14;
	EXTI_GPIOB14.EXTI_Line = EXTI_Line14;
	EXTI_GPIOB14.EXTI_LineCmd = ENABLE;
	EXTI_GPIOB14.EXTI_Mode =EXTI_Mode_Interrupt;
	EXTI_GPIOB14.EXTI_Trigger = EXTI_Trigger_Falling;
	
	EXTI_Init(&EXTI_GPIOB14);

	/*NVIC初始化*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_GPIOB12;
	NVIC_GPIOB12.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_GPIOB12.NVIC_IRQChannelCmd = ENABLE;
	NVIC_GPIOB12.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_GPIOB12.NVIC_IRQChannelSubPriority = 2;
	
	NVIC_Init (&NVIC_GPIOB12);
	
	
}


/**
  * 函    数：按键获取键码
  * 参    数：无
  * 返 回 值：按下按键的键码值，范围：0~2，返回0代表没有按键按下
  * 注意事项：此函数是阻塞式操作，当按键按住不放时，函数会卡住，直到按键松手
  */
  
  /*
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;		//定义变量，默认键码值为0
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)			//读PB1输入寄存器的状态，如果为0，则代表按键1按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 1;												//置键码为1
	}
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)			//读PB11输入寄存器的状态，如果为0，则代表按键2按下
	{
		Delay_ms(20);											//延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);	//等待按键松手
		Delay_ms(20);											//延时消抖
		KeyNum = 2;												//置键码为2
	}
	
	return KeyNum;			//返回键码值，如果没有按键按下，所有if都不成立，则键码为默认值0
}


  */
  
  
uint16_t Get_KeyNum(void)
{
	int A= KeyNum;
	return A;
	 
}

void Key_OKButton(void)           //确认按键的函数
{
	
	if(Once_ues(&Key_OKFlag,&Key_OKFlag) == 1)
	{
	/***第一页第一行和第一页第二行的确认按钮作用********************************************************************* */
		if(Line == 1 && current_Menu->Menu_Page == 1)
		{
			current_Menu = &OLED_Menu_Page2;
			cursor = 1;
		}

	/************************************************************************ */

	/***More seting页的确认按钮作用********************************************************************* */
		else if(Line == 1 && current_Menu->Menu_Page == 2)
		{	
			Menu_Flag =(Menu_Flag == 0)?1:0;
			OLED_ShowString(1,1,"          <-");
			Button_rise_fall_falg = (Button_rise_fall_falg == 0)?1:0;  //标志位翻转
			if(Button_rise_fall_falg == 1)								//标志位为1 显示选中
			{
				OLED_ShowString(1,1,"wTe:");
				OLED_ShowDeciNum(1,6,Value_temp_warning,2,1);
				OLED_ShowChar(1,5,'[');
				OLED_ShowChar(1,10,']');
			}
			else
			{
				OLED_ShowString(1,2,"wTe:");
				OLED_ShowDeciNum(1,6,Value_temp_warning,2,1);
				OLED_ShowChar(1,1,'[');
				OLED_ShowChar(1,10,']');
			}
		}
		else if(Line == 2 && current_Menu->Menu_Page == 2)
		{
			Menu_Flag =(Menu_Flag == 0)?1:0;
			OLED_ShowString(2,1,"           <-");
			Button_rise_fall_falg = (Button_rise_fall_falg == 0)?1:0; //标志位翻转
			if(Button_rise_fall_falg == 1)					//标志位为1 显示选中		
			{
				OLED_ShowString(2,1,"whu:");
				OLED_ShowDeciNum(2,6,Value_humi_warning,2,1);
				OLED_ShowChar(2,5,'[');
				OLED_ShowChar(2,10,']');
			}
			else
			{
				OLED_ShowString(2,2,"whu:");
				OLED_ShowDeciNum(2,6,Value_humi_warning,2,1);
				OLED_ShowChar(2,1,'[');
				OLED_ShowChar(2,10,']');
			}
		}
		else if(Line == 3 && current_Menu->Menu_Page == 2)
		{
			Menu_Flag =(Menu_Flag == 0)?1:0;
			OLED_ShowString(3,1,"           <-");
			Button_rise_fall_falg = (Button_rise_fall_falg == 0)?1:0; //标志位翻转
			if(Button_rise_fall_falg == 1)					//标志位为1 显示选中		
			{
				OLED_ShowString(3,1,"Lig:");
				OLED_ShowNum(3,6,Value_light_warning,5);
				OLED_ShowChar(3,5,'[');
				OLED_ShowChar(3,11,']');
			}
			else
			{
				OLED_ShowString(3,2,"Lig:");
				OLED_ShowNum(3,6,Value_light_warning,5);
				OLED_ShowChar(3,1,'[');
				OLED_ShowChar(3,11,']');
			}
		}
		else if(Line == 4 && current_Menu->Menu_Page == 2)
		{
			current_Menu = &OLED_Menu_Page2;
			cursor = 1;
		}
	/************************************************************************ */

	/***第二页的确认按钮作用********************************************************************* */
		else if(Line == 1 && current_Menu->Menu_Page == 3)
		{
			current_Menu = &OLED_Menu_Moresetting;
			cursor = 1;
		}
		else if(Line == 2 && current_Menu->Menu_Page == 3)
		{
			Voice_control=(Voice_control == 0)?1:0;
			Voice_Flag = 1;								//标志位为1 显示选中
		}
		else if(Line == 4 && current_Menu->Menu_Page == 3)
		{
			current_Menu = &OLED_Menu_Page1;
			cursor = 1;
		}
	/************************************************************************ */

	}
}


void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12) == 1)
	{
		TIM_Cmd(TIM2,ENABLE);
		KeyNum = 1;
		TIM_SetCounter(TIM2,0);
		EXTI_ClearFlag(EXTI_Line12);
	} 

	else if(EXTI_GetITStatus(EXTI_Line13) == 1)
	{
		TIM_Cmd(TIM2,ENABLE);
		KeyNum = 2;
		TIM_SetCounter(TIM2,0);
		EXTI_ClearFlag(EXTI_Line13);
	}

	else if(EXTI_GetITStatus(EXTI_Line14) == 1)
	{
		TIM_Cmd(TIM2,ENABLE);
		KeyNum = 3;
		TIM_SetCounter(TIM2,0);
		EXTI_ClearFlag(EXTI_Line14);
	}
}
