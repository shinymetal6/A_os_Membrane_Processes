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
 * flash_code.h
 *
 *  Created on: Aug 28, 2024
 *      Author: fil
 */


#ifndef STM32H743_MEMBRANE_FLASHCODE_H_
#define STM32H743_MEMBRANE_FLASHCODE_H_

#define	HEADER_SIZE	QSPI_PAGE_SIZE

extern	void init_ihex(void);
extern	void process_ihex(void);
extern	uint32_t get_code_summary_from_flash(uint32_t flash_block);
extern	uint32_t store_code_summary(uint8_t *buf,uint32_t size);
extern	uint32_t store_code_in_flash(uint32_t count);
extern	uint32_t get_flashcode(void);
extern	uint32_t decode_and_store(uint8_t *buf,uint32_t size);
extern	uint32_t read_code(uint32_t code_index);
extern	uint32_t read_summary(uint32_t summary_index);

#endif /* STM32H743_MEMBRANE_FLASHCODE_H_ */
