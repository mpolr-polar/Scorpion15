/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main
 **
 **  Environment : Eclipse with Atollic TrueSTUDIO(R) Engine
 **                STMicroelectronics STM32F4xx Standard Peripherals Library
 **
 **
 **
 *****************************************************************************
 */

/* ver 0.0.0 */

/* Includes */
#include "lib/system.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */
int main(void)
{
	System_Configuration(linetrace);
	while(1){(*MainProcess[0])();}
/*	System_Configuration(yawpitchroll);
	while(1){
		GetYPRData();
		USART_PutDec2(scorpion.yaw[0]);Indention;
		USART_PutDec2(gyro.cur[0]);Indention;
		USART_PutDec2(gyro.cur[0]*GyroDPSRate);Indention;
		USART_PutDec2((int16_t)((gyro.cur[0]*GyroDPSRate)*gyro.time/2000000));Indention;
		USART_PutDec(gyro.time/2000000);Linefeed;
	}*/
/*	System_Configuration(linetrace);
	while(1){
		GetSensorData(LINE);
		if(line.dec_cur == 0){Move(0,120);}
		else{Move(60*line.dec_cur/ABS(line.dec_cur),60);}
	}*/
/*	System_Configuration(selfoption);
	InitSensor(LINE|COLOR);
	Motor_Configuration();
	static int8_t aflag=0;
	GetSensorData(LINE|COLOR);
	if(aflag!=0)
	{
		if(aflag>0)
		{
			line.bin_cur &= 0b0000111;
		}else
		{
			line.bin_cur &= 0b1110000;
		}
		line.dec_cur = Translate_Line(line.bin_cur);
	}else if(line.dec_cur!=BLACK_OUT && line.dec_cur!=NO_LINE)
	{
		if(line.dec_cur > 0)
		{
			line.bin_cur &= 0b0001111;
		}else if(line.dec_cur < 0)
		{
			line.bin_cur &= 0b1111000;
		}
		line.dec_cur = Translate_Line(line.bin_cur);
	}

	//error:ラインから外れた時にmotor.Rdutyに補正を加える
	static uint8_t error=0;
	static int8_t  turn=0;
	switch(line.dec_cur)
	{
	case BLACK_OUT:
		Move(40*turn,40);
		delay_ms(100);
		break;

	case NO_LINE:	//線が途切れた時
		if((line.dec_log==NO_LINE || !(line.bin_log&0b1100011)) && turn==0)
		{
			Move(0,120);
		}else
		{
			error++;
			if(line.bin_log & 0b1100000)
			{
				motor.Rduty += (-40-error);
				line.bin_cur = 0b1000000;
				line.dec_cur = -6;
			}else if(line.bin_log & 0b0000011)
			{
				motor.Rduty += (40+error);
				line.bin_cur = 0b0000001;
				line.dec_cur = 6;
			}
			motor.Mduty = 0;
			Move(motor.Rduty,motor.Mduty);
		}
		break;

	default:		//線が外れていない時(通常のライントレース)
		if(error){error = 0;}
		if(line.num_cur < 2)
		{//ラインが1本
			if(line.dec_cur == 0)
			{
				turn = 0;
				Move(0,120);
			}
			else
			{
				if(line.bin_cur&0b0001000)
				{
					Move(line.dec_cur*ABS(line.dec_cur)*20,100);
					turn = 0;
				}else
				{
					motor.Rduty += line.dec_cur*3;
					motor.Mduty = 100;
					Move(motor.Rduty,motor.Mduty);
					turn = line.dec_cur/ABS(line.dec_cur);
				}
			}
		}else
		{//ラインが2本以上
			if(aflag==0)
			{
				if((aflag=line.dec_cur/ABS(line.dec_cur))>0)
				{
					line.bin_cur &= 0b0000111;
				}else
				{
					line.bin_cur &= 0b1110000;
				}
				line.dec_cur = Translate_Line(line.bin_cur);
			}
			if(line.dec_cur==NO_LINE)
			{
				aflag=0;
			}else
			{
				turn = line.dec_cur/ABS(line.dec_cur);
				motor.Rduty += line.dec_cur*ABS(line.dec_cur);
				motor.Mduty = 80-ABS(line.dec_cur)*12;
				Move(motor.Rduty,motor.Mduty);
			}
		}
	}*/
/*	//被災者発見
	uint8_t x,y;						//コートのサイズ
	uint8_t flag=0,i=0;	//缶があると思われる角度
	System_Configuration(selfoption);
	Motor_Configuration();
	InitSensor(TOUCH|PING);
	InitModule(ACC|GYRO);
	{//フロアの寸法を測る
		GetSensorData(PING);
		x = ping.centi_cur[right_ping]+10;//((ping.cur[left_ping]<ping.cur[right_ping])?(ping.centi_cur[right_ping]):(ping.centi_cur[left_ping]))+9;
		y = ping.centi_cur[front_ping]+20;
	}

	if(y > (120-20))
	{//缶が正面にない
		motor.Rduty = 30;//(ping.cur[left_ping] < ping.cur[right_ping])?(30):(-30);
		Move(motor.Rduty,0);
		while(scorpion.yaw[0] <= 90)
		{//その場回転で被災者の方角を特定
			GetSensorData(PING);
			GetYPRData();
			if(cos(RADIAN(scorpion.yaw[0]))*(ping.centi_cur[front_ping]+20)<(x-20) && sin(RADIAN(scorpion.yaw[0]))*(ping.centi_cur[front_ping]+20)<(y-20))
			{
				if(flag==0)
				{
					flag = 1;
					start_count(&TIM_TimeBaseStructureMili);
				}
			}else
			{
				if(flag==1)
				{
					stop_count();
					break;
				}
			}
		}
		Brake(both_motors);
		delay_ms(1000);
		Move((deg1<deg2)?(-30):(30),0);
		delay_ms(timer_count);
	}
	Brake(both_motors);
	while(Get_Touch()==nothing)
	{
		GetYPRData();
		Move(0,100);
	}
	Brake(both_motors);
	while(1)
	{
		for(i=0;i<4;i++)
		{
			Light_OBLED(0b0001<<i);
			delay_ms(500);
		}
	}*/
	return 0;
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
	/* TODO, implement your code here */
	return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
	/* TODO, implement your code here */
	return -1;
}

