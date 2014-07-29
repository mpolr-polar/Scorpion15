/*
 * system.c
 *
 *  Created on: 2014/06/07
 *      Author: 峻平
 */

#include "system.h"

GPIO_InitTypeDef 		GPIO_InitStructure;
USERSWITCH_InitTypeDef	user;
System_StatusTypeDef	scorpion = {0,0,0,0,0,0};
POINT_DataTypeDef		point[SAVE_POINT],curpo;
//curpo:現在のpoint値(cmps.shiftは最初に設定した値)

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

/* メインプロセス */
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

	//error:ラインから外れた時にmotor.Rdutyに補正を加える
	static uint8_t error=0;
	static int8_t  turn=0;
	switch(line.dec_cur)
	{
	case NO_LINE:	//線が途切れた時
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

	default:		//線が外れていない時(通常のライントレース)
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

	GetSensorData(PING|TOUCH);										//ここでセンサーデータを読む

	if(touch.cur!=0){
		switch(touch.cur){
		case left_touch://左のタッチセンサーが反応したとき

			switch(scorpion.Oflag){
			case 0:	//障害物のフラグが立っていなかったとき
//				if(scorpRduty<0)//左に曲がろうとしていたとき;

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
			case 1://右に障害物があるフラグが立っていたとき
				if(ping.centi_cur[FRONT]<DISTANCE){	//	前に障害物がある
					scorpion.Oflag=0;
					goto frontlabel;
				}
				if(ping.centi_cur[P_RIGHT]>=DISTANCE){//右側に障害物が接近してなかったとき
					right=-OSPEED;
					left=0;
				}else{											//接近していたとき
					scorpion.Oflag=2;
					 right=-OSPEED;
					 left=0;
				}
				break;
			case -1://左に障害物があるフラグが立っていたとき
				right=-OSPEED;
				left=0;
				break;
			case 10://右旋回中のフラグが立っていたとき
				right=-OSPEED;
				left=0;
				break;
			case -10://左旋回中のフラグが立っていたとき
				scorpion.Oflag=2;
				right=-OSPEED;
				left=0;
				break;
			case 2://両側に障害物があるフラグが立っていたとき
				right=-OSPEED;
				left=0;
				break;
			default:
				right=0;
				left=0;
				break;
			}

			break;
		case right_touch://右のタッチセンサーが反応したとき

			switch(scorpion.Oflag){
			case 0://障害物のフラグが立っていなかったとき
//				if(scorpion.Rduty>=0)//右に曲がろうとしていたとき;

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
			case 1://右に障害物があるフラグが立っていたとき
				right=0;
				left=-OSPEED;
				break;
			case -1:///左に障害物があるフラグが立っていたとき
				if(ping.centi_cur[FRONT]<DISTANCE){	//	前に障害物がある
					scorpion.Oflag=0;
					goto frontlabel;
				}
				if(ping.centi_cur[P_LEFT]>=DISTANCE){//左側に障害物が接近してなかったとき
					right=0;
					left=-OSPEED;
				}else{											//接近していたとき
					scorpion.Oflag=2;
					 right=0;
					 left=-OSPEED;
				}
				break;
			case 10://右旋回中のフラグが立っていたとき
				scorpion.Oflag=2;
				right=0;
				left=-OSPEED;
				break;
			case -10://左旋回中のフラグが立っていたとき
				right=0;
				left=-OSPEED;
				break;
			case 2:	//両側に障害物がある
				right=0;
				left=-OSPEED;
				break;
			default:
				right=0;
				left=0;
				break;
			}

			break;
		default://両方のタッチセンサーが反応したとき
			if(ping.centi_cur[P_FRONT]<DISTANCE){	//距離センサーで前にあるかどうかチェック

				frontlabel:
				switch(scorpion.Oflag){
				case 0://障害物のフラグが立っていなかったとき
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
				case 1://右に障害物があるフラグが立っていたとき
					right=0;
					left=-OSPEED;
					break;
				case -1://左に障害物があるフラグが立っていたとき
					right=-OSPEED;
					left=0;
					break;
				case 10://右旋回中のフラグが立っていたとき
					right=0;
					left=0;
					break;
				case -10://左旋回中のフラグが立っていたとき
					right=-OSPEED;
					left=0;
					break;
				case 2:
					if(ping.centi_cur[P_RIGHT]>DISTANCE){//右に障害物が接近していないとき
						scorpion.Oflag=-1;
						right=-OSPEED;
						left=0;
					}else{										//接近しているとき
						if(ping.centi_cur[P_LEFT]>DISTANCE){//左に障害物が接近していないとき
							scorpion.Oflag=1;
							right=0;
							left=-OSPEED;
						}else{										//接近しているとき(袋小路)
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
			}else{								//前に障害物がなかったとき,つまり両側に障害物があるとき
				scorpion.Oflag=2;
				right=OSPEED;
				left=OSPEED;
			}

			break;
		}
	}else{
		switch(scorpion.Oflag){
//		case 0:	//障害物のフラグが立っていなかったとき
//			scorpion.mode=0;
//			break;
		case 1://右に障害物があるフラグが立っていたとき
			if(ping.centi_cur[P_RIGHT]<DISTANCE){//右に障害物が接近しているとき
				right=OSPEED;
				left=OSPEED/4;
			}else{
				if(ping.centi_cur[P_RIGHT]>DISTANCE){//接近していないとき
					right=OSPEED/2;
					left=OSPEED;
				}else{										//ジャスト!
					right=OSPEED;
					left=OSPEED;
				}
			}
			if(line.bin_cur&0b00001111){//ラインが右側に検出されたとき
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
		case -1://左に障害物があるフラグが立っていたとき
			if(ping.centi_cur[P_LEFT]<DISTANCE){//左に障害物が接近しているとき
				right=OSPEED/4;
				left=OSPEED;
			}else{
				if(ping.centi_cur[P_LEFT]>DISTANCE){//接近していないとき
					right=OSPEED;
					left=OSPEED/2;
				}else{										//ジャスト!
					right=OSPEED;
					left=OSPEED;
				}
			}
			if(line.bin_cur& 0b11110000){//ラインが左側に検出されたとき
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
		case 10://右旋回中のフラグが立っていたとき
			if(ping.centi_cur[P_LEFT]<DISTANCE){//左側に障害物が接近しているとき
				scorpion.Oflag=-1;
				right=OSPEED;
				left=OSPEED/4;
			}else{
				right=-OSPEED;
				left=0;
			}

			break;
		case -10://左旋回中のフラグが立っていたとき
			if(ping.centi_cur[P_RIGHT]<DISTANCE){//右側に障害物が接近しているとき
				scorpion.Oflag=1;
				right=OSPEED/4;
				left=OSPEED;
			}else{
				right=0;
				left=-OSPEED;
			}
			break;
		case 2://両側に障害物があるフラグが立っていたとき
			if(ping.centi_cur[P_RIGHT]>DISTANCE){//右側に障害物が接近していないとき
				if(ping.centi_cur[P_LEFT]>DISTANCE){//左側も接近していないとき
					if(line.bin_cur!=0)
						scorpion.Oflag=0;
					right=OSPEED;
					left=OSPEED;
				}else{										//左は接近しているとき
					scorpion.Oflag=-1;
					right=OSPEED;
					left=0;
				}
			}else{										//右側に障害物が接近しているとき
				if(ping.centi_cur[P_LEFT]>DISTANCE){//左側は接近していないとき
					scorpion.Oflag=1;
					right=0;
					left=OSPEED;
				}else{										//左も接近しているとき
					if(ping.cur[P_RIGHT]>ping.cur[P_LEFT]){//↓広い方に移動↓
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
	RotateMotor(left,right);										//実際にモーターを動かすのはここ(回り込みなどの例外は除く)
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


/*コントロールプロセス(分岐)*/
void Linetrace_cp(void)
{
	//障害物への移行
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
