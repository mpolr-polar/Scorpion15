/*
 * spi.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef SPI_H_
#define SPI_H_

#include "lib/int.h"

/* define macros */
#define SPI_SCK_Pin		GPIO_Pin_13
#define SPI_MOSI_Pin	GPIO_Pin_14
#define SPI_MISO_Pin	GPIO_Pin_15
#define SPI_NSS_GYRO	GPIO_Pin_1
#define SPI_NSS_ACC		GPIO_Pin_0

#define X_DATA_8b		0x06
#define Y_DATA_8b		0x07
#define Z_DATA_8b		0x08

#define WHO_AM_I		0x0F
#define CTRL_REG1		0x20
#define CTRL_REG2		0x21
#define CTRL_REG3		0x22
#define CTRL_REG4		0x23
#define CTRL_REG5		0x24
#define REFERENCE		0x25
#define OUT_TEMP		0x26
#define STATUS_REG		0x27
#define OUT_X_L			0x28
#define OUT_X_H			0x29
#define OUT_Y_L			0x2A
#define OUT_Y_H			0x2B
#define OUT_Z_L			0x2C
#define OUT_Z_H			0x2D
#define FIFO_CTRL_REG	0x2E
#define FIFO_SRC_REG	0x2F
#define INT1_CFG		0x30
#define INT_SRC			0x31
#define INT1_TSH_XH		0x32
#define INT1_TSH_XL		0x33
#define INT1_TSH_YH		0x34
#define INT1_TSH_YL		0x35
#define INT1_TSH_ZH		0x36
#define INT1_TSH_ZL		0x37
#define INT1_DURATION	0x38

#define GyroX_DATA		0
#define GyroY_DATA		1
#define GyroZ_DATA		2
#define GyroReadMode	0x80
#define GyroAddIncMode	0x40

//グローバル関数・変数
extern void		SPI_Configuration();
extern uint8_t 	SPI_Send(uint8_t Data);
extern void		SPI_Select(uint16_t SPI_NSS_Pin);
extern void		SPI_Deselect(uint16_t SPI_NSS_Pin);
extern void		Accel_Init();
extern void		Accel_Write(uint8_t add, uint8_t val);
extern int8_t	Accel_Read(uint8_t add);
extern void 	Accel_SetOffset(int16_t xoff, int16_t yoff, int16_t zoff);
extern void		Gyro_Init();
extern void		Gyro_Write(uint8_t add, uint8_t val);
extern uint8_t	Gyro_Read(uint8_t add);
extern int16_t	Gyro_ReadMultiByte(uint8_t add);

#endif /* SPI_H_ */
