/*
 * uart.h
 *
 *  Created on: 2014/05/31
 *      Author: mpolr
 */

#ifndef UART_H_
#define UART_H_

#include "lib/int.h"

#define Indention	USART_PutStr("\t")
#define Linefeed	USART_PutStr("\n\r")
#define Baudrate	9600

//�O���[�o���֐��E�ϐ�
extern void 	USART_Configuration();				//USART��������
extern void 	USART_PutChar(const char c);		//�����f�[�^1�o�C�g�𑗐M
extern void 	USART_PutStr(const char* str);		//������f�[�^�𑗐M
extern void 	USART_PutDec(uint16_t i);			//�����Ȃ������l�𕶎���f�[�^�ő��M
extern void 	USART_PutDec2(int16_t i);			//�����t�������l�𕶎���f�[�^�ő��M
extern void 	USART_PutBin(const uint8_t bin);	//2�i���̒l�𕶎���f�[�^�ő��M
extern void 	USART_PutHex(const uint8_t hex);	//16�i���̒l�𕶎���f�[�^�ő��M

#endif /* UART_H_ */
