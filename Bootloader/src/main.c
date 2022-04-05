#include "main.h"
#include "version.h"

/*Macros*/
#define		LED_PIN		PIN_A7

/*Globals*/
uint8_t rx_buff[COMM_MAX_BUFF_SIZE], tx_buff[COMM_MAX_BUFF_SIZE];
uint16_t rx_data_index = 0;

/*protos*/
bool comm_checksum_ok(uint8_t *dt_buff, uint16_t count);
uint8_t comm_calculate_checksum(uint8_t *dt_buff, uint16_t count);
void comm_uart_send_buff(uint8_t *dt_buff, uint16_t count);

#org 0x0008, 0x000E
void _jmp_isr_h(void)
{
	#asm 
		goto ISR_RELOC_H 
	#endasm
}
////////////////////////////////////////////
#org 0x0018, 0x001E
void _jmp_isr_l(void)
{
	#asm 
		goto ISR_RELOC_L 
	#endasm
}

/**
 * @brief 
 * 
 * @param dt_buff 
 * @param count 
 * @return true 
 * @return false 
 */
bool comm_checksum_ok(uint8_t *dt_buff, uint16_t count)
{
    uint8_t checksum = 0;
    uint16_t i;

    for (i = 0; i < count; i++)
    {
        checksum += dt_buff[i];     /*accumulate*/
    }

    return (checksum == 0) ? true : false;
}

/**
 * @brief 
 * 
 * @param dt_buff 
 * @param count 
 * @return uint8_t 
 */
uint8_t comm_calculate_checksum(uint8_t *dt_buff, uint16_t count)
{
    uint8_t checksum = 0;
    uint16_t i;

    for (i = 0; i < count - 1; i++)
    {
        checksum += dt_buff[i];     /*accumulate*/
    }
    checksum = (~checksum) + 1;     /*2's complement*/

    return checksum;
}

/**
 * @brief 
 * 
 * @param dt_buff 
 * @param count 
 */
void comm_uart_send_buff(uint8_t *dt_buff, uint16_t count)
{
    uint16_t index;

    for (index = 0; index < count; index++)
    {
        putc(dt_buff[index]);
    }
}

