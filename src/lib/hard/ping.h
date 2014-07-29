/*
 * ping.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef PING_H_
#define PING_H_

#include "lib/hard.h"

//構造体内の配列の要素
typedef enum PING_pos{
	front_ping,	//0
	right_ping,	//1
	back_ping,	//2
	left_ping	//3
}Ppos;

//グローバル関数・変数
extern void PING_Configuration();				//PING初期化
extern void PING_Read();						//PING全読み(非最適化)
extern void	PING_Read2(const uint8_t ping_num);	//指定されたPINGだけを読む
extern void PING_Read_ast();					//PING全読み(最適化)   ←推奨

#endif /* PING_H_ */
