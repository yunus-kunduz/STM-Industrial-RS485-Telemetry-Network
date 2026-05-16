/*
 * uart_ex.h
 *
 *  Created on: 9 Eyl 2023
 *      Author: mfati
 */

#ifndef OUR_DRIVERS_UART_EX_INC_UART_EX_H_
#define OUR_DRIVERS_UART_EX_INC_UART_EX_H_

#include "main.h"
#include "ring_buffer.h"

#define UART_TX_INT						1 << 7
#define UART_TX_EMPTY					1 << 7

typedef struct
{
	UART_HandleTypeDef *huart;
	Ring_Buffer_t	   *ringBufferIn;
	Ring_Buffer_t      *ringBufferOut;
}Uart_Comm_t;

void UARTx_Initialization(Uart_Comm_t *uart);
void UARTx_Write_Single_Char(Uart_Comm_t *uart, char ch);
void UARTx_Write_String(Uart_Comm_t *uart, char *str);

#endif /* OUR_DRIVERS_UART_EX_INC_UART_EX_H_ */
