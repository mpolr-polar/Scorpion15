/*
 * i2c.c
 *
 *  Created on: 2014/06/07
 *      Author: 峻平
 */

#include "i2c.h"

//初期化用構造体
GPIO_InitTypeDef GPIO_InitStructure;
I2C_InitTypeDef		I2C_InitStructure;
int addresssend[3];
int busycheckflag[3]={1,1,1};
int transmitter[3]={0,0,0};

void I2C_Configuration(){
	/* クロックの供給 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,  ENABLE);

	/* GPIOBの初期化(i2c) */
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

	I2C_InitStructure.I2C_Mode					= I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle				= I2C_DutyCycle_16_9;
	I2C_InitStructure.I2C_Ack					= I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress	= I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed			= 100000;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
}

void	Cmps_Init()
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	delay_ms(10);
	I2C_Send7bitAddress(I2C1, 0x42, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, 'G');
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, 0x74);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, 0x70);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C1, ENABLE);

/*	//ポーリング処理
	__IO uint16_t SR1_Tmp= 0;
	__IO uint32_t polling_count = 0;
	do{
		I2C_GenerateSTART(I2C1, ENABLE);
		SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
		I2C_Send7bitAddress(I2C1, 0x42, I2C_Direction_Transmitter);
		polling_count++;
	}while(!I2C_ReadRegister(I2C1, I2C_Register_SR1 & 0x0002));
	I2C_ClearFlag(I2C1, I2C_FLAG_AF);
	I2C_GenerateSTOP(I2C1, ENABLE);*/
}

uint16_t Cmps_Read()
{
	uint16_t data;
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, 0x42, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, (uint8_t)('A'));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, 0x42, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	data = I2C_ReceiveData(I2C1)<<8;
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	data += I2C_ReceiveData(I2C1);
	return data;
}

int16_t	Cmps_Shift(uint16_t data,int16_t shift)
{
	if(shift < 0)
	{
		if(data+shift < 0)
		{
			return data+shift+3600;
		}else
		{
			return data+shift;
		}
	}else
	{
		if(data+shift > 3599)
		{
			return data+shift-3600;
		}else
		{
			return data+shift;
		}
	}
}

void Cmps_Reset(uint16_t dir_3600)
{
	uint8_t i=0;
	cmps.shift += dir_3600-cmps.dir_3600;
	cmps.dir_3600 = dir_3600;
	cmps.dir_360  = cmps.dir_3600/10;
	{//4方位算出
		if(cmps.dir_3600 >= 3150 || cmps.dir_3600 < 450)
		{
			cmps.dir_4 = north;
		}else
		{
			for(i=0; i<=2; i++)
			{
				if(cmps.dir_3600 >= 450+900+i && cmps.dir_3600 < 1350+900*i)
				{
					cmps.dir_4 = east+i;
				}
			}
		}
	}
	{//8方位算出
		if(cmps.dir_3600 >= 3375 || cmps.dir_3600 < 225)
		{
			cmps.dir_8 = 0;
		}else
		{
			for(i=0; i<=6; i++)
			{
				if(cmps.dir_3600 >= 225+450*i && cmps.dir_3600 < 675+450*i)
				{
					cmps.dir_8 = i+1;
					break;
				}
			}
		}
	}
	{//16方位算出
		if(cmps.dir_3600 >= 3487 || cmps.dir_3600 < 112)
		{
			cmps.dir_16 = 0;
		}else
		{
			for(i=0; i<=14; i++)
			{
				if(cmps.dir_3600 >= 112+225*i && cmps.dir_8 < 337+225*i)
				{
					cmps.dir_16 = i+1;
					break;
				}
			}
		}
	}
}
