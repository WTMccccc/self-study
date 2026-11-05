#include "wifi.h"

uint8_t state = 0;
char Wifi_RXData[100];
char Wifi_RXData_Online[512];
uint8_t w = 0;
uint16_t w_Online = 0;
uint16_t wifi_state = 2;
volatile uint8_t wifi_Flag = 0;
volatile uint8_t wifi_online_flag = 0;
volatile uint8_t wifi_OnlineGetting_Flag = 0;
uint8_t brace_count = 0;
float wifi_Online_Tem;
float wifi_Online_Humi;
char Current_Online_Time[17];

void WiFI_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef Wifi2;
	Wifi2.GPIO_Mode=GPIO_Mode_AF_PP;
	Wifi2.GPIO_Pin=GPIO_Pin_2;
	Wifi2.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&Wifi2);
	
	GPIO_InitTypeDef Wifi3;
	Wifi3.GPIO_Mode=GPIO_Mode_IPU;
	Wifi3.GPIO_Pin=GPIO_Pin_3;
	Wifi3.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&Wifi3);
	
	
	USART_InitTypeDef wifi;
	wifi.USART_BaudRate= 115200;
	wifi.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	wifi.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	wifi.USART_Parity=USART_Parity_No;
	wifi.USART_StopBits=USART_StopBits_1;
	wifi.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&wifi);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef Wifi_NVIC;
	Wifi_NVIC.NVIC_IRQChannel = USART2_IRQn;
	Wifi_NVIC.NVIC_IRQChannelCmd =  ENABLE;
	Wifi_NVIC.NVIC_IRQChannelPreemptionPriority = 0;
	Wifi_NVIC.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&Wifi_NVIC);
	
	USART_Cmd(USART2,ENABLE);
	
}


void Wifi_SendBit(uint8_t BIT)
{
	USART_SendData(USART2,BIT);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == 0);
}

void Wifi_SendString(char *String)
{	
	uint8_t i;
	for(i=0;String[i] != 0;i++)
	{	
		Wifi_SendBit(String[i]);
	}

}

void Wifi_SendArray(uint16_t *Array,uint16_t Length)
{	
	uint8_t i;
	for(i=0;i<Length;i++)
	{	
		Wifi_SendBit(Array[i]);
	}

}

uint32_t Wifi_Pow(uint32_t X,uint32_t Y)  
{
	uint32_t Result=1; //  瀹氫箟涓€涓棤绗﹀彿32浣嶆暣鍨嬪彉閲廟esult锛屽苟鍒濆鍖栦负1
	while(Y--) //  浣跨敤while寰幆锛屽綋Y澶т簬0鏃舵墽琛屽惊鐜?
	{
		Result *=X; //  灏哛esult涔樹互X锛屽疄鐜板箓杩愮畻
	}
	return Result; //  杩斿洖璁＄畻缁撴灉
}

void Wifi_SendNum(uint32_t Number,uint16_t Length)
{	
	uint8_t i;
	for(i=0;i<Length;i++)
	{	
		Wifi_SendBit(Number/Wifi_Pow(10,Length-i-1)%10 + 0x30);
	}
}

void Wifi_SendPacket(uint16_t Wifi_Pack[])
{ 
	Wifi_SendBit(0XFF);
	Wifi_SendArray(Wifi_Pack,4);
	Wifi_SendBit(0XFE);
}

uint16_t wifi_flag_check(char *response)
{
    int32_t timeout = 200; 
    while(strstr(Wifi_RXData, response) == NULL)
    {
        timeout--;
        Delay_ms(1);
        if(timeout <= 0 )
        {
            return 0;
        }
    }
    return 1;
}

