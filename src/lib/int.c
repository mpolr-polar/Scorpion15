/*
 * int.c
 *
 *  Created on: 2014/06/11
 *      Author: èsïΩ
 */

#include "int.h"

CMPS_InputTypeDef	cmps={0,0,0,0,0,0};
ACC_InputTypeDef	acc={{0,0,0,0},{0,0,0}};
GYRO_InputTypeDef	gyro={{0,0,0,0},{0,0,0}};

void InitModule(uint8_t param)
{
	uint8_t i=0;
	if(param&0b001)
	{
		Accel_Init();
		Accel_SetOffset(0,0,0);
		for(i=0;i<3;i++){acc.cur[i]=0;}
	}
	if(param&0b010)
	{
		Gyro_Init();
		for(i=0;i<3;i++){gyro.cur[i]=0;}
	}
	if(param&0b100)
	{
		Cmps_Init();
		cmps.dir_3600 	= 0;
		cmps.dir_16		= 0;
		cmps.dir_8		= 0;
		cmps.dir_4		= 0;
	}
	scorpion.enabled_modules = param;
}

void GetModuleData(uint8_t param)
{
	uint8_t i=0;
	if(param&0b001)
	{
		for(i=0; i<3; i++)
		{
			acc.log[i] = acc.cur[i];
			acc.cur[i] = LowPass_Filter(Accel_Read(X_DATA_8b+i),acc.log[i]);
		}
	}
	if(param&0b010)
	{
		for(i=0; i<3; i++)
		{
			gyro.log[i] = gyro.cur[i];
			gyro.cur[i] = LowPass_Filter(Gyro_ReadMultiByte(GyroX_DATA+i),gyro.log[i]);
		}
	}
	if(param&0b100)
	{
		cmps.dir_3600 = Cmps_Shift(Cmps_Get(),cmps.shift);
		cmps.dir_360  = cmps.dir_3600/10;
		{//4ï˚à éZèo
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
		{//8ï˚à éZèo
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
		{//16ï˚à éZèo
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
}
