/*
 * int.c
 *
 *  Created on: 2014/06/11
 *      Author: èsïΩ
 */

#include "int.h"

CMPS_InputTypeDef	cmps={0,0,0,0,0,0};
ACC_InputTypeDef	acc={{0,0,0,0},{0,0,0}};
GYRO_InputTypeDef	gyro={{0,0,0},{0,0,0},0};
uint32_t gtime = 0;

void InitModule(uint8_t param)
{
	uint8_t i=0;
	if(param&(ACC|GYRO))
	{
		SPI_Configuration();
		if(param&ACC)
		{
			Accel_Init();
			Accel_Read(Z_DATA_8b);
			acc.cur[g_data] = -acc.cur[z_data];
			Accel_SetOffset(-27,3,-197);
			for(i=0;i<3;i++){acc.cur[i]=0;}
			GetModuleData(ACC);
			setQangle(&pitch,DEGREES(atan2(acc.cur[y_data],acc.cur[z_data])));
			setQangle(&roll,DEGREES(atan2(acc.cur[x_data],acc.cur[z_data])));
		}
		if(param&GYRO)
		{
			Gyro_Init();
			for(i=0;i<3;i++){gyro.cur[i]=0;}
		}
	}
	if(param&CMPS)
	{
		I2C_Configuration();
		Cmps_Init();
		GetModuleData(CMPS);
		Cmps_Reset(0);
	}
	scorpion.enabled_modules = param;
}

void GetModuleData(uint8_t param)
{
	uint8_t i=0;
	if(param&ACC)
	{
		for(i=0; i<3; i++)
		{
			acc.log[i] = acc.cur[i];
		}
		acc.cur[x_data] = acc.offset[x_data]+LowPass_Filter((Accel_Read(X_DATA_8b)*( 1)),acc.log[x_data]);
		acc.cur[y_data] = acc.offset[y_data]+LowPass_Filter((Accel_Read(Y_DATA_8b)*(-1)),acc.log[y_data]);
		acc.cur[z_data] = LowPass_Filter((Accel_Read(Z_DATA_8b)*( 1)),acc.log[z_data]);
	}
	if(param&GYRO)
	{
		TIM_Cmd(TIM3,DISABLE);
		gyro.time = TIM3->CNT;
		TIM3->CNT = 0;
		for(i=0; i<3; i++){gyro.log[i] = gyro.cur[i];}
		gyro.cur[yaw_data] 	 = Gyro_ReadMultiByte(GyroZ_DATA)*(-1);
		gyro.cur[pitch_data] = Gyro_ReadMultiByte(GyroY_DATA)*(-1);
		gyro.cur[roll_data]  = Gyro_ReadMultiByte(GyroX_DATA)*( 1);
		TIM_Cmd(TIM3,ENABLE);
//		for(i=0; i<3; i++){gyro.cur[i]*=10;}
	}
	if(param&CMPS)
	{
		cmps.crude = Cmps_Read();
		cmps.dir_3600 = Cmps_Shift(cmps.crude,cmps.shift);
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