void wifi_Provide(void)
{   
	Wifi_SendString("AT+RST\r\n");
	Delay_ms(2000);
    Wifi_SendString("ATE0\r\n");
    Delay_ms(100);
    Wifi_SendString("AT\r\n");
    if(wifi_flag_check("OK"))
    {
        printf("ESP32 is ready.\r\n");
        wifi_state = 0;
    }
    else
    {
        printf("ESP32 is not responding.\r\n");
        return;
    }
    Wifi_SendString("AT+CWMODE=2\r\n");
    Delay_ms(100);
    Wifi_SendString("AT+CWSAP=\"MyESP32\",\"12345678\",1,3\r\n");
    Delay_ms(100);
    Wifi_SendString("AT+CIPMUX=1\r\n");
    Delay_ms(100);
    Wifi_SendString("AT+CIPSERVER=1,8080\r\n");
    

}
void wifi_Provide_text(void)
{   
	Wifi_SendString("AT+RST\r\n");
	Delay_ms(2000);
	if(wifi_flag_check("ready"))
    {
        printf("AT+RST is OK.\r\n");
    }
    else
    {
        printf("AT+RST is error.\r\n");
        return;
    }

	Wifi_SendString("ATE0\r\n");
	Delay_ms(100);
 	if(wifi_flag_check("OK"))
    {
        printf("ATE0 is OK.\r\n");
    }
    else
    {
        printf("ATE0 is error.\r\n");
        return;
    }
    
    Wifi_SendString("AT\r\n");
    if(wifi_flag_check("OK"))
    {
        printf("ESP32 is ready.\r\n");
    }
    else
    {
        printf("ESP32 is not responding.\r\n");
        return;
    }

    Wifi_SendString("AT+CWMODE=2\r\n");
	Delay_ms(100);
	if(wifi_flag_check("OK"))
    {
        printf("AT+CWMODE=2 is OK.\r\n");
    }
    else
    {
        printf("AT+CWMODE=2 is error.\r\n");
        return;
    }
   

    Wifi_SendString("AT+CWSAP=\"MyESP32\",\"12345678\",1,3\r\n");
	Delay_ms(100);
	if(wifi_flag_check("OK"))
    {
        printf("AT+CWSAP is OK.\r\n");
    }
    else
    {
        printf("AT+CWSAP is error.\r\n");
        return;
    }
	
    Wifi_SendString("AT+CIPMUX=1\r\n");
	Delay_ms(100);
	if(wifi_flag_check("OK"))
    {
        printf("AT+CIPMUX=1 is OK.\r\n");
    }
    else
    {
        printf("AT+CIPMUX=1 is error.\r\n");
        return;
    }

    Wifi_SendString("AT+CIPSERVER=1,8080\r\n");
	Delay_ms(100);
    if(wifi_flag_check("OK"))
    {
        printf("AT+CIPSERVER=1,8080 is OK.\r\n");
    }
    else
    {
        printf("AT+CIPSERVER=1,8080 is error.\r\n");
        return;
    }

	Wifi_SendString("AT+CIPAP?\r\n");
	Delay_ms(200);
	if(wifi_flag_check("OK"))
    {
        printf("AT+CIPAP is OK.\r\n");
		wifi_state = 0;
    }
    else
    {
        printf("AT+CIPAP is error.\r\n");
        return;
    }
}

