#include "Tem_Sensor.h"

DHT11_Data_TypeDef Tem_And_Humi;
uint8_t warning_Flag_tem = 0;
uint8_t warning_Flag_humi = 0;
uint8_t Tem_GPIORead(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void Tem_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIOB1;
	GPIOB1.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOB1.GPIO_Pin = GPIO_Pin_1;
	GPIOB1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIOB1);
	
}


void Tem_Mod_IPU(void)
{
	GPIO_InitTypeDef GPIOB1  ;
	GPIOB1.GPIO_Pin = GPIO_Pin_1;
	GPIOB1.GPIO_Mode = GPIO_Mode_IPU;
	GPIOB1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIOB1);
}

void Tem_Mod_OutPP(void)
{
	GPIO_InitTypeDef GPIOB1;
	GPIOB1.GPIO_Pin = GPIO_Pin_1;
	GPIOB1.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOB1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIOB1);
}

static uint8_t Tem_Recive(void)
{
	uint8_t i, temp=0;
	

	for(i=0;i<8;i++)    
	{	 
		/*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
		while(Tem_GPIORead() == 0);

		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
		 */
		Delay_us(40); //延时x us 这个延时需要大于数据0持续的时间即可	   	  

		if(Tem_GPIORead() == 1)/* x us后仍为高电平表示数据“1” */
		{
			/* 等待数据1的高电平结束 */
			while(Tem_GPIORead() == 1);

			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
		}
		else	 // x us后为低电平表示数据“0”
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
		}
	}
	
	return temp;
}









uint8_t Tem_Inf(DHT11_Data_TypeDef *DHT11_Data)                                 
{
	Tem_Mod_OutPP();
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	Delay_ms(18);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	Delay_us(30);
	
	Tem_Mod_IPU();
	if(Tem_GPIORead() == 0)
	{
		while(Tem_GPIORead() == 0);
		
		while(Tem_GPIORead() == 1);
		
		DHT11_Data->humi_int = Tem_Recive();
		DHT11_Data->humi_deci = Tem_Recive();
		DHT11_Data->temp_int = Tem_Recive();
		DHT11_Data->temp_deci = Tem_Recive();
		DHT11_Data->check_sum = Tem_Recive();
		
		Tem_Mod_OutPP();
		GPIO_SetBits(GPIOB,GPIO_Pin_1);
		
		
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return 1;
		else 
			return 0;
	}
	else
		return 0;
	
	
	
	
}


