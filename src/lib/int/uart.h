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

//グローバル関数・変数
extern void 	USART_Configuration();				//USARTを初期化
extern void 	USART_PutChar(const char c);		//文字データ1バイトを送信
extern void 	USART_PutStr(const char* str);		//文字列データを送信
extern void 	USART_PutDec(uint16_t i);			//符号なし整数値を文字列データで送信
extern void 	USART_PutDec2(int16_t i);			//符号付き整数値を文字列データで送信
extern void 	USART_PutBin(const uint8_t bin);	//2進数の値を文字列データで送信
extern void 	USART_PutHex(const uint8_t hex);	//16進数の値を文字列データで送信

#endif /* UART_H_ */
