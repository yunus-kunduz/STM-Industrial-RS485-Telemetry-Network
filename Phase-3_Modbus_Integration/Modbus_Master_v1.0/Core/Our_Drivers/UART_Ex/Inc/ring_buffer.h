/*
 * ring_buffer.h
 *
 *  Created on: 9 Eyl 2023
 *      Author: mfati
 */

#ifndef OUR_DRIVERS_UART_EX_INC_RING_BUFFER_H_
#define OUR_DRIVERS_UART_EX_INC_RING_BUFFER_H_

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

typedef struct
{
	unsigned char *pBuffer;
	uint32_t 	   writePoint;
	uint32_t 	   readPoint;
	uint16_t	   length;
}Ring_Buffer_t;

void Ring_Buffer_Initialization(Ring_Buffer_t *pRingBuffer, uint8_t *buffer, uint16_t length);
bool Ring_Buffer_Is_Empty(Ring_Buffer_t *pRingBuffer);
bool Ring_Buffer_Is_Fully(Ring_Buffer_t *pRingBuffer);
bool Ring_Buffer_Add(Ring_Buffer_t *pRingBuffer, unsigned char ch);
int  Ring_Buffer_Subtract(Ring_Buffer_t *pRingBuffer);
void Ring_Buffer_Clear(Ring_Buffer_t *pRingBuffer);

#endif /* OUR_DRIVERS_UART_EX_INC_RING_BUFFER_H_ */
