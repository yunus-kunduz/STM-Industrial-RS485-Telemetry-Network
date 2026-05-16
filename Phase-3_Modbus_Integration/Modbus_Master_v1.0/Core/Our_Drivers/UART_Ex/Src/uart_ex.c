/*
 * uart_ex.c
 *
 *  Created on: 9 Eyl 2023
 *      Author: mfati
 */

#include "uart_ex.h"

Ring_Buffer_t *usart2In;
Ring_Buffer_t *usart2Out;

extern UART_HandleTypeDef huart2;

void UARTx_Initialization(Uart_Comm_t *uart)
{
	if(uart->huart == &huart2)
	{
		usart2In  = uart->ringBufferIn;
		usart2Out = uart->ringBufferOut;
	}
}

void UARTx_Write_Single_Char(Uart_Comm_t *uart, char ch)
{
	if(Ring_Buffer_Is_Fully(uart->ringBufferOut))
	{
		if(!(uart->huart->Instance->CR1 & UART_TX_INT))
		{
			HAL_UART_Transmit_IT(&huart2, (uint8_t*)Ring_Buffer_Subtract(uart->ringBufferOut), 1);
		}
		while(Ring_Buffer_Is_Fully(uart->ringBufferOut));
	}

	Ring_Buffer_Add(uart->ringBufferOut, ch);
}

void UARTx_Write_String(Uart_Comm_t *uart, char *str)
{
	int check;
	char *ptr;
	ptr = str;

	while(*ptr)
	{
		UARTx_Write_Single_Char(uart, *ptr);
		ptr++;
	}

	if(!(uart->huart->Instance->CR1 & UART_TX_INT))
	{
		check = Ring_Buffer_Subtract(uart->ringBufferOut);

		if(check > -1)
		{
			uart->huart->Instance->DR = (uint8_t)(check & 0xFF);
			__HAL_UART_ENABLE_IT(uart->huart, UART_IT_TXE);
		}
	}
}


