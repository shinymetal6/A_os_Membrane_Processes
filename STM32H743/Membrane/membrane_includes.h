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
 * membrane_includes.h
 *
 *  Created on: Feb 13, 2024
 *      Author: fil
 */

#ifndef STM32U575_MEMBRANE_INCLUDES_H_
#define STM32U575_MEMBRANE_INCLUDES_H_

#define		MAX_SENSORS			16
#define		MAX_LINES			4
#define		MAX_BOARDS			4
#define		USB_BUF_LEN			64
#define 	SINGLE_PKT_SIZE		8
#define		SENSORS_NUM			MAX_SENSORS
#define		SENSORS_LINE		MAX_LINES

#define		SENSOR_WATER			1
#define		SENSOR_TEMPERATURE		2
#define		SENSOR_NOTPRESENT		0

#define		PRC1_MAILBOX_ID		0
#define		PRC1_MAILBOX_LEN	128
#define		PRC2_MAILBOX_ID		0
#define		PRC2_MAILBOX_LEN	128
#define		SENSORS_TX_LEN		32
#define		SAMPLES_LEN			32
#define		SENSORS_RX_LEN		64
#define		SENSORS_RX_TIMEOUT	25
#define		SENSORS_RX_DISABLE	4

#define		PRC1_TIMER_INTERVAL	100

#define		SENSORS_APPNAME		32
#define		SENSORS_APPVERSION	12


#define		SENSORS_UPDATE_LEN		266	// <DAC[256 bytes data][4 bytes CRC32]D> , crc is on bytes data only
#define		SENSORS_UPDATE_PAYLOAD	256

#define		SENSORS_INITIATOR		0
#define		SENSORS_REPLY_CMD		1
#define		SENSORS_REPLY_ADDRESS	2

#define		SENSORS_UPDATE_CMD		SENSORS_REPLY_CMD
#define		SENSORS_UPDATE_ADDRESS	SENSORS_REPLY_ADDRESS
#define		SENSORS_UPDATE_PKTCNT	3
#define		SENSORS_UPDATE_DATA		4
#define		SENSORS_CRC_HH			(SENSORS_UPDATE_LEN-6)
#define		SENSORS_CRC_HL			(SENSORS_UPDATE_LEN-5)
#define		SENSORS_CRC_LH			(SENSORS_UPDATE_LEN-4)
#define		SENSORS_CRC_LL			(SENSORS_UPDATE_LEN-3)
#define		SENSORS_CRC_HH_SHIFT	24
#define		SENSORS_CRC_HL_SHIFT	16
#define		SENSORS_CRC_LH_SHIFT	8
#define		SENSORS_CRC_LL_SHIFT	0
#define		SENSORS_CLOSING_FLAG	(SENSORS_UPDATE_LEN-2)
#define		SENSORS_TERMINATOR		(SENSORS_UPDATE_LEN-1)

#define		SENSORS_BROADCAST		0xff
#define		SENSORS_RESERVED		0x00

#define		INVALID_UART			0xff


typedef struct
{
	uint8_t 		sensors_status;
	uint8_t 		line_selector;
	uint8_t 		sensor_selector;
	uint8_t 		sensor_scan_time;
	uint8_t 		sensor_scan_counter;
	uint8_t 		sensor_rxbuf[SENSORS_LINE][SENSORS_RX_LEN];
	uint8_t 		sensor_sentinel_start;
	uint8_t 		sensor_sentinel_end;
	uint8_t			sensor_reply[SENSORS_LINE][SENSORS_NUM][SENSORS_RX_LEN];
	uint8_t			sensor_scratchbuf[USB_BUF_LEN];
	uint8_t			sensor_map[SENSORS_LINE][SENSORS_NUM];
	uint8_t			sensor_map_line_index;
	uint8_t			sensor_map_sensor_index;
	uint8_t 		sensor_discovery_line_selector;
	uint8_t 		sensor_discovery_sensor_selector;

	/* USB section */
	uint8_t			command_from_usb;
	uint32_t		parameter1_from_usb;
	uint32_t		parameter2_from_usb;
	uint32_t		parameter3_from_usb;
	uint8_t			usb_rx_buf_rxed[USB_BUF_LEN];
	uint8_t			usb_rx_buf[USB_BUF_LEN];
	uint8_t			usb_rx_buf_len;
	uint8_t			usb_rx_buf_index;
	uint8_t			usb_tx_buf[USB_BUF_LEN];
	uint8_t			usb_tx_buf_len;
	uint8_t 		usb_flags;
	uint32_t		xtract_line_selector;
	uint32_t		xtract_sensor_selector;

	/* downloader */
	int		 		dwnld_index;
	uint32_t 		dwnld_line_selector;
	uint32_t 		dwnld_program_selector;
	uint32_t 		dwnld_sensor_selector;
	uint32_t 		dwnld_datalen;
	uint8_t 		dwnld_acknak;

	/* Flash area */
	uint8_t			flash_summary_buffer[QSPI_PAGE_SIZE];
	uint8_t			flash_app_name[SENSORS_APPNAME];
	uint8_t			flash_app_version[SENSORS_APPVERSION];
	uint32_t		flash_hex_app_size;
	uint32_t		flash_binary_app_size;
	uint32_t		flash_block_count;
	uint32_t		flash_written_size;
	uint32_t		flash_line_num;
	uint32_t		flash_address;
	uint32_t		flash_index;
	uint32_t		flash_crc;

	/* sensors update section */
	uint32_t 		update_datalen;
	uint8_t			update_line[SENSORS_UPDATE_LEN];
	uint8_t			*update_data_src_address;
	uint32_t		update_data_count;
	uint8_t 		update_pkt_counter;

	/* mailbox */
	uint8_t			prc1_mailbox[PRC1_MAILBOX_LEN];
	uint8_t			prc2_mailbox[PRC2_MAILBOX_LEN];

	/* others */
	uint8_t			name_version_string[USB_BUF_LEN];
	uint8_t			name_version_string_len;

	uint8_t			scratch_buf[QSPI_PAGE_SIZE];

}MembraneSystem_TypeDef;

