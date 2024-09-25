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
 * sensors_updater.c
 *
 *  Created on: Sep 6, 2024
 *      Author: fil
 */

#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

#include "membrane_includes.h"

uint32_t	crc_packet(uint32_t 	*flash_data_ptr,uint32_t flash_data_len)
{
	FLASH_CRC.Instance->INIT = 0xffffffff;
	return HAL_CRC_Calculate(&FLASH_CRC, flash_data_ptr, flash_data_len);
}

void updater_create_packet(uint8_t address)
{
uint32_t	pktcrc,i;
	MembraneSystem.update_line[SENSORS_INITIATOR] = '<';
	MembraneSystem.update_line[SENSORS_UPDATE_CMD] = 'D';
	MembraneSystem.update_line[SENSORS_UPDATE_ADDRESS] = address;
	MembraneSystem.update_line[SENSORS_UPDATE_PKTCNT] = MembraneSystem.update_pkt_counter;
	for(i=0;i<SENSORS_UPDATE_PAYLOAD;i++,MembraneSystem.update_data_src_address++)
		MembraneSystem.update_line[i+SENSORS_UPDATE_DATA] = *MembraneSystem.update_data_src_address;
	MembraneSystem.update_data_count+=SENSORS_UPDATE_PAYLOAD;

	pktcrc = crc_packet((uint32_t *)&MembraneSystem.update_line[SENSORS_UPDATE_DATA],SENSORS_UPDATE_PAYLOAD);
	MembraneSystem.update_line[SENSORS_CRC_HH] = pktcrc >> SENSORS_CRC_HH_SHIFT;
	MembraneSystem.update_line[SENSORS_CRC_HL] = pktcrc >> SENSORS_CRC_HL_SHIFT;
	MembraneSystem.update_line[SENSORS_CRC_LH] = pktcrc >> SENSORS_CRC_LH_SHIFT;
	MembraneSystem.update_line[SENSORS_CRC_LL] = pktcrc >> SENSORS_CRC_LL_SHIFT;
	MembraneSystem.update_line[SENSORS_CLOSING_FLAG] = 'D';
	MembraneSystem.update_line[SENSORS_TERMINATOR] = '>';
	MembraneSystem.update_pkt_counter++;
}

void updater_create_info_packet(uint8_t address,uint32_t len)
{
	sprintf((char *)MembraneSystem.update_line,"<U0 datalen %d 0x%08x U>",(int )len,(int )MembraneSystem.flash_crc );
	MembraneSystem.update_line[2] = address;
}

void updater_initialize(void)
{
uint8_t		*update_data;

	update_data = (uint8_t *)&_d2ram_start;
	update_data += QSPI_PAGE_SIZE;
	MembraneSystem.update_data_count = 0;
	MembraneSystem.update_pkt_counter = 0;
	MembraneSystem.update_data_src_address = update_data;
	MembraneSystem.update_pkt_counter = 0;
	read_summary(0);
	read_code(0);
}

#endif // #ifdef	MEMBRANE_2412172_00
