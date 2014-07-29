/*
 * spi.c
 *
 *  Created on: 2014/06/07
 *      Author: 峻平
 */

#include "spi.h"

//変数
static uint8_t gyro_buf[6];
GPIO_InitTypeDef GPIO_InitStructure;
SPI_InitTypeDef SPI_InitStructure;

void SPI_Configuration()
{
	/* クロックの供給 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/* SCK,MISO,MOSIピンの初期化 */
	GPIO_InitStructure.GPIO_Pin		= SPI_SCK_Pin | SPI_MISO_Pin | SPI_MOSI_Pin;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

	/* NSSピンをHigh(スレーブデバイスを選択しない)にする */
	GPIO_InitStructure.GPIO_Pin		= SPI_NSS_ACC | SPI_NSS_GYRO;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->ODR |= SPI_NSS_ACC | SPI_NSS_GYRO;

	/* SPI2の初期化 */
	SPI_InitStructure.SPI_Direction			= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode				= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize			= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL				= SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA				= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS				= SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
	SPI_InitStructure.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit			= SPI_FirstBit_MSB;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}

uint8_t SPI_Send(uint8_t Data)
{
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);		// 送信バッファが空の間待機
	SPI_I2S_SendData(SPI2,Data);										// データをスレーブに送信
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);	// 受信バッファが空になるまで待機
	return (uint8_t)SPI_I2S_ReceiveData(SPI2);							// スレーブから返ってきたデータを返り値として渡す
}

void SPI_Select(uint16_t SPI_NSS_Pin)
{
	GPIO_ResetBits(GPIOB,SPI_NSS_Pin);
}

void SPI_Deselect(uint16_t SPI_NSS_Pin)
{
	GPIO_SetBits(GPIOB,SPI_NSS_Pin);
}

void Accel_Init()
{
	Accel_Write(0x16, 0b01010101);
	Accel_SetOffset(+20, +82, -357);
}

void Accel_Write(uint8_t add, uint8_t val)
{
	SPI_Select(SPI_NSS_ACC);
	uint8_t RxData;

	RxData = SPI_Send(0b10000000 | (add<<1));
	RxData = SPI_Send(val);

	SPI_Deselect(SPI_NSS_ACC);
}

int8_t Accel_Read(uint8_t add)
{
	SPI_Select(SPI_NSS_ACC);
	uint8_t RxData;

	RxData = SPI_Send(0b00000000 | (add<<1));
	RxData = SPI_Send(0x0);

	SPI_Deselect(SPI_NSS_ACC);
	return RxData;
}

void Accel_SetOffset(int16_t xoff, int16_t yoff, int16_t zoff)
{

	Accel_Write(0x10, xoff);
	Accel_Write(0x11, (xoff & 0b0000001100000000) >> 8 | (xoff & 0b1000000000000000) >> 13);
	Accel_Write(0x12, yoff);
	Accel_Write(0x13, (yoff & 0b0000001100000000) >> 8 | (yoff & 0b1000000000000000) >> 13);
	Accel_Write(0x14, zoff);
	Accel_Write(0x15, (zoff & 0b0000001100000000) >> 8 | (zoff & 0b1000000000000000) >> 13);
}

void Gyro_Init()
{
	while(Gyro_Read(WHO_AM_I) != 0xD4);
	Gyro_Write(CTRL_REG4, 0b00110000);	//500dps
	Gyro_Write(CTRL_REG1, 0x0F);
}

void Gyro_Write(uint8_t add, uint8_t val)
{
	SPI_Select(SPI_NSS_GYRO);
	uint8_t RxData;

	RxData = SPI_Send(add);
	RxData = SPI_Send(val);

	SPI_Deselect(SPI_NSS_GYRO);
}

uint8_t Gyro_Read(uint8_t add)
{
	uint8_t data;
	SPI_Select(SPI_NSS_GYRO);

	data = SPI_Send(add | GyroReadMode);
	data = SPI_Send(0x0);

	SPI_Deselect(SPI_NSS_GYRO);
	return data;
}

int16_t Gyro_ReadMultiByte(uint8_t add)
{
	int16_t data=0;
	uint8_t i=0;
	SPI_Select(SPI_NSS_GYRO);
	data = SPI_Send(OUT_X_L | GyroReadMode | GyroAddIncMode);
	for(i=0; i<6; i++)
	{
		gyro_buf[i] = SPI_Send(0x0);
	}
	SPI_Deselect(SPI_NSS_GYRO);

	data = (add==0)?(gyro_buf[1]<<8 | gyro_buf[0]):
		   (add==1)?(gyro_buf[3]<<8 | gyro_buf[2]):
		   (add==2)?(gyro_buf[5]<<8 | gyro_buf[4]):
				    (0xffff);
	return data;
}
