#include "main.h"
#include "hardware.h"
#include "scheduler.h"

void heart_beat_led(s_task_handle_t me, s_task_msg_t **msg, void* arg);

/**
 * @brief Configures IO ports
 * @author m_richa
 */
void init_io_ports(void)
{
   set_tris_a(0x03); /*A*/
   output_a(0x00);

   set_tris_b(0x07); /*B*/
   output_b(0x00);

   set_tris_c(0x9c); /*C*/
   output_c(0x20);
}
///////////////////////////////////////////////////////////////////////////////
/**
 * 
 * @return 
 */
bool init_hw(void)
{
    init_io_ports();

    //Timer0: used for scheduler
    setup_timer_0(T0_INTERNAL|T0_DIV_8);      //for 1ms interrupt

    //Analog:
    setup_adc(ADC_CLOCK_DIV_64 | ADC_TAD_MUL_8);
    setup_adc_ports(sAN0|sAN1);
    set_adc_channel(0);
    setup_comparator(NC_NC_NC_NC);

    //Input Capture:
    setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
    setup_ccp1(CCP_CAPTURE_RE);
    
    /*Create related task(s)*/
    return s_task_create(1, S_TASK_NORMAL_PRIORITY, 100, heart_beat_led, NULL, NULL);         /*runs every 100ms*/
}

/**
 * 
 * @param me
 * @param msg
 */
void heart_beat_led(s_task_handle_t me, s_task_msg_t **msg, void* arg)
{
   static uint8_t HeartBeatCtr = 0;

   //Heart Beat
    if (bit_test(HeartBeatCtr, 4))
    {
        HeartBeatCtr = 0;
        LED1Off();
        restart_wdt();  //reset WDT
    } else if ((!bit_test(HeartBeatCtr, 2)) && (!bit_test(HeartBeatCtr, 3)))
    {
        LED1Toggle();
        HeartBeatCtr++;
    } else
    {
        HeartBeatCtr++;
    }
    
#ifdef USE_MESSAGING    
   //A GOOD PRACTICE IS TO ALWAYS FLUSH MESSAGES BEFORE EXIT:
   s_task_flush_msgs(msg);
#endif
}
