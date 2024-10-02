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
 * serial_parser.c
 *
 *  Created on: Sep 17, 2024
 *      Author: fil
 */

#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

#include "membrane_includes.h"
#include <ctype.h>

uint8_t	tmp_rx[32];

uint8_t wakeup_to_uart(uint32_t wakeup)
{
	if (( wakeup & EVENT_UART4_IRQ) == EVENT_UART4_IRQ)
		return HW_UART4;
	else if (( wakeup & EVENT_UART5_IRQ) == EVENT_UART5_IRQ)
		return HW_UART5;
	else if (( wakeup & EVENT_UART7_IRQ) == EVENT_UART7_IRQ)
		return HW_UART7;
	else if (( wakeup & EVENT_UART8_IRQ) == EVENT_UART8_IRQ)
		return HW_UART8;
	else
		return 255;
}

uint8_t uart_to_line(uint8_t uart)
{
	if (uart == HW_UART4)
		return 0;
	if (uart == HW_UART5)
		return 1;
	if (uart == HW_UART7)
		return 2;
	if (uart == HW_UART8)
		return 3;
	return 255;
}

uint8_t line_to_uart(uint8_t line)
{
	if ( line == 0 )
		return HW_UART4;
	if ( line == 1 )
		return HW_UART5;
	if ( line == 2 )
		return HW_UART7;
	if (line == 3 )
		return HW_UART8;
	return INVALID_UART;
}

uint8_t serial_compiler(uint8_t line,uint8_t rx_line_len)
{
uint8_t		sensor;
uint8_t		i;

	if ( MembraneSystem.sensor_rxbuf[line][SENSORS_INITIATOR]  == '<' )
	{
		sensor = MembraneSystem.sensor_rxbuf[line][SENSORS_REPLY_ADDRESS];
		if ( ( sensor < MAX_SENSORS )  && ( sensor > 0 ) )
			sensor--;
		else
			return 1;

		switch(MembraneSystem.sensor_rxbuf[line][SENSORS_REPLY_CMD])
		{
		case SENSORS_FLASH_CHECKREQ	:
			bzero((char *)MembraneSystem.prc2_mailbox,PRC2_MAILBOX_LEN);
			for(i=0;i<rx_line_len;i++)
				MembraneSystem.prc2_mailbox[i] = MembraneSystem.sensor_reply[line][sensor][i];
			mbx_send(INTERFACE_PROCESS_ID,PRC2_MAILBOX_ID,MembraneSystem.prc2_mailbox,rx_line_len);
			break;
		case SENSORS_DISCOVERY_COMMAND	:
			MembraneSystem.sensor_map[line][sensor] = MembraneSystem.sensor_rxbuf[line][3];
			break;
		case SENSORS_GETACQ_COMMAND	:
			MembraneData.sensor_type[line][sensor] 		   =  MembraneSystem.sensor_rxbuf[line][3];
			MembraneData.sensor_conductivity[line][sensor] = (MembraneSystem.sensor_rxbuf[line][5]  << 8) | MembraneSystem.sensor_rxbuf[line][6];
			MembraneData.sensor_scale_factor[line][sensor] = (MembraneSystem.sensor_rxbuf[line][8]  << 8) | MembraneSystem.sensor_rxbuf[line][9];
			MembraneData.sensor_da_outval[line][sensor]    = (MembraneSystem.sensor_rxbuf[line][11] << 8) | MembraneSystem.sensor_rxbuf[line][12];
			MembraneData.sensor_calibration[line][sensor]  = (MembraneSystem.sensor_rxbuf[line][14] << 8) | MembraneSystem.sensor_rxbuf[line][15];
			MembraneData.sensor_temperature[line][sensor]  = (MembraneSystem.sensor_rxbuf[line][17] << 8) | MembraneSystem.sensor_rxbuf[line][18];
			break;
		case SENSORS_FLASH_GETINFO :
			MembraneSystem.prc2_mailbox[0] = SENSORS_FLASH_GETINFO;
			MembraneSystem.prc2_mailbox[1] = ' ';

			MembraneSystem.prc2_mailbox[2] = MembraneSystem.sensor_rxbuf[line][2] + 0x30;
			MembraneSystem.prc2_mailbox[3] = ' ';
			for(i=4;i<PRC2_MAILBOX_LEN;i++)
			{
				if ( MembraneSystem.sensor_rxbuf[line][i] == '>' )
					break;
				else
					MembraneSystem.prc2_mailbox[i] = MembraneSystem.sensor_rxbuf[line][i];
			}
			mbx_send(INTERFACE_PROCESS_ID,PRC2_MAILBOX_ID,MembraneSystem.prc2_mailbox,i-1);
			break;
		default : return 1;
		}
	}
	return 0;
}

uint8_t sensors_serial_parser(uint32_t wakeup,uint32_t flags)
{
uint8_t			line,rx_line_len,ret_val=0;
uint32_t		l_wakeup;

	l_wakeup = wakeup;

	while ( l_wakeup != 0 )
	{
		if (( l_wakeup & EVENT_UART4_IRQ) == EVENT_UART4_IRQ)
		{
			l_wakeup &= ~EVENT_UART4_IRQ;
			rx_line_len = hw_get_uart_receive_len(HW_UART4);
			line = 0;
		}
		else if (( l_wakeup & EVENT_UART5_IRQ) == EVENT_UART5_IRQ)
		{
			l_wakeup &= ~EVENT_UART5_IRQ;
			rx_line_len = hw_get_uart_receive_len(HW_UART5);
			line = 1;
		}
		else if (( l_wakeup & EVENT_UART7_IRQ) == EVENT_UART7_IRQ)
		{
			l_wakeup &= ~EVENT_UART7_IRQ;
			rx_line_len = hw_get_uart_receive_len(HW_UART7);
			line = 2;
		}
		else if (( l_wakeup & EVENT_UART8_IRQ) == EVENT_UART8_IRQ)
		{
			l_wakeup &= ~EVENT_UART8_IRQ;
			rx_line_len = hw_get_uart_receive_len(HW_UART8);
			line = 3;
		}
		else
			ret_val ++;
		ret_val += serial_compiler(line,rx_line_len);
	}
	return ret_val;
}

#endif // #ifdef	MEMBRANE_2412172_00
