/*
 * touch.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef TOUCH_H_
#define TOUCH_H_

#include "lib/hard.h"

//�\���̂Ɋi�[�������̓f�[�^
typedef enum touch_pos{
	nothing,	//0
	right_touch,//1
	left_touch,	//2
	front_touch	//3
}Tpos;

//�O���[�o���֐��E�ϐ�
extern void Touch_Configuration();	//�^�b�`�Z���T������
extern void Touch_Read();			//�^�b�`�Z���T�̔����󋵎擾


#endif /* TOUCH_H_ */
