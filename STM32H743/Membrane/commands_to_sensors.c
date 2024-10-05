/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * commands_to_sensors.c
 *
 *  Created on: Oct 4, 2024
 *      Author: fil
 */


#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

#include "membrane_includes.h"

extern	MembraneParameters_TypeDef	MembraneParameters;

void check_sensors_flash(uint8_t line, uint8_t sensor)
{
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'C';
	MembraneSystem.sensor_scratchbuf[3] = sensor;
	MembraneSystem.sensor_scratchbuf[4] = '>';
	MembraneSystem.sensor_scratchbuf[5] = 0;
	switch ( line )
	{
	case	1	:	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,6);break;
	case	2	:	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,6);break;
	case	3	:	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,6);break;
	case	4	:	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,6);break;
	}
}

uint8_t sensors_send_discovery(void)
{
	if ( MembraneSystem.sensor_map_sensor_index < MAX_SENSORS)
	{
		MembraneSystem.sensor_scratchbuf[0] = 0;
		MembraneSystem.sensor_scratchbuf[1] = '<';
		MembraneSystem.sensor_scratchbuf[2] = SENSORS_DISCOVERY_COMMAND;
		MembraneSystem.sensor_scratchbuf[3] = MembraneSystem.sensor_map_sensor_index+1;
		MembraneSystem.sensor_scratchbuf[4] = '>';
		MembraneSystem.sensor_scratchbuf[5] = 0;

		hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,6);
		hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,6);
		hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,6);
		hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,6);

		MembraneSystem.sensor_map_sensor_index++;
		return 0;
	}
	return 1;
}

uint8_t sensors_send_write_command_to_sensors(uint8_t line,uint8_t sensor)
{
	if ( MembraneSystem.dwnld_line_selector < MAX_LINES)
	{
		if ( (MembraneSystem.dwnld_sensor_selector < MAX_SENSORS) || (MembraneSystem.dwnld_sensor_selector == SENSORS_BROADCAST))
		{
			MembraneSystem.sensor_scratchbuf[0] = 0;
			MembraneSystem.sensor_scratchbuf[1] = '<';
			MembraneSystem.sensor_scratchbuf[2] = SENSORS_FLASH_WRITE;
			MembraneSystem.sensor_scratchbuf[3] = MembraneSystem.dwnld_sensor_selector;
			MembraneSystem.sensor_scratchbuf[4] = '>';
			MembraneSystem.sensor_scratchbuf[5] = 0;

			switch ( MembraneSystem.dwnld_line_selector )
			{
			case 1:
				hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,6);
				break;
			case 2:
				hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,6);
				break;
			case 3:
				hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,6);
				break;
			case 4:
				hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,6);
				break;
			}
			return 0;
		}
	}
	return 1;
}

void sensors_get_data(void)
{
	MembraneData.sensor_type[0][MembraneSystem.sensor_selector] = 0;
	MembraneData.sensor_type[1][MembraneSystem.sensor_selector] = 0;
	MembraneData.sensor_type[2][MembraneSystem.sensor_selector] = 0;
	MembraneData.sensor_type[3][MembraneSystem.sensor_selector] = 0;
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'A';
	MembraneSystem.sensor_scratchbuf[3] = MembraneSystem.sensor_selector;
	MembraneSystem.sensor_scratchbuf[4] = '>';
	MembraneSystem.sensor_scratchbuf[5] = 0;
	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,6);
	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,6);
	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,6);
	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,6);
}

void sensors_go_special(uint8_t special)
{
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'x';
	MembraneSystem.sensor_scratchbuf[3] = SENSORS_BROADCAST;
	MembraneSystem.sensor_scratchbuf[4] = special+0x30;
	MembraneSystem.sensor_scratchbuf[5] = '>';
	MembraneSystem.sensor_scratchbuf[6] = 0;
	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,7);
	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,7);
	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,7);
	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,7);
}

void sensors_get_info(uint8_t line,uint8_t sensor)
{
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'I';
	MembraneSystem.sensor_scratchbuf[3] = sensor;
	MembraneSystem.sensor_scratchbuf[4] = '>';
	MembraneSystem.sensor_scratchbuf[5] = 0;
	switch(line)
	{
	case	1:	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,6); break;
	case	2:	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,6); break;
	case	3:	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,6); break;
	case	4:	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,6); break;
	}
}

void sensors_send_special(void)
{
uint32_t	len , i , index;
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'x';
	MembraneSystem.sensor_scratchbuf[3] = SENSORS_BROADCAST;
	MembraneSystem.sensor_scratchbuf[4] = '2';
	MembraneSystem.sensor_scratchbuf[5] = (uint8_t )MembraneSystem.new_board_address;
	index = 6;
	len = strlen(MembraneSystem.new_DSC_serial_string);
	for(i=0;i<len;i++,index++)
		MembraneSystem.sensor_scratchbuf[index] = MembraneSystem.new_DSC_serial_string[i];
	len = strlen(MembraneSystem.new_DSC_date);
	MembraneSystem.sensor_scratchbuf[index] = ' ';
	index++;
	for(i=0;i<len;i++,index++)
		MembraneSystem.sensor_scratchbuf[index] = MembraneSystem.new_DSC_date[i];
	MembraneSystem.sensor_scratchbuf[index] = '>';
	index++;
	MembraneSystem.sensor_scratchbuf[index] = 0;
	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,index);
	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,index);
	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,index);
	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,index);
}

void sensors_send_kparameters(uint8_t line)
{
uint32_t	len;
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'K';
	MembraneSystem.sensor_scratchbuf[3] = SENSORS_BROADCAST;
	sprintf((char *)&MembraneSystem.sensor_scratchbuf[4],
			" %d %d %d %d %d %d >",
			MembraneParameters.threshold_low,
			MembraneParameters.threshold_high,
			MembraneParameters.hysteresis_K,
			MembraneParameters.hard_limit_low,
			MembraneParameters.hard_limit_high,
			MembraneParameters.sine_number
	);
	len = strlen((char *)&MembraneSystem.sensor_scratchbuf[4]) + 4;
	MembraneSystem.sensor_scratchbuf[len] = 0;
	switch(line)
	{
	case	1:	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,len+1); break;
	case	2:	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,len+1); break;
	case	3:	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,len+1); break;
	case	4:	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,len+1); break;
	}
}

void sensors_get_kparameters(uint8_t line)
{
uint32_t	len;
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'K';
	MembraneSystem.sensor_scratchbuf[3] = SENSORS_BROADCAST;
	sprintf((char *)&MembraneSystem.sensor_scratchbuf[4],
			" %d %d %d %d %d %d >",
			MembraneParameters.threshold_low,
			MembraneParameters.threshold_high,
			MembraneParameters.hysteresis_K,
			MembraneParameters.hard_limit_low,
			MembraneParameters.hard_limit_high,
			MembraneParameters.sine_number
	);
	len = strlen((char *)&MembraneSystem.sensor_scratchbuf[4]) + 4;
	MembraneSystem.sensor_scratchbuf[len] = 0;
	switch(line)
	{
	case	1:	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,len+1); break;
	case	2:	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,len+1); break;
	case	3:	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,len+1); break;
	case	4:	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,len+1); break;
	}
}


#endif // #ifdef	MEMBRANE_2412172_00
