/*
 * process_1_sensors.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"

#ifdef	MEMBRANE_2412172_00
/*
 * serial management process
 */
#include "membrane_includes.h"
#include <stdlib.h>

uint8_t	prc1_mbx_rxbuf[PRC1_MAILBOX_LEN];

MembraneSystem_TypeDef		MembraneSystem;
MembraneData_TypeDef		MembraneData;


uint8_t	led_tim_cntr = 0;

/* data
 * 0x00,<Gn>,0x00
 * write update reply
 * 0x00,<Fn[Y | N][line num]>,0x00
 */

static void power_on_serials(void)
{
	HAL_GPIO_WritePin(PW_N_GPIO_Port, PW_N_Pin,GPIO_PIN_RESET);
	MembraneSystem.sensors_status |= SENSORS_POWERED;
}

static void power_off_serials(void)
{
	HAL_GPIO_WritePin(PW_N_GPIO_Port, PW_N_Pin,GPIO_PIN_SET);
	MembraneSystem.sensors_status &= ~SENSORS_POWERED;
}

static void led_ops(void)
{
	led_tim_cntr++;
	if (( led_tim_cntr == 14) || ( led_tim_cntr == 18))
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	else if ( led_tim_cntr == 20)
	{
		led_tim_cntr = 0;
	}
	else
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

uint8_t tx_update_packet(uint8_t selected_uart)
{
	if (( MembraneSystem.sensors_status & SENSORS_UPDINFO) == SENSORS_UPDINFO)
	{
		MembraneSystem.sensors_status &= ~SENSORS_UPDINFO;
		updater_create_info_packet(SENSORS_BROADCAST,MembraneSystem.flash_binary_app_size);
		hw_send_uart_dma(selected_uart,MembraneSystem.update_line,SENSORS_UPDATE_LEN);
		return 1;
	}
	else
	{
		if ( MembraneSystem.update_data_count < MembraneSystem.flash_binary_app_size)
		{
			updater_create_packet(SENSORS_BROADCAST);
			hw_send_uart_dma(selected_uart,MembraneSystem.update_line,SENSORS_UPDATE_LEN);
			return 1;
		}
		else
		{
			MembraneSystem.sensors_status &= ~SENSORS_ON_UPDATE;
			sprintf((char *)MembraneSystem.prc2_mailbox,"Transfer Done");
			mbx_send(INTERFACE_PROCESS_ID,PRC2_MAILBOX_ID,MembraneSystem.prc2_mailbox,strlen((char *)MembraneSystem.prc2_mailbox));
			return 0;
		}
	}
	return 0;
}

uint8_t serial_send(uint8_t *buf,uint8_t line , uint8_t sensor)
{
uint8_t len = strlen((char *)buf),selected_uart;
	if (( line < MAX_LINES) && ( sensor < MAX_SENSORS ))
	{
		selected_uart = line_to_uart(line);
		buf[0] = 0;
		buf[3] = sensor;
		buf[len-1] = 0;
		hw_send_uart_dma(selected_uart,buf,len);
		return 0;
	}
	return 1;
}

void clear_discovery_array(void)
{
uint32_t	i,j;
	MembraneSystem.sensor_map_line_index = MembraneSystem.sensor_map_sensor_index = 0;
	for(i=0;i<MAX_LINES;i++)
	{
		for(j=0;j<MAX_SENSORS;j++)
		{
			MembraneSystem.sensor_map[i][j] = 0;
		}
	}
}

uint8_t sensors_discovery(void)
{
	if ( MembraneSystem.sensor_map_sensor_index < MAX_SENSORS)
	{
		MembraneSystem.sensor_scratchbuf[0] = 0;
		MembraneSystem.sensor_scratchbuf[1] = '<';
		MembraneSystem.sensor_scratchbuf[2] = SENSORS_DISCOVERY_COMMAND;
		MembraneSystem.sensor_scratchbuf[3] = MembraneSystem.sensor_map_sensor_index+1;
		MembraneSystem.sensor_scratchbuf[4] = '>';
		MembraneSystem.sensor_scratchbuf[5] = 0;

		hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,6);
		hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,6);
		hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,6);
		hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,6);

		MembraneSystem.sensor_map_sensor_index++;
		return 0;
	}
	return 1;
}

