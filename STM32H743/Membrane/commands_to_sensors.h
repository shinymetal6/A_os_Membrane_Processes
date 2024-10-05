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
 * commands_to_sensors.h
 *
 *  Created on: Oct 4, 2024
 *      Author: fil
 */


#ifndef STM32H743_MEMBRANE_COMMANDS_TO_SENSORS_H_
#define STM32H743_MEMBRANE_COMMANDS_TO_SENSORS_H_

extern	uint8_t sensors_send_discovery(void);
extern	uint8_t sensors_send_write_command_to_sensors(uint8_t line,uint8_t sensor);
extern	void sensors_get_data(void);
extern	void sensors_go_special(uint8_t special);
extern	void sensors_get_info(uint8_t line,uint8_t sensor);
extern	void sensors_send_special(void);
extern	void check_sensors_flash(uint8_t line, uint8_t sensor);
extern	void sensors_send_kparameters(uint8_t line);
extern	void sensors_get_kparameters(uint8_t line);

#endif /* STM32H743_MEMBRANE_COMMANDS_TO_SENSORS_H_ */
