/*
 * timer_ex.h
 *
 *  Created on: May 10, 2026
 *      Author: theyu
 */

#ifndef OUR_DRIVERS_TIMER_EX_INC_TIMER_EX_H_
#define OUR_DRIVERS_TIMER_EX_INC_TIMER_EX_H_

#include "stdint.h"
#include "stdbool.h"

typedef struct
{
	uint32_t startTick;
	uint32_t interval;
	bool	 activated;
}Tick_Time_t;

void Tick_Timer_Initialization(Tick_Time_t *time, uint32_t intervalMs);
void Tick_Timer_Set_Time(Tick_Time_t *time, uint32_t intervalMs);
void Tick_Timer_Stop_Time(Tick_Time_t *time);
bool Tick_Timer_Check_Elapsed_Time(Tick_Time_t *time);

#endif /* OUR_DRIVERS_TIMER_EX_INC_TIMER_EX_H_ */
