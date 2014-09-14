/*
 * linecolor.c
 *
 *  Created on: 2014/06/14
 *      Author: 峻平
 */

#include "linecolor.h"



GPIO_InitTypeDef GPIO_InitStructure;
int8_t LineValue[7] =
{
	6,4,2,0,-2,-4,-6
};

void Line_Configuration()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_DOWN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void Line_Read()
{
	uint16_t i=0,j=0;
	uint8_t input[CC_VALUE];
	uint8_t flag=0;

	//line構造体(log)に値を代入する
	line.bin_log = line.bin_cur;
	line.cnt_log = line.cnt_cur;
	line.dec_log = line.dec_cur;
	line.num_log = line.num_cur;

	line.bin_cur = 0b1111111;
	line.cnt_cur = 0;
	line.dec_cur = 0;
	line.num_cur = 0;

	for(i=0; i<CC_VALUE; i++)
	{
		for(j=0; j<10; j++)
		{
			input[i] |= Get_Line(All_Line);
			delay_us(10);
		}
		line.bin_cur &= input[i];
	}

	for(j=0; j<7; j++)
	{
		if(line.bin_cur&(1<<j))
		{
			line.dec_cur+=LineValue[j];
			if(flag == 0)
			{
				flag = 1;
				line.num_cur++;
			}
			line.cnt_cur++;
		}else
		{
			if(flag != 0)flag = 0;
		}
	}
	if(line.bin_cur == 0b1111111)
	{
		line.dec_cur = BLACK_OUT;
	}else
	{
		if(line.cnt_cur != 0)
		{
			line.dec_cur /= line.cnt_cur;
		}else
		{
			line.dec_cur = NO_LINE;
		}
	}
}

int8_t  Translate_Line(uint8_t line)
{
	int8_t i=0,total=0,count=0;
	for(i=0; i<7; i++)
	{
		if(line&(1<<i))
		{
			total += LineValue[i];
			count++;
		}
	}
	if(count != 0)
	{
		return total/count;
	}else
	{
		return NO_LINE;
	}
}

void Color_Configuration()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	/* CLK,RANGE,GATEピンの初期化 */
	GPIO_InitStructure.GPIO_Pin		= CLK | RANGE | GATE;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* DOUT1~3ピンの初期化 */
	GPIO_InitStructure.GPIO_Pin		= DOUT1 | DOUT2 | DOUT3;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void Color_Read()
{
	uint8_t i=0,j=0;
	GPIOD->ODR &= ~(CLK|RANGE|GATE);
	delay_us(2000);
	GPIOD->ODR |= GATE;
	delay_ms(10);
	GPIOD->ODR &= ~GATE;
	delay_us(4);
	for(i=0;i<3;i++)
	{
		for(j=0;j<12;j++)
		{
			GPIOD->ODR |= CLK;
			color.cru_cur[0][i] |= ((GPIOD->IDR&DOUT1)>>10)<<j;
			color.cru_cur[1][i] |= ((GPIOD->IDR&DOUT2)>>11)<<j;
			color.cru_cur[2][i] |= ((GPIOD->IDR&DOUT3)>>12)<<j;
			delay_25ns(10);
			GPIOD->ODR &= ~CLK;
			if(j<11){delay_25ns(10);}
		}
		delay_us(3);
	}
}

uint8_t	Translate_Color(uint8_t color)
{
	/* colorが0の時、nothing  右端、左端が反応していればそれぞれright_color,left_color  それ以外はcenter_colorを返す */
	if(color==0){return nothing;}
	else{return ( color&0b001 )?( right_color ):( (color&0b100)?(left_color):(center_color) );}
}
