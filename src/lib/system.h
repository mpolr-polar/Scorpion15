/*
 * system.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

//���ׂĂ̌n��̃w�b�_�t�@�C�����C���N���[�h
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "delay.h"
#include "int.h"
#include "act.h"
#include "hard.h"
//#include "sdio.h"

typedef struct userswitch_tag{
	uint8_t onboard;	//F4-Discovery�ɍڂ��Ă郆�[�U�[�X�C�b�`
}USERSWITCH_InitTypeDef;

typedef struct option{
	uint8_t wakeup_mode;		//�N�����[�h
	int8_t  mode;				//���䃂�[�h
	int8_t	Oflag;				//��Q�����o�̃t���O
	uint8_t enabled_sensers;	//�N�����ꂽ�Z���T�[
	uint8_t enabled_modules;	//�N�����ꂽ���W���[��
	uint8_t motor_status;		//���[�^�[�̏��
	uint8_t tail_status;		//�K��(�A�[��)�̏��
}System_StatusTypeDef;

typedef struct save_point{
	uint16_t cmps;		//�����n�_�Ō����Ă�����p��0�Ƃ���B(4��rad = 3600)(���Ƀ��C����������Ă��Ȃ�����Acmps�l��0�̌�����ping�𑪂�)
	uint16_t ping[4];	//���̒n�_�A���̌����ł�PING�̒l(cm) (0:front_pintg  1:right_ping  2:back_ping  3:left_ping)
	uint16_t tag;		//���̒n�_�̏ꏊ(�� : �X�^�[�g�n�_�A�Q�ڂ̕����̎n�_ etc...)
}POINT_DataTypeDef;

typedef enum savepoint_tag{
	ENTRANCE		= 0b00000001,	//�G���A�̎n�_
	OBSTACLE		= 0b00000010,	//��Q���̏ꏊ
	FIRST_ROOM		= 0b00000100,	//�ŏ��̕���
	SECOND_ROOM 	= 0b00001000,	//�Q�ڂ̕���
	PASSAGE			= 0b00010000,	//�ʘH
	HIGH_LANDING	= 0b00100000,	//�����̑O�̗x���
	LOW_LANDING		= 0b01000000,	//����̑O�̗x���
	LAST_ROOM		= 0b10000000	//����
}SAVEPOINT_TAG;

typedef enum wakeup_mode{
	active_all,		//�{��
	debug_mode,		//�f�o�b�O���[�h
	linetrace		//���C���g���[�X�̃e�X�g
}WAKEUP_MODE;

typedef enum debug{
//--�Z���T�[----------------------//
	line_sensor,
	color_sensor,
	touch_sensor,
	distance_sensor,
//-----------------------------//

//--���W���[��---------------------//
	accel_module,
	gyro_module,
	compass_module
//-----------------------------//
}DEBUG;

typedef enum Motor_Status{
//	ENABLE 		= 0b00000001,	//����������Ă���
	STOP		= 0b00000000,	//Rduty=0, Mduty=0
	RUN			= 0b00000010,	//left>0 || right>0
	BACK		= 0b00000100,	//!RUN
	STRAIGHT 	= 0b00001000,	//0.5 < (right/left) < 2
	TURN_RIGHT	= 0b00010000,	//left >= right*2
	TURN_LEFT	= 0b00100000,	//left*2 <= right
}MOTOR_STATUS;

//���[�p�X�t�B���^�[(���݂̒l�ƑO��̒l���K�v)
#define NULL ((void*)0)
#define LowPass_Filter(value,past_value)	(((past_value)*4 + (value)*6)/10);
#define OnBoardSwitch()						((GPIOA->IDR&1)?(SET):(RESET))
#define ABS(x)		((x<0)?(-x):(x))
#define SAVE_POINT	10

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

extern void System_Configuration(uint8_t mode);
extern void Debug(uint8_t device);
//extern void SetUp();					//����̐��䃂�[�h�ɓ��鎖�O����
//extern void CleanUp();				//����̐��䃂�[�h����ʂ̃��[�h�Ɉڍs���邽�߂̌㏈��
extern void	ProcessInput();				//���ׂĂ̓��͂���������
extern void OnBoardLED_Init();			//�I���{�[�hLED������������
extern void Light_OBLED(uint8_t led);	//�w�肳�ꂽ�I���{�[�hLED��_��������
extern void UserSwitch_Init();			//�I���{�[�h�̃��[�U�[�X�C�b�`������
//extern void InputUserSwitch();			//���[�U�[�X�C�b�`�̓��͂��擾

/*���C���v���Z�X*//*
extern void Linetrace(void);			//���C���g���[�X	0
extern void Obstacle(void);				//��Q��		1
extern void Passage(void);				//�ʘH�E����	2
extern void Downhill(void);				//�����		3
extern void Shelter(void);*/			//����	4
extern void (*MainProcess[])(void);		//�e���[�h�̃T�u���[�`���̔z��
extern void (*ControlProcess[])(void);	//�e���[�h�̏�������

extern USERSWITCH_InitTypeDef	user;
extern System_StatusTypeDef		scorpion;
extern POINT_DataTypeDef		point[SAVE_POINT];


#endif /* SYSTEM_H_ */


/*
 * �E����́A�X�΂̂����Ă����C���̈�����Ă��Ȃ��R�[�X���w��
 * �E�ʘH�́A�X�΂��Ȃ��Ă����C���̈�����Ă��Ȃ��R�[�X���w��
 * �E�����́A���C���̗L���ɂ�����炸�}�ȉ�����z�ł���R�[�X���w��
 * �E���C���g���[�X���[�h�ɓ�������́A�N������ł��邱�Ƃ��㏈���̎��_�Ń��C�������m����邱��
 * �E��Q�����[�h�ɓ�������́A���C���g���[�X���Ƀ^�b�`�Z���T���������A�����������̂��ǂł͂Ȃ�����
 * �E����E�ʘH���[�h�ɓ�������́A���C������O��Ă��O���Ƒ���(�E�����̂ǂ��炩)���ǂƐڋ߂��Ă��邱��
 * �E����⃂�[�h�ɓ�������́A�ʘH�Ƃ��ĔF�����ꂽ
 * �E�������[�h�ɓ�������́A�n�ʂɋ�F�̃}�[�J�[�����m���邱�Ƃł���
 * �E�������[�h�̓t���[��������Ȃ��̂ŁA�ʂ���ꏊ�ɒu���Ȃ��Ă�����͂����ŏI���
 * �E���[�^�[�̐���X�C�b�`��OFF�ɂ��ꂽ���A���s���ł���~���A�w�肳�ꂽ��������s����
 *
 * */
