/*
 * uart_qrbcom.h
 *
 *  Created on: 10 Eyl 2023
 *      Author: mfati
 */

#ifndef OUR_DRIVERS_UART_EX_INC_UART_QRBCOM_H_
#define OUR_DRIVERS_UART_EX_INC_UART_QRBCOM_H_

#include "uart_ex.h"

#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define BUFF_SIZE							1024

void UARTx_Cell_Comm_Initialization(void);
int UARTx_Printf(const char *format, ...);

#endif /* OUR_DRIVERS_UART_EX_INC_UART_QRBCOM_H_ */
