/*
 * linecolor.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef LINECOLOR_H_
#define LINECOLOR_H_

#include "lib/hard.h"

//define宣言
#define Get_Line(line)	((line!=7)?((GPIOD->IDR&(0b1000<<line))>>(line+3)):((GPIOD->IDR&0b0000001111111000)>>3))
#define Line0			0		//edge of right
#define Line1			1
#define Line2			2
#define Line3			3		//center
#define Line4			4
#define Line5			5
#define Line6			6		//edge of left
#define All_Line		7
#define NO_LINE			7		//ラインが途切れた
#define BLACK_OUT		127		//地面から赤外線が反射していない -> (アリーナ外の地面 or 持ち上げられている)
#define CC_VALUE		50

#define CLK				GPIO_Pin_11	//PE11
#define RANGE			GPIO_Pin_12	//PE12
#define GATE			GPIO_Pin_13	//PE13
#define DOUT1			GPIO_Pin_10	//PD10
#define DOUT2			GPIO_Pin_11	//PD11
#define DOUT3			GPIO_Pin_12	//PD12

typedef enum COLOR_pos{
	right_color = 1,//1
	left_color,		//2
	center_color,	//3
	silver
}Cpos;

typedef enum color_pos{
	rc_data = 0,
	cc_data,
	lc_data,
	red		= 0,
	green,
	blue
}cpos;

/*-line------------------------------*/
extern void 	Line_Configuration();			//ラインセンサ入力ピンの初期化
extern void 	Line_Read();					//ラインセンサ読み込み
extern void		Filter_Line(uint8_t line);		//ラインデータにフィルタをかける(引数で指定されたポートだけ有効にする)
extern int8_t	Translate_Line(uint8_t line);	//バイナリデータを10進数データに変換する。
/*-----------------------------------*/

/*-color-----------------------------*/
extern void		Color_Configuration();			//カラーセンサ入力ピンの初期化
extern void		Color_Read();					//カラーセンサ読み込み
extern uint8_t	Translate_Color(uint8_t color);	//バイナリデータを10進数データに変換する。
/*-----------------------------------*/
#endif /* LINECOLOR_H_ */
