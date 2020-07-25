/*
 * DNVIC.c
 *
 *  Created on: Feb 22, 2020
 *      Author: Mohanad Sallam
 */
#include "STD_TYPES.h"
#include "DNVIC.h"

#define SCB_AIRCR *((volatile uint_32t*) 0XE000ED0C)
#define SCB_VTOR  *((volatile uint_32t*) 0xE000ED08)
#define NVIC	  (( NVIC_t*) 0XE000E100)
#define SCB_AIRCR *((volatile uint_32t*) 0XE000ED0C)


#define FLASH_BASE_ADDRESS 0x08000000
#define PASSWORD_MASK  0X05FA0000
#define OFFSET_POSITION 0x08
#define RESET_MASK 0x04


#define MAX_NUMBER_OF_INTERRUPTS	 240
#define NVIC_REG_GAP				 24
#define INT_PRIORITY_GAP			 56
#define WORD_SIZE					 32
#define INT_WORDS					 8

typedef struct
{
	uint_32t ISER[INT_WORDS];
	uint_32t Reserved[NVIC_REG_GAP];
	uint_32t ICER[INT_WORDS];
	uint_32t Reserved1[NVIC_REG_GAP];
	uint_32t ISPR[INT_WORDS];
	uint_32t Reserved2[NVIC_REG_GAP];
	uint_32t ICPR[INT_WORDS];
	uint_32t Reserved3[NVIC_REG_GAP];
	uint_32t IABR[INT_WORDS];
	uint_32t Reserved4[INT_PRIORITY_GAP];
	uint_8t IPR[MAX_NUMBER_OF_INTERRUPTS];
}NVIC_t;



uint_8t DNVIC_EnableIRQ(uint_8t IRQn)
{
	if( IRQn<MAX_NUMBER_OF_INTERRUPTS)
	{
		NVIC->ISER[IRQn/WORD_SIZE]=1<<IRQn%WORD_SIZE;
	}
	else
	{
		return NOT_OK;
	}
	return OK;
}


uint_8t DNVIC_DisableIRQ(uint_8t IRQn)
{
	if( IRQn<MAX_NUMBER_OF_INTERRUPTS)
	{
		NVIC->ICER[IRQn/WORD_SIZE]= 1<<IRQn%WORD_SIZE;
	}
	else
	{
		return NOT_OK;
	}
	return OK;
}

uint_8t DNVIC_SetPendingIRQ (uint_8t IRQn)
{
	if( IRQn<MAX_NUMBER_OF_INTERRUPTS)
	{
		NVIC->ISPR[IRQn/WORD_SIZE] = 1<<IRQn%WORD_SIZE;
	}
	else
	{
		return NOT_OK;
	}
	return OK;
}

uint_8t DNVIC_ClearPendingIRQ (uint_8t IRQn)
{
	if( IRQn<MAX_NUMBER_OF_INTERRUPTS)
	{
		NVIC->ICPR[IRQn/WORD_SIZE] = 1<<IRQn%WORD_SIZE;
	}
	else
	{
		return NOT_OK;
	}
	return OK;
}

uint_8t DNVIC_GetPendingIRQ (uint_8t IRQn, uint_8t *Val)
{
	if( IRQn<MAX_NUMBER_OF_INTERRUPTS)
	{
		*Val= (NVIC->ICPR[IRQn/WORD_SIZE] >> IRQn%WORD_SIZE) & 1 ;
	}
	else
	{
		return NOT_OK;
	}
	return OK;
}

uint_8t DNVIC_GetActive (uint_8t IRQn, uint_8t *Val)
{
	if( IRQn<MAX_NUMBER_OF_INTERRUPTS)
	{
		*Val= (NVIC->IABR[IRQn/WORD_SIZE] >> IRQn%WORD_SIZE) & 1 ;
	}
	else
	{
		return NOT_OK;
	}
	return OK;
}

uint_8t DNVIC_SetPriorityGrouping(uint_32t priority_grouping)
{
	if(priority_grouping==ALL_PREEMPTION || priority_grouping==THREE_PREEMPTION_ONE_SUBGROUP || priority_grouping==TWO_PREEMPTION_TWO_SUBGROUP || priority_grouping==ONE_PREEMPTION_THREE_SUBGROUP || priority_grouping==ALL_SUBGROUP )
	{
		SCB_AIRCR=priority_grouping|PASSWORD_MASK;
	}
	else
	{
		return NOT_OK;
	}
	return OK;

}

uint_8t DNVIC_SetPriority (uint_8t IRQn, uint_8t priority)
{
	if( IRQn<MAX_NUMBER_OF_INTERRUPTS)
	{
		if(priority<16)
		{
			NVIC->IPR[IRQn] = priority<<4 ;
		}
		else
		{
			return NOT_OK;
		}
	}
	else
	{
		return NOT_OK;
	}
	return OK;
}

uint_8t DNVIC_GetPriority (uint_8t IRQn, uint_8t *priority)
{
	if( IRQn<MAX_NUMBER_OF_INTERRUPTS)
	{

		*priority=NVIC->IPR[IRQn]>>4 ;
	}
	else
	{
		return NOT_OK;
	}
	return OK;
}

void DNVIC_voidDisableAllPeripherals(void)
{
	asm("MOV R0, #1");
	asm("MSR PRIMASK, R0");
}


void DNVIC_voidEnableAllPeripherals(void)
{
	asm("MOV R0, #0");
	asm("MSR PRIMASK, R0");
}

void DNVIC_voidDisableAllFaults(void)
{
	asm("MOV R0, #1");
	asm("MSR FAULTMASK, R0");
}


void DNVIC_voidEnableAllFaults(void)
{
	asm("MOV R0, #0");
	asm("MSR FAULTMASK, R0");
}

void DNVIC_voidSetBASEPRI(volatile uint_8t priority)
{

	asm("LSLS R0,R0,#4");
	asm("MSR  BASEPRI, R0");
}

void DNVIC_voidChangeVectorOffset (uint_32t offset)
{
	SCB_VTOR = FLASH_BASE_ADDRESS | (offset<<OFFSET_POSITION);
}

void DNVIC_voidSysReset(void)
{
	SCB_AIRCR= RESET_MASK|PASSWORD_MASK;
}