uint8_t sensors_send_write_command_to_sensors(uint8_t line,uint8_t sensor)
{
	if ( MembraneSystem.dwnld_line_selector < MAX_LINES)
	{
		if ( (MembraneSystem.dwnld_sensor_selector < MAX_SENSORS) || (MembraneSystem.dwnld_sensor_selector == SENSORS_BROADCAST))
		{
			MembraneSystem.sensor_scratchbuf[0] = 0;
			MembraneSystem.sensor_scratchbuf[1] = '<';
			MembraneSystem.sensor_scratchbuf[2] = SENSORS_FLASH_WRITE;
			MembraneSystem.sensor_scratchbuf[3] = MembraneSystem.dwnld_sensor_selector;
			MembraneSystem.sensor_scratchbuf[4] = '>';
			MembraneSystem.sensor_scratchbuf[5] = 0;

			switch ( MembraneSystem.dwnld_line_selector )
			{
			case 1:
				hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,6);
				break;
			case 2:
				hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,6);
				break;
			case 3:
				hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,6);
				break;
			case 4:
				hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,6);
				break;
			}
			return 0;
		}
	}
	return 1;
}

void sensors_get_data(void)
{
	MembraneData.sensor_type[0][MembraneSystem.sensor_selector] = 0;
	MembraneData.sensor_type[1][MembraneSystem.sensor_selector] = 0;
	MembraneData.sensor_type[2][MembraneSystem.sensor_selector] = 0;
	MembraneData.sensor_type[3][MembraneSystem.sensor_selector] = 0;
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'A';
	MembraneSystem.sensor_scratchbuf[3] = MembraneSystem.sensor_selector;
	MembraneSystem.sensor_scratchbuf[4] = '>';
	MembraneSystem.sensor_scratchbuf[5] = 0;
	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,6);
	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,6);
	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,6);
	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,6);
}

void sensors_go_special(uint8_t special)
{
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'x';
	MembraneSystem.sensor_scratchbuf[3] = SENSORS_BROADCAST;
	MembraneSystem.sensor_scratchbuf[4] = special+0x30;
	MembraneSystem.sensor_scratchbuf[5] = '>';
	MembraneSystem.sensor_scratchbuf[6] = 0;
	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,7);
	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,7);
	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,7);
	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,7);
}

void sensors_get_info(uint8_t line,uint8_t sensor)
{
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'I';
	MembraneSystem.sensor_scratchbuf[3] = sensor;
	MembraneSystem.sensor_scratchbuf[4] = '>';
	MembraneSystem.sensor_scratchbuf[5] = 0;
	switch(line)
	{
	case	1:	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,6); break;
	case	2:	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,6); break;
	case	3:	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,6); break;
	case	4:	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,6); break;
	}
}

void sensors_send_special(void)
{
uint32_t	len , i , index;
	MembraneSystem.sensor_scratchbuf[0] = 0;
	MembraneSystem.sensor_scratchbuf[1] = '<';
	MembraneSystem.sensor_scratchbuf[2] = 'x';
	MembraneSystem.sensor_scratchbuf[3] = SENSORS_BROADCAST;
	MembraneSystem.sensor_scratchbuf[4] = '2';
	MembraneSystem.sensor_scratchbuf[5] = (uint8_t )MembraneSystem.new_board_address;
	index = 6;
	len = strlen(MembraneSystem.new_DSC_serial_string);
	for(i=0;i<len;i++,index++)
		MembraneSystem.sensor_scratchbuf[index] = MembraneSystem.new_DSC_serial_string[i];
	len = strlen(MembraneSystem.new_DSC_date);
	MembraneSystem.sensor_scratchbuf[index] = ' ';
	index++;
	for(i=0;i<len;i++,index++)
		MembraneSystem.sensor_scratchbuf[index] = MembraneSystem.new_DSC_date[i];
	MembraneSystem.sensor_scratchbuf[index] = '>';
	index++;
	MembraneSystem.sensor_scratchbuf[index] = 0;
	hw_send_uart_dma(HW_UART4,MembraneSystem.sensor_scratchbuf,index);
	hw_send_uart_dma(HW_UART5,MembraneSystem.sensor_scratchbuf,index);
	hw_send_uart_dma(HW_UART7,MembraneSystem.sensor_scratchbuf,index);
	hw_send_uart_dma(HW_UART8,MembraneSystem.sensor_scratchbuf,index);
}

void check_sensors_flash(uint8_t line, uint8_t sensor)
{
	sprintf((char *)MembraneSystem.sensor_scratchbuf," <%c0> ",SENSORS_FLASH_CHECKREQ);
	serial_send(MembraneSystem.sensor_scratchbuf,line,sensor);
}

