#include "OLED.h"
#include "OLED_Font.h"

OLED_MenuTypeDef OLED_Menu_Page1;
OLED_MenuTypeDef OLED_Menu_Moresetting;
OLED_MenuTypeDef OLED_Menu_Page2;
OLED_MenuTypeDef *current_Menu;
uint8_t Line = 0;
int8_t cursor = 1;
uint8_t Voice_control = 0;
uint8_t Button_rise_fall_falg = 0;
uint8_t Voice_Flag = 1;
/*引脚配置*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x))

/*引脚初始化*/
void OLED_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(!!(Byte & (0x80 >> i)));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  *
  *
  *
  *
  */
void OLED_ShowDeciNum(uint8_t Line, uint8_t Column, float Number, uint8_t Length_int,uint8_t Length_Deci)
{
	uint64_t temp;
	uint8_t i;
	for (i = 0; i < Length_int; i++)							
	{
		temp = Number*OLED_Pow(10,Length_Deci);
		OLED_ShowChar(Line, Column + i, temp / OLED_Pow(10, Length_int +Length_Deci - i - 1) % 10 + '0');
	}
	OLED_ShowChar(Line,Column + Length_int , '.' );
	for (i = 0; i < Length_Deci; i++)							
	{
		temp = Number*OLED_Pow(10,Length_Deci);
		OLED_ShowChar(Line, Column + Length_int + 1 + i, temp / OLED_Pow(10, Length_Deci - i - 1) % 10 + '0');
	}

}



void OLED_ShowChoose(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	OLED_ShowString(Line, Column, "[");
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i + 1, String[i]);
	}
	OLED_ShowString(Line, Column + i + 1, "]");
}




/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}

/*********************菜单初始化*************************************************** */
void OLED_Menu_Init(void)
{
	current_Menu = &OLED_Menu_Page1;
	OLED_Menu_Page1.Menu_Page = 1;
	OLED_Menu_Page1.Item_cnt = 1;

	
	OLED_Menu_Moresetting.Menu_Page = 2;	
	OLED_Menu_Moresetting.Item_cnt = 4;


	OLED_Menu_Page2.Menu_Page = 3;
	OLED_Menu_Page2.Item_cnt = 4;

}
/************************************************************************ */

/*********************第一页面（主页面）的显示内容*************************************************** */
void OLED_Begin_Mainmuen(void)
{
	if(current_Menu->Menu_Page == 1)
	{
		OLED_ShowString(4,1,"Next page");
	}
}
/************************************************************************ */

/********************更多设置页面的显示内容**************************************************** */
void OLED_Begin_Moresettingmuen(void)
{
	if(current_Menu->Menu_Page == 2)
	{
		OLED_ShowString(1,1,"wTe:");
		OLED_ShowDeciNum(1,6,Value_temp_warning,2,1);
		OLED_ShowString(2,1,"whu:");
		OLED_ShowDeciNum(2,6,Value_humi_warning,2,1);
		OLED_ShowString(3,1,"Lig:");
		OLED_ShowNum(3,6,Value_light_warning,5);
	
		OLED_ShowString(4,1,"Return");
	}
}
/************************************************************************ */

/*********************第二页的显示内容*************************************************** */
void OLED_Begin_Page2(void)
{
	if(current_Menu->Menu_Page == 3)
	{
		OLED_ShowString(1,1,"More Set");  
		OLED_ShowString(4,1,"Last Page");
		if(Voice_control == 1)
		{	
			OLED_ShowString(2,1,"Voice:ON");  
		}
		else if(Voice_control == 0)
		{
			OLED_ShowString(2,1,"Voice:OFF");  
		}
	}
}
/************************************************************************ */

/*********************没咋用 输出Line*cursor的值，调用后清除cursor的值，（cursor为1，0，-1，分别表示向下一个选项，静止，向上一个选项）*************************************************** */
uint8_t Cursor(void)
{	
	Line = cursor+Line;
	Limit(&Line, 1, current_Menu->Item_cnt); 
	uint8_t temp = Line*cursor;
	cursor = 0;
	return temp;
}
/************************************************************************ */

/**
 * @brief OLED显示菜单函数
 * 该函数用于在OLED屏幕上显示不同的菜单页面和内容，并根据用户操作更新显示
 */
