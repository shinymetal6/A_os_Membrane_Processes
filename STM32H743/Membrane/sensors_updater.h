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
 * sensors_updater.h
 *
 *  Created on: Sep 6, 2024
 *      Author: fil
 */


#ifndef STM32H743_MEMBRANE_SENSORS_UPDATER_H_
#define STM32H743_MEMBRANE_SENSORS_UPDATER_H_

extern	void updater_initialize(void);
extern	void updater_create_packet(uint8_t address);
extern	void updater_create_info_packet(uint8_t address,uint32_t len);

#endif /* STM32H743_MEMBRANE_SENSORS_UPDATER_H_ */