char	tmp_buf[64];
void parse_mbx_in(void)
{
uint32_t	pnum,mbprc2len,sensor;
uint8_t		i;
int			sensor_data_index,line_data_index;

	bzero((char *)prc1_mbx_rxbuf,PRC1_MAILBOX_LEN);
	if ( mbx_receive(PRC1_MAILBOX_ID,prc1_mbx_rxbuf) == 0)
		return;

	bzero((char *)MembraneSystem.prc2_mailbox,PRC2_MAILBOX_LEN);

	switch(prc1_mbx_rxbuf[0])
	{
	case	SENSORS_POWER_ON :	/* P : Power on */
		power_on_serials();
		sprintf((char *)MembraneSystem.prc2_mailbox,"Power ON");
		break;
	case	SENSORS_POWER_OFF :	/* O : Power off */
		power_off_serials();
		sprintf((char *)MembraneSystem.prc2_mailbox,"Power OFF");
		break;
	case	SENSORS_FLASH_CHECKREQ:	// C : Sensors flash check request
		pnum = sscanf((char *)prc1_mbx_rxbuf,"%c %d %d",(char *)SENSORS_FLASH_CHECKREQ,(int *)&MembraneSystem.line_selector,(int *)&MembraneSystem.sensor_selector);
		if ( pnum == 3 )
		{
			if (( MembraneSystem.sensor_selector < SENSORS_NUM ) && ( MembraneSystem.line_selector < SENSORS_LINE ))
				MembraneSystem.sensors_status &= ~SENSORS_RXED;
			check_sensors_flash(MembraneSystem.line_selector,MembraneSystem.sensor_selector);
		}
		break;
	case	SENSORS_FLASH	: /* F : Send flash command to sensors */
		pnum = sscanf((char *)prc1_mbx_rxbuf,"F %d %d",(int *)&MembraneSystem.dwnld_line_selector,(int *)&MembraneSystem.dwnld_sensor_selector);
		if ( pnum == 2 )
		{
			updater_initialize();
			MembraneSystem.sensors_status |= SENSORS_UPDINFO;
			MembraneSystem.sensors_status |= SENSORS_ON_UPDATE;
		}
		break;
	case	SENSORS_FLASH_WRITE	: /* W : Send Write_data command */
		pnum = sscanf((char *)prc1_mbx_rxbuf,"W %d %d",(int *)&MembraneSystem.dwnld_line_selector,(int *)&MembraneSystem.dwnld_sensor_selector);
		if ( pnum == 2 )
		{
			sprintf((char *)MembraneSystem.prc2_mailbox,"Writing");
			MembraneSystem.sensors_status &= ~SENSORS_ON_UPDATE;
			sensors_send_write_command_to_sensors(MembraneSystem.dwnld_line_selector,MembraneSystem.dwnld_sensor_selector);
		}
		break;
	case	CONCENTRATOR_FLASH_GETINFO	: /* Get stored file info */
		if ( read_code(MembraneSystem.parameter1_from_usb) != 0 )
		{
			read_summary(MembraneSystem.parameter1_from_usb);
			sprintf((char *)MembraneSystem.prc2_mailbox,"%c %d %s %s",
					MembraneSystem.command_from_usb,
					(int )MembraneSystem.parameter1_from_usb,
					(char *)MembraneSystem.flash_app_name,
					(char *)MembraneSystem.flash_app_version);
		}
		else
			sprintf((char *)MembraneSystem.prc2_mailbox,"Read flash error");
		break;
	case	SENSORS_DISCOVERY_COMMAND	: /* Z : discovery command */
		if (( MembraneSystem.sensors_status & SENSORS_DISCOVERY ) == SENSORS_DISCOVERY )
			sprintf((char *)MembraneSystem.prc2_mailbox,"Discovery in progress");
		else
		{
			clear_discovery_array();
			sprintf((char *)MembraneSystem.prc2_mailbox,"Discovery From USB");
			MembraneSystem.sensors_status |= SENSORS_DISCOVERY;
		}
		break;
	case	SENSORS_GETMAP_COMMAND	: /* M : Get sensors map after a discovery command */
		if (( MembraneSystem.sensors_status & SENSORS_DISCOVERY ) == SENSORS_DISCOVERY )
			sprintf((char *)MembraneSystem.prc2_mailbox,"Discovery in progress");
		else
		{
			pnum = sscanf((char *)prc1_mbx_rxbuf,"M %d",(int *)&sensor);
			if ( pnum == 1 )
			{
				if ( sensor <= MAX_LINES)
				{
					sensor -= 1;
					MembraneSystem.prc2_mailbox[0] = 'M';
					MembraneSystem.prc2_mailbox[1] = ' ';
					MembraneSystem.prc2_mailbox[2] = sensor+0x31;
					MembraneSystem.prc2_mailbox[3] = ' ';
					for(i=0;i<MAX_SENSORS;i++)
					{
						MembraneSystem.prc2_mailbox[i+4] =  MembraneSystem.sensor_map[sensor][i] + 0x30;
					}
					MembraneSystem.prc2_mailbox[i] = 0;
				}
			}
		}
		break;
	case	SENSORS_GETACQ_COMMAND	: /* A : Get data */
		pnum = sscanf((char *)prc1_mbx_rxbuf,"A %d %d",&line_data_index,&sensor_data_index);
		if ( pnum == 2 )
		{
			if ( line_data_index )
				line_data_index--;
			else
				return;
			if ( sensor_data_index )
				sensor_data_index--;
			else
				return;
			if (( line_data_index < SENSORS_LINE ) && ( sensor_data_index < SENSORS_NUM ))
			{
				bzero((char *)MembraneSystem.prc2_mailbox,PRC2_MAILBOX_LEN);
				sprintf((char *)MembraneSystem.prc2_mailbox,"DSC %1d Sensor %2d Type %2d Readout %4d Scale Factor %4d DAC %d Calibration %4d Temperature %2d",
						line_data_index+1,
						sensor_data_index+1,
						MembraneData.sensor_type[line_data_index][sensor_data_index],
						MembraneData.sensor_conductivity[line_data_index][sensor_data_index],
						MembraneData.sensor_scale_factor[line_data_index][sensor_data_index],
						MembraneData.sensor_da_outval[line_data_index][sensor_data_index],
						MembraneData.sensor_calibration[line_data_index][sensor_data_index],
						MembraneData.sensor_temperature[line_data_index][sensor_data_index]
						);
			}
			else
			{
				sprintf((char *)MembraneSystem.prc2_mailbox,"Param error in get acq command");
			}
		}
		break;
	case	SENSORS_SCAN_COMMAND	: /* S : Scan all sensors */
		if (( MembraneSystem.sensors_status & SENSORS_RUN) == SENSORS_RUN)
		{
			sprintf((char *)MembraneSystem.prc2_mailbox,"Scan in progress");
		}
		else
		{
			MembraneSystem.line_selector   = 0;
			MembraneSystem.sensor_selector = 0;
			MembraneSystem.sensors_status |= SENSORS_RUN;
			sprintf((char *)MembraneSystem.prc2_mailbox,"Scan start");
		}
		break;
	case	SENSORS_SPECIAL_COMMAND	: /* S : Scan all sensors , sensor_data_index used as command*/
		pnum = sscanf((char *)prc1_mbx_rxbuf,"x %d %d %s %s",&sensor_data_index,&MembraneSystem.new_board_address,MembraneSystem.new_DSC_serial_string,MembraneSystem.new_DSC_date);
		if ( pnum == 1 )
		{
			if ((sensor_data_index == 0 ) || (sensor_data_index == 1 ))
			{
				if (sensor_data_index == 0 )
					sprintf((char *)MembraneSystem.prc2_mailbox,"Special off");
				else
					sprintf((char *)MembraneSystem.prc2_mailbox,"Special on");
				sensors_go_special(sensor_data_index);
			}
		}
		if ( pnum == 4 )
		{
			if (sensor_data_index == 2 )
			{
				sensors_send_special();
				sprintf((char *)MembraneSystem.prc2_mailbox,"Special %d %s %s",MembraneSystem.new_board_address,MembraneSystem.new_DSC_serial_string,MembraneSystem.new_DSC_date);
			}
		}
		break;
	case SENSORS_FLASH_GETINFO:
		pnum = sscanf((char *)prc1_mbx_rxbuf,"I %d %d",&line_data_index,&sensor_data_index);
		if ( pnum == 2 )
		{
			sensors_get_info(line_data_index,sensor_data_index);
		}
		/*
		pnum = sscanf((char *)prc1_mbx_rxbuf,"I %d %d",&line_data_index,&sensor_data_index);
		if ( pnum == 2 )
		{
			sensors_get_info(line_data_index,sensor_data_index);
			sprintf((char *)MembraneSystem.prc2_mailbox,"Info request sent to line %d sensor %d",line_data_index,sensor_data_index);
		}
		*/
		break;
	default:
		sprintf((char *)MembraneSystem.prc2_mailbox,"ERROR : UNKNOWN COMMAND");
		break;
	}
	mbprc2len =  strlen((char *)MembraneSystem.prc2_mailbox);
	if ( mbprc2len )
		mbx_send(INTERFACE_PROCESS_ID,PRC2_MAILBOX_ID,MembraneSystem.prc2_mailbox,mbprc2len);
}


