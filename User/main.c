#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "LED.h"
#include "Tem_Sensor.h"
#include "Key.h"
#include "Time.h"
#include "Sundires.h"
#include "string.h"
#include "Light.h"
#include "wifi.h"

char* p;

int main(void)
{	
	
	Key_Init();                    
	Time_Init();                   
	TIM3_Init();                   
	LED_Init();                    
	OLED_Init();
	OLED_Menu_Init();
	Tem_Init();
	Serial_Inint();
	Buzzer_Init();
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
	Light_Init();
	Light_Sensor_Read_Init();
	WiFI_Init();
	
	while(1)
	{	
	
		Tem_Inf(&Tem_And_Humi);
		/***************************菜单显示 */
		OLED_ShowMenu();
		Key_OKButton();
		if(current_Menu->Menu_Page == 1)
		{
			OLED_ShowString(1,1,"Tem:");
			OLED_ShowDeciNum(1,5,Tem_And_Humi.temp_int+Tem_And_Humi.temp_deci*0.1,2,1);    		
			OLED_ShowString(2,1,"Hum:");
			OLED_ShowDeciNum(2,5,Tem_And_Humi.humi_int+Tem_And_Humi.humi_deci*0.1,2,1);
			OLED_ShowString(3,1,"Light:");
			OLED_ShowNum(3,7,Data,5);
			if(i % 5 == 0)                                  //每1s读取一次光照强度
			{
				Light_Sensor_Read();
			}
		}
	
		//***************************菜单显示 */


		
		if(i >= 25)
		{
			if(Voice_control == 1)
			{
				if(warning_Flag_tem == 1)
				{
					
					printf("warningtem\r\n");
					printf("VoiceONbbb\r\n");

				}		
				if(warning_Flag_humi == 1)
				{
					printf("warninghumi\r\n");
					printf("VoiceONbbb\r\n");

				}
				if(warning_Flag_light == 1)
				{
					printf("warninglight\r\n");
					printf("VoiceONbbb\r\n");
				}
			}
			else if(Voice_control == 0)
			{
				if(warning_Flag_tem == 1)
				{
					printf("warningtem\r\n");
				}		
				if(warning_Flag_humi == 1)
				{
					printf("warninghumi\r\n");
				}
				if(warning_Flag_light == 1)
				{
					printf("warninglight\r\n");
				}
			}

			i = 0;

		}

		

		if(RXFlag == 1)
		{	
			if(strcasecmp(RXPack,"data") == 0)
			{
			
				printf("Temeperture:%.1f\r\n",Tem_And_Humi.temp_int+Tem_And_Humi.temp_deci*0.1);
				printf("Humidity:%.1f\r\n",Tem_And_Humi.humi_int+Tem_And_Humi.humi_deci*0.1);
				printf("Light:%d\r\n",Data);
			}
			else if(strcasecmp(RXPack,"warning") == 0)
			{
				printf("Temperture Warning Value:%.1f\r\n",Value_temp_warning);
				printf("Humidity Warning Value:%.1f\r\n",Value_humi_warning);
				printf("Light Warning Value:%d\r\n",Value_light_warning);
			}
			else if(strcasecmp(RXPack,"settem") == 0)
			{
				Value_temp_warning = Combination_Pack(RXPack_Numint,RXPack_Numdeci);  //将整数部分和小数部分组合成一个浮点数
				printf("Set Warning Temperature:%.1f\r\n",Value_temp_warning);
			}
			else if(strcasecmp(RXPack,"sethumi") == 0)
			{
				Value_humi_warning = Combination_Pack(RXPack_Numint,RXPack_Numdeci);
				printf("Set Warning Humidity:%.1f\r\n",Value_humi_warning);
			}
			else if(strcasecmp(RXPack,"setlight") == 0)
			{
				Value_light_warning = Combination_Pack(RXPack_Numint,RXPack_Numdeci);
				printf("Set Warning Light:%d\r\n",Value_light_warning);
			}
			else if(strcasecmp(RXPack,"voice") == 0)
			{
				Voice_control = (Voice_control == 0)?1:0;  //语音控制标志位翻转
				if(Voice_control == 1)
				{
					printf("VoiceON");
					wifi_GetOnlineTemAndHumi();
				}
				else
				{
					printf("VoiceOFF");
					printf("json:%s\r\n",Wifi_RXData_Online);
					printf("Online Temperature:%.1f\r\n",wifi_Online_Tem);
					printf("Online Humidity:%.1f\r\n",wifi_Online_Humi);
					printf("Online Time:%s\r\n",Current_Online_Time);
				}
			}
			else if(strcasecmp(RXPack,"wifi_provide") == 0)
			{
				wifi_state = 2;  
			}
			else if(strcasecmp(RXPack,"wifi_connect") == 0)
			{
				wifi_state = 1;
			}
			else if(strcasecmp(RXPack,"getonline") == 0)
			{
				wifi_GetOnlineTemAndHumi();
				printf("please wait...\r\n");
			}
			RXFlag = 0;
		}
		
		if(wifi_Flag == 1)
		{
			printf("WIFI: %s\r\n", Wifi_RXData);
			wifi_Flag = 0;
		}
		if(wifi_online_flag == 1)
		{
			p = strstr(Wifi_RXData_Online, "temperature_2m");
			if(p != NULL)
			{
				wifi_Online_Tem = atof(p + 16);  
			}

			p = strstr(Wifi_RXData_Online, "relative_humidity_2m");
			if(p != NULL)
			{
				wifi_Online_Humi = atof(p + 22); 
			}

			p = strstr(Wifi_RXData_Online, "time");
			if(p != NULL)
			{
				strncpy(Current_Online_Time, p + 7, 16);
				Current_Online_Time[16] = '\0';
			}
			
			printf("json:%s\r\n",Wifi_RXData_Online);
			printf("Online Temperature:%.1f\r\n",wifi_Online_Tem);
			printf("Online Humidity:%.1f\r\n",wifi_Online_Humi);
			printf("Online Time:%s\r\n",Current_Online_Time);
			wifi_online_flag = 0;
		
		}


		if(wifi_state == 1)
		{
			wifi_connect();
			
		}
		else if(wifi_state == 2)
		{
			wifi_Provide_text();
		}

	}

	
}
