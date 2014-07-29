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
	//addsp:直線の時、motor.Mdutyに補正を加える。(motor.Rdutyの最大値は100)
	static uint8_t error=0,addsp=0;
	static int8_t  turn=0;
	switch(line.dec_cur)
	{
	case NO_LINE:	//線が途切れた時
		if(line.dec_log==NO_LINE || line.dec_log*line.dec_log < 25)
		{
			Move(0,0);
			error = 0;
			addsp = 0;
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
			if(addsp){addsp=0;}
			motor.Rduty += line.dec_cur*ABS(line.dec_cur);
			motor.Mduty = 80-line.dec_cur*10;
			Move(motor.Rduty,motor.Mduty);
		}else
		{
			if((line.bin_cur&0b0001000)!=0 && (line.bin_cur&0b0010100)==0)
			{
				if((line.dec_cur>0&&turn<0)||(line.dec_cur<0&&turn>0)){motor.Rduty=0;}
				turn = line.dec_cur/ABS(line.dec_cur);
				if(addsp>20){addsp=20;}
				Move(0,80+addsp);
				addsp+=2;
				motor.Rduty = 0;
			}else
			{
				turn=0;
				Move(line.dec_cur*ABS(line.dec_cur),80);
			}
		}
		break;
	}
	delay_us(100);
}

void Obstacle(void)
{

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
		Move(0,80);
		while(Get_Line(All_Line)==0);
	}
}

void (*MainProcess[])(void) = {
	Linetrace,Obstacle,Passage,Downhill,Shelter,WakeUp
};


/*コントロールプロセス(分岐)*/
void Linetrace_cp(void){}

void Obstacle_cp(void){}

void Passage_cp(void){}

void Downhill_cp(void){}

void Shelter_cp(void){}

void WakeUp_cp(void)
{
	scorpion.mode = 0;//linetrace
}

void (*ControlProcess[])(void) = {
	Linetrace_cp,Obstacle_cp,Passage_cp,Downhill_cp,Shelter_cp,WakeUp_cp
};
