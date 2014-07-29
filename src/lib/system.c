/*
 * system.c
 *
 *  Created on: 2014/06/07
 *      Author: �s��
 */

#include "system.h"

GPIO_InitTypeDef 		GPIO_InitStructure;
USERSWITCH_InitTypeDef	user;
System_StatusTypeDef	scorpion = {0,0,0,0,0,0};
POINT_DataTypeDef		point[SAVE_POINT],curpo;
//curpo:���݂�point�l(cmps.shift�͍ŏ��ɐݒ肵���l)

void System_Configuration(uint8_t mode)
{
	SystemInit();
	delay_init();
	OnBoardLED_Init();
	UserSwitch_Init();
	switch(mode)
	{
	case active_all:
		InitSensor(ALL_SENSOR);
		InitModule(ALL_MODULE);
		Motor_Configuration();
		break;

	case debug_mode:
		InitSensor(ALL_SENSOR);
		USART_Configuration();
		break;

	case linetrace:
		InitSensor(LINE);
		USART_Configuration();
		Motor_Configuration();
		break;

	default:
		break;
	}
	scorpion.wakeup_mode = mode;
	scorpion.mode = 5;//WakeUp
}

void Debug(uint8_t device)
{
	switch(device)
	{
	case line_sensor:
		GetSensorData(LINE);
		USART_PutStr("Line : ");
		USART_PutBin(line.bin_cur);Indention;
		switch(line.dec_cur)
		{
		case NO_LINE:
			USART_PutStr("NO_LINE");
			break;

		case BLACK_OUT:
			USART_PutStr("BLACK_OUT");
			break;

		default:
			USART_PutDec2(line.dec_cur);
			break;
		}
		Indention;
		USART_PutDec(line.cnt_cur);Indention;
		USART_PutDec(line.num_cur);Linefeed;
		break;

	case color_sensor:
		break;

	case distance_sensor:
		GetSensorData(PING);
		USART_PutStr("PING : ");
		USART_PutDec(ping.cur[front_ping]);Indention;
		USART_PutDec(ping.cur[right_ping]);Indention;
		USART_PutDec(ping.cur[back_ping]);Indention;
		USART_PutDec(ping.cur[left_ping]);Linefeed;
		break;

	case touch_sensor:
		GetSensorData(TOUCH);
		USART_PutStr("Touch : ");
		switch(touch.cur)
		{
		case nothing:
			USART_PutStr("nothing");
			break;
		case right_touch:
			USART_PutStr("right_touch");
			break;
		case left_touch:
			USART_PutStr("left_touch");
			break;
		case front_touch:
			USART_PutStr("front_touch");
			break;
		default:
			USART_PutStr("unknown status");
			break;
		}
		Linefeed;
		break;

	default:
		USART_PutStr("Designated an enabled value.");Linefeed;
		break;
	}
}

void ProcessInput()
{
	uint8_t i=0;
	GetSensorData(ALL_SENSOR);
	GetModuleData(ALL_MODULE);
	curpo.cmps = cmps.dir_3600;
	for(i=0;i<4;i++){curpo.ping[i]=ping.centi_cur[i];}
}

void OnBoardLED_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Pin 	= 0b1111000000000000;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
}

void Light_OBLED(uint8_t led)
{
	GPIOD->BSRRH = 0b1111<<12;
	GPIOD->BSRRL = led<<12;
}

void UserSwitch_Init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin 	= 0b0000000000000001;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void InputUserSwitch()
{

}

