/*
 * ping.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef PING_H_
#define PING_H_

#include "lib/hard.h"

//�\���̓��̔z��̗v�f
typedef enum PING_pos{
	front_ping,	//0
	right_ping,	//1
	back_ping,	//2
	left_ping	//3
}Ppos;

//�O���[�o���֐��E�ϐ�
extern void PING_Configuration();				//PING������
extern void PING_Read();						//PING�S�ǂ�(��œK��)
extern void	PING_Read2(const uint8_t ping_num);	//�w�肳�ꂽPING������ǂ�
extern void PING_Read_ast();					//PING�S�ǂ�(�œK��)   ������

#endif /* PING_H_ */
