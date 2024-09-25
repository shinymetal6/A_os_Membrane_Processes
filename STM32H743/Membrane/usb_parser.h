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
 * usb_parser.h
 *
 *  Created on: Aug 7, 2024
 *      Author: fil
 */

#ifndef STM32H743_MEMBRANE_USB_PARSER_H_
#define STM32H743_MEMBRANE_USB_PARSER_H_

#define	USB_STD_PACKET	1
#define	USB_IHEX_PACKET	2
#define	USB_S3_PACKET	3
#define	USB_NO_PACKET	0

#define	USB_IHEX_TITLE_VALID		0
#define	USB_IHEX_LINE_VALID			1
#define	USB_IHEX_LAST_LINE_VALID	2
#define	USB_IHEX_LINE_ERROR			255

extern	uint8_t Host_decode_USB_packet(uint8_t* Buf);
extern	void	System_Process_USB_Replies(void);

extern	uint8_t Host_decode_USB_ihex_packet(uint8_t* Buf,uint16_t BufLen);
extern  void    System_Process_USB_ihex_Replies(uint8_t reply);

extern	uint8_t Host_pack_USB_packet(uint8_t *rx_buf,uint8_t len);
extern	void	Create_VersionString(void);
extern	void 	Download_Process_USB_Replies(uint8_t acknak);

#endif /* STM32H743_MEMBRANE_USB_PARSER_H_ */
