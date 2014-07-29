/*
 * i2c.c
 *
 *  Created on: 2014/06/07
 *      Author: 峻平
 */

#include "i2c.h"

//初期化用構造体
GPIO_InitTypeDef GPIO_InitStructure;
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
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	I2C_Cmd(I2C1, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
}

void I2C_Start()
{
	if(busycheckflag[0]){
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	   busycheckflag[0] = 0;
	}
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	addresssend[0] = 1;
}

void I2C_Stop()
{
	if(transmitter[0])I2C_GenerateSTOP(I2C2, ENABLE);
	else I2C_AcknowledgeConfig(I2C2, ENABLE);
	addresssend[1] = 0;
	busycheckflag[1] = 1;
}

int8_t	I2C_WriteByte(uint8_t dat)
{
	//アドレスを送信
	if(addresssend[1]){
		if(dat%2 == 0){
			I2C_Send7bitAddress(I2C1, dat, I2C_Direction_Transmitter);
	        transmitter[1]=1;
	        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	    }else{
	        I2C_Send7bitAddress(I2C1, dat&0xfe, I2C_Direction_Receiver);
	        transmitter[1]=0;
	        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		   }
	    addresssend[1]=0;
	}
	//データを送信
	else{
		I2C_SendData(I2C1, (uint8_t)(dat));
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	return 0;
}

uint8_t	I2C_ReadByte(char noack)
{
	uint8_t data;
	if(noack){
		I2C_AcknowledgeConfig(I2C1, DISABLE);
	    I2C_GenerateSTOP(I2C1, ENABLE);
	}
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)){}
	data = I2C_ReceiveData(I2C1);

	return data;
}

void	Cmps_Init()
{
	delay_ms(300);
	uint8_t flag = 0;
	//CMPS setup
	I2C_Start();
	if(I2C_WriteByte(0x42 + 0) == 0)
	{
		I2C_WriteByte('G');
		I2C_WriteByte(0x74);
		I2C_WriteByte(0x70);
		flag = 1;
	}
	I2C_Stop();
}

int16_t Cmps_Get()
{
	int16_t cmps = 0;
	I2C_Start();
	if(I2C_WriteByte(0x42 + 0) == 0)
	{
		I2C_WriteByte('A');
	}else
	{
		cmps = -1;
	}
	I2C_Stop();
	if(cmps>=0)
	{
		delay_ms(1);
		I2C_Start();
		if(I2C_WriteByte(0x42 + 1) == 0)
		{
			cmps = (I2C_ReadByte(0) << 8);
			cmps += I2C_ReadByte(1);
		}else
		{
			I2C_Stop();
		}
	}
	return cmps;
}

int16_t	Cmps_Shift(int16_t data,int16_t shift)
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
