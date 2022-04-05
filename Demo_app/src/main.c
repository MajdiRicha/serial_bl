#include "main.h"
#include "scheduler.h"
#include "hardware.h"
#include "timer.h"
#include "system.h"
#include "comm.h"

void main()
{
	bool ret;

    delay_ms(250);              /*power up delay*/

	ret = scheduler_init(get_ticks_counter);    /*Initialize scheduler*/
    if (true == ret)
    {
        /*Create tasks:*/
        ret &= init_hw();       /*initialize HW and launch Heartbeat task*/
        ret &= init_system();   /*initialize system and create task*/
        ret &= init_comm();     /*initialize serial comm. and create task*/

        if (true == ret)        /*success?*/
        {
            while (true)
            {
                scheduler();    /*run scheduler forever*/
            }
        }
        else
        {
            LED1On();
            while (true);       /*stall*/
        }
    }
    else
    {
        LED1On();
        while (true);           /*stall*/
    }
}