void process_1_sensors(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		tx_prog_packet;

	MembraneSystem.sensor_selector = 0;
	MembraneSystem.sensor_sentinel_start = '<';
	MembraneSystem.sensor_sentinel_end = '>';
	power_off_serials();
	allocate_hw(HW_UART4,0);
	hw_receive_uart_sentinel(HW_UART4,MembraneSystem.sensor_rxbuf[0],SENSORS_RX_LEN,MembraneSystem.sensor_sentinel_start, MembraneSystem.sensor_sentinel_end,SENSORS_RX_TIMEOUT);
	allocate_hw(HW_UART5,0);
	hw_receive_uart_sentinel(HW_UART5,MembraneSystem.sensor_rxbuf[1],SENSORS_RX_LEN,MembraneSystem.sensor_sentinel_start, MembraneSystem.sensor_sentinel_end,SENSORS_RX_TIMEOUT);
	allocate_hw(HW_UART7,0);
	hw_receive_uart_sentinel(HW_UART7,MembraneSystem.sensor_rxbuf[2],SENSORS_RX_LEN,MembraneSystem.sensor_sentinel_start, MembraneSystem.sensor_sentinel_end,SENSORS_RX_TIMEOUT);
	allocate_hw(HW_UART8,0);
	hw_receive_uart_sentinel(HW_UART8,MembraneSystem.sensor_rxbuf[3],SENSORS_RX_LEN,MembraneSystem.sensor_sentinel_start, MembraneSystem.sensor_sentinel_end,SENSORS_RX_TIMEOUT);
	clear_discovery_array();
	create_timer(TIMER_ID_0,50,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_MBX | EVENT_UART4_IRQ|  EVENT_UART5_IRQ| EVENT_UART7_IRQ| EVENT_UART8_IRQ);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_MBX) == WAKEUP_FROM_MBX)
		{
			parse_mbx_in();
		}

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			led_ops();

			if (( MembraneSystem.sensors_status & SENSORS_POWERED ) == SENSORS_POWERED )
			{
				if (( MembraneSystem.sensors_status & SENSORS_DISCOVERY ) == SENSORS_DISCOVERY )
				{
					if ( sensors_discovery() )
						MembraneSystem.sensors_status &= ~SENSORS_DISCOVERY;
				}

				if (( MembraneSystem.sensors_status & SENSORS_ON_UPDATE ) == SENSORS_ON_UPDATE )
				{
					switch ( MembraneSystem.dwnld_line_selector )
					{
					case 1:
						tx_prog_packet = tx_update_packet(HW_UART4);
						break;
					case 2:
						tx_prog_packet = tx_update_packet(HW_UART5);
						break;
					case 3:
						tx_prog_packet = tx_update_packet(HW_UART7);
						break;
					case 4:
						tx_prog_packet = tx_update_packet(HW_UART8);
						break;
					}
					if ( tx_prog_packet == 0 ) // update finished
						MembraneSystem.sensors_status |= SENSORS_READY_TO_FLASH;
				}

				if ((MembraneSystem.sensors_status & SENSORS_RUN ) == SENSORS_RUN)
				{
					MembraneSystem.sensor_selector++;
					if ( MembraneSystem.sensor_selector > MAX_SENSORS)
					{
						MembraneSystem.sensors_status &= ~SENSORS_RUN;
					}
					else
					{
						sensors_get_data();
					}
				}
			}
		}

		if ( wakeup & (EVENT_UART4_IRQ|EVENT_UART5_IRQ|EVENT_UART7_IRQ|EVENT_UART8_IRQ) )
		{
			if (( flags & WAKEUP_FLAGS_UART_RX) == WAKEUP_FLAGS_UART_RX)
				sensors_serial_parser(wakeup,flags);
			/*
			if (( flags & WAKEUP_FLAGS_UART_TX) == WAKEUP_FLAGS_UART_TX)
				selected_uart = wakeup_to_uart(wakeup);
				*/
		}
	}
}

#endif // #ifdef	MEMBRANE_2412172_00