void wifi_connect(void)
{
	Wifi_SendString("AT+RST\r\n");
	Delay_ms(2000);
	if(wifi_flag_check("ready"))
    {
        printf("AT+RST is OK.\r\n");
    }
    else
    {
        printf("AT+RST is error.\r\n");
        return;
    }
	Wifi_SendString("ATE0\r\n");
	Delay_ms(100);
	if(wifi_flag_check("OK"))
    {
        printf("ATE0 is OK.\r\n");
    }
    else
    {
        printf("ATE0 is error.\r\n");
        return;
    }
    

    Wifi_SendString("AT\r\n");
	Delay_ms(200);
    if(wifi_flag_check("OK"))
    {
        printf("ESP32 is ready.\r\n");
    }
    else
    {
        printf("ESP32 is not responding.\r\n");
        return;
    }
	

    Wifi_SendString("AT+CWMODE=1\r\n");
	Delay_ms(200);
	if(wifi_flag_check("OK"))
    {
        printf("AT+CWMODE=1 is OK.\r\n");
    }
    else
    {
        printf("AT+CWMODE=1 is error.\r\n");
        return;
    }

    Wifi_SendString("AT+CWJAP=\"sb\",\"zhangzicong\"\r\n");
    Delay_s(4);
	if(wifi_flag_check("OK"))
    {
        printf("AT+CWJAP is OK.\r\n");
    }
    else
    {
        printf("AT+CWJAP is error.\r\n");
        return;
    }

    Wifi_SendString("AT+CIPMUX=1\r\n");
    Delay_ms(200);
	if(wifi_flag_check("OK"))
    {
        printf("AT+CIPMUX=1 is OK.\r\n");
    }
    else
    {
        printf("AT+CIPMUX=1 is error.\r\n");
        return;
    }

    Wifi_SendString("AT+CIPSERVER=1,8080\r\n");
	Delay_ms(200);
	if(wifi_flag_check("OK"))
    {
        printf("AT+CIPSERVER is OK.\r\n");
    }
    else
    {
        printf("AT+CIPSERVER is error.\r\n");
        return;
    }

    Wifi_SendString("AT+CIFSR\r\n");
	Delay_ms(200);
	if(wifi_flag_check("OK"))
    {
        printf("AT+CIFSR is OK.\r\n");
		wifi_state = 0;
    }
    else
    {
        printf("AT+CIFSR is error.\r\n");
        return;
    }
}

void wifi_GetOnlineTemAndHumi(void)
{
    Wifi_SendString("AT+CIPSTART=0,\"TCP\",\"api.open-meteo.com\",80\r\n");
	Delay_ms(2000);
	Wifi_SendString("AT+CIPSEND=0,160\r\n");
	Delay_ms(100);
	state = 0;
	brace_count = 0;
	wifi_OnlineGetting_Flag = 1;
	Wifi_SendString("GET /v1/forecast?latitude=23.02&longitude=113.75&current=temperature_2m,relative_humidity_2m&timezone=Asia/Shanghai HTTP/1.1\r\nHost:api.open-meteo.com\r\n\r\n");
	Delay_s(5);
	Wifi_SendString("AT+CIPCLOSE\r\n");
}

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2,USART_IT_RXNE) == 1)
    {
        uint8_t wifi_RxData = USART_ReceiveData(USART2);
		if(state == 0  && wifi_OnlineGetting_Flag == 0)
		{		 
			if(wifi_RxData == '@')
			{
				state = 1;
				w = 0;
			}
			else if(wifi_RxData == '\r'	 || wifi_RxData == '\n')
			{
				state = 1;
				w = 0;
			}
			else if(wifi_RxData == ' ')
			{

			}
		}
		else if(state == 1 && wifi_OnlineGetting_Flag == 0)
		{
			if(wifi_RxData == '\r'	 ||  wifi_RxData == '\n')
			{
				if(w > 0)
				{ 
					Wifi_RXData[w] = '\0'; 
					w = 0; 
					wifi_Flag = 1;
					state = 0;
				}
			}
			else
			{
				Wifi_RXData[w] = wifi_RxData;
				w++;
				if(w >= 99) 
				{
					w=0;
					state = 0;
				}
			}
		}

		if(state == 0 && wifi_OnlineGetting_Flag == 1 )
		{		
			if(wifi_RxData == '{')
			{
				brace_count++;
				if(brace_count >=3)
				{
					state = 1;
					w_Online = 0;
					brace_count = 0;
				}
			}
		}
		else if(state == 1 && wifi_OnlineGetting_Flag == 1)
		{
			
			if(wifi_RxData == '}')
			{
				brace_count++;
				if(brace_count >= 2)
				{
					Wifi_RXData_Online[w_Online] = '\0'; 
					w_Online = 0; 
					state = 0;
					brace_count = 0;
					wifi_online_flag = 1;
					wifi_OnlineGetting_Flag = 0;
				}
			}
			else 
			{
				Wifi_RXData_Online[w_Online] = wifi_RxData;
				w_Online++;
				if(w_Online >= 511) 
				{
					w_Online=0;
					state = 0;
				}
			}
		}
	
	
		
    }
}
