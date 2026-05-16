/*
 * uart.qrbcom.c
 *
 *  Created on: 10 Eyl 2023
 *      Author: mfati
 */

#include "uart_qrbcom.h"

Ring_Buffer_t usart2_InRb;
uint8_t 	  usart2_inRingBuff[BUFF_SIZE];

Ring_Buffer_t usart2_OutRb;
uint8_t		  usart2_outRingBuff[BUFF_SIZE];

Uart_Comm_t uartComm;

extern UART_HandleTypeDef huart2;

void UARTx_Cell_Comm_Initialization(void)
{
	// Ring Buffer Başlat
	Ring_Buffer_Initialization(&usart2_InRb,  usart2_inRingBuff,  BUFF_SIZE);
	Ring_Buffer_Initialization(&usart2_OutRb, usart2_outRingBuff, BUFF_SIZE);
	// Uart Başlat
	uartComm.huart 			= &huart2;
	uartComm.ringBufferIn 	= &usart2_InRb;
	uartComm.ringBufferOut 	= &usart2_OutRb;

	UARTx_Initialization(&uartComm);

	__HAL_UART_ENABLE_IT(uartComm.huart, UART_IT_RXNE);
}

#define PRINT_BUFFER_SIZE		256
char    printBuffer[PRINT_BUFFER_SIZE];
va_list arg;

int UARTx_Printf(const char *format, ...)
{
	int status;

	va_start(arg, format);
	status = vsnprintf(printBuffer, PRINT_BUFFER_SIZE, format, arg);
	va_end(arg);

	UARTx_Write_String(&uartComm, printBuffer);

	return status;
}





