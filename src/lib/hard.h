/*
 * hard.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef HARD_H_
#define HARD_H_

//�n�[�h�E�F�A(���C���Z���T�{�[�h,�^�b�`�Z���T,PING(Ultrasonic Distance Sensor))
//hard�t�H���_���̃w�b�_�t�@�C�����C���N���[�h
#include "system.h"
#include "hard/linecolor.h"
#include "hard/touch.h"
#include "hard/ping.h"

//�Z���T���͗p�\����
typedef struct line_tag{
	uint8_t  bin_cur;		//���݂̃��C���Z���T�̒l(2�i��,7bit)
	uint8_t  bin_log;		//1���[�v�O�̃��C���Z���T�̒l(2�i��.7bit)
	uint8_t  cnt_cur;		//���݂̔������Ă��郉�C���Z���T�̐�(10�i��)
	uint8_t  cnt_log;		//1���[�v�O�̔����������C���Z���T�̐�(10�i��)
	int8_t   dec_cur;		//���݂̃��C���Z���T�̒l(10�i��,���C���̈ʒu�𕄍��t�������l�Ŏ���)
	int8_t   dec_log;		//1���[�v�O�̃��C���Z���T�̒l(10�i��,���C���̈ʒu�𕄍��t�������l�Ŏ���)
	uint8_t  num_cur;		//���݌��m���Ă��郉�C���̖{��(10�i��)
	uint8_t  num_log;		//1���[�v�O���m�������C���̖{��(10�i��)
}LINE_InputTypeDef;

typedef struct color_tag{
	uint16_t cru_cur[3][3];	//���݂̐��f�[�^(10�i��)
	uint16_t cru_log[3][3];	//1���[�v�O�̐��f�[�^(10�i��)
	uint8_t  bin_cur;		//���݂̃J���[�Z���T�̒l(2�i��,4bit)
	uint8_t  bin_log;		//1���[�v�O�̃J���[�Z���T�̒l(2�i��,4bit)
	int8_t   dec_cur;		//���݂̃J���[�Z���T�̒l(10�i��, 0:nothing 1:right_color 2:left_color 3:center_color 4:silver)
	int8_t   dec_log;		//1���[�v�O�̃J���[�Z���T�̒l(10�i��, 0:nothing 1:right_color 2:left_color 3:center_color 4:silver)
}COLOR_InputTypeDef;

typedef struct ping_tag{
	uint16_t cur[4];		//���݂�PING�̐��f�[�^(1:front_ping, 2:right_ping, 3:back_ping, 4:left_ping)
	uint16_t log[4];		//1���[�v�O��PING�̐��f�[�^(1:front_ping, 2:right_ping, 3:back_ping, 4:left_ping)
	uint16_t centi_cur[4];	//���݂�PING�̕ϊ��σf�[�^(1:front_ping, 2:right_ping, 3:back_ping, 4:left_ping)   (cm)
	uint16_t centi_log[4];	//1���[�v�O��PING�̕ϊ��σf�[�^(1:front_ping, 2:right_ping, 3:back_ping, 4:left_ping)(cm)
}PING_InputTypeDef;

typedef struct touch_tag{
	uint8_t  cur;		//���݂̃^�b�`�Z���T�̐��f�[�^(2�i��:���f�[�^)(10�i�� : 0:nothing, 1:right_touch, 2:left_touch, 3:front_touch)
	uint8_t  log;		//1���[�v�O�̃^�b�`�Z���T�̐��f�[�^(2�i��:���f�[�^)(10�i�� : 0:nothing, 1:right_touch, 2:left_touch, 3:front_touch)
}TOUCH_InputTypeDef;

typedef enum Sensor{
	LINE  		= 0b0001,
	COLOR 		= 0b0010,
	TOUCH 		= 0b0100,
	PING  		= 0b1000,
	ALL_SENSOR	= 0b1111
}sensor;

//�O���[�o���֐��E�ϐ�
extern void	InitSensor(uint8_t param);		//�Z���T������
/*	--param--
 * 	ALL_SENSOR : ���ׂẴZ���T������������
 * 	LINE : ���C���Z���T������������
 * 	COLOR : �J���[�Z���T������������
 * 	TOUCH : �^�b�`�Z���T������������
 */

extern void	GetSensorData(uint8_t param);	//�Z���T�̓��͒l������
/*	--param--
 * 	ALL_SENSOR : ���ׂẴZ���T�̓��͒l���e�\���̂Ɋi�[����
 * 	LINE : ���C���Z���T�̓��͒l���\���̂Ɋi�[����
 * 	COLOR : �J���[�Z���T�̓��͒l���\���̂Ɋi�[����
 * 	TOUCH : �^�b�`�Z���T�̓��͒l���\���̂Ɋi�[����
 */

extern LINE_InputTypeDef	line;
extern COLOR_InputTypeDef	color;
extern TOUCH_InputTypeDef	touch;
extern PING_InputTypeDef 	ping;

#endif /* HARD_H_ */
