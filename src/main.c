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
/*	System_Configuration(active_all);
	while(scorpion.mode != 5)
	{
		SetUp();
		(*MainProcess[socprion.mode])(void);
		CleanUp();
	}
*/
	/*　モジュールテストプログラム */
	System_Configuration(linetrace);
	while(1)
	{
		(*ControlProcess[5])();
		(*MainProcess[0])();
		if(scorpion.mode > 5){break;}
	}
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