/* ���C���v���Z�X */
void Linetrace(void)
{
	ProcessInput();
	if(line.dec_cur!=BLACK_OUT && line.dec_cur!=NO_LINE)
	{
		if(line.dec_cur > 0)
		{
			line.bin_cur &= 0b0001111;
		}else if(line.dec_cur < 0)
		{
			line.bin_cur &= 0b1111000;
		}
		line.dec_cur = Translate_Line(line.bin_cur);
	}

	//error:���C������O�ꂽ����motor.Rduty�ɕ␳��������
	static uint8_t error=0;
	static int8_t  turn=0;
	switch(line.dec_cur)
	{
	case NO_LINE:	//�����r�؂ꂽ��
		if(line.dec_log==NO_LINE || line.dec_log*line.dec_log < 25)
		{
			Move(0,0);
			error = 0;
		}else
		{
			error++;
			if(line.dec_log <= -5)
			{
				motor.Rduty += (-36-error);
				line.dec_cur = -6;
			}else if(line.dec_log >= 5)
			{
				motor.Rduty += (36+error);
				line.dec_cur = 6;
			}
			motor.Mduty = 0;
			Move(motor.Rduty,motor.Mduty);
		}
		break;

	default:		//�����O��Ă��Ȃ���(�ʏ�̃��C���g���[�X)
		if(error){error = 0;}
		if(!(line.bin_cur & 0b0011100))
		{
			if((line.dec_cur>0&&turn<0)||(line.dec_cur<0&&turn>0)){motor.Rduty=0;}
			turn = line.dec_cur/ABS(line.dec_cur);
			motor.Rduty += line.dec_cur*ABS(line.dec_cur);
			motor.Mduty = 80-line.dec_cur*10;
			Move(motor.Rduty,motor.Mduty);
		}else
		{
			if((line.bin_cur&0b0001000)!=0 && (line.bin_cur&0b0010100)==0)
			{
				if((line.dec_cur>0&&turn<0)||(line.dec_cur<0&&turn>0)){motor.Rduty=0;}
				turn = line.dec_cur/ABS(line.dec_cur);
				Move(0,100);
				motor.Rduty = 0;
			}else
			{
				turn=0;
				Move(25*line.dec_cur/ABS(line.dec_cur),25);
			}
		}
		break;
	}
	delay_us(100);
}

