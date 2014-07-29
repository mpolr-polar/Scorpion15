/********************************************************************************/
/*--
	@file			SubBoard_ping.c
	@author			Radiandat
	@version		1.00a
	@date			2013/09/14
	@brief			There are the functions to read current distance by PING.

	@section HISTORY
		2013/09/14	1.00a	start here.
--*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include	"ping.h"

/* Variables -----------------------------------------------------------------*/
GPIO_InitTypeDef	GPIO_InitStructure;

static	uint16_t	time[4];				//timer buffer
static  uint8_t		flag[4];
static	uint8_t		read_flag;

static uint16_t		ping_val[4];			//current distance
uint8_t				ping_flag;

/* Functions -----------------------------------------------------------------*/
/**
  * @brief	Function to configure PING
  * @param  None
  * @retval None
  **/
void	PING_Configuration(void)
{
	/* Supply the clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* Configure GPIO for PING */
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure Timer */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period					= 60000-1;
    TIM_TimeBaseStructure.TIM_Prescaler					= 40;
    TIM_TimeBaseStructure.TIM_ClockDivision				= 0;
    TIM_TimeBaseStructure.TIM_CounterMode				= TIM_CounterMode_Down;
    TIM_TimeBaseStructure.TIM_RepetitionCounter			= 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_SelectOnePulseMode(TIM4, TIM_OPMode_Single);
}

/**
  * @brief	Function to read the current distance by PING.
  * @param  None
  * @retval None
  **/
void	PING_Read(void)
{
	uint8_t i=0;
	for(i=0;i<4;i++)
	{
		TIM_SetCounter(TIM4,60000);

		/* Send Signal to PING */
		GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_SetBits(GPIOB,GPIO_Pin_2<<i);
		delay_us(4);
		GPIO_ResetBits(GPIOB,GPIO_Pin_2<<i);

		/* Read mode */
		GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2<<i));
		TIM_Cmd(TIM4, ENABLE);
		while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2<<i));
		TIM_Cmd(TIM4, DISABLE);
		if((ping_val[i] = TIM_GetCounter(TIM4)) != 0) ping_val[i] = 60000-ping_val[i];

		ping.log[i] = ping.cur[i];
		ping.cur[i] = ping_val[i];
	}
}

void	PING_Read2(const uint8_t ping_num)
{
	ping_flag = 1;

//	auto uint8_t i = ping_num;
	auto uint8_t j = 0;
	auto uint8_t time;

	switch(ping_num){
	case front_ping: j=0; break;
	case right_ping: j=1; break;
	case left_ping:  j=2; break;
	case back_ping:  j=3; break;
	}

	ping.log[j] = ping.cur[j];

	TIM_SetCounter(TIM4, 60000);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,(uint16_t)ping_num);
	delay_us(4);
	GPIO_ResetBits(GPIOB,(uint16_t)ping_num);

	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	delay_us(30);

	TIM_Cmd(TIM4, ENABLE);
	while(GPIO_ReadInputDataBit(GPIOB, ping_num) == 0 && TIM4->CNT > 55000);
	time = TIM4->CNT;
	while(GPIO_ReadInputDataBit(GPIOB, ping_num) == 1 && TIM4->CNT > 42000);
	TIM_Cmd(TIM4, DISABLE);

	ping.cur[j] = time - TIM4->CNT;
}

/**
  * @brief	Function to read the current distance by PING at the same time.
  * @param  None
  * @retval None
  **/
void	PING_Read_ast(void)
{
	ping_flag = 1;
	uint8_t i;
	for(i=0; i<4; i++)
	{
		ping.log[i] = ping.cur[i];
	}

	/* Configure Variables */
	read_flag = 0;
	TIM_SetCounter(TIM4, 60000);

	/* Control GPIO for Signal */
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->ODR |= 0b111100; delay_us(4); GPIOB->ODR &= ~0b111100;
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	delay_us(30);

	TIM_Cmd(TIM4, ENABLE);
	while(read_flag != 0x0f && TIM4->CNT > 0){
		for(i=0;i<4;i++){
			if((read_flag & (0b001<<i)) == 0){
				if(TIM4->CNT > 54000){
					if((GPIOB->IDR & (0b00100<<i))!=0 && flag[i]==0){ flag[i]=1; time[i]=TIM4->CNT; }
				}else{
					if(flag[i]==0){ read_flag|= (0b001<<i); ping_val[i]=40000; }
				}

				if(TIM4->CNT > 20000){
					if((GPIOB->IDR & (0b00100<<i))==0 && flag[i]==1){ flag[i]=0; ping_val[i]=time[i]-TIM4->CNT; read_flag|=(0b001<<i); }
				}else{
					read_flag|=(0b001<<i); ping_val[i]=40000; flag[i]=0;
				}
			}
		}
	}
	TIM_Cmd(TIM4, DISABLE);
	for(i=0; i<4; i++)
	{
		ping.cur[i] = LowPass_Filter(ping_val[i],ping.log[i]);
	}
}

