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
extern void 	I2C_Configuration();					//I2C�������֐�
extern void 	I2C_Start();							//�ʐM�J�n
extern void 	I2C_Stop();								//�ʐM�I��
extern int8_t	I2C_WriteByte(uint8_t dat);				//1�o�C�g���M
extern uint8_t	I2C_ReadByte(char noack);				//1�o�C�g��M
extern void		Cmps_Init();							//�R���p�X���W���[���̏�����
extern int16_t	Cmps_Get();								//�R���p�X���W���[��������ʂ��擾
extern int16_t	Cmps_Shift(int16_t data,int16_t shift);	//�R���p�X���W���[���̒l�����炷
extern void		Cmps_Reset(uint16_t dir_3600);			//�������Ă���������w�肵���l�ɂȂ�悤��shift�l��ݒ肷��

typedef enum Direction{north, east, south, west}direction;

#endif /* I2C_H_ */
