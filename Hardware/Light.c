#include "Light.h"

uint16_t Data;
uint16_t Data_high;
uint16_t Data_low;
uint16_t Value_light_warning = 1000;
uint16_t warning_Flag_light = 0;
void Light_checkeven(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
    int32_t Time_out = 10000;
    while(I2C_CheckEvent(I2Cx,I2C_EVENT) != 1)
    {
        Time_out--;
        if(Time_out <= 0)
        {   
            break;
        }
    }

}

void Light_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    GPIO_InitTypeDef GPIO_I2C;
    GPIO_I2C.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_I2C.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_I2C.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_I2C);

    I2C_InitTypeDef I2C2_A;
    I2C2_A.I2C_Ack = I2C_Ack_Enable;
    I2C2_A.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C2_A.I2C_ClockSpeed = 100000;
    I2C2_A.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C2_A.I2C_Mode = I2C_Mode_I2C;
    I2C2_A.I2C_OwnAddress1 = 0x00;
    I2C_Init(I2C2, &I2C2_A);
    I2C_Cmd(I2C2, ENABLE);
}

void Light_write(uint8_t Data)
{
    I2C_GenerateSTART(I2C2,ENABLE);
    Light_checkeven(I2C2,I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2,0x23 << 1,I2C_Direction_Transmitter);
    Light_checkeven(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_SendData(I2C2,Data);
    Light_checkeven(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_GenerateSTOP(I2C2,ENABLE);

}

uint16_t Light_Read(void)
{
    I2C_GenerateSTART(I2C2,ENABLE);
    Light_checkeven(I2C2,I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2,0x23 << 1,I2C_Direction_Transmitter);
    Light_checkeven(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_GenerateSTART(I2C2,ENABLE);
    Light_checkeven(I2C2,I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2,0x23 << 1,I2C_Direction_Receiver);
    Light_checkeven(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
    I2C_AcknowledgeConfig(I2C2,ENABLE);
    Light_checkeven(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);
    Data_high = I2C_ReceiveData(I2C2)<<8;
    I2C_AcknowledgeConfig(I2C2,DISABLE);
    I2C_GenerateSTOP(I2C2,ENABLE);
    Light_checkeven(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);
    Data_low = I2C_ReceiveData(I2C2);
    I2C_AcknowledgeConfig(I2C2,ENABLE);
    
    return Data_high | Data_low;
}
void Light_Sensor_Read_Init(void)
{
    Light_write(0x01);  // 通电
    Delay_ms(100);  // 等待传感器稳定
    Light_write(0x10);  // 连续高分辨率模式
}
void Light_Sensor_Read(void)
{
    Delay_ms(120);  // 等待测量完成
    Data = Light_Read();
}
















uint8_t Light_Ack(void)
{
    uint32_t timeout;

    // 发送 START
    I2C_GenerateSTART(I2C2, ENABLE);
    timeout = 10000;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(--timeout == 0) return 0;
    }

    // 发送设备地址（写）
    I2C_Send7bitAddress(I2C2, 0x23 << 1, I2C_Direction_Transmitter);
    timeout = 10000;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(I2C_GetFlagStatus(I2C2, I2C_FLAG_AF) == SET)  // 无应答
        {
            I2C_ClearFlag(I2C2, I2C_FLAG_AF);
            I2C_GenerateSTOP(I2C2, ENABLE);
            return 0;  // 设备没应答
        }
        if(--timeout == 0) return 0;
    }

    // 收到 ACK，设备正常
    I2C_GenerateSTOP(I2C2, ENABLE);
    return 1;

}