/*
 * my_usart.h
 *
 *  Created on: 2 de mar de 2018
 *      Author: Bruno
 */

#ifndef USER_MY_USART_H_
#define USER_MY_USART_H_

#include <stdint.h>

void Init_Usart(void);
void uartPutChar(uint8_t ch);
void uartPutData(uint8_t * dataPtr, uint32_t dataLen);
uint32_t uartGetData(uint8_t * dataPtr, uint32_t dataLen);
void USART_STR (uint8_t *message);

#endif /* USER_MY_USART_H_ */
