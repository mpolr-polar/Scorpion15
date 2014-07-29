/*
 * hard.c
 *
 *  Created on: 2014/06/14
 *      Author: èsïΩ
 */

#include "hard.h"

LINE_InputTypeDef  line={0,0,0,0,0,0,0,0};
COLOR_InputTypeDef color={{{0,0,0},{0,0,0},{0,0,0}},{{0,0,0},{0,0,0},{0,0,0}},0,0,0,0};
PING_InputTypeDef  ping={{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
TOUCH_InputTypeDef touch={0,0};

void	InitSensor(uint8_t param)
{
	if(param|LINE)
	{
		Line_Configuration();
	}
	if(param|COLOR)
	{
		Color_Configuration();
	}
	if(param|TOUCH)
	{
		Touch_Configuration();
	}
	if(param|PING)
	{
		PING_Configuration();
	}
	scorpion.enabled_sensers = param;
}

void GetSensorData(uint8_t param)
{
	if(param|LINE)
	{
		Line_Read();
	}
	if(param|COLOR)
	{
//		Color_Read();
	}
	if(param|TOUCH)
	{
		Touch_Read();
	}
	if(param|PING)
	{
		PING_Read_ast();
	}
}
