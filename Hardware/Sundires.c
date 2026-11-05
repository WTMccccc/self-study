#include "Sundires.h"

void Limit(uint8_t *x, uint16_t a, uint16_t b) //限制x在a-b之间
{
    *x= (*x>b)?b:((*x<a)?a:*x);
}

uint8_t Once_ues(uint8_t *x, uint8_t *y)     //return x，y为暂存，调用后x被清零
{
    uint8_t temp = *x;
    *y = *x;
    *x = 0;
    return temp;
}

void Data_clear(int8_t *x, uint8_t *y)    //x为清零，y存储x清零前的数值
{
    uint8_t temp = *x;
    *y = temp;
    *x = 0;
}

void Buzzer_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitTypeDef Buzzer;
	Buzzer.GPIO_Mode = GPIO_Mode_Out_PP;
	Buzzer.GPIO_Pin = GPIO_Pin_3;
	Buzzer.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &Buzzer);
}

void Buzzer_ON(void)
{
    GPIO_ResetBits(GPIOB,GPIO_Pin_3);
}

void Buzzer_OFF(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_3);
}

void Buzzer_Turn(void)
{
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0)
    {
        Buzzer_OFF();
    }
    else
    {
        Buzzer_ON();
    }
}

float Combination_Pack(uint16_t *RXPack_Numint ,uint16_t *RXPack_Numdeci)
{
    uint8_t i,j;
    uint16_t Com_Num_int = 0;
    float Com_Num_deci = 0;
    for(i=0;i<CNT2;i++)
    {
        Com_Num_int = Com_Num_int * 10 + RXPack_Numint[i];
    }
    for(j=0;j<CNT3;j++)
    {
      Com_Num_deci =( Com_Num_deci + RXPack_Numdeci[CNT3-j-1] ) *0.1;
    }
    return Com_Num_int + Com_Num_deci;
}

