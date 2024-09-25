/*
 * process_3.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */

#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

//#define FLASH_TEST	1

#ifdef FLASH_TEST
DTCM_VECTORS_DATA	uint8_t	w_scratch_buf1[QSPI_PAGE_SIZE];
DTCM_VECTORS_DATA	uint8_t	w_scratch_buf2[QSPI_PAGE_SIZE];
DTCM_VECTORS_DATA	uint8_t	r_scratch_buf1[QSPI_PAGE_SIZE];
DTCM_VECTORS_DATA	uint8_t	r_scratch_buf2[QSPI_PAGE_SIZE];
uint8_t	retret;
#endif

void process_3(uint32_t process_id)
{
#ifdef FLASH_TEST
uint32_t	wakeup,flags;
uint32_t	i;
uint8_t		tim_cntr = 0,err_prc3 = 0;

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

				for(i=0;i<256;i++)
				{
					w_scratch_buf1[i] = 'C';
					w_scratch_buf2[i] = 'D';
					r_scratch_buf1[i] = 'Y';
					r_scratch_buf2[i] = 'Z';
				}
				retret = qspi_WriteEnable();
				//retret = qspi_Erase_Chip();
				retret = qspi_EraseBlockByNumber(0);
				retret = qspi_EraseBlockByNumber(1);

				retret = qspi_Read(r_scratch_buf1,0,QSPI_PAGE_SIZE);
				retret = qspi_Read(r_scratch_buf2,32768,QSPI_PAGE_SIZE);
				qspi_Write(w_scratch_buf1,0,QSPI_PAGE_SIZE);
				retret = qspi_Write(w_scratch_buf2,32768,QSPI_PAGE_SIZE);
				retret = qspi_WriteDisable();

				retret = qspi_Read(r_scratch_buf1,0,QSPI_PAGE_SIZE);
				retret = qspi_Read(r_scratch_buf2,32768,QSPI_PAGE_SIZE);
				HAL_Delay(10);
			}
		}
	}
#else
	wait_event(HW_SLEEP_FOREVER);
#endif // #ifdef FLASH_TEST
}

#endif // #ifdef MEMBRANE_2412172_00