void OLED_ShowMenu(void)
{	
//*******************第二页第一，二行的数据刷新（按键按下改变变量同时刷新数值）********************************************************************************************************** *//
	// 检查是否需要刷新温度和湿度警告值
	if(Once_ues(&sX, &sX) == 1)
	{
		// 显示温度警告值，位置在第1行第6列，2位小数，右对齐
		OLED_ShowDeciNum(1,6,Value_temp_warning,2,1);
		// 显示湿度警告值，位置在第2行第6列，2位小数，右对齐
		OLED_ShowDeciNum(2,6,Value_humi_warning,2,1);
		// 显示光强警告值，位置在第3行第6列，5位整数，右对齐
		OLED_ShowNum(3,6,Value_light_warning,5);
	}
//***************************************************************************************************************************** *//
	
//********************外设的开关切换（ON/OFF）********************************************************************************************************* *//
	// 处理语音开关状态
	if(Voice_Flag == 1 && Menu_Flag == 1)
	{
		Voice_Flag = 0;  // 复位语音标志位
		// 如果当前在第二页且语音控制开启
		if(current_Menu->Menu_Page == 3 && Voice_control == 1)
		{	
			// 显示返回箭头和"Voice:ON"
			OLED_ShowString(2,1,"           <-");
			OLED_ShowChoose(2,1,"Voice:ON");  
		}
		// 如果当前在第二页且语音控制关闭
		else if(current_Menu->Menu_Page == 3 && Voice_control == 0)
		{
			OLED_ShowString(2,1,"           <-");
			OLED_ShowChoose(2,1,"Voice:OFF");  
		}
	}
//***************************************************************************************************************************** *//

//********************菜单每一页每一行显示的内容********************************************************************************************************* *//
	/***每一页第一行的显示内容（cursor为行，Menu_Page为页数）********************************************************************* */
	if(cursor == 1 && Menu_Flag == 1)
	{	
		Data_clear(&cursor, &Line);
		if(current_Menu->Menu_Page == 1)
		{	
			OLED_Clear();
			OLED_Begin_Mainmuen();  
			OLED_ShowString(4,12,"<-"); 
			OLED_ShowChoose(4,1,"<1 page>");
		}
		else if(current_Menu->Menu_Page == 2)
		{
			OLED_Clear();
			OLED_Begin_Moresettingmuen();
			OLED_ShowString(1,12,"<-");
			OLED_ShowChar(1,1,'[');
			OLED_ShowString(1,2,"wTe:");
			OLED_ShowChar(1,10,']');
		}
		else if(current_Menu->Menu_Page == 3)
		{
			OLED_Clear();
			OLED_Begin_Page2();
			OLED_ShowString(1,12,"<-");    
			OLED_ShowChoose(1,1,"More Set");
		}
	}
	/************************************************************************ */

	/***每一页第二行的显示内容（cursor为行，Menu_Page为页数）********************************************************************* */
	else if(cursor == 2)
	{ 	
		Data_clear(&cursor, &Line);
		if(current_Menu->Menu_Page == 2)
		{
			OLED_Clear();
			OLED_Begin_Moresettingmuen();
			OLED_ShowString(2,12,"<-"); 
			OLED_ShowChar(2,1,'[');
			OLED_ShowString(2,2,"whu:");
			OLED_ShowChar(2,10,']');
		}
		else if(current_Menu->Menu_Page == 3)
		{
			OLED_Clear();
			OLED_Begin_Page2();
			if(Voice_control == 1)
			{	
				OLED_ShowChoose(2,1,"Voice:ON");  
			}
				else if(Voice_control == 0)
			{
				OLED_ShowChoose(2,1,"Voice:OFF");  
			}
		OLED_ShowString(2,12,"<-"); 
			
		}

	}
	/************************************************************************ */

	/***每一页第三行的显示内容（cursor为行，Menu_Page为页数）********************************************************************* */
	else if(cursor == 3)
	{
		Data_clear(&cursor, &Line);
		if(current_Menu->Menu_Page == 2)
		{
			OLED_Clear();
			OLED_Begin_Moresettingmuen();
			OLED_ShowString(3,12,"<-"); 
			OLED_ShowChar(3,1,'[');
			OLED_ShowString(3,2,"Lig:");
			OLED_ShowChar(3,11,']');
			
		}
		if(current_Menu->Menu_Page == 3)
		{
			OLED_Clear();
			OLED_Begin_Page2();
			OLED_ShowString(3,2,"warning");

		}
	}
	/************************************************************************ */

	/***每一页第四行的显示内容（cursor为行，Menu_Page为页数）********************************************************************* */
	else if(cursor == 4)
	{	
		Data_clear(&cursor, &Line);
		if(current_Menu->Menu_Page == 2)
		{
			OLED_Clear(); 
			OLED_Begin_Moresettingmuen();
			OLED_ShowString(4,12,"<-"); 
			OLED_ShowChoose(4,1,"Return");
		}
		if(current_Menu->Menu_Page == 3)
		{
			OLED_Clear(); 
			OLED_Begin_Page2();
			OLED_ShowString(4,12,"<-");
			OLED_ShowChoose(4,1,"Last Page");
		}
	}
	/************************************************************************ */
//***************************************************************************************************************************** *//
}
