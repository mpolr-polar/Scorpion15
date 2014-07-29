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
#define Baudrate	115200

//グローバル関数・変数
extern void 	USART_Configuration();
extern void 	USART_PutChar(const char c);
extern void 	USART_PutStr(const char* str);
extern void 	USART_PutDec(uint16_t i);
extern void 	USART_PutDec2(int16_t i);
extern void 	USART_PutBin(const uint8_t bin);
extern void 	USART_PutHex(const uint8_t hex);

#endif /* UART_H_ */