void process_data(void)
{
	static bool get_size = true;
    static uint16_ut rx_data_size;
    static uint32_t iter_count = 0;
    uint16_ut tx_data_size = {0};
    comm_function_t curr_fnc;
    comm_command_t curr_cmd;
    bool rx_data_ready = false;
	static uint32_ut flash_address = {0};

	if (rx_data_index > 4)  /*check for EOP?*/
	{
		if (get_size)
		{
			/*get data size*/
			rx_data_size.bytes.low = rx_buff[COMM_INDEX_LEN_L];
			rx_data_size.bytes.high = rx_buff[COMM_INDEX_LEN_H];
			get_size = false;   /*done*/
		}

		if (rx_data_index == (rx_data_size.data))
		{
			/*clear counters and modify flags:*/
			rx_data_index = 0;
			rx_data_ready = true;
			get_size = true;
		}

		if (rx_data_ready)
		{
			if (rx_buff[COMM_INDEX_CMD] & COMM_REQUEST)     /*is it a request?*/
			{
				if (comm_checksum_ok(rx_buff, (rx_data_size.data)))   /*data valid?*/
				{
					/*get function*/
					curr_fnc = (comm_function_t)rx_buff[COMM_INDEX_FUNC];
					/*get command*/
					curr_cmd = (comm_command_t)(rx_buff[COMM_INDEX_CMD] & ~COMM_REQUEST);

					/*init tx buffer to default:*/
					tx_data_size.data = 5;
					tx_buff[COMM_INDEX_FUNC] = curr_fnc;
					tx_buff[COMM_INDEX_CMD] = COMM_REQUEST;

					/*fetch request packet*/
					switch (curr_fnc)
					{
						/*Bootloader functions*/
						case COMM_FUNC_FLASH:
							switch(curr_cmd)
							{
								case COMM_CMD_WR:
									switch(rx_buff[COMM_INDEX_ARG0])
									{
										case 00:	/*Data*/
											flash_address.words.low = make16(rx_buff[COMM_INDEX_ARG3], rx_buff[COMM_INDEX_ARG2]);
											if (flash_address.words.high != 0x0030)	/*not config*/
											{
												/*Do it*/
												write_program_memory(flash_address.data, (rx_buff + COMM_INDEX_ARG4), (uint16_t)rx_buff[COMM_INDEX_ARG1]); 
												output_toggle(LED_PIN);		/*visual effect*/
											}
											break;
										
										case 01:	/*EOF*/
											output_low(LED_PIN);		/*LED OFF*/
											break;

										case 04:	/*Address H*/
											flash_address.words.high = make16(rx_buff[COMM_INDEX_ARG4], rx_buff[COMM_INDEX_ARG5]);
											break;
									}
									/*reply*/
									tx_data_size.data = 2 + 4;
									tx_buff[COMM_INDEX_ARG0] = rx_buff[COMM_INDEX_ARG0];
									tx_buff[COMM_INDEX_CMD] = COMM_CMD_WR;
									break;

								case COMM_CMD_CONFIG:
									set_boot_info(rx_buff[COMM_INDEX_ARG0]);
									/*reply*/
									tx_data_size.data = 2 + 4;
									tx_buff[COMM_INDEX_LEN_L] = tx_data_size.bytes.low;
									tx_buff[COMM_INDEX_LEN_H] = tx_data_size.bytes.high;
									tx_buff[COMM_INDEX_CMD] = COMM_CMD_WR;
									tx_buff[COMM_INDEX_ARG0] = rx_buff[COMM_INDEX_ARG0];
									tx_buff[(tx_data_size.data) - 1] = comm_calculate_checksum(tx_buff, (tx_data_size.data));
									/*send reply:*/
									comm_uart_send_buff(tx_buff, tx_data_size.data);    /*Do it*/
									delay_ms(1);
									reset_cpu();   //reboot
									break;
							}
							break;

						/*device functions*/
						case COMM_FUNC_FW_REV:
							if (COMM_CMD_RD == curr_cmd)
							{
								tx_data_size.data = 2 + 6;
								tx_buff[COMM_INDEX_CMD] = COMM_CMD_WR;
								tx_buff[COMM_INDEX_ARG0] = FW_REV_MAJ;
								tx_buff[COMM_INDEX_ARG1] = FW_REV_MIN;
								tx_buff[COMM_INDEX_ARG2] = FW_REV_BLD;
							}
							break;

						case COMM_FUNC_FW_DAT:
							if (COMM_CMD_RD == curr_cmd)
							{
								tx_data_size.data = 2 + 6;
								tx_buff[COMM_INDEX_CMD] = COMM_CMD_WR;
								tx_buff[COMM_INDEX_ARG0] = FW_REV_DAY;
								tx_buff[COMM_INDEX_ARG1] = FW_REV_MNT;
								tx_buff[COMM_INDEX_ARG2] = FW_REV_YER;
							}
							break;
					}
				}
				else
				{
					/*reset counters and flags*/
					rx_data_index = 0;
					get_size = true;
					/*init tx buffer to default:*/
					tx_data_size.data = 5;
					tx_buff[COMM_INDEX_FUNC] = curr_fnc;
					tx_buff[COMM_INDEX_CMD] = COMM_REQUEST;
				}
			}
			else
			{
				/*reset counters and flags*/
				rx_data_index = 0;
				get_size = true;
				/*init tx buffer to default:*/
				tx_data_size.data = 5;
				tx_buff[COMM_INDEX_FUNC] = curr_fnc;
				tx_buff[COMM_INDEX_CMD] = COMM_REQUEST;
			}

			/*prepare reply*/
			tx_buff[COMM_INDEX_LEN_L] = tx_data_size.bytes.low;
			tx_buff[COMM_INDEX_LEN_H] = tx_data_size.bytes.high;
			tx_buff[(tx_data_size.data) - 1] = comm_calculate_checksum(tx_buff, (tx_data_size.data));
			/*send reply:*/
			comm_uart_send_buff(tx_buff, tx_data_size.data);    /*Do it*/
		}
		else	/*check for timeout based on number of iterations*/
		{
#if 0
			if (rx_data_index)	/*any bytes received?*/
			{
				if (++iter_count == COMM_TIMEOUT_RECOVERY)
				{
					/*reset counters and flags*/
					rx_data_index = 0;
					get_size = true;
				}
			}
			else
			{
				iter_count = 0;		/*clear counter*/
			}
#endif
		}
	}
}

void main()
{	
	boot_state_t bs;
	
	/*read boot info*/
	bs = get_boot_info();
#if 1
	if (BOOT_APP == bs)
	{
		/*Jump to application firmware*/
		#asm 
			goto APPLICATION_START
		#endasm
	}
#endif
	/*Stay in loader*/
	output_drive(LED_PIN);		/*Set pin out*/
	output_high(LED_PIN);		/*LED ON*/

	while(true)
	{
		if (kbhit())	/*incoming byte?*/
		{
			rx_buff[rx_data_index] = getc();   /*store byte*/
   			/*Safe increment index*/
   			rx_data_index = (rx_data_index + 1) & COMM_BUFF_INDEX_MASK;
		}

		process_data();
	}
}
