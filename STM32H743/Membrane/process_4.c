/*
 * process_4.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */

#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

//#define FLASH_TEST1	1

#ifdef FLASH_TEST1
#include "membrane_includes.h"
extern	uint8_t				*_d2ram_start;
uint32_t	stored_bytes=0;
void process_4(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		tim_cntr = 0;
uint8_t		*flash_data_dest_ptr;

	flash_data_dest_ptr = (uint8_t *)&_d2ram_start;
	flash_data_dest_ptr +=0x20000;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if ( ( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			tim_cntr++;
			if ( tim_cntr == 7)
			{
				read_code(0);
			}
		}
	}
}

#else
void process_4(uint32_t process_id)
{
	wait_event(HW_SLEEP_FOREVER);
}

#endif // #ifdef FLASH_TEST
#endif // #ifdef MEMBRANE_2412172_00
