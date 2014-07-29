/*
 * motor.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "lib/act.h"

#define GetMotorSwitch()	((GPIOE->IDR&GPIO_Pin_4)?(1):(0))

extern void Motor_Configuration();
extern void RotateMotor(int16_t left, int16_t right);
extern void Move(int16_t Rduty, int16_t Mduty);
extern void Brake(uint8_t motor);

typedef enum motor{
	right_motor = 0b01,
	left_motor  = 0b10,
	both_motors = 0b11
}MOTOR;

#endif /* MOTOR_H_ */
