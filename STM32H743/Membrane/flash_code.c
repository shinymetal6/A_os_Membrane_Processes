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
 * flash_code.c
 *
 *  Created on: Aug 28, 2024
 *      Author: fil
 */

#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

#include "membrane_includes.h"

DWNLD_DATA_AREA	uint8_t		dwnld_area[IHEX_BUFFER_SIZE];

void write_page(uint8_t *buf,uint32_t flash_address)
{
	qspi_WriteEnable();
	qspi_Write(buf,flash_address,QSPI_PAGE_SIZE);
	qspi_WriteDisable();
}

void read_page(uint8_t *buf,uint32_t flash_address)
{
	qspi_Read(buf,flash_address,QSPI_PAGE_SIZE);
}

void erase_blocks(uint8_t block_num,uint32_t start_block)
{
uint32_t	i;
	qspi_WriteEnable();
	for(i=start_block;i<start_block+block_num;i++)
		qspi_EraseBlockByNumber(i);
	qspi_WriteDisable();
}


uint32_t	crc_flash(uint32_t 	*flash_data_ptr,uint32_t flash_data_len)
{
	return HAL_CRC_Calculate(&FLASH_CRC, flash_data_ptr, flash_data_len);
}

void write_summary(uint32_t summary_index)
{
uint32_t	i;
uint8_t		*flash_data;

	flash_data = (uint8_t *)&_d2ram_start;
	flash_data += QSPI_PAGE_SIZE;
	MembraneSystem.flash_crc = crc_flash((uint32_t 	*)flash_data,MembraneSystem.flash_binary_app_size);

	for(i=0;i<QSPI_PAGE_SIZE;i++)
		MembraneSystem.flash_summary_buffer[i] = 0xff;
	sprintf((char *)MembraneSystem.flash_summary_buffer,"CHEADER %s %s %d %d %d 0x%08x B",MembraneSystem.flash_app_name,MembraneSystem.flash_app_version,(int )MembraneSystem.flash_binary_app_size,(int )MembraneSystem.flash_written_size,(int )MembraneSystem.flash_block_count,(unsigned int )MembraneSystem.flash_crc);
	write_page(MembraneSystem.flash_summary_buffer,0);
}

void write_code(uint32_t code_index)
{
uint8_t		*flash_data;
uint32_t	i;
	MembraneSystem.flash_address = QSPI_PAGE_SIZE;
	flash_data = (uint8_t *)&_d2ram_start;
	flash_data += QSPI_PAGE_SIZE;
	MembraneSystem.flash_written_size = 0;
	for(i=0;i<MembraneSystem.flash_binary_app_size;i+=QSPI_PAGE_SIZE)
	{
		write_page(flash_data,MembraneSystem.flash_address);
		flash_data += QSPI_PAGE_SIZE;
		MembraneSystem.flash_address += QSPI_PAGE_SIZE;
		MembraneSystem.flash_written_size += QSPI_PAGE_SIZE;
	}
}

uint32_t read_summary(uint32_t summary_index)
{
uint32_t	i;
/*
uint8_t 	*flash_data;
uint32_t	flash_crc;
*/
	MembraneSystem.flash_address = summary_index * FLASH_USED_BLOCKS * QSPI_BLOCK_SIZE;
	read_page(MembraneSystem.flash_summary_buffer,MembraneSystem.flash_address);
	i = sscanf((char *)MembraneSystem.flash_summary_buffer,"CHEADER %s %s %d %d %d 0x%08x B",MembraneSystem.flash_app_name,MembraneSystem.flash_app_version,(int *)&MembraneSystem.flash_binary_app_size,(int *)&MembraneSystem.flash_written_size,(int *)&MembraneSystem.flash_block_count,(int *)&MembraneSystem.flash_crc);
	if ( i != 6 )
	{
		sprintf((char *)MembraneSystem.flash_app_name,"U name");
		sprintf((char *)MembraneSystem.flash_app_version,"U version");
		MembraneSystem.flash_binary_app_size = 0;
		MembraneSystem.flash_block_count = 0;
	}
	/*
	flash_data = (uint8_t *)&_d2ram_start;
	flash_data += QSPI_PAGE_SIZE;
	flash_crc = crc_flash((uint32_t *)flash_data,MembraneSystem.flash_binary_app_size);
	if ( flash_crc != MembraneSystem.flash_crc )
		return 1;
		*/
	return 0;
}

uint32_t read_code(uint32_t code_index)
{
uint8_t		*flash_data;
uint32_t	i,numbytes=0;

	read_summary(code_index);
	MembraneSystem.flash_address = code_index * FLASH_USED_BLOCKS * QSPI_BLOCK_SIZE;
	flash_data = (uint8_t *)&_d2ram_start;
	for(i=0;i<MembraneSystem.flash_binary_app_size;i+=QSPI_PAGE_SIZE)
	{
		read_page(flash_data,MembraneSystem.flash_address);
		flash_data += QSPI_PAGE_SIZE;
		numbytes += QSPI_PAGE_SIZE;
		MembraneSystem.flash_address += QSPI_PAGE_SIZE;
	}
	if ( numbytes < MembraneSystem.flash_binary_app_size)
		return 0;
	return numbytes;
}

uint32_t decode_and_store(uint8_t *buf,uint32_t size)
{
uint32_t	i;
uint32_t	linelen;
uint8_t		ihex_result;
uint8_t 	*ihex_data;
uint32_t	ihex_data_len;

	if ( buf[0] != ':')
		return 1;
	if ( buf[1] == 'T')
	{
		i = sscanf((char *)buf,":T %s %s %d",MembraneSystem.flash_app_name,MembraneSystem.flash_app_version,(int *)&MembraneSystem.flash_hex_app_size);
		if ( i == 3 )
		{
			MembraneSystem.flash_address = QSPI_PAGE_SIZE;
			MembraneSystem.flash_block_count = FLASH_USED_BLOCKS;
			MembraneSystem.flash_binary_app_size = 0;
			erase_blocks(MembraneSystem.flash_block_count,0);
			MembraneSystem.flash_index = 0;
			for(i=0;i<IHEX_BUFFER_SIZE;i++)
				dwnld_area[MembraneSystem.flash_address+i] = 0xff;
			return 1;
		}
		else
			return 255;	// error
	}

	if ((linelen = get_hex_crlflen(buf)) == 0 )
		return 255;	// error

	ihex_result = ihex_decode(buf,size);
	if ( ihex_result > 127 )
	{
		if ( ihex_result == 255 )
			return 255;	// error
		else
			return 0;	// no data associated to this record
	}

	if ( ihex_result )
	{
		ihex_data = get_ihex_data_ptr();
		ihex_data_len = ihex_get_data_len(buf);
		MembraneSystem.flash_address = get_ihex_address();
		MembraneSystem.flash_address += QSPI_PAGE_SIZE;

		for(i=0;i<ihex_data_len;i++,MembraneSystem.flash_address++)
			dwnld_area[MembraneSystem.flash_address] = ihex_data[i];
	}
	else
	{
		MembraneSystem.flash_binary_app_size = IHEX_BUFFER_SIZE;
		write_code(0);
		write_summary(0);
		read_summary(0);
		read_code(0);
		return 2;
	}
	return 0;
}

#endif // #ifdef	MEMBRANE_2412172_00


