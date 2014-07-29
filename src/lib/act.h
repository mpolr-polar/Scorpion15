/*
 * act.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef ACT_H_
#define ACT_H_

//アクチュエータ(モータ、尻尾(アーム))
//actフォルダ内のヘッダファイルをインクルード
#include "system.h"
#include "act/motor.h"
#include "act/tail.h"

//#define	CLOCKCMD	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE)
//#define ARMCMD		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE)
//#define	AHRCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE)
//#define MGPIO		GPIOE
//#define AF_TIM		GPIO_AF_TIM9
//#define	TIM			TIM9
//#define PWM1		GPIO_Pin_5
//#define PWM1Source	GPIO_PinSource5
//#define PWM2		GPIO_Pin_6
//#define PWM2Source	GPIO_PinSource6
#define	TIM_OCLInit	TIM_OC2Init							//左モーター
#define	TIM_OCLPreloadConfig	TIM_OC2PreloadConfig	//
#define	TIM_OCRInit	TIM_OC1Init							//
#define	TIM_OCRPreloadConfig	TIM_OC1PreloadConfig	//右モーター

//#define SGPIO		GPIOE
//#define SIG1		GPIO_Pin_7
//#define SIG2		GPIO_Pin_8
//#define SIG3		GPIO_Pin_9
//#define SIG4		GPIO_Pin_10

//#define	SWGPIO		GPIOE
//#define	SWITCH		GPIO_Pin_4


//#define	ARMCMD		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE)
#define	HAND	GPIO_Pin_3
#define	JOINT1	GPIO_Pin_1
#define	JOINT2	GPIO_Pin_2
//#define	ARM_PT	GPIOA
//#define	ARM_SW	GPIO_Pin_4
//#define	ARM_SW_PT	GPIOA
//#define	ARM_T	TIM5
//#define	ARM_AF	GPIO_AF_TIM5
//#define	ARM1_S	GPIO_PinSource0
//#define	ARM2_S	GPIO_PinSource1
//#define	ARM3_S	GPIO_PinSource2
#define	TIM_CInit	TIM_OC4Init						//鋏の部分
#define	TIM_CPreloadConfig	TIM_OC4PreloadConfig	//↑
#define	TIM_MInit	TIM_OC3Init						//真ん中の部分
#define	TIM_MPreloadConfig	TIM_OC3PreloadConfig	//↑
#define	TIM_BInit	TIM_OC2Init						//根元の部分
#define	TIM_BPreloadConfig	TIM_OC2PreloadConfig	//↑
#define BRAKE	191

#define HOUSE	11000,7000,0						//↓
#define SLOPE	13500,4000,0						//↓
#define SPREAD	5100,5700,0							//↓
#define LIFT	10500,16000,0						//arm_motionの引数
#define CLOSE	0,0,5700							//↑
#define OPEN	0,0,3200							//↑

typedef struct MOTOR{
	int16_t right;
	int16_t left;
	int16_t Rduty;
	int16_t Mduty;
}MOTOR_ParamTypeDef;

extern MOTOR_ParamTypeDef motor;

#endif /* ACT_H_ */
