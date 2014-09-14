/*
 * hard.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef HARD_H_
#define HARD_H_

//ハードウェア(ラインセンサボード,タッチセンサ,PING(Ultrasonic Distance Sensor))
//hardフォルダ内のヘッダファイルをインクルード
#include "system.h"
#include "hard/linecolor.h"
#include "hard/touch.h"
#include "hard/ping.h"

//センサ入力用構造体
typedef struct line_tag{
	uint8_t  bin_cur;		//現在のラインセンサの値(2進数,7bit)
	uint8_t  bin_log;		//1ループ前のラインセンサの値(2進数.7bit)
	uint8_t  cnt_cur;		//現在の反応しているラインセンサの数(10進数)
	uint8_t  cnt_log;		//1ループ前の反応したラインセンサの数(10進数)
	int8_t   dec_cur;		//現在のラインセンサの値(10進数,ラインの位置を符号付き整数値で示す)
	int8_t   dec_log;		//1ループ前のラインセンサの値(10進数,ラインの位置を符号付き整数値で示す)
	uint8_t  num_cur;		//現在検知しているラインの本数(10進数)
	uint8_t  num_log;		//1ループ前検知したラインの本数(10進数)
}LINE_InputTypeDef;

typedef struct color_tag{
	uint16_t cru_cur[3][3];	//現在の生データ(10進数)
	uint16_t cru_log[3][3];	//1ループ前の生データ(10進数)
	uint8_t  bin_cur;		//現在のカラーセンサの値(2進数,4bit)
	uint8_t  bin_log;		//1ループ前のカラーセンサの値(2進数,4bit)
	int8_t   dec_cur;		//現在のカラーセンサの値(10進数, 0:nothing 1:right_color 2:left_color 3:center_color 4:silver)
	int8_t   dec_log;		//1ループ前のカラーセンサの値(10進数, 0:nothing 1:right_color 2:left_color 3:center_color 4:silver)
}COLOR_InputTypeDef;

typedef struct ping_tag{
	uint16_t cur[4];		//現在のPINGの生データ(1:front_ping, 2:right_ping, 3:back_ping, 4:left_ping)
	uint16_t log[4];		//1ループ前のPINGの生データ(1:front_ping, 2:right_ping, 3:back_ping, 4:left_ping)
	uint16_t centi_cur[4];	//現在のPINGの変換済データ(1:front_ping, 2:right_ping, 3:back_ping, 4:left_ping)   (cm)
	uint16_t centi_log[4];	//1ループ前のPINGの変換済データ(1:front_ping, 2:right_ping, 3:back_ping, 4:left_ping)(cm)
}PING_InputTypeDef;

typedef struct touch_tag{
	uint8_t  cur;		//現在のタッチセンサの生データ(2進数:生データ)(10進数 : 0:nothing, 1:right_touch, 2:left_touch, 3:front_touch)
	uint8_t  log;		//1ループ前のタッチセンサの生データ(2進数:生データ)(10進数 : 0:nothing, 1:right_touch, 2:left_touch, 3:front_touch)
}TOUCH_InputTypeDef;

typedef enum Sensor{
	LINE  		= 0b0001,
	COLOR 		= 0b0010,
	TOUCH 		= 0b0100,
	PING  		= 0b1000,
	ALL_SENSOR	= 0b1111
}sensor;

//グローバル関数・変数
extern void	InitSensor(uint8_t param);		//センサ初期化
/*	--param--
 * 	ALL_SENSOR : すべてのセンサを初期化する
 * 	LINE : ラインセンサを初期化する
 * 	COLOR : カラーセンサを初期化する
 * 	TOUCH : タッチセンサを初期化する
 */

extern void	GetSensorData(uint8_t param);	//センサの入力値を処理
/*	--param--
 * 	ALL_SENSOR : すべてのセンサの入力値を各構造体に格納する
 * 	LINE : ラインセンサの入力値を構造体に格納する
 * 	COLOR : カラーセンサの入力値を構造体に格納する
 * 	TOUCH : タッチセンサの入力値を構造体に格納する
 */

extern LINE_InputTypeDef	line;
extern COLOR_InputTypeDef	color;
extern TOUCH_InputTypeDef	touch;
extern PING_InputTypeDef 	ping;

#endif /* HARD_H_ */
