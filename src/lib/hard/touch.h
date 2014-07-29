/*
 * touch.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef TOUCH_H_
#define TOUCH_H_

#include "lib/hard.h"

//構造体に格納される入力データ
typedef enum touch_pos{
	nothing,	//0
	right_touch,//1
	left_touch,	//2
	front_touch	//3
}Tpos;

//グローバル関数・変数
extern void Touch_Configuration();	//タッチセンサ初期化
extern void Touch_Read();			//タッチセンサの反応状況取得


#endif /* TOUCH_H_ */
