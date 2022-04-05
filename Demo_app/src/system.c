#include "main.h"
#include "hardware.h"
#include "system.h"
#include "stdlib.h"
#include "scheduler.h"

//Global vars:
cntr_size_t sys_time_up = 0;
system_info  current_system_info;

/*Protos:*/
void update_sys_up_time(s_task_handle_t me, s_task_msg_t **msg, void* arg);
void update_system(s_task_handle_t me, s_task_msg_t **msg, void* arg);

/**
 * 
 * @return 
 */
bool init_system(void)      //returns TRUE if success
{
   bool ret = true;

   enable_interrupts(INT_TIMER0);   /*Enable TIMER0 interrupt (event)*/
   enable_interrupts(INT_CCP1);     /*Input capture interrupt*/
   enable_interrupts(INT_RDA);      /*UART interrpt*/
   enable_interrupts(GLOBAL);       /*Enable global interrupt*/

   /*Create related task(s)*/
   ret &= s_task_create(true, S_TASK_NORMAL_PRIORITY, 1000, update_sys_up_time, NULL, NULL); /*runs every 1s*/
   //ret &= s_task_create(true, S_TASK_LOW_PRIORITY, 8000, update_system, NULL, NULL);         /*runs every 8s*/

   return ret;
}
/**
 * 
 */
void init_system_info(void)
{
   //if any
}

/**
 * 
 */
void fill_system_params(void)
{
   //if any
}


/**
 * 
 * @param me
 * @param msg
 */
void update_sys_up_time(s_task_handle_t me, s_task_msg_t **msg, void* arg)
{
   sys_time_up++;

#ifdef USE_MESSAGING    
   //A GOOD PRACTICE IS TO ALWAYS FLUSH MESSAGES BEFORE EXIT:
   s_task_flush_msgs(msg);
#endif
}

/**
 * 
 * @param me
 * @param msg
 */
void update_system(s_task_handle_t me, s_task_msg_t **msg, void* arg)
{



#ifdef USE_MESSAGING    
   //A GOOD PRACTICE IS TO ALWAYS FLUSH MESSAGES BEFORE EXIT:
   s_task_flush_msgs(msg);
#endif
}
