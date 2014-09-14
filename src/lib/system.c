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
POINT_DataTypeDef		point[SAVE_POINT],curpo;//curpo:現在のpoint値(cmps.shiftは最初に設定した値)
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

int16_t gyro_ypr[3]={0,0,0};
int16_t ypr_log[3][11] = {{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0}};
double gdata[3]={0,0,0},grate[3]={0,0,0};

void System_Configuration(uint8_t mode)
{
	SystemInit();
	delay_init();
	OnBoardLED_Init();
	UserSwitch_Init();
	InitKalman(&yaw);
	InitKalman(&pitch);
	InitKalman(&roll);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,  ENABLE);
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period    = 0xffffffff - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 42 - 1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3, ENABLE);

	switch(mode)
	{
	case active_all:
		InitSensor(ALL_SENSOR);
		InitModule(ALL_MODULE);
		Motor_Configuration();
		break;

	case debug_mode:
		InitSensor(ALL_SENSOR);
		InitModule(ACC|GYRO);
		USART_Configuration();
		break;

	case linetrace:
		InitSensor(LINE|PING);
		USART_Configuration();
		Motor_Configuration();
		break;

	case yawpitchroll:
		InitModule(ACC|GYRO);
		USART_Configuration();
		break;

	default:
		break;
	}
	scorpion.wakeup_mode = mode;
	scorpion.mode = 5;//WakeUp
}

void Debug(uint8_t device)
{
	static uint8_t count=0;
	static int time=0;
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

	case accelerometer:
		GetModuleData(ACC);
		USART_PutStr("Accel : ");
		USART_PutDec2(acc.cur[x_data]);Indention;
		USART_PutDec2(acc.cur[y_data]);Indention;
		USART_PutDec2(acc.cur[z_data]);Linefeed;
		break;

	case gyrosensor:
		GetModuleData(GYRO);
		USART_PutStr("GYRO : ");
		USART_PutDec2(gyro.cur[yaw_data]);Indention;
		USART_PutDec2(gyro.cur[pitch_data]);Indention;
		USART_PutDec2(gyro.cur[roll_data]);Linefeed;
		break;

	case yawpitchroll:
		time = TIM3->CNT;
		GetYPRData();
		USART_PutStr("YPR : ");
		USART_PutDec2(scorpion.yaw[0]);Indention;
		USART_PutDec2(scorpion.pitch[0]);Indention;
		USART_PutDec2(scorpion.roll[0]);Indention;
		USART_PutDec2((int16_t)yaw.angle);Indention;
		USART_PutDec2((int16_t)pitch.angle);Indention;
		USART_PutDec2((int16_t)roll.angle);Indention;
		USART_PutDec(time);Linefeed;
		break;

	case speedtest:
		USART_PutStr("Now measuring the input speed...");Linefeed;
		TIM3->CNT = 0;
		for(count=0;count<10;count++)
		{
			GetSensorData(scorpion.enabled_sensers);
			GetModuleData(scorpion.enabled_modules);
		}
		time = TIM3->CNT;
		USART_PutStr("Once all input takes for ");
		USART_PutDec(time);

	default:
		USART_PutStr("Designated an enabled value.");Linefeed;
		break;
	}
}

