#include "Serial.h"

uint16_t RXData;
uint16_t RXFlag = 0;
uint16_t RXPack_Numint[10];
uint16_t RXPack_Numdeci[10];
char RXPack[10];
uint16_t RX_State = 0;
uint16_t CNT1 = 0;
uint16_t CNT2 = 0;
uint16_t CNT3 = 0;
void Serial_Inint(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef AAAA;
	AAAA.GPIO_Mode=GPIO_Mode_AF_PP;
	AAAA.GPIO_Pin=GPIO_Pin_9;
	AAAA.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&AAAA);
	
	GPIO_InitTypeDef CCCC;
	CCCC.GPIO_Mode=GPIO_Mode_IPU;
	CCCC.GPIO_Pin=GPIO_Pin_10;
	CCCC.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&CCCC);
	
	
	USART_InitTypeDef BBBB;
	BBBB.USART_BaudRate= 115200;
	BBBB.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	BBBB.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	BBBB.USART_Parity=USART_Parity_No;
	BBBB.USART_StopBits=USART_StopBits_1;
	BBBB.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1,&BBBB);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef DDDD;
	DDDD.NVIC_IRQChannel = USART1_IRQn;
	DDDD.NVIC_IRQChannelCmd =  ENABLE;
	DDDD.NVIC_IRQChannelPreemptionPriority = 0;
	DDDD.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&DDDD);
	
	USART_Cmd(USART1,ENABLE);
	
}

void Serial_SendBit(uint8_t BIT)
{
	USART_SendData(USART1,BIT);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == 0);

}

void Serial_SendString(char *String)
{	
	uint8_t i;
	for(i=0;String[i] != 0;i++)
	{	
		Serial_SendBit(String[i]);
	}

}

void Serial_SendArray(uint16_t *Array,uint16_t Length)
{	
	uint8_t i;
	for(i=0;i<Length;i++)
	{	
		Serial_SendBit(Array[i]);
	}

}

uint32_t Serial_Pow(uint32_t X,uint32_t Y)  
{
	uint32_t Result=1; //  定义一个无符号32位整型变量Result，并初始化为1
	while(Y--) //  使用while循环，当Y大于0时执行循环
	{
		Result *=X; //  将Result乘以X，实现幂运算
	}
	return Result; //  返回计算结果
}

void Serial_SendNum(uint32_t Number,uint16_t Length)
{	
	uint8_t i;
	for(i=0;i<Length;i++)
	{	
		Serial_SendBit(Number/Serial_Pow(10,Length-i-1)%10 + 0x30);
	}
}

/****************************************
int fputc(int ch,FILE *f)
{
	Serial_SendBit(ch);
	return ch;
}

****************************************/

int fputc(int ch, FILE *f)
{
	Serial_SendBit(ch);
	return ch;
}


uint16_t USART_GetRXData(void)
{
	return RXData;
}

uint16_t USART_GetRXFlag(void)
{
	if(RXFlag == 1)
	{
		RXFlag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

void Serial_SendPacket(uint16_t Serial_Pack[])
{
	Serial_SendBit(0XFF);
	Serial_SendArray(Serial_Pack,4);
	Serial_SendBit(0XFE);

}

float Serial_NumGet()
{	
	uint8_t i;
	uint16_t Temp_int = 0;
	float Temp = 0 , Temp_Deci = 0;
	for(i=0;i<CNT2;i++)
	{
		Temp_int = Temp_int*10 + RXPack_Numint[i];
	}
	for(i=0;i<CNT3;i++)
	{
		Temp_Deci = Temp_Deci*10 +RXPack_Numdeci[i];
	}
	Temp = Temp_int + Temp_Deci / Serial_Pow(10,CNT3);
	return Temp;
}

void USART1_IRQHandler(void)
{	
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == 1)
	{
		RXData = USART_ReceiveData(USART1);
		if(RX_State == 0)
		{
			if(RXData == '@')
			{
				CNT1 = 0;
				CNT2 = 0;
				CNT3 = 0;
				RX_State = 1;
			}
			else
			{}
		}
		else if(RX_State == 1)
		{
			if(RXData == '\r')
			{
				RX_State = 4;
				RXPack[CNT1] = '\0';
			} 
			else if(RXData == ':')
			{
				RX_State = 2;
				RXPack[CNT1] = '\0';
			}
			else
			{
				if(CNT1 < 9)
				{
					RXPack[CNT1] =  RXData;
					CNT1++;
				}
			}
			
		}
		else if(RX_State == 2)
		{
			if(RXData == '\r')
			{
				RX_State = 4;
			}
			else if(RXData == '.')
			{
				RX_State = 3;
			}
			else
			{
				if(CNT2 < 9)
				{
					RXPack_Numint[CNT2] = RXData - '0';
					CNT2++; 
				}
			}
		}
		else if(RX_State == 3)
		{
			if(RXData == '\r')
			{
				RX_State = 4;
			}
			else 
			{	
				if(CNT3 < 9)
				{
					RXPack_Numdeci[CNT3] = RXData - '0';
					CNT3++; 
			 	}
			}
		}

		else if(RX_State == 4)
		{
			if(RXData == '\n')
			{
				RX_State = 0;
				RXFlag = 1;
			}
			
			
		}
	}
	
}