void Obstacle(void)
{
	static int16_t	right;
	static int16_t	left;

	GetSensorData(PING|TOUCH);										//�����ŃZ���T�[�f�[�^��ǂ�

	if(touch.cur!=0){
		switch(touch.cur){
		case left_touch://���̃^�b�`�Z���T�[�����������Ƃ�

			switch(scorpion.Oflag){
			case 0:	//��Q���̃t���O�������Ă��Ȃ������Ƃ�
//				if(scorpRduty<0)//���ɋȂ��낤�Ƃ��Ă����Ƃ�;

				if(ping.centi_cur[FRONT]<DISTANCE){
					Brake(both_motors);
					delay_ms(1000);

					RotateMotor(-OSPEED,-OSPEED);
					delay_ms(BACKING);
					Brake(both_motors);
					delay_ms(500);
					GetSensorData(PING);

					if(ping.cur[RIGHT]<ping.cur[LEFT]){
						RotateMotor(-OSPEED,OSPEED);
						delay_ms(TURNING);
						Brake(both_motors);
						delay_ms(1000);
						scorpion.Oflag=1;
					}else{
						RotateMotor(OSPEED,-OSPEED);
						delay_ms(TURNING);
						Brake(both_motors);
						delay_ms(1000);
						scorpion.Oflag=-1;
					}
				}else{
					right=-OSPEED;
					left=0;
					break;
				}
			case 1://�E�ɏ�Q��������t���O�������Ă����Ƃ�
				if(ping.centi_cur[FRONT]<DISTANCE){	//	�O�ɏ�Q��������
					scorpion.Oflag=0;
					goto frontlabel;
				}
				if(ping.centi_cur[P_RIGHT]>=DISTANCE){//�E���ɏ�Q�����ڋ߂��ĂȂ������Ƃ�
					right=-OSPEED;
					left=0;
				}else{											//�ڋ߂��Ă����Ƃ�
					scorpion.Oflag=2;
					 right=-OSPEED;
					 left=0;
				}
				break;
			case -1://���ɏ�Q��������t���O�������Ă����Ƃ�
				right=-OSPEED;
				left=0;
				break;
			case 10://�E���񒆂̃t���O�������Ă����Ƃ�
				right=-OSPEED;
				left=0;
				break;
			case -10://�����񒆂̃t���O�������Ă����Ƃ�
				scorpion.Oflag=2;
				right=-OSPEED;
				left=0;
				break;
			case 2://�����ɏ�Q��������t���O�������Ă����Ƃ�
				right=-OSPEED;
				left=0;
				break;
			default:
				right=0;
				left=0;
				break;
			}

			break;
		case right_touch://�E�̃^�b�`�Z���T�[�����������Ƃ�

			switch(scorpion.Oflag){
			case 0://��Q���̃t���O�������Ă��Ȃ������Ƃ�
//				if(scorpion.Rduty>=0)//�E�ɋȂ��낤�Ƃ��Ă����Ƃ�;

				if(ping.centi_cur[FRONT]<DISTANCE){
					Brake(both_motors);
					delay_ms(1000);
					RotateMotor(-OSPEED,-OSPEED);
					delay_ms(BACKING);
					Brake(both_motors);
					GetSensorData(PING);

					if(ping.cur[RIGHT]<ping.cur[LEFT]){
						RotateMotor(-OSPEED,OSPEED);
						delay_ms(TURNING);
						Brake(both_motors);
						delay_ms(1000);
						scorpion.Oflag=1;
					}else{
						RotateMotor(OSPEED,-OSPEED);
						delay_ms(TURNING);
						Brake(both_motors);
						delay_ms(1000);
						scorpion.Oflag=-1;
					}
				}else{
					right=0;
					left=-OSPEED;
					break;
				}
			case 1://�E�ɏ�Q��������t���O�������Ă����Ƃ�
				right=0;
				left=-OSPEED;
				break;
			case -1:///���ɏ�Q��������t���O�������Ă����Ƃ�
				if(ping.centi_cur[FRONT]<DISTANCE){	//	�O�ɏ�Q��������
					scorpion.Oflag=0;
					goto frontlabel;
				}
				if(ping.centi_cur[P_LEFT]>=DISTANCE){//�����ɏ�Q�����ڋ߂��ĂȂ������Ƃ�
					right=0;
					left=-OSPEED;
				}else{											//�ڋ߂��Ă����Ƃ�
					scorpion.Oflag=2;
					 right=0;
					 left=-OSPEED;
				}
				break;
			case 10://�E���񒆂̃t���O�������Ă����Ƃ�
				scorpion.Oflag=2;
				right=0;
				left=-OSPEED;
				break;
			case -10://�����񒆂̃t���O�������Ă����Ƃ�
				right=0;
				left=-OSPEED;
				break;
			case 2:	//�����ɏ�Q��������
				right=0;
				left=-OSPEED;
				break;
			default:
				right=0;
				left=0;
				break;
			}

			break;
		default://�����̃^�b�`�Z���T�[�����������Ƃ�
			if(ping.centi_cur[P_FRONT]<DISTANCE){	//�����Z���T�[�őO�ɂ��邩�ǂ����`�F�b�N

				frontlabel:
				switch(scorpion.Oflag){
				case 0://��Q���̃t���O�������Ă��Ȃ������Ƃ�
					Brake(both_motors);
					delay_ms(1000);
					RotateMotor(-OSPEED,-OSPEED);
					delay_ms(BACKING);
					Brake(both_motors);
					delay_ms(500);
					GetSensorData(PING);

					if(ping.cur[RIGHT]<ping.cur[LEFT]){
						RotateMotor(-OSPEED,OSPEED);
						delay_ms(TURNING);
						Brake(both_motors);
						delay_ms(1000);
						scorpion.Oflag=1;
					}else{
						RotateMotor(OSPEED,-OSPEED);
						delay_ms(TURNING);
						Brake(both_motors);
						delay_ms(1000);
						scorpion.Oflag=-1;
					}

					break;
				case 1://�E�ɏ�Q��������t���O�������Ă����Ƃ�
					right=0;
					left=-OSPEED;
					break;
				case -1://���ɏ�Q��������t���O�������Ă����Ƃ�
					right=-OSPEED;
					left=0;
					break;
				case 10://�E���񒆂̃t���O�������Ă����Ƃ�
					right=0;
					left=0;
					break;
				case -10://�����񒆂̃t���O�������Ă����Ƃ�
					right=-OSPEED;
					left=0;
					break;
				case 2:
					if(ping.centi_cur[P_RIGHT]>DISTANCE){//�E�ɏ�Q�����ڋ߂��Ă��Ȃ��Ƃ�
						scorpion.Oflag=-1;
						right=-OSPEED;
						left=0;
					}else{										//�ڋ߂��Ă���Ƃ�
						if(ping.centi_cur[P_LEFT]>DISTANCE){//���ɏ�Q�����ڋ߂��Ă��Ȃ��Ƃ�
							scorpion.Oflag=1;
							right=0;
							left=-OSPEED;
						}else{										//�ڋ߂��Ă���Ƃ�(�܏��H)
							right=-OSPEED;
							left=-OSPEED;
						}
					}
					break;
				default:
					right=0;
					left=0;
					break;
				}
			}else{								//�O�ɏ�Q�����Ȃ������Ƃ�,�܂藼���ɏ�Q��������Ƃ�
				scorpion.Oflag=2;
				right=OSPEED;
				left=OSPEED;
			}

			break;
		}
	}else{
		switch(scorpion.Oflag){
//		case 0:	//��Q���̃t���O�������Ă��Ȃ������Ƃ�
//			scorpion.mode=0;
//			break;
		case 1://�E�ɏ�Q��������t���O�������Ă����Ƃ�
			if(ping.centi_cur[P_RIGHT]<DISTANCE){//�E�ɏ�Q�����ڋ߂��Ă���Ƃ�
				right=OSPEED;
				left=OSPEED/4;
			}else{
				if(ping.centi_cur[P_RIGHT]>DISTANCE){//�ڋ߂��Ă��Ȃ��Ƃ�
					right=OSPEED/2;
					left=OSPEED;
				}else{										//�W���X�g!
					right=OSPEED;
					left=OSPEED;
				}
			}
			if(line.bin_cur&0b00001111){//���C�����E���Ɍ��o���ꂽ�Ƃ�
				Brake(both_motors);
				delay_ms(100);
				RotateMotor(OSPEED,OSPEED);
				delay_ms(LBACKING);
				Brake(both_motors);
				delay_ms(100);
				RotateMotor(-OSPEED,OSPEED);
				delay_ms(BACKLINE);
				Brake(both_motors);
				delay_ms(100);
				scorpion.Oflag=0;
			}
			break;
		case -1://���ɏ�Q��������t���O�������Ă����Ƃ�
			if(ping.centi_cur[P_LEFT]<DISTANCE){//���ɏ�Q�����ڋ߂��Ă���Ƃ�
				right=OSPEED/4;
				left=OSPEED;
			}else{
				if(ping.centi_cur[P_LEFT]>DISTANCE){//�ڋ߂��Ă��Ȃ��Ƃ�
					right=OSPEED;
					left=OSPEED/2;
				}else{										//�W���X�g!
					right=OSPEED;
					left=OSPEED;
				}
			}
			if(line.bin_cur& 0b11110000){//���C���������Ɍ��o���ꂽ�Ƃ�
				Brake(both_motors);
				delay_ms(100);
				RotateMotor(OSPEED,OSPEED);
				delay_ms(LBACKING);
				Brake(both_motors);
				delay_ms(100);
				RotateMotor(OSPEED,-OSPEED);
				delay_ms(BACKLINE);
				Brake(both_motors);
				delay_ms(100);
				scorpion.Oflag=0;
			}
			break;
		case 10://�E���񒆂̃t���O�������Ă����Ƃ�
			if(ping.centi_cur[P_LEFT]<DISTANCE){//�����ɏ�Q�����ڋ߂��Ă���Ƃ�
				scorpion.Oflag=-1;
				right=OSPEED;
				left=OSPEED/4;
			}else{
				right=-OSPEED;
				left=0;
			}

			break;
		case -10://�����񒆂̃t���O�������Ă����Ƃ�
			if(ping.centi_cur[P_RIGHT]<DISTANCE){//�E���ɏ�Q�����ڋ߂��Ă���Ƃ�
				scorpion.Oflag=1;
				right=OSPEED/4;
				left=OSPEED;
			}else{
				right=0;
				left=-OSPEED;
			}
			break;
		case 2://�����ɏ�Q��������t���O�������Ă����Ƃ�
			if(ping.centi_cur[P_RIGHT]>DISTANCE){//�E���ɏ�Q�����ڋ߂��Ă��Ȃ��Ƃ�
				if(ping.centi_cur[P_LEFT]>DISTANCE){//�������ڋ߂��Ă��Ȃ��Ƃ�
					if(line.bin_cur!=0)
						scorpion.Oflag=0;
					right=OSPEED;
					left=OSPEED;
				}else{										//���͐ڋ߂��Ă���Ƃ�
					scorpion.Oflag=-1;
					right=OSPEED;
					left=0;
				}
			}else{										//�E���ɏ�Q�����ڋ߂��Ă���Ƃ�
				if(ping.centi_cur[P_LEFT]>DISTANCE){//�����͐ڋ߂��Ă��Ȃ��Ƃ�
					scorpion.Oflag=1;
					right=0;
					left=OSPEED;
				}else{										//�����ڋ߂��Ă���Ƃ�
					if(ping.cur[P_RIGHT]>ping.cur[P_LEFT]){//���L�����Ɉړ���
						right=0;
						left=OSPEED;
					}else{
						if(ping.cur[P_RIGHT]<ping.cur[P_LEFT]){
							right=OSPEED;
							left=0;
						}else{
							right=OSPEED;
							left=OSPEED;
						}
					}
				}
			}
			break;
		default:
			right=0;
			left=0;
			break;
		}
	}
	RotateMotor(left,right);										//���ۂɃ��[�^�[�𓮂����̂͂���(��荞�݂Ȃǂ̗�O�͏���)
//	scorpion.mode = 1;
}

