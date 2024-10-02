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
 * usb_parser.c
 *
 *  Created on: Aug 7, 2024
 *      Author: fil
 */
#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

#include "membrane_includes.h"
#include <ctype.h>

extern	uint8_t	app_name[32];
extern	uint8_t	app_version[32];

void System_Process_USB_ihex_Replies(uint8_t reply)
{
	bzero(MembraneSystem.usb_tx_buf,USB_BUF_LEN);
	sprintf((char *)MembraneSystem.usb_tx_buf,":%c\n\r",reply);
	MembraneSystem.usb_tx_buf_len = strlen((char *)MembraneSystem.usb_tx_buf);
	hw_send_usb(MembraneSystem.usb_tx_buf, MembraneSystem.usb_tx_buf_len);
}

uint8_t Host_decode_USB_ihex_packet(void)
{
uint32_t	pnum;

	if ( MembraneSystem.usb_rx_buf[0] == ':')
	{
		if (( MembraneSystem.usb_rx_buf[7] == '0') && ( MembraneSystem.usb_rx_buf[8] == '1'))
			return USB_IHEX_LAST_LINE_VALID;
		else if ( MembraneSystem.usb_rx_buf[1] == 'T')
		{
			bzero(MembraneSystem.flash_app_name,SENSORS_APPNAME);
			pnum = sscanf((char *)MembraneSystem.usb_rx_buf,":T %s %s %d",(char *)MembraneSystem.flash_app_name,(char *)MembraneSystem.flash_app_version,(int *)&MembraneSystem.flash_hex_app_size);
			if ( pnum == 3 )
				return USB_IHEX_TITLE_VALID;
		}
		else
			return USB_IHEX_LINE_VALID;
	}
	return USB_IHEX_LINE_ERROR;
}

uint8_t Host_decode_USB_packet(uint8_t* Buf)
{
uint16_t	pnum;
char p0;
int	p1,p2,p3;

	pnum = sscanf((char * )Buf,"<%c%d%d%d", &p0, &p1,&p2,&p3);
	if ( p0 == 'x')
	{
		pnum = sscanf((char * )Buf,"<x %d %d %s %s", (int *)&MembraneSystem.parameter1_from_usb, (int *)&MembraneSystem.parameter2_from_usb,MembraneSystem.string1_from_usb,MembraneSystem.string2_from_usb);
		if ( pnum == 4 )
		{
			MembraneSystem.command_from_usb = 'x';
			pnum = 5;
		}
		if ( pnum == 1 )
		{
			MembraneSystem.command_from_usb = 'x';
			MembraneSystem.parameter1_from_usb = p1;
			pnum = 2;
		}
	}
	else
	{
		switch(pnum)
		{
		case	4:
			//MembraneSystem.command_from_usb = toupper(p0);
			MembraneSystem.command_from_usb = p0;
			MembraneSystem.parameter1_from_usb = p1;
			MembraneSystem.parameter2_from_usb = p2;
			MembraneSystem.parameter3_from_usb = p3;
			break;
		case	3:
			//MembraneSystem.command_from_usb = toupper(p0);
			MembraneSystem.command_from_usb = p0;
			MembraneSystem.parameter1_from_usb = p1;
			MembraneSystem.parameter2_from_usb = p2;
			break;
		case	2:
			//MembraneSystem.command_from_usb = toupper(p0);
			MembraneSystem.command_from_usb = p0;
			MembraneSystem.parameter1_from_usb = p1;
			break;
		case	1:
			//MembraneSystem.command_from_usb = toupper(p0);
			MembraneSystem.command_from_usb = p0;
			break;
		default:
			break;
		}
	}

	return pnum;
}

uint8_t Host_pack_USB_packet(uint8_t *rx_buf,uint8_t len)
{
uint32_t	i;

	for(i=0;i<len;i++)
	{
		i &= (USB_BUF_LEN-1);
		if ((( MembraneSystem.usb_flags & USB_FLAGS_HEADEROK) == 0 ) && (( MembraneSystem.usb_flags & USB_FLAGS_IHEXOK) == 0 ))
		{
			if ( rx_buf[i] == '<')
			{
				MembraneSystem.usb_rx_buf[0] = rx_buf[i];
				MembraneSystem.usb_rx_buf_index = 1;
				MembraneSystem.usb_flags |= USB_FLAGS_HEADEROK;
			}
			if ( rx_buf[i] == ':')
			{
				MembraneSystem.usb_rx_buf[0] = rx_buf[i];
				MembraneSystem.usb_rx_buf_index = 1;
				MembraneSystem.usb_flags |= USB_FLAGS_IHEXOK;
			}
		}
		else
		{
			if ((MembraneSystem.usb_flags & USB_FLAGS_HEADEROK) == USB_FLAGS_HEADEROK )
			{
				MembraneSystem.usb_rx_buf[MembraneSystem.usb_rx_buf_index ] = rx_buf[i];
				if ( MembraneSystem.usb_rx_buf[MembraneSystem.usb_rx_buf_index] == '>')
				{
					MembraneSystem.usb_flags |= USB_FLAGS_PKTCOMPLETE;
					MembraneSystem.usb_flags &= ~USB_FLAGS_HEADEROK;
					MembraneSystem.usb_rx_buf_len = MembraneSystem.usb_rx_buf_index;
					MembraneSystem.usb_rx_buf_index = 0;
					return	USB_STD_PACKET;
				}
				MembraneSystem.usb_rx_buf_index++;
			}
			if ((MembraneSystem.usb_flags & USB_FLAGS_IHEXOK) == USB_FLAGS_IHEXOK )
			{
				MembraneSystem.usb_rx_buf[MembraneSystem.usb_rx_buf_index ] = rx_buf[i];
				if ( MembraneSystem.usb_rx_buf[MembraneSystem.usb_rx_buf_index] == 0x0a)
				{
					MembraneSystem.usb_flags |= USB_FLAGS_PKTCOMPLETE;
					MembraneSystem.usb_flags &= ~USB_FLAGS_IHEXOK;
					MembraneSystem.usb_rx_buf_len = MembraneSystem.usb_rx_buf_index+1;
					MembraneSystem.usb_rx_buf_index = 0;
					return	USB_IHEX_PACKET;
				}
				MembraneSystem.usb_rx_buf_index++;
			}
		}
	}
	return USB_NO_PACKET;
}

uint8_t	version_string[USB_BUF_LEN];
void Create_VersionString(void)
{
	bzero(MembraneSystem.name_version_string,USB_BUF_LEN);
	sprintf((char *)MembraneSystem.name_version_string,"%s",app_name);
	bzero(version_string,USB_BUF_LEN);
	sprintf((char *)version_string," %s\n\r",app_version);
	strcat((char *)MembraneSystem.name_version_string,(char *)version_string);
	MembraneSystem.name_version_string_len = strlen((char *)MembraneSystem.name_version_string);
}

#endif // #ifdef	MEMBRANE_2412172_00


