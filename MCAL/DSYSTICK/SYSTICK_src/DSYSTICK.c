/*
 * DSYSTICK.c
 *
 *  Created on: Mar 8, 2020
 *      Author: Mohanad Sallam
 */
#include "STD_TYPES.h"
#include "DSYSTICK.h"
#include "DSYSTICK_config.h"

typedef struct 
{
	uint_32t CTRL;
	uint_32t LOAD;
	uint_32t VAL;	
}STK_t;
#define STK   ((volatile STK_t*)0xE000E010)

systickcbf_t APPcbf ;

#define SYSTICK_ENABLE   0x00000001
#define SYSTICK_DISABLE  0xFFFFFFFE

#define PRESCALE_RATIO1	 1000000
#define PRESCALE_RATIO2	 8000000ULL
uint_8t SYSTICK_Init (void)
{
	STK->CTRL |= SYSTICK_CLK_SRC | SYSTICK_TICK_INT;
	return OK;
}
uint_8t SYSTICK_Start (void)
{
	STK->CTRL |= SYSTICK_ENABLE;
	return OK;
}
uint_8t SYSTICK_Stop (void)
{
	STK->CTRL &= SYSTICK_DISABLE;
	return OK;
}
uint_8t SYSTICK_SetCallback (systickcbf_t cbf)
{
	if(cbf)
	{
		APPcbf = cbf;
		return OK;
	}
	else
	{
		return NOT_OK;
	}
}
uint_8t SYSTICK_SetTime (uint_32t timeus,uint_32t clk)
{
	if (STK->CTRL & SYSTICK_CLK_AHB)
	{
		STK->LOAD=(timeus * clk)/PRESCALE_RATIO1;
	}
	else if (!(STK->CTRL & SYSTICK_CLK_AHB))
	{
		STK->LOAD=((uint_64t)timeus * (uint_64t)clk)/PRESCALE_RATIO2;
	}
	else
	{
		return NOT_OK;
	}
	return OK;
}
void SysTick_Handler(void)
{
	if (APPcbf)
	{
		APPcbf();
	}
	else
	{
		/*Do nothing*/
	}
}
