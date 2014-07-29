/*
 * int.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef INT_H_
#define INT_H_

//�C���^�[�t�F�C�X(SPI,I2C,UART)
//int�t�H���_���̃w�b�_����system.h���C���N���[�h
#include "system.h"
#include "int/spi.h"
#include "int/i2c.h"
#include "int/uart.h"

typedef enum accgyro{
	x_data,y_data,z_data,g_data
}ag;

typedef struct compass_tag{
	int16_t		shift;
	uint16_t	dir_3600;
	uint16_t	dir_360;
	uint8_t		dir_16;
	uint8_t		dir_8;
	uint8_t		dir_4;
}CMPS_InputTypeDef;

typedef struct accel_tag{
	uint8_t	cur[4];
	uint8_t	log[3];
}ACC_InputTypeDef;

typedef struct gyro_tag{
	uint16_t	cur[4];
	uint16_t	log[3];
}GYRO_InputTypeDef;

typedef enum Module{
	ALL_MODULE	= 0b111,
	ACC			= 0b001,
	GYRO		= 0b010,
	CMPS		= 0b100
}module;

extern void InitModule(uint8_t param);
extern void GetModuleData(uint8_t param);
extern CMPS_InputTypeDef	cmps;
extern ACC_InputTypeDef		acc;
extern GYRO_InputTypeDef	gyro;

#endif /* INT_H_ */
