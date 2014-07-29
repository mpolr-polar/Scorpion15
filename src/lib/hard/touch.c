/*
 * touch.c
 *
 *  Created on: 2014/06/25
 *      Author: �s��
 */

#include "touch.h"

GPIO_InitTypeDef GPIO_InitStructure;

void Touch_Configuration()
{
	//GPIOB�ɃN���b�N����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//GPIOB�̃^�b�`�Z���T���͕�������
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Touch_Read()
{
	touch.log = touch.cur;
	touch.cur =(GPIOB->IDR&0x1800)>>11;	//���݂�PB11/12�̓��͂�2bit�̃f�[�^�ɕϊ����Ď擾
}