typedef struct
{
	uint16_t 		sensor_type[SENSORS_LINE][SENSORS_NUM];
	uint16_t 		sensor_conductivity[SENSORS_LINE][SENSORS_NUM];
	uint16_t 		sensor_scale_factor[SENSORS_LINE][SENSORS_NUM];
	uint16_t 		sensor_da_outval[SENSORS_LINE][SENSORS_NUM];
	uint16_t 		sensor_calibration[SENSORS_LINE][SENSORS_NUM];
	uint16_t 		sensor_temperature[SENSORS_LINE][SENSORS_NUM];
}MembraneData_TypeDef;

/* sensors_status */
#define	SENSORS_RXED			0x01
#define	SENSORS_DISCOVERY		0x02
#define	SENSORS_RUN				0x04
#define	SENSORS_UPDINFO			0x08
#define	SENSORS_ON_UPDATE		0x10
#define	SENSORS_READY_TO_FLASH	0x20
#define	SENSORS_RUN_STATE		0x40
#define	SENSORS_POWERED			0x80
/* usb_flags */
#define	USB_FLAGS_LOGOSENT		0x02
#define	USB_FLAGS_SENDINFO		0x04
#define	USB_FLAGS_SENDDATA		0x08
#define	USB_FLAGS_SENDREPLY		0x10
#define	USB_FLAGS_IHEXOK		0x20
#define	USB_FLAGS_HEADEROK		0x40
#define	USB_FLAGS_PKTCOMPLETE	0x80

#define	FLASH_BUF_STARTADDR		0x100


#define	LINE_PROCESS_ID			1
#define	INTERFACE_PROCESS_ID	2
#define	LINE_LEN				32

#define	FLASH_USED_BLOCKS		2
#define	IHEX_BUFFER_SIZE		(QSPI_BLOCK_SIZE*FLASH_USED_BLOCKS)

/* commands */
#define	SENSORS_POWER_ON			'P'
#define	SENSORS_POWER_OFF			'O'
#define	SENSORS_FLASH				'F'
#define	CONCENTRATOR_FLASH_GETINFO	'G'
#define	SENSORS_FLASH_WRITE			'W'
#define	SENSORS_FLASH_CHECKREQ		'C'
#define	SENSORS_DISCOVERY_COMMAND	'Z'
#define	SENSORS_GETMAP_COMMAND		'M'
#define	SENSORS_GETACQ_COMMAND		'A'
#define	SENSORS_SCAN_COMMAND		'S'

#define	SENSORS_DISCOVERY_TIME		10

extern	uint8_t		*_d2ram_start;
extern	uint8_t		dwnld_area[IHEX_BUFFER_SIZE];

extern	uint32_t	valid_samples,invalid_samples;
extern	uint8_t		sensors_samples[MAX_LINES][MAX_SENSORS][SENSORS_TX_LEN];
extern	uint8_t		sensors_map[MAX_LINES][MAX_SENSORS][SENSORS_TX_LEN];

extern	MembraneSystem_TypeDef	MembraneSystem;
extern	MembraneData_TypeDef	MembraneData;

extern	CRC_HandleTypeDef 					hcrc;
#define	FLASH_CRC							hcrc

#include "usb_parser.h"
#include "flash_code.h"
#include "sensors_updater.h"
#include "serial_parser.h"

#endif /* STM32U575_MEMBRANE_INCLUDES_H_ */
