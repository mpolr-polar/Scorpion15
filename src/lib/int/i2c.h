/*
 * i2c.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef I2C_H_
#define I2C_H_

#include "lib/int.h"

//グローバル関数・変数(全モジュールからアクセス可能)
extern void 	I2C_Configuration();					//I2C初期化関数
extern void 	I2C_Start();							//通信開始
extern void 	I2C_Stop();								//通信終了
extern int8_t	I2C_WriteByte(uint8_t dat);				//1バイト送信
extern uint8_t	I2C_ReadByte(char noack);				//1バイト受信
extern void		Cmps_Init();							//コンパスモジュールの初期化
extern int16_t	Cmps_Get();								//コンパスモジュールから方位を取得
extern int16_t	Cmps_Shift(int16_t data,int16_t shift);	//コンパスモジュールの値をずらす
extern void		Cmps_Reset(uint16_t dir_3600);			//今向いている方向を指定した値になるようにshift値を設定する

typedef enum Direction{north, east, south, west}direction;

#endif /* I2C_H_ */
