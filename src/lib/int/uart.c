/*
 * USART.c
 *
 *  Created on: 2014/06/09
 *      Author: èsïΩ
 */

#include "uart.h"

GPIO_InitTypeDef  GPIO_InitUSART;
USART_InitTypeDef USART_InitStructure;

void USART_Configuration()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	GPIO_InitUSART.GPIO_Pin	= GPIO_Pin_10;
	GPIO_InitUSART.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitUSART.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitUSART.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitUSART.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitUSART);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);

	USART_InitStructure.USART_BaudRate				= Baudrate;
	USART_InitStructure.USART_WordLength			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits				= USART_StopBits_1;
	USART_InitStructure.USART_Parity				= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode					= USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	USART_Cmd(USART3,ENABLE);
}

void USART_PutChar(const char c)
{
	/*Wait while there is the data to send*/
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET){}

	/* Output the character data */
	USART_SendData(USART3,c);

	/* Wait while it have not finished sending */
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET){}
}

void USART_PutStr(const char* str)
{
	char c;	//for output string

	/* Output the string data by one character */
	while(1)
	{
		c = *str++;
		if(c == 0)break;
		USART_PutChar(c);
	}
}

void USART_PutDec(uint16_t i)
{
	USART_PutChar(i/10000 + '0');
	i %= 10000;
	USART_PutChar(i/1000 + '0');
	i %= 1000;
	USART_PutChar(i/100 + '0');
	i %= 100;
	USART_PutChar(i/10 + '0');
	i %= 10;
	USART_PutChar(i + '0');
}

void USART_PutDec2(int16_t i)
{
	if(i<0)
	{
		USART_PutChar('-');
		i *= -1;
	}else
	{
		USART_PutChar(' ');
	}
	USART_PutChar(i/10000 + '0');
	i %= 10000;
	USART_PutChar(i/1000 + '0');
	i %= 1000;
	USART_PutChar(i/100 + '0');
	i %= 100;
	USART_PutChar(i/10 + '0');
	i %= 10;
	USART_PutChar(i + '0');
}

void USART_PutBin(const uint8_t bin)
{
	/* Wait while the buffer to send data is not empty */
	USART_PutChar('0');
	USART_PutChar('b');
	uint8_t i;
	for(i=0; i<8; i++)
	{
		if(bin&(0x80>>i)){
			USART_PutChar('1');
		}else{
			USART_PutChar('0');
		}
	}
}

void USART_PutHex(const uint8_t hex)
{
	uint8_t i=0;
	USART_PutChar('0');
	USART_PutChar('x');
	i=(hex>>4);
	if(i<10)
	{
		USART_PutChar(i+'0');
	}else
	{
		USART_PutChar(i+'a'-10);
	}

	i=(hex&0x0f);
	if(i<10)
	{
		USART_PutChar(i+'0');
	}else
	{
		USART_PutChar(i+'a'-10);
	}
}
