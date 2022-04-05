#include "main.h"
#include "comm.h"
#include "version.h"

/*protos:*/
static bool comm_checksum_ok(uint8_t *dt_buff, uint16_t count);
static uint8_t comm_calculate_checksum(uint8_t *dt_buff, uint16_t count);
static void comm_uart_send_buff(uint8_t *dt_buff, uint16_t count);
void comm_process_task(s_task_handle_t me, s_task_msg_t **msg, void* arg);

/*globals:*/
uint8_t rx_buff[COMM_MAX_BUFF_SIZE], tx_buff[COMM_MAX_BUFF_SIZE];
uint16_t rx_data_index = 0;

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool init_comm(void)
{

    return s_task_create(true, S_TASK_NORMAL_PRIORITY, 5, comm_process_task, NULL, NULL);   /*runs every 5ms*/
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


void comm_process_task(s_task_handle_t me, s_task_msg_t **msg, void* arg)
{
    static bool get_size = true;
    static uint16_ut rx_data_size;
    static uint16_t iter_count = 0;
    uint16_ut tx_data_size;
    comm_function_t curr_fnc;
    comm_command_t curr_cmd;
    bool rx_data_ready = false;

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
                    /*list other functions*/

                    /*Bootloader functions*/
   				case COMM_FUNC_FLASH:
                        switch(curr_cmd)
                        {
                            case COMM_CMD_CONFIG:
                                set_boot_info(rx_buff[COMM_INDEX_ARG0]);
                                /*reply:*/
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
                /*init tx buffer to default:*/
                tx_data_size.data = 5;
                tx_buff[COMM_INDEX_FUNC] = curr_fnc;
                tx_buff[COMM_INDEX_CMD] = COMM_REQUEST;
            }
        }
        else
        {
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
    }
}

#INT_RDA
void  RDA_isr(void) 
{
	rx_buff[rx_data_index] = getc();   /*store byte*/
    /*Safe increment index*/
    rx_data_index = (rx_data_index + 1) & COMM_BUFF_INDEX_MASK;
    
}
