#include "main.h"
#include "timer.h"


//Globals:
cntr_size_t local_sys_tick_counter  = 0; //tick counter

/**
 * ISR for TIMER0 in order to generate a 1ms local_sys_tick
 * 
 * @author m_richa 
 * @date 
 */
#int_TIMER0   //HIGH   //high priority interrupt
void TIMER0_isr(void)
{
    set_timer0(get_timer0() - TMR1Reload);  /*re-load timer*/
    
    local_sys_tick_counter++;
}

/**
 * Function to return system ticks counter in ms
 * 
 * @author m_richa 
 * @date 
 * 
 * @return cntr_size_t system ticks count in ms
 */
cntr_size_t get_ticks_counter(void)
{
    return local_sys_tick_counter;
}