void ProcessInput()
{
	GetSensorData(ALL_SENSOR);
	GetModuleData(ALL_MODULE);
	GetYPRData();
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

void SavePoint(uint16_t tag)
{
	uint8_t i=0;
	for(i=0;i<=scorpion.point_num;i++){if(point[i].tag==tag)return;}
	if(tag!=(ENTRANCE|FIRST_ROOM))
	{
		point[scorpion.point_num].line = line.bin_cur;
		point[scorpion.point_num].touch = touch.cur;
		point[scorpion.point_num].color = color.bin_cur;
		point[scorpion.point_num].ypr[yaw_data] = scorpion.yaw[0];
		point[scorpion.point_num].ypr[pitch_data] = scorpion.pitch[0];
		point[scorpion.point_num].ypr[roll_data] = scorpion.roll[0];
		for(i=0;i<4;i++){point[scorpion.point_num].ping[i] = ping.cur[i];}
	}else
	{
		Cmps_Reset(0);
		GetYPRData();
		point[scorpion.point_num].line = 0;
		point[scorpion.point_num].touch = 0;
		point[scorpion.point_num].color = 0;
		point[scorpion.point_num].ypr[yaw_data] = 0;
		point[scorpion.point_num].ypr[pitch_data] = 0;
		point[scorpion.point_num].ypr[roll_data] = 0;
		for(i=0;i<4;i++){point[scorpion.point_num].ping[i] = 0;}
	}
	point[scorpion.point_num].tag = tag;
	scorpion.point_num++;
}

void GetYPRData()
{
	int8_t i=0,j=0;
	double dt=0;
	for(i=10;i>=0;i--){
		scorpion.yaw[i+1]=scorpion.yaw[i];
		scorpion.pitch[i+1]=scorpion.pitch[i];
		scorpion.roll[i+1]=scorpion.roll[i];
	}
	for(i=0;i<3;i++){for(j=10;j>=0;j--){ypr_log[i][j+1]=ypr_log[i][j];}}
	if(scorpion.enabled_modules&CMPS)
	{
		GetModuleData(CMPS);
		scorpion.yaw[0]   = ((cmps.dir_3600/10)>180)?((cmps.dir_3600/10)-360):((cmps.dir_3600/10)<-180)?((cmps.dir_3600/10)+360):(cmps.dir_3600/10);
	}
	if(scorpion.enabled_modules&ACC)
	{
		GetModuleData(ACC);
		scorpion.pitch[0] = (DEGREES(atan2(acc.cur[y_data],acc.cur[z_data])));
		scorpion.roll[0]  = (DEGREES(atan2(acc.cur[x_data],acc.cur[z_data])));
	}
	if(scorpion.enabled_modules&GYRO)
	{
		GetModuleData(GYRO);
		for(i=0;i<3;i++){
			grate[i] = (double)(gyro.cur[i])*GyroDPSRate;
			dt = (double)(gyro.time/2000000.0);
			gdata[i]+=(grate[i]*dt);
		}
		if(scorpion.enabled_modules&CMPS){
			getAngle(&yaw,scorpion.yaw[0],grate[0],dt);
			scorpion.yaw[0]   = (scorpion.yaw[1]+(int16_t)(grate[0]*dt))*0.93 + scorpion.yaw[0]*0.07;
		}else{
			scorpion.yaw[0] = (int16_t)gdata[0];
		}
		if(scorpion.enabled_modules&ACC){
			getAngle(&pitch,scorpion.pitch[0],grate[1],dt);
			getAngle(&roll,scorpion.roll[0],grate[2],dt);
			scorpion.pitch[0] = (int16_t)(gdata[1]*0.93 + scorpion.pitch[0]*0.07);
			scorpion.roll[0]  = (int16_t)(gdata[2]*0.93 + scorpion.roll[0]*0.07);
		}else{
			scorpion.pitch[0] = (int16_t)gdata[1];
			scorpion.roll[0]  = (int16_t)gdata[2];
		}
	}
/*	static uint64_t now_time=0,timer=0;
	for(i=0;i<6;i++){
		ypr_log[0][i+1]=ypr_log[0][i];ypr_log[1][i+1]=ypr_log[1][i];ypr_log[2][i+1]=ypr_log[2][i];
	}
	if(scorpion.enabled_modules&CMPS)
	{//加速度とジャイロとコンパス
		GetModuleData(ACC|GYRO|CMPS);
		yaw.rate =   (double)((gyro.cur[yaw_data]*0.01750)/131.0);
		pitch.rate = (double)((gyro.cur[pitch_data]*0.01750)/131.0);
		roll.rate =  (double)((gyro.cur[roll_data]*0.01750)/131.0);

		yaw.value   = ((cmps.dir_3600/10)>180)?((cmps.dir_3600/10)-360):((cmps.dir_3600/10)<-180)?((cmps.dir_3600/10)+360):(cmps.dir_3600/10);
		pitch.value = (DEGREES(atan2(acc.cur[y_data],acc.cur[z_data])));
		roll.value  = (DEGREES(atan2(acc.cur[x_data],acc.cur[z_data])));

		ypr_log[yaw_data][0]   = getAngle(&yaw,yaw.value,yaw.rate,(double)(TIM3->CNT + now_time*0xffff-timer)/1000000);
		ypr_log[pitch_data][0] = getAngle(&pitch,pitch.value,pitch.rate,(double)(TIM3->CNT + now_time*0xffff-timer)/1000000);
		ypr_log[roll_data][0]  = getAngle(&roll,roll.value,roll.rate,(double)(TIM3->CNT + now_time*0xffff-timer)/1000000);
	}else if(scorpion.enabled_modules&GYRO)
	{//加速度とジャイロ
		GetModuleData(ACC|GYRO);
		yaw.rate =   (double)((gyro.cur[yaw_data]*0.01750)/131.0);
		pitch.rate = (double)((gyro.cur[pitch_data]*0.01750)/131.0);
		roll.rate =  (double)((gyro.cur[roll_data]*0.01750)/131.0);

		pitch.value = (DEGREES(atan2(acc.cur[y_data],acc.cur[z_data])));
		roll.value  = (DEGREES(atan2(acc.cur[x_data],acc.cur[z_data])));

		ypr_log[pitch_data][0] = getAngle(&pitch,pitch.value,pitch.rate,(double)(TIM3->CNT + now_time*0xffff-timer)/1000000);
		ypr_log[roll_data][0]  = getAngle(&roll,roll.value,roll.rate,(double)(TIM3->CNT + now_time*0xffff-timer)/1000000);
	}else
	{//加速度だけ
		GetModuleData(ACC);
		ypr_log[pitch_data][0] = ((int16_t)DEGREES(atan2(acc.cur[y_data],acc.cur[z_data])));
		ypr_log[pitch_data][0]  = ((int16_t)DEGREES(atan2(acc.cur[x_data],acc.cur[z_data])));
	}
	timer = TIM3->CNT + now_time*0xffff;
	for(i=0;i<3;i++)
	{
		for(j=0;j<7;j++)
		{
			s_ypr[i][j] = ypr_log[i][j];
		}
		qsort((void*)(s_ypr[i]),(size_t)7,sizeof(s_ypr[i][0]),int_sort);
	}
	scorpion.yaw = s_ypr[yaw_data][3];
	scorpion.pitch = s_ypr[pitch_data][3];
	scorpion.roll = s_ypr[roll_data][3];*/
//	scorpion.yaw[0] = AveYPR(ypr_log[0]);
//	scorpion.pitch[0] = AveYPR(ypr_log[1]);
//	scorpion.roll[0] = AveYPR(ypr_log[2]);
}

int16_t AveYPR(int16_t *data)
{//4 7 3 12 10 5 7
	uint8_t i=0;
	int16_t out=data[0];
	for(i=1;i<10;i++)
	{
		if(pow(out,2) > pow(data[i],2))
		{
			out=data[i];
		}else if(pow(out,2) == pow(data[i],2))
		{
			out=(out+data[i])/2;
		}
	}
	return out;
}

/* メインプロセス */
void Linetrace(void)
{
/*	static int8_t wflag=0,aflag=0;
//	ProcessInput();
	GetSensorData(LINE|TOUCH);
	GetYPRData();
	if(aflag!=0)
	{
		if(aflag>0)
		{
			line.bin_cur &= 0b0000111;
		}else
		{
			line.bin_cur &= 0b1110000;
		}
		line.dec_cur = Translate_Line(line.bin_cur);
	}else if(line.dec_cur!=BLACK_OUT && line.dec_cur!=NO_LINE)
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
	case BLACK_OUT:
		Move(40*turn,40);
		delay_ms(100);
		break;

	case NO_LINE:	//線が途切れた時
		if((line.dec_log==NO_LINE || !(line.bin_log&0b1100011)) && turn==0)
		{
			Move(0,120);
		}else
		{
			error++;
			if(line.bin_log & 0b1100000)
			{
				motor.Rduty += (-40-error);
				line.bin_cur = 0b1000000;
				line.dec_cur = -6;
			}else if(line.bin_log & 0b0000011)
			{
				motor.Rduty += (40+error);
				line.bin_cur = 0b0000001;
				line.dec_cur = 6;
			}
			motor.Mduty = 0;
			Move(motor.Rduty,motor.Mduty);
		}
		break;

	default:		//線が外れていない時(通常のライントレース)
		if(error){error = 0;}
		if((scorpion.enabled_sensers&PING) && ((ping.centi_cur[right_ping]+ping.centi_cur[left_ping]+20)<40) && (pow(line.dec_cur,2)<=1)){wflag=1;}
		else if((scorpion.enabled_sensers&PING) && ((ping.centi_cur[right_ping]+ping.centi_cur[left_ping]+20)>40)){wflag=0;}
		if(wflag==1)
		{
			if(ping.centi_cur[left_ping] < ping.centi_cur[right_ping])
			{
				Move(20,120);
			}else if(ping.centi_cur[left_ping] > ping.centi_cur[right_ping])
			{
				Move(-20,120);
			}else
			{
				Move(0,120);
			}
		}else
		{
			if(line.num_cur < 2)
			{//ラインが1本
				if(line.dec_cur == 0)
				{
					turn = 0;
					Move(0,120);
				}
				else
				{
					if(line.bin_cur&0b0001000)
					{
						Move(line.dec_cur*50,70);
						turn = 0;
					}else
					{
						if((line.dec_cur>0&&turn<0)||(line.dec_cur<0&&turn>0)){motor.Rduty=0;}
						turn = line.dec_cur/ABS(line.dec_cur);
						motor.Rduty += line.dec_cur*3;
						motor.Mduty = 80;
						Move(motor.Rduty,motor.Mduty);
					}
				}
			}else
			{//ラインが2本以上
				if(aflag==0)
				{
					if((aflag=line.dec_cur/ABS(line.dec_cur))>0)
					{
						line.bin_cur &= 0b0000111;
					}else
					{
						line.bin_cur &= 0b1110000;
					}
					line.dec_cur = Translate_Line(line.bin_cur);
				}
				if(line.dec_cur==NO_LINE)
				{
					aflag=0;
				}else
				{
					if((line.dec_cur>0&&turn<0)||(line.dec_cur<0&&turn>0)){motor.Rduty=0;}
					turn = line.dec_cur/ABS(line.dec_cur);
					motor.Rduty += line.dec_cur*ABS(line.dec_cur);
					motor.Mduty = 80-ABS(line.dec_cur)*10;
					Move(motor.Rduty,motor.Mduty);
				}
			}
		}
		break;
	}*/
	static uint8_t error=0;
	static int8_t  turn=0;
	static int8_t  aflag=0;

	GetSensorData(LINE);
	if(aflag!=0){
		if(!line.bin_cur){aflag*=2;}
		line.dec_cur = Translate_Line((line.bin_cur&=(aflag>0)?(0b0000011):(0b1100000)));
	}
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
	switch(line.dec_cur)
	{
	case BLACK_OUT:
		Move(40*turn,40);
		delay_ms(100);
		break;

	case NO_LINE:	//線が途切れた時
		if(aflag*aflag == 4){
			if(aflag > 0){
				line.bin_log = 0b0000011;
			}else{
				line.bin_log = 0b1100000;
			}
			aflag=0;
		}
		if((line.dec_log==NO_LINE || !(line.bin_log&0b1100011)) && turn==0)
		{
			Move(0,120);
		}else
		{
			error++;
			if((line.dec_log>0&&turn<0)||(line.dec_log<0&&turn>0)){motor.Rduty=0;}
			if(line.bin_log & 0b1100000)
			{
				motor.Rduty += (-40-error);
				line.bin_cur = 0b1000000;
				line.dec_cur = -6;
				turn = -1;
			}else if(line.bin_log & 0b0000011)
			{
				motor.Rduty += (40+error);
				line.bin_cur = 0b0000001;
				line.dec_cur = 6;
				turn = 1;
			}
			motor.Mduty = 0;
			Move(motor.Rduty,motor.Mduty);
		}
		break;

	default:		//線が外れていない時(通常のライントレース)
		if(error){error = 0;}
		if(line.dec_cur == 0)
		{
			turn = 0;
			Move(0,120);
		}
		else
		{
			if(line.num_cur == 1)
			{
				if(!(line.bin_cur&0b0001000))
				{
					if((line.dec_cur>0&&turn<0)||(line.dec_cur<0&&turn>0)){motor.Rduty=0;}
					turn = line.dec_cur/ABS(line.dec_cur);
					motor.Rduty += line.dec_cur*3;
					motor.Mduty = 80;
					Move(motor.Rduty,motor.Mduty);

				}else
				{
					motor.Rduty = line.dec_cur*50;
					motor.Mduty = 70;
					Move(motor.Rduty,motor.Mduty);
					turn = 0;
				}
			}else
			{
				if(!aflag){
					aflag=(turn=line.dec_cur/ABS(line.dec_cur));
					line.dec_cur = Translate_Line((line.bin_cur&=0b1100011));
				}
				motor.Rduty += line.dec_cur*3;
				motor.Mduty = 100;
				Move(motor.Rduty,motor.Mduty);
			}
		}
		break;
	}
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
		case 0:	//障害物のフラグが立っていなかったとき
//			scorpion.mode=0;
			break;
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
	static uint8_t i=0,num=0;
	if(num==0){for(i=1;i<scorpion.point_num;i++){if((num=point[i].tag)==(ENTRANCE|PASSAGE)){return;}}}
//	ProcessInput();
	GetSensorData(LINE|PING|TOUCH);
	GetYPRData();
	if(scorpion.Pflag == run_ahead || scorpion.Pflag == run_uphill)
	{//普通の通路

	}else
	{//登る準備
	}
}

void Downhill(void)
{
	GetSensorData(PING);
	GetYPRData();
	if(ping.cur[left_ping] < ping.cur[right_ping])
	{
		Move(8,-40);
	}else if(ping.cur[left_ping] > ping.cur[right_ping])
	{
		Move(-8,-40);
	}else
	{
		Move(0,-40);
	}
	delay_us(100);
}

void Shelter(void)
{

}

void WakeUp(void)
{
	ProcessInput();
	SavePoint(ENTRANCE|FIRST_ROOM);
	while(Get_Line(All_Line)==0b1111111);
	delay_ms(500);
	while(!GetMotorSwitch());

	if(line.dec_cur == NO_LINE)
	{
		Move(0,120);
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
	if(AveYPR(scorpion.pitch)<-15)
	{
		//下り坂
		Brake(both_motors);
		scorpion.mode = 3;
		scorpion.Pflag = ENTRANCE|HIGH_LANDING;
		Move(0,-80);
		delay_ms(5000);
		Move(50,0);
		delay_ms(RT90DEG(50)*2);
		Move(0,-40);
		while(AveYPR(scorpion.pitch) < 20){GetYPRData();}
	}else
	{
		if(line.dec_cur == NO_LINE)
		{
			//ラインが無いとき -> ギャップ or 通路
			if((ping.centi_cur[right_ping]+ping.centi_cur[left_ping]+15) < 40)
			{
				//両側を壁で挟まれている時 -> 通路
				scorpion.mode = 2;
				SavePoint(ENTRANCE|PASSAGE);
			}else
			{
				if(((ping.centi_cur[right_ping]<SDISTANCE)^(ping.centi_cur[left_ping]<SDISTANCE))&&(ping.centi_cur[front_ping]<FDISTANCE))
				{
					//左右どちらか一方に壁が接近していてかつ前方にも壁があった場合 -> 通路
					Move(0,50);
					while(Get_Touch()==nothing)
					{
						if(Get_Line(All_Line)!=NO_LINE)
						{//通路だと思ったが、やっぱりギャップだったよ。
							scorpion.mode = 0;
							break;
						}
					}
					Brake(both_motors);
					Move(((ping.cur[left_ping]>ping.cur[right_ping])?(-50):(50)),0);
					delay_ms(RT90DEG(50));
					SavePoint(ENTRANCE|PASSAGE);
					Move(0,80);
					while(ping.centi_cur[left_ping]+ping.centi_cur[right_ping]+15>40);
					scorpion.mode = 2;//Passage
				}else
				{
					//ギャップ
					if(!(line.dec_cur))
					{//機体の向き補正
						Brake(both_motors);
						delay_ms(500);
						Move(0,-80);
						delay_ms(1000);
						Brake(both_motors);
						delay_ms(500);
					}else
					{//ギャップを直進
						Move(0,80);
						while(Get_Line(All_Line)==0);
					}
				}
			}
		}else
		{
			if(touch.cur!=nothing)
			{//障害物への移行
				scorpion.mode = 1;
			}
		}
	}
}

void Obstacle_cp(void)
{
	if(scorpion.Oflag==0 && touch.cur==nothing)
	{
		scorpion.mode = 0;
	}
}

void Passage_cp(void)
{
	if(scorpion.Pflag==run_ahead && AveYPR(scorpion.pitch)>-10)
	{//通路もしくは上り坂
		if((ping.centi_cur[right_ping]+ping.centi_cur[left_ping]+15)>40 && line.dec_cur!=NO_LINE)
		{
			scorpion.mode = 0;	//linetrace
			scorpion.Pflag = 0;
		}else if((ping.centi_cur[right_ping]+ping.centi_cur[left_ping]+15)>40)
		{
			Move(0,50);
			while(Get_Touch()==front_touch);
			Brake(both_motors);
			delay_ms(500);
			Move(0,(ping.cur[left_ping]<ping.cur[right_ping])?(1):(-1)*50);
			delay_ms(RT90DEG(50));
			Brake(both_motors);
			delay_ms(500);
			SavePoint(ENTRANCE|SECOND_ROOM);
			Move(0,80);
			while(Get_Line(All_Line)==0);
			scorpion.mode = 0;
		}
	}else
	{//下り坂
		Brake(both_motors);
		Move(0,-50);
		delay_ms(2000);
		Move(50,0);
		delay_ms(2*RT90DEG(50));
		Move(0,-50);
		while(AveYPR(scorpion.pitch) < 10){GetYPRData();}
		Move(0,-30);
		while(AveYPR(scorpion.pitch) > 10){GetYPRData();}
	}
}

void Downhill_cp(void)
{
	if(pow(AveYPR(scorpion.pitch),2) <= 1)
	{
		Move(50,0);
		delay_ms(RT90DEG(50)*2);
		if(Get_Line(All_Line)&0b1111111){scorpion.mode = 0;}
		else
		{
			Move(0,80);
			while(ping.centi_cur[left_ping]+ping.centi_cur[right_ping]+20 < 40){GetSensorData(PING);}
			delay_ms(500);
			Move(-50,0);
			delay_ms(RT90DEG(50));
			Move(0,80);
			while(!(Get_Line(All_Line)&0b1111111));
			scorpion.mode = 0;
		}
	}
}

void (*ControlProcess[])(void) = {
	Linetrace_cp,Obstacle_cp,Passage_cp,Downhill_cp,NULL,NULL
};


/*
 *         +-+->Downhill--+
 * 		   | +->Passage---++
 *         | |			   ↓
 * Start-->+-+--Linetrace--+-+-->Shelter--->END
 * 		   ↑ |			   ↑ |
 * 		   | +->Obstacle---+ |
 * 		   +<----------------+
 */
