/*
 * system.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

//すべての系列のヘッダファイルをインクルード
#include <math.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "delay.h"
#include "int.h"
#include "act.h"
#include "hard.h"
//#include "sdio.h"
#include "kalman.h"

typedef struct userswitch_tag{
	uint8_t onboard;	//F4-Discoveryに載ってるユーザースイッチ
}USERSWITCH_InitTypeDef;

typedef struct option{
	uint8_t		wakeup_mode;		//起動モード
	int8_t  	mode;				//制御モード
	int8_t		Oflag;				//障害物検出のフラグ
	int8_t		Pflag;				//通路(坂道)のフラグ
	uint8_t		point_num;			//セーブポイントの数
	uint8_t 	enabled_sensers;	//起動されたセンサー
	uint8_t 	enabled_modules;	//起動されたモジュール
	uint8_t 	motor_status;		//モーターの状態
	uint8_t 	tail_status;		//尻尾(アーム)の状態
	int16_t		yaw[11];				//ヨー角(0.1deg単位)
	int16_t		pitch[11];			//ピッチ角(0.1deg単位)
	int16_t		roll[11];			//ロール角(0.1deg単位)
	uint8_t		aoinc;				//Angle Of Inclination(傾斜角)
}System_StatusTypeDef;

typedef struct save_point{
	uint8_t  line;		//ラインのデータ
	uint8_t  touch;		//タッチセンサのデータ
	uint8_t  color;		//カラーセンサのデータ
	uint8_t  ypr[3];	//ヨーピッチロールのデータ。 (Yawに関して)初期地点を基準(0)とする。
	uint16_t ping[4];	//その地点、その向きでのPINGの値(cm) (0:front_pintg  1:right_ping  2:back_ping  3:left_ping)
	uint16_t tag;		//その地点の場所(例 : スタート地点、２つ目の部屋の始点 etc...)
}POINT_DataTypeDef;

typedef enum savepoint_tag{
	ENTRANCE		= 0b000000001,	//エリアの始点
	EXIT			= 0b000000010,	//エリアの終点
	OBSTACLE		= 0b000000100,	//障害物の場所
	FIRST_ROOM		= 0b000001000,	//最初の部屋
	SECOND_ROOM 	= 0b000010000,	//２つ目の部屋
	PASSAGE			= 0b000100000,	//通路
	HIGH_LANDING	= 0b001000000,	//下り坂の前の踊り場
	LOW_LANDING		= 0b010000000,	//上り坂の前の踊り場
	LAST_ROOM		= 0b100000000	//避難部屋
}SAVEPOINT_TAG;

typedef enum wakeup_mode{
	active_all,		//本番
	debug_mode,		//デバッグモード
	linetrace,		//ライントレースのテスト
	obstacle,		//障害物のテスト
	accelerometer,	//加速度センサの計測
	gyrosensor,		//ジャイロセンサの計測
	compass,		//コンパスセンサの計測
	yawpitchroll,	//ヨー・ピッチ・ロールの計測
	speedtest,
	selfoption		//手動での初期化
}WAKEUP_MODE;

typedef enum debug{
//--センサー----------------------//
	line_sensor,
	color_sensor,
	touch_sensor,
	distance_sensor,
//-----------------------------//

//--モジュール---------------------//
	accel_module,
	gyro_module,
	compass_module,
//-----------------------------//
}DEBUG;

typedef enum Motor_Status{
//	ENABLE 		= 0b00000001,	//初期化されている
	STOP		= 0b00000000,	//Rduty=0, Mduty=0
	RUN			= 0b00000010,	//left>0 || right>0
	BACK		= 0b00000100,	//!RUN
	STRAIGHT 	= 0b00001000,	//0.5 < (right/left) < 2
	TURN_RIGHT	= 0b00010000,	//left >= right*2
	TURN_LEFT	= 0b00100000,	//left*2 <= right
}MOTOR_STATUS;

typedef enum passageflag{
	run_ahead		= 0b00001,	//通路を進む
	run_uphill		= 0b00010,	//上り坂を登る
	run_downhill	= 0b00100,	//下り坂を下る
	ready_up		= 0b01000,	//上り坂を登る前処理
	ready_down		= 0b10000	//下り坂を下る前処理
}PFLAG;

#define LowPass_Filter(value,past_value)	(((past_value)*3 + (value)*7)/10);	//超簡易ローパスフィルタ
#define OnBoardSwitch()						((GPIOA->IDR&1)?(SET):(RESET))		//STM32F4-Discoveryのオンボードスイッチ(青)の入力を取得
#define ABS(x)		(((x)<0)?(-(x)):(x))		//xの絶対値
#define SAVE_POINT	10							//セーブポイントの最大作成数
#define PI			(double)(3.14159265358979)	//π(double型)
#define DEGREES(x)	(double)((x)*180/PI)		//ラジアン(x)から角度に変換
#define RADIAN(x)	(double)((x)*PI/180)		//角度(x)からラジアンに変換

//Obstacle
#define	FRONT	front_ping
#define	RIGHT	right_ping
#define	LEFT	left_ping
#define	BACK	back_ping
#define OSPEED	50
#define	P_FRONT	front_ping
#define	P_RIGHT	right_ping
#define	P_LEFT	left_ping
#define	P_BACK	back_ping
#define DISTANCE	7//cm
#define BACKING 200
#define TURNING	3000
#define BACKLINE 2500
#define	LBACKING 400

//Passage
#define FDISTANCE	30					//通路に差し掛かったと思われるときのfront_pingの値(cm)
#define SDISTANCE	8					//通路に差し掛かるときのright/left_pingの値(cm)
#define RT90DEG(rd)	(2600/50*ABS(rd))	//90度旋回するのにかかる時間(ms)(rd=Rduty)

extern void System_Configuration(uint8_t mode);	//各機能の初期化(mode : System_StatusTypeDef)
extern void Debug(uint8_t device);		//各機能のデバッグ(あまり使わない)
//extern void SetUp();					//特定の制御モードに入る事前処理
//extern void CleanUp();				//特定の制御モードから別のモードに移行するための後処理
extern void	ProcessInput();				//すべての入力を処理する
extern void OnBoardLED_Init();			//オンボードLEDを初期化する
extern void Light_OBLED(uint8_t led);	//指定されたオンボードLEDを点灯させる
extern void UserSwitch_Init();			//オンボードのユーザースイッチ初期化
//extern void InputUserSwitch();		//ユーザースイッチの入力を取得
extern void SavePoint(uint16_t tag);	//セーブポイントの計測データを保存
extern void GetYPRData();				//ヨーピッチロールの値を取得
extern int16_t AveYPR(int16_t *data);

/*メインプロセス*//*
extern void Linetrace(void);			//ライントレース	0
extern void Obstacle(void);				//障害物		1
extern void Passage(void);				//通路・上り坂	2
extern void Downhill(void);				//下り坂		3
extern void Shelter(void);*/			//避難部屋	4
extern void (*MainProcess[])(void);		//各モードのサブルーチンの配列
extern void (*ControlProcess[])(void);	//各モードの処理分岐

extern USERSWITCH_InitTypeDef	user;
extern System_StatusTypeDef		scorpion;
extern POINT_DataTypeDef		point[SAVE_POINT];


#endif /* SYSTEM_H_ */


/*
 * ・上り坂は、傾斜のあってかつラインの引かれていないコースを指す
 * ・通路は、傾斜がなくてかつラインの引かれていないコースを指す
 * ・下り坂は、ラインの有無にかかわらず急な下り勾配であるコースを指す
 * ・ライントレースモードに入る条件は、起動直後であることか後処理の時点でラインが検知されること
 * ・障害物モードに入る条件は、ライントレース中にタッチセンサが反応し、当たったものが壁ではないこと
 * ・上り坂・通路モードに入る条件は、ラインから外れてかつ前方と側面(右か左のどちらか)が壁と接近していること
 * ・下り坂モードに入る条件は、通路として認識された
 * ・避難部屋モードに入る条件は、地面に銀色のマーカーを検知することである
 * ・避難部屋モードはフロー制御をしないので、缶を避難場所に置けなくても制御はそこで終わる
 * ・モーターの制御スイッチがOFFにされた時、走行中でも停止し、指定された制御を実行する
 *
 * */
