/*
 * i2c.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef I2C_H_
#define I2C_H_

#include "lib/int.h"

//�O���[�o���֐��E�ϐ�(�S���W���[������A�N�Z�X�\)

extern void		I2C_Configuration();					//I2C�������֐�
extern void		Cmps_Init();							//�R���p�X���W���[���̏�����
extern uint16_t	Cmps_Read();							//�R���p�X���W���[��������ʂ��擾
extern int16_t	Cmps_Shift(uint16_t data,int16_t shift);	//�R���p�X���W���[���̒l�����炷
extern void		Cmps_Reset(uint16_t dir_3600);			//�������Ă���������w�肵���l�ɂȂ�悤��shift�l��ݒ肷��

typedef enum Direction{north, east, south, west}direction;

//Compass : HMC6352

#endif /* I2C_H_ */
