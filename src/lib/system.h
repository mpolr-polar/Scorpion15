/*
 * system.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

//���ׂĂ̌n��̃w�b�_�t�@�C�����C���N���[�h
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
	uint8_t onboard;	//F4-Discovery�ɍڂ��Ă郆�[�U�[�X�C�b�`
}USERSWITCH_InitTypeDef;

typedef struct option{
	uint8_t		wakeup_mode;		//�N�����[�h
	int8_t  	mode;				//���䃂�[�h
	int8_t		Oflag;				//��Q�����o�̃t���O
	int8_t		Pflag;				//�ʘH(�⓹)�̃t���O
	uint8_t		point_num;			//�Z�[�u�|�C���g�̐�
	uint8_t 	enabled_sensers;	//�N�����ꂽ�Z���T�[
	uint8_t 	enabled_modules;	//�N�����ꂽ���W���[��
	uint8_t 	motor_status;		//���[�^�[�̏��
	uint8_t 	tail_status;		//�K��(�A�[��)�̏��
	int16_t		yaw[11];				//���[�p(0.1deg�P��)
	int16_t		pitch[11];			//�s�b�`�p(0.1deg�P��)
	int16_t		roll[11];			//���[���p(0.1deg�P��)
	uint8_t		aoinc;				//Angle Of Inclination(�X�Ίp)
}System_StatusTypeDef;

typedef struct save_point{
	uint8_t  line;		//���C���̃f�[�^
	uint8_t  touch;		//�^�b�`�Z���T�̃f�[�^
	uint8_t  color;		//�J���[�Z���T�̃f�[�^
	uint8_t  ypr[3];	//���[�s�b�`���[���̃f�[�^�B (Yaw�Ɋւ���)�����n�_���(0)�Ƃ���B
	uint16_t ping[4];	//���̒n�_�A���̌����ł�PING�̒l(cm) (0:front_pintg  1:right_ping  2:back_ping  3:left_ping)
	uint16_t tag;		//���̒n�_�̏ꏊ(�� : �X�^�[�g�n�_�A�Q�ڂ̕����̎n�_ etc...)
}POINT_DataTypeDef;

typedef enum savepoint_tag{
	ENTRANCE		= 0b000000001,	//�G���A�̎n�_
	EXIT			= 0b000000010,	//�G���A�̏I�_
	OBSTACLE		= 0b000000100,	//��Q���̏ꏊ
	FIRST_ROOM		= 0b000001000,	//�ŏ��̕���
	SECOND_ROOM 	= 0b000010000,	//�Q�ڂ̕���
	PASSAGE			= 0b000100000,	//�ʘH
	HIGH_LANDING	= 0b001000000,	//�����̑O�̗x���
	LOW_LANDING		= 0b010000000,	//����̑O�̗x���
	LAST_ROOM		= 0b100000000	//����
}SAVEPOINT_TAG;

typedef enum wakeup_mode{
	active_all,		//�{��
	debug_mode,		//�f�o�b�O���[�h
	linetrace,		//���C���g���[�X�̃e�X�g
	obstacle,		//��Q���̃e�X�g
	accelerometer,	//�����x�Z���T�̌v��
	gyrosensor,		//�W���C���Z���T�̌v��
	compass,		//�R���p�X�Z���T�̌v��
	yawpitchroll,	//���[�E�s�b�`�E���[���̌v��
	speedtest,
	selfoption		//�蓮�ł̏�����
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
	compass_module,
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

typedef enum passageflag{
	run_ahead		= 0b00001,	//�ʘH��i��
	run_uphill		= 0b00010,	//�����o��
	run_downhill	= 0b00100,	//����������
	ready_up		= 0b01000,	//�����o��O����
	ready_down		= 0b10000	//����������O����
}PFLAG;

#define LowPass_Filter(value,past_value)	(((past_value)*3 + (value)*7)/10);	//���ȈՃ��[�p�X�t�B���^
#define OnBoardSwitch()						((GPIOA->IDR&1)?(SET):(RESET))		//STM32F4-Discovery�̃I���{�[�h�X�C�b�`(��)�̓��͂��擾
#define ABS(x)		(((x)<0)?(-(x)):(x))		//x�̐�Βl
#define SAVE_POINT	10							//�Z�[�u�|�C���g�̍ő�쐬��
#define PI			(double)(3.14159265358979)	//��(double�^)
#define DEGREES(x)	(double)((x)*180/PI)		//���W�A��(x)����p�x�ɕϊ�
#define RADIAN(x)	(double)((x)*PI/180)		//�p�x(x)���烉�W�A���ɕϊ�

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
#define FDISTANCE	30					//�ʘH�ɍ����|�������Ǝv����Ƃ���front_ping�̒l(cm)
#define SDISTANCE	8					//�ʘH�ɍ����|����Ƃ���right/left_ping�̒l(cm)
#define RT90DEG(rd)	(2600/50*ABS(rd))	//90�x���񂷂�̂ɂ����鎞��(ms)(rd=Rduty)

extern void System_Configuration(uint8_t mode);	//�e�@�\�̏�����(mode : System_StatusTypeDef)
extern void Debug(uint8_t device);		//�e�@�\�̃f�o�b�O(���܂�g��Ȃ�)
//extern void SetUp();					//����̐��䃂�[�h�ɓ��鎖�O����
//extern void CleanUp();				//����̐��䃂�[�h����ʂ̃��[�h�Ɉڍs���邽�߂̌㏈��
extern void	ProcessInput();				//���ׂĂ̓��͂���������
extern void OnBoardLED_Init();			//�I���{�[�hLED������������
extern void Light_OBLED(uint8_t led);	//�w�肳�ꂽ�I���{�[�hLED��_��������
extern void UserSwitch_Init();			//�I���{�[�h�̃��[�U�[�X�C�b�`������
//extern void InputUserSwitch();		//���[�U�[�X�C�b�`�̓��͂��擾
extern void SavePoint(uint16_t tag);	//�Z�[�u�|�C���g�̌v���f�[�^��ۑ�
extern void GetYPRData();				//���[�s�b�`���[���̒l���擾
extern int16_t AveYPR(int16_t *data);

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
