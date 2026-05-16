/*
 * ring_buffer.c
 *
 *  Created on: 9 Eyl 2023
 *      Author: mfati
 */

#include "ring_buffer.h"

void Ring_Buffer_Initialization(Ring_Buffer_t *pRingBuffer, uint8_t *buffer, uint16_t length)
{
	pRingBuffer->writePoint = 0;
	pRingBuffer->readPoint  = 0;
	pRingBuffer->length		= length;
	pRingBuffer->pBuffer	= buffer;
}

bool Ring_Buffer_Is_Empty(Ring_Buffer_t *pRingBuffer)
{
	return (pRingBuffer->writePoint == pRingBuffer->readPoint) ? true : false;
}

bool Ring_Buffer_Is_Fully(Ring_Buffer_t *pRingBuffer)
{
	int div = (pRingBuffer->writePoint - pRingBuffer->readPoint);

	if(div < 0)
		div = pRingBuffer->length + div;

	return (div == (pRingBuffer->length - 1)) ? true : false;
}

bool Ring_Buffer_Add(Ring_Buffer_t *pRingBuffer, unsigned char ch)
{
	bool retVal = false;

	if(!Ring_Buffer_Is_Fully(pRingBuffer))
	{
		pRingBuffer->pBuffer[pRingBuffer->writePoint] = ch;
		pRingBuffer->writePoint++;

		if(pRingBuffer->writePoint == pRingBuffer->length)
			pRingBuffer->writePoint = 0;

		retVal = true;
	}

	return retVal;
}

int  Ring_Buffer_Subtract(Ring_Buffer_t *pRingBuffer)
{
	int ch = -1;

	if(!Ring_Buffer_Is_Empty(pRingBuffer))
	{
		ch = pRingBuffer->pBuffer[pRingBuffer->readPoint];
		pRingBuffer->readPoint++;

		if(pRingBuffer->readPoint == pRingBuffer->length)
			pRingBuffer->readPoint = 0;
	}

	return ch;
}

void Ring_Buffer_Clear(Ring_Buffer_t *pRingBuffer)
{
	pRingBuffer->writePoint = 0;
	pRingBuffer->readPoint = 0;
}