void Passage(void)
{
	ProcessInput();

}

void Downhill(void)
{

}

void Shelter(void)
{

}


void WakeUp(void)
{
	uint8_t i=0;
	ProcessInput();
	point[0].cmps = cmps.dir_3600;
	for(i=0;i<4;i++){point[0].ping[i]=ping.centi_cur[i];}
	Cmps_Reset(0);

	while(Get_Line(All_Line)==0b1111111);
	delay_ms(500);
	while(!GetMotorSwitch());

	if(line.dec_cur == NO_LINE)
	{
		Move(0,100);
		while(Get_Line(All_Line)==0);
	}
	scorpion.mode = 0;//linetrace
}

void (*MainProcess[])(void) = {
	Linetrace,Obstacle,Passage,Downhill,Shelter,WakeUp
};


/*�R���g���[���v���Z�X(����)*/
void Linetrace_cp(void)
{
	//��Q���ւ̈ڍs
	if(touch.cur!=nothing)
	{
		if((scorpion.motor_status|TURN_RIGHT)&&touch.cur==right_touch)
		{
			scorpion.Oflag = 1;
		}else if((scorpion.motor_status|TURN_LEFT)&&touch.cur==left_touch)
		{
			scorpion.Oflag = -1;
		}
		scorpion.mode = 1;
	}
}

void Obstacle_cp(void)
{
	if(scorpion.Oflag==0 && touch.cur==nothing)
	{
		scorpion.mode = 0;
	}
}

void Passage_cp(void){}

void Downhill_cp(void){}

void (*ControlProcess[])(void) = {
	Linetrace_cp,Obstacle_cp,Passage_cp,Downhill_cp,NULL,NULL
};
