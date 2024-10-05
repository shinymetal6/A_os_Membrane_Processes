/*
 * processes_table.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */

#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

#include "membrane_includes.h"

extern	void process_1_sensors(uint32_t process_id);		//This is process1
extern	void process_2_usb_485(uint32_t process_id);		//This is process2
extern	void process_3(uint32_t process_id);			//This is process3
extern	void process_4(uint32_t process_id);			//This is process4


VERSIONING	uint8_t	app_name[32] 		= "Membrane C";
VERSIONING	uint8_t	app_version[32] 	= "v2024.10.00.rc0";

__attribute__ ((aligned (32))) const MembraneParameters_TypeDef	MembraneFlashParameters =
{
		.threshold_low  = PARAM_THRESHOLD_MIN,
		.threshold_high = PARAM_THRESHOLD_MAX,
		.hysteresis_K   = PARAM_HYSTERESIS,
		.hard_limit_low = PARAM_HARDLIMIT_LOW,
		.hard_limit_high= PARAM_HARDLIMIT_HIGH,
		.sine_number	= PARAM_SINE_NUMBER,
};
MembraneParameters_TypeDef	MembraneParameters[MAX_LINES];

USRprcs_t	UserProcesses[USR_PROCESS_NUMBER] =
{
		{
				.user_process = process_1_sensors,
				.stack_size = 4096,
		},
		{
				.user_process = process_2_usb_485,
				.stack_size = 4096,
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
