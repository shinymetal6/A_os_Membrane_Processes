/*
 * process_2_usb_485.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */

#include "main.h"

#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00

#include "membrane_includes.h"

uint8_t						prc2_mbx_rxbuf[PRC2_MAILBOX_LEN];

uint8_t		tpacket = 0;

void process_2_usb_485(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	mbx_size;
uint32_t	decode_result,decode_ret;
uint8_t		packet_type;
uint8_t		usb_reply;

	allocate_hw(HW_USB_DEVICE,0);
	hw_set_usb_rx_buffer(MembraneSystem.usb_rx_buf_rxed);
	MembraneSystem.xtract_sensor_selector = 0;
	MembraneSystem.xtract_line_selector = 0;
	Create_VersionString();

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	qspi_init();
	while(1)
	{
		wait_event(EVENT_USB_DEVICE_IRQ | EVENT_TIMER | EVENT_MBX);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
		}
		if (( wakeup & WAKEUP_FROM_MBX) == WAKEUP_FROM_MBX)
		{
			mbx_size = mbx_receive(PRC2_MAILBOX_ID,prc2_mbx_rxbuf);
			if ( mbx_size )
				hw_send_usb(prc2_mbx_rxbuf, mbx_size);
		}
		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
			MembraneSystem.usb_rx_buf_len = hw_UsbGetRXLen();
			packet_type = Host_pack_USB_packet(MembraneSystem.usb_rx_buf_rxed,MembraneSystem.usb_rx_buf_len);
			if ( packet_type == USB_IHEX_PACKET )
			{
				decode_result = Host_decode_USB_ihex_packet(MembraneSystem.usb_rx_buf,MembraneSystem.usb_rx_buf_len);
				usb_reply = 'N';
				if ( decode_result != USB_IHEX_LINE_ERROR)
				{
					decode_ret = decode_and_store(MembraneSystem.usb_rx_buf,MembraneSystem.usb_rx_buf_len);
					if ( decode_ret == 0)
						usb_reply ='Y';
					if ( decode_ret == 1)
						usb_reply ='T';
					if ( decode_ret == 2)
						usb_reply ='E';
				}
				System_Process_USB_ihex_Replies(usb_reply);
			}

			if ( packet_type == USB_STD_PACKET )
			{
				bzero((char *)MembraneSystem.prc1_mailbox,PRC1_MAILBOX_LEN);
				decode_result = Host_decode_USB_packet(MembraneSystem.usb_rx_buf);
				if ( decode_result == 1 )
				{
					sprintf((char *)MembraneSystem.prc1_mailbox,"%c",(char )MembraneSystem.command_from_usb);
					mbx_send(LINE_PROCESS_ID,PRC1_MAILBOX_ID,MembraneSystem.prc1_mailbox,strlen((char *)MembraneSystem.prc1_mailbox));
				}
				if ( decode_result == 2 )
				{
					sprintf((char *)MembraneSystem.prc1_mailbox,"%c %d",(char )MembraneSystem.command_from_usb,(int )MembraneSystem.parameter1_from_usb);
					mbx_send(LINE_PROCESS_ID,PRC1_MAILBOX_ID,MembraneSystem.prc1_mailbox,strlen((char *)MembraneSystem.prc1_mailbox));
				}
				if ( decode_result == 3 )
				{
					sprintf((char *)MembraneSystem.prc1_mailbox,"%c %d %d",(char )MembraneSystem.command_from_usb,(int )MembraneSystem.parameter1_from_usb,(int )MembraneSystem.parameter2_from_usb);
					mbx_send(LINE_PROCESS_ID,PRC1_MAILBOX_ID,MembraneSystem.prc1_mailbox,strlen((char *)MembraneSystem.prc1_mailbox));
				}
				if ( decode_result == 4 )
				{
					sprintf((char *)MembraneSystem.prc1_mailbox,"%c %d %d %d",(char )MembraneSystem.command_from_usb,(int )MembraneSystem.parameter1_from_usb,(int )MembraneSystem.parameter2_from_usb,(int )MembraneSystem.parameter3_from_usb);
					mbx_send(LINE_PROCESS_ID,PRC1_MAILBOX_ID,MembraneSystem.prc1_mailbox,strlen((char *)MembraneSystem.prc1_mailbox));
				}
				bzero(MembraneSystem.usb_rx_buf,USB_BUF_LEN);
			}
		}
	}
}

#endif // #ifdef MEMBRANE_2412172_00
