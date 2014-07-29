/*
 * motor.c
 *
 *  Created on: 2014/07/05
 *      Author: s•½
 */

#include "motor.h"

TIM_OCInitTypeDef	TIM_OCInitStructure;
int8_t right_m=0,left_m=0;

void Motor_Configuration()
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	GPIO_InitTypeDef		GPIO_SwitchStructure;
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

	GPIO_InitStructure.GPIO_Pin		=	GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType	=	GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	=	GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_TIM9);

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_SwitchStructure.GPIO_Pin 	= GPIO_Pin_4;
	GPIO_SwitchStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_SwitchStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_SwitchStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_SwitchStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_SwitchStructure);

	TIM_TimeBaseStructure.TIM_Period			= 200-1;
	TIM_TimeBaseStructure.TIM_Prescaler			= 42-1;
	TIM_TimeBaseStructure.TIM_ClockDivision		= 0;
	TIM_TimeBaseStructure.TIM_CounterMode		= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter	= 0;
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode			= TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity		= TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState		= TIM_OutputState_Enable;

	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCLInit(TIM9,&TIM_OCInitStructure);
	TIM_OCLPreloadConfig(TIM9,TIM_OCPreload_Disable);

	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCRInit(TIM9,&TIM_OCInitStructure);
	TIM_OCRPreloadConfig(TIM9,TIM_OCPreload_Disable);

	TIM_Cmd(TIM9,ENABLE);
	TIM_ARRPreloadConfig(TIM9,ENABLE);

	scorpion.motor_status |= ENABLE;
}

void RotateMotor(int16_t left, int16_t right)
{
	uint8_t flag=0;
	if(left>190)
	{
		left=190;
	}else if(left<-190)
	{
		left=-190;
	}

	if(right>190)
	{
		right=190;
	}else if(right<-190)
	{
		right=-190;
	}

	if((right_m<0 && right>0) || (right_m>0 && right<0)){flag|=0b01;}
	if((left_m <0 && left >0) || (left_m >0 && left <0)){flag|=0b10;}
	Brake(flag);

	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==0){
		GPIO_ResetBits(GPIOE,GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OCLInit(TIM9,&TIM_OCInitStructure);
		TIM_OCLPreloadConfig(TIM9,TIM_OCPreload_Disable);

		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OCRInit(TIM9,&TIM_OCInitStructure);
		TIM_OCRPreloadConfig(TIM9,TIM_OCPreload_Disable);
	}
	if(left==0){
		GPIO_ResetBits(GPIOE,GPIO_Pin_9|GPIO_Pin_10);
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OCLInit(TIM9,&TIM_OCInitStructure);
		TIM_OCLPreloadConfig(TIM9,TIM_OCPreload_Disable);
	}else{
		if(left<0){
			GPIO_ResetBits(GPIOE,GPIO_Pin_9);
			GPIO_SetBits(GPIOE,GPIO_Pin_10);
			TIM_OCInitStructure.TIM_Pulse = -left;
			TIM_OCLInit(TIM9,&TIM_OCInitStructure);
			TIM_OCLPreloadConfig(TIM9,TIM_OCPreload_Disable);
		}else{
			GPIO_SetBits(GPIOE,GPIO_Pin_9);
			GPIO_ResetBits(GPIOE,GPIO_Pin_10);
			TIM_OCInitStructure.TIM_Pulse = left;
			TIM_OCLInit(TIM9,&TIM_OCInitStructure);
			TIM_OCLPreloadConfig(TIM9,TIM_OCPreload_Disable);
		}
	}
	if(right==0){
		GPIO_ResetBits(GPIOE,GPIO_Pin_7|GPIO_Pin_8);
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OCRInit(TIM9,&TIM_OCInitStructure);
		TIM_OCRPreloadConfig(TIM9,TIM_OCPreload_Disable);
	}else{
		if(right<0){
				GPIO_ResetBits(GPIOE,GPIO_Pin_7);
				GPIO_SetBits(GPIOE,GPIO_Pin_8);
				TIM_OCInitStructure.TIM_Pulse = -right;
				TIM_OCRInit(TIM9,&TIM_OCInitStructure);
				TIM_OCRPreloadConfig(TIM9,TIM_OCPreload_Disable);
		}else{
			GPIO_SetBits(GPIOE,GPIO_Pin_7);
			GPIO_ResetBits(GPIOE,GPIO_Pin_8);
			TIM_OCInitStructure.TIM_Pulse = right;
			TIM_OCRInit(TIM9,&TIM_OCInitStructure);
			TIM_OCRPreloadConfig(TIM9,TIM_OCPreload_Disable);
		}
	}
	left_m = left; right_m = right;
	motor.right = right;
	motor.left  = left;
	motor.Rduty = (motor.left-motor.right)/2;
	motor.Mduty = motor.left-motor.Rduty;
	if(motor.Rduty==0 && motor.Mduty==0)
	{
		scorpion.motor_status |= STOP;
	}else
	{
		if(motor.left>0 || motor.right>0)
		{
			scorpion.motor_status |= RUN;
		}else
		{
			scorpion.motor_status |= BACK;
		}

		if(motor.left >= motor.right*2)
		{
			scorpion.motor_status |= TURN_RIGHT;
		}else if(motor.left*2 <= motor.right)
		{
			scorpion.motor_status |= TURN_LEFT;
		}else
		{
			scorpion.motor_status |= STRAIGHT;
		}
	}
}

void Move(int16_t Rduty, int16_t Mduty)
{
	RotateMotor(Mduty+Rduty, Mduty-Rduty);
}

void Brake(uint8_t motor)
{
	if(motor!=0)
	{
		if(motor&0b01)
		{
			GPIO_SetBits(GPIOE,GPIO_Pin_7|GPIO_Pin_8);
			TIM_OCInitStructure.TIM_Pulse = 0;
			TIM_OCRInit(TIM9,&TIM_OCInitStructure);
			TIM_OCRPreloadConfig(TIM9,TIM_OCPreload_Disable);
		}
		if(motor&0b10)
		{
			GPIO_SetBits(GPIOE,GPIO_Pin_9|GPIO_Pin_10);
			TIM_OCInitStructure.TIM_Pulse = 0;
			TIM_OCLInit(TIM9,&TIM_OCInitStructure);
			TIM_OCLPreloadConfig(TIM9,TIM_OCPreload_Disable);
		}
		delay_ms(2);
		RotateMotor(0,0);
	}
}
