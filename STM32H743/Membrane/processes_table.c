/*
 * processes_table.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */

#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

extern	void process_1_sensors(uint32_t process_id);		//This is process1
extern	void process_2_usb_485(uint32_t process_id);		//This is process2
extern	void process_3(uint32_t process_id);			//This is process3
extern	void process_4(uint32_t process_id);			//This is process4


VERSIONING	uint8_t	app_name[32] 		= "Membrane C";
VERSIONING	uint8_t	app_version[32] 	= "v2024.10.00.rc0";

USRprcs_t	UserProcesses[USR_PROCESS_NUMBER] =
{
		{
				.user_process = process_1_sensors,
				.stack_size = 2048,
		},
		{
				.user_process = process_2_usb_485,
				.stack_size = 2048,
		},
		{
				.user_process = process_3,
				.stack_size = 256,
		},
		{
				.user_process = process_4,
				.stack_size = 256,
		}
};

#endif // #ifdef MEMBRANE_2412172_00
