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
 * serial_parser.h
 *
 *  Created on: Sep 17, 2024
 *      Author: fil
 */


#ifndef STM32H743_MEMBRANE_SERIAL_PARSER_H_
#define STM32H743_MEMBRANE_SERIAL_PARSER_H_

extern	uint8_t wakeup_to_uart(uint32_t wakeup);
extern	uint8_t uart_to_line(uint8_t uart);
extern	uint8_t line_to_uart(uint8_t line);

extern	uint8_t sensors_serial_parser(uint32_t wakeup,uint32_t flags);

/*
 				MembraneSystem.work_sensor_txbuf[0] = SENSORS_INITIATOR_CHAR;
				MembraneSystem.work_sensor_txbuf[1] = SENSORS_GET_DATA_COMMAND_REPLY;
				MembraneSystem.work_sensor_txbuf[2] = MembraneInfo.board_address;
				MembraneSystem.work_sensor_txbuf[3] = SENSORS_BOARD_TYPE;
				MembraneSystem.work_sensor_txbuf[4] = 'C';
				MembraneSystem.work_sensor_txbuf[5] = AcqSystem.calibration_value >> 8;
				MembraneSystem.work_sensor_txbuf[6] = AcqSystem.calibration_value & 0xff;
				MembraneSystem.work_sensor_txbuf[7] = 'W';
				MembraneSystem.work_sensor_txbuf[8] = AcqSystem.conductivity_value >> 8;
				MembraneSystem.work_sensor_txbuf[9] = AcqSystem.conductivity_value & 0xff;
				MembraneSystem.work_sensor_txbuf[10] = 'S';
				MembraneSystem.work_sensor_txbuf[11] = AcqSystem.acquisition_steady_value >> 8;
				MembraneSystem.work_sensor_txbuf[12] = AcqSystem.acquisition_steady_value & 0xff;
				MembraneSystem.work_sensor_txbuf[13] = 'T';
				MembraneSystem.work_sensor_txbuf[14] = AcqSystem.temperature_data >> 8;
				MembraneSystem.work_sensor_txbuf[15] = AcqSystem.temperature_data & 0xff;
				MembraneSystem.work_sensor_txbuf[16] = SENSORS_TERMINATOR_CHAR;
				MembraneSystem.work_sensor_txbuflen = 17;
*/

#endif /* STM32H743_MEMBRANE_SERIAL_PARSER_H_ */
