/**
 *
 * @file		scheduler.c
 * @brief		Co-operative scheduler
 * @details		Contains co-operative scheduler API
 * @author		m_richa
 * @date		12/06/21
   @version 	1.0.E	Added s_arg to task
				1.0.D	YIELD() ) macro added 
				1.0.C	added s_task_b_delay   
				1.0.B fixed s_mode assignment 
				1.0.A	Fixed s_task_idle prototype 
				1.0.9 Added support to XC16 MCP compiler 
				1.0.8 s_task_mode added (SYNC & ASYNC) ) 
				1.0.7 s_task_resume modified to support run mode 
				1.0.6 malloc c changed to calloc in msg data 
				1.0.5 Modified for IPGLB code style Added
   					mailbox s_task 3-level priority added
   			1.0.4	eTicks added to sTask to count elapsed
   					ticks Semaphore handling added
						(5.084)
				1.0.3	InitScheduler function added
						s_task_idle sTask added to
						GetSchedulerStats functionBusy
						LED option added
						(5.084)
				1.0.2	Added support to NIOS2 (15.1)
				1.0.1	Functions modified:
						sTaskCreate(..)
						sTaskResume(..)
						sTaskSetPeriodicTicks(..)
						Scheduler()
						Optimized scheduling procedure
						Better timing management
						(CCS 5.083)
				1.0.0	Alpha release
						(CCS 5.083)
						Beta
						sTask parameter changed as
						per jeremiah's suggestion
						(CCS 5.083)
 * @warning		Anything that might crash the program.
 * @copyright
 * 2019 IPG Photonics Corporation ALL RIGHTS RESERVED
 * This program may not be reproduced, in whole or in part in any
 * form or any means whatsoever without the written permission of:\n
 * IPG Photonics Corporation
 * 50 Old Webster Road
 * Oxford, MA 01540, USA
 */

#if (defined (__PCH__) || defined (__PCD__))	/*CCS C compiler?*/
#include "main.h"
#endif

/*includes*/
#include "scheduler.h"


#ifdef USE_RETIMER
	#if (defined (__PCH__) || defined (__PCD__)) || defined (__NIOS2__) || defined(STM32F40XX) || defined(STM32F4XX) || defined(STM32F2XX) || defined(__XC16__)
		#warning "When retimer is enabled, all asynchronous tasks will run at the rate of 1ms"
	#else
		#pragma message ("WARNING: When retimer is enabled, all asynchronous tasks will run at the rate of 1ms")
	#endif
#endif


/*local macros*/
#define	TICKSPERSECOND			1000	/*!<Ticks per second, given 1ms tick*/
#define	NULL_TASK				0		/*!<Handle for null task*/
#define	PRIORITIES_CYCLE_LEN	6		/*!<Priorities order table size*/

/**
 * Task mode enumeration
 */
typedef enum s_task_mode
{
	S_TASK_ASYNC_MODE,				/*!<Asynchronous and fast*/
	S_TASK_SYNC_MODE				/*!<Syncronized to multiple of 1ms*/
}s_stask_mode_t;

/**
* Task node
*/
typedef struct s_task_node
{
	s_task_handle_t		s_task_id;	/*!<s_task ID (Handle)*/
	bool				active;		/*!<s_task status*/
	s_stask_mode_t		s_mode;		/*!<s_task mode*/
	s_task_priority_t	s_priority;	/*!<s_task priority*/
	cntr_size_t 		s_ticks;	/*!<s_task periodic ticks*/
	cntr_size_t 		n_ticks;	/*!<s_task current tick count*/
	cntr_size_t 		e_ticks;	/*!<elapsed ticks*/
	s_task_t 			s_task;		/*!<s_task function's pointer*/
	s_task_msg_t*		s_task_msg;	/*!<s_task message head*/
	void*				a_task_arg;	/*!<Task argument passed upon creation*/
	struct s_task_node* next;		/*!<pointer to next s_task*/
	struct s_task_node* previous;	/*!<pointer to previous s_task*/
}s_task_node_t;

/**
 * List of tasks definition
 */
typedef struct scheduler_info
{
	s_task_node_t*	s_task_head;	/*!<s_tasks linked list's head*/
	s_task_node_t*	s_task_tail;	/*!<s_tasks linked list's tail*/
	uint16_t		s_tasks_count;	/*!<s_tasks total count*/
}scheduler_info_t;

/**
 * List of mailbox.
 */
typedef struct mailbox
{
	s_task_msg_t*	mailbox_head;	/*!<Mailbox linked list's head*/
	s_task_msg_t*	mailbox_tail;	/*!<Mailbox linked list's tail*/
	uint16_t		mailbox_count;	/*!<Mailbox total count*/
} mailbox_t;

/*global vars*/
scheduler_info_t	sched_info;		/*!<Contains head, tail, count*/
/*s_task's priority cycle table*/
const s_task_priority_t	priority_cycle[PRIORITIES_CYCLE_LEN] =
								{S_TASK_HIGH_PRIORITY, S_TASK_NORMAL_PRIORITY,
								 S_TASK_HIGH_PRIORITY, S_TASK_LOW_PRIORITY,
								 S_TASK_HIGH_PRIORITY, S_TASK_NORMAL_PRIORITY};
mailbox_t			mbox_info;					/*!<Contains head, tail, count*/
s_task_handle_t		idle_s_task_hndl = 0;		/*!<Handle to idle_task*/
uint8_t				cpu_usage_p = 0;			/*!<Percent CPU usage*/
ticks_cllbk_t		get_system_ticks = NULL;	/*!<Callback for system ticks*/

/*local prototypes*/
static void s_task_idle(s_task_handle_t me, s_task_msg_t** msg, void* arg);
static s_task_node_t* s_task_locate(s_task_handle_t s_task_hndl);
static void s_task_delete_node(s_task_node_t* s_task_node);
static void s_task_suspend_node(s_task_node_t* s_task_node);
static void s_task_resume_node(s_task_node_t* node, bool run);
#ifdef USE_MESSAGING
static void s_task_insert_message(s_task_msg_t* s_task_msg);
static void s_task_remove_message(s_task_msg_t* s_task_msg);
static s_task_msg_t* s_task_locate_tail(s_task_msg_t* msg_head);
static void s_task_extract_msg(s_task_handle_t s_task_dst_hndl, s_task_msg_t** msg_head);
#endif

/**
 * Function to initialize scheduler and create idle
 * s_task_node_t
 *
 * @author m_richa
 * @date 04/07/19
 *
 * @param[in] tks_cllbk ticks counter callback
 *
 * @return false: fail, true: success
 */
bool scheduler_init(ticks_cllbk_t tks_cllbk)
{
	sched_info.s_task_head	 = NULL;
	sched_info.s_task_tail	 = NULL;
	sched_info.s_tasks_count = 0;

	mbox_info.mailbox_head	 = NULL;
	mbox_info.mailbox_tail	 = NULL;
	mbox_info.mailbox_count	 = 0;

	if(tks_cllbk != NULL)
	{
		get_system_ticks = tks_cllbk; /*assign callback function*/
	}
	/*create idle s_task*/
	return s_task_create(1, S_TASK_HIGH_PRIORITY, 1, s_task_idle, &idle_s_task_hndl, NULL);
}

/**
 * Function to dispatch s_tasks sequentially based on priorities
 *
 * @author m_richa
 * @date 03/25/19
 */
void scheduler(void)
{
	s_task_node_t*		cs_task = sched_info.s_task_head;	/*Pointer to head*/
	cntr_size_t    		csys_tick_counter;
	cntr_size_t    		ticks_diff;

#ifdef	USE_RETIMER	/*1st alternative: retimer is used for synchronization*/
	uint8_t				prior_cycle_index = 0;				/*Priority index*/
	static cntr_size_t	ticks_snap = 0;						/*Ticks snapshot*/													  
	cntr_size_t			elapsed_ticks;						/*Elapsed ticks counter*/

	elapsed_ticks = (*get_system_ticks)() - ticks_snap;		/*get elapsed ticks*/

	if (elapsed_ticks)	/*at least 1 tick elapsed?*/
	{
		/*scan priorities*/
		for (prior_cycle_index = 0; prior_cycle_index <= PRIORITIES_CYCLE_LEN; prior_cycle_index++)
		{
			cs_task = sched_info.s_task_head;	/*Pointer to head*/
			while(cs_task)	/*scan s_tasks nodes*/
			{
				if(cs_task->active) /*is it active?*/
				{
					if(cs_task->s_task != NULL) /*task's function exists?*/
					{
	  
						if (priority_cycle[prior_cycle_index] == cs_task->s_priority)	/*check priority*/
						{
							/* Take a snapshot of system ticks just to
							 * make sure value does not change during this process
							 * */
							csys_tick_counter   = (*get_system_ticks)();
							ticks_diff			= csys_tick_counter - cs_task->n_ticks;

							if ((ticks_diff >= cs_task->s_ticks) || (S_TASK_ASYNC_MODE == cs_task->s_mode)) /*should run?*/
							{
								cs_task->n_ticks = csys_tick_counter; /*reload*/
	#ifdef USE_BUSY_LED
								BLEDON();
	#endif
	#ifdef USE_MESSAGING
								//msg_head = s_task_extract_msg(cs_task->s_task_id);
								s_task_extract_msg(cs_task->s_task_id, &(cs_task->s_task_msg));
	#endif
								/*prepare call:*/
								cs_task->e_ticks = csys_tick_counter; /*save snapshot*/
								(*cs_task->s_task)(cs_task->s_task_id, &(cs_task->s_task_msg), (cs_task->a_task_arg)); /*run task*/

								/*calculate elapsed ticks*/
								cs_task->e_ticks = (*get_system_ticks)() - cs_task->e_ticks;
	#ifdef USE_BUSY_LED
								BLEDOFF();
	#endif
							}
						}
					}
				}
				cs_task = cs_task->next;	/*move to next s_task*/
			}
		}
	}

	ticks_snap = (*get_system_ticks)();	/*save snapshot*/

#else	/*2nd alternative: no retimer, s_tasks are served immediately*/
	static uint8_t	prior_cycle_index = 0;	/*Priority index*/

	while(cs_task)	/*scan s_tasks nodes*/
	{
		if(cs_task->active) /*is it active?*/
		{
			if(cs_task->s_task != NULL) /*task's function exists?*/
			{
				if (priority_cycle[prior_cycle_index] == cs_task->s_priority)	/*check priority*/
				{
					/* Take a snapshot of system ticks just to
					 * make sure value does not change during this process
					 * */
					csys_tick_counter   = (*get_system_ticks)();
					ticks_diff			= csys_tick_counter - cs_task->n_ticks;

					if ((ticks_diff >= cs_task->s_ticks) || (S_TASK_ASYNC_MODE == cs_task->s_mode)) /*should run?*/
					{
						cs_task->n_ticks = csys_tick_counter; /*reload*/
#ifdef USE_BUSY_LED
						BLEDON();
#endif
#ifdef USE_MESSAGING
						//msg_head = s_task_extract_msg(cs_task->s_task_id);
						s_task_extract_msg(cs_task->s_task_id, &(cs_task->s_task_msg));
#endif
						/*prepare call:*/
						cs_task->e_ticks = csys_tick_counter; /*save snapshot*/
						(*cs_task->s_task)(cs_task->s_task_id, &(cs_task->s_task_msg), (cs_task->a_task_arg)); /*run task*/

						/*calculate elapsed ticks*/
						cs_task->e_ticks = (*get_system_ticks)() - cs_task->e_ticks;
#ifdef USE_BUSY_LED
						BLEDOFF();
#endif
					}
				}
			}
		}
		cs_task = cs_task->next;	/*move to next s_task*/
	}

	/*move to next priority level and check*/
	if (++prior_cycle_index == PRIORITIES_CYCLE_LEN)	
	{
		prior_cycle_index = 0;	/*reset index*/
	}

#endif
}

/**
 * Function to create a scheduled task
 * 
 * @author			m_richa
 * @date			03/21/19
 * 
 * @param[in]		active s_task_node_t initial state
 * @param[in]		s_task_prio: s_task priority level
 * @param[in]		s_ticks periodic ticks (synchronous); 0: asynchronous
 * @param[in]		s_task_ptr pointer to task's function
 * @param[in,out]	s_tsk_hndl_ptr pointer to handle or NULL if
 * 					not used
 * @param[in]		s_arg pointer to task's argument
 * 
 * @return false: fail, true: success
 */
bool s_task_create(bool active, s_task_priority_t s_task_prio, cntr_size_t s_ticks, 									   
				   s_task_t s_task_ptr, s_task_handle_t* s_tsk_hndl_ptr, void* s_arg)
{
	static uint16_t s_task_id = 0; /*task ID AKA handle*/
	s_task_node_t *new_s_task = malloc(sizeof(s_task_node_t));
	bool res = false;

	if(new_s_task != NULL)
	{
		/*assign values:*/
		new_s_task->s_task_id	= ++s_task_id;
		new_s_task->active		= active;
		new_s_task->s_mode		= (s_ticks > 0) ? S_TASK_SYNC_MODE : S_TASK_ASYNC_MODE;
		new_s_task->s_priority	= s_task_prio;
		new_s_task->s_ticks		= s_ticks;
		new_s_task->n_ticks		= (*get_system_ticks)();
		new_s_task->e_ticks		= 0;
		new_s_task->s_task		= s_task_ptr;
		new_s_task->s_task_msg	= NULL;
		new_s_task->a_task_arg	= s_arg;
		new_s_task->next		= NULL;
		new_s_task->previous	= NULL;

		sched_info.s_tasks_count++; /*increment count*/

		if(sched_info.s_task_head != NULL) /*nth s_task_node?*/
		{
			new_s_task->previous			= sched_info.s_task_tail;
			sched_info.s_task_tail->next	= new_s_task;
			sched_info.s_task_tail			= new_s_task; /*point tail*/
		}
		else /*1st s_task_node_t?*/
		{
			sched_info.s_task_head = new_s_task; /*point head*/
			sched_info.s_task_tail = new_s_task; /*point tail*/
		}

		if(s_tsk_hndl_ptr != NULL)
		{
			*s_tsk_hndl_ptr = new_s_task->s_task_id;
		}

		res = true; /*success*/
	}

	return (res); /*fail, no available RAM*/
}

/**
 * Function to handle a scheduled task
 * 
 * @author m_richa 
 * @date 10/04/19
 * 
 * @param[in] s_task_ptr task's function
 * 
 * @return s_task_handle_t s_task handle or NULL_TASK (0) if does not exists
 */
s_task_handle_t s_task_get_handle(s_task_t s_task_ptr)
{
	s_task_node_t* task_node_ptr = sched_info.s_task_head;	/*point to start*/
	s_task_handle_t task_handle = NULL_TASK;

	if (sched_info.s_tasks_count != 0)
	{
		while ((task_node_ptr != NULL) && (NULL_TASK == task_handle) )
		{
			if (task_node_ptr->s_task == s_task_ptr)
			{
				task_handle = task_node_ptr->s_task_id;
			}
			else
			{
				task_node_ptr = task_node_ptr->next;
			}
		}
	}
	return (task_handle);
}


/**
 * Function to delete a given s_task
 * 
 * @author m_richa 
 * @date 03/22/19
 * 
 * @param[in] s_task_hndl s_task's handle
 * 
 * @return false: fail, true: success 
 */
bool s_task_delete(s_task_handle_t s_task_hndl)
{
	bool			res  = false;
	s_task_node_t* 	node = s_task_locate(s_task_hndl);/*get node by handle*/

	if(node != NULL)
	{
		s_task_delete_node(node);
		res = true;
	}
	return	(res);
}

/**
 * Function to get s_task's priority
 *
 * @author m_richa
 * @date 10/14/19
 *
 * @param[in] s_task_hndl s_task's handle
 * @param[out] s_task_prio s_task's priority pointer
 *
 * @return false: fail, true: success
 */
bool s_task_get_priority(s_task_handle_t s_task_hndl,
                         s_task_priority_t* s_task_prio)
{
	bool			res  = false;
	s_task_node_t* 	node = s_task_locate(s_task_hndl);/*get node by handle*/

	if(node != NULL)
	{
		*s_task_prio = node->s_priority;	/*copy priority level*/
		res = true;
	}
	return	(res);
}

/**
 * Function to set s_task's priority
 *
 * @author m_richa
 * @date 10/14/19
 *
 * @param[in] s_task_hndl s_task's handle
 * @param[out] s_task_prio s_task's priority
 *
 * @return false: fail, true: success
 */
bool s_task_set_priority(s_task_handle_t s_task_hndl,
                         s_task_priority_t s_task_prio)
{
	bool			res  = false;
	s_task_node_t* 	node = s_task_locate(s_task_hndl);/*get node by handle*/

	if(node != NULL)
	{
		node->s_priority = s_task_prio;	/*copy priority level*/
		res = true;
	}
	return	(res);
}

/**
 * Function to suspend a given s_task
 * 
 * @author m_richa 
 * @date 03/22/19
 * 
 * @param[in] s_task_hndl s_task's handle
 * 
 * @return false: fail, true: success 
 */
bool s_task_suspend(s_task_handle_t s_task_hndl)
{
	bool			res		= false;
	s_task_node_t* 	node	= s_task_locate(s_task_hndl);

	if(node != NULL)
	{
		s_task_suspend_node(node);
		res = true;
	}
	return	(res);
}

/**
 * Function to resume a given s_task
 * 
 * @author m_richa 
 * @date 03/22/19
 * 
 * @param[in] s_task_hndl s_task's handle 
 * @param[in] run:	false: reset s_task's n_tick count.
 * 					true : run s_task on next tick. 
 * 
 * @return false: fail, true: success 
 */
bool s_task_resume(s_task_handle_t s_task_hndl, bool run)
{
	bool			res  = false;
	s_task_node_t* 	node = s_task_locate(s_task_hndl);

	if(node != NULL)
	{
		s_task_resume_node(node, run);
		res = true; /*success*/
	}

	return (res);
}

/**
 * Function to get number of periodic ticks of a given s_task
 * 
 * @author m_richa 
 * @date 03/22/19
 * 
 * @param[in] s_task_hndl s_task's handle 
 * @param[in,out] ticks_count s_task's ticks 
 *  
 * @return false: fail, true: success   
 */
bool s_task_get_periodic_ticks(s_task_handle_t s_task_hndl,
                               cntr_size_t* ticks_count)
{
	bool			res  = false;
	s_task_node_t* 	node = s_task_locate(s_task_hndl);

	if(node != NULL)
	{
		*ticks_count = node->s_ticks;
		res = true;
	}

	return	(res);
}

/**
 * Function to set number of periodic ticks of a given
 * s_task 
 * 
 * @author m_richa 
 * @date 03/22/19
 * 
 * @param[in] s_task_hndl s_task's handle 
 * @param[in] s_ticks number of ticks 
 * @param[in] run:	false: reset s_task's n_tick count.
 * 					true : run s_task on next tick.
 *  
 * @return false: fail, true: success    
 */
bool s_task_set_periodic_ticks(s_task_handle_t s_task_hndl, cntr_size_t s_ticks,
                               bool run)
{
	bool			res  = false;
	s_task_node_t* 	node = s_task_locate(s_task_hndl);

	if(node != NULL)
	{
		node->s_ticks     = s_ticks;
		if(run) /*should run on next tick?*/
		{
			node->n_ticks = (*get_system_ticks)() - s_ticks; /*load ticks*/
		}
		else
		{
			node->n_ticks = (*get_system_ticks)(); /*load ticks*/
		}
		res = true; /*success*/
	}

	return	(res);
}

#ifdef USE_MESSAGING
/**
 * Function to send bytes message to a given s_task.
 * NOTE: data bytes are copied inside this function; thus they 
 * can be destroyed in the source task right after sending. 
 * 
 * @author m_richa 
 * @date 03/22/19
 *  
 * @param[in] s_task_src_hndl source s_task handle
 * @param[in] s_task_dst_hndl desination s_task handle
 * @param[in] msg_bytes data pointer to be sent 
 * @param[in] bytes_count data size
 * 
 * @return false: fail, true: success
 */
bool s_task_send_msg(s_task_handle_t s_task_src_hndl,
                     s_task_handle_t s_task_dst_hndl, uint8_t* msg_bytes,
                     uint16_t bytes_count)
{
	bool			res	 = false;
	s_task_msg_t*	msg_node = malloc(sizeof(s_task_msg_t));

	if(msg_node != NULL)
	{
		/*fill fields:*/
		msg_node->s_task_src_hndl	= s_task_src_hndl;
		msg_node->s_task_dst_hndl	= s_task_dst_hndl;
		msg_node->data_size		= bytes_count;
		msg_node->next 				= NULL;
		msg_node->previous			= NULL;
		msg_node->data 				= calloc(bytes_count, sizeof(uint8_t));
		if(msg_node->data != NULL)
		{
			memcpy(msg_node->data, msg_bytes, bytes_count);
			s_task_insert_message(msg_node);
			res = true;
		}
		else
		{
			/*
			 * we couldn't allocate enough space for the data
			 * drop the whole message
			 * */
			free(msg_node);
		}
	}

	return	(res);
}

/**
 * Function to flush messages' list
 * 
 * @author m_richa 
 * @date 10/07/19
 * 
 * @param[in,out] msg_head pointer to head of messages' list
 */
void s_task_flush_msgs(s_task_msg_t** msg_head)
{
#if (defined (__PCH__) || defined (__PCD__))
	/*CCS compiler is non re-entrant, thus iterative*/
	s_task_msg_t* temp;
	while ((*msg_head) != NULL)
	{
		temp = (*msg_head)->next;
		free((*msg_head)->data); /*data*/
		free((*msg_head)); /*node*/
		*msg_head = temp;
	}

#else   /*other re-entrant, thus recursive*/
	if(*msg_head != NULL)
	{
		s_task_flush_msgs(&((*msg_head)->next));
		free((*msg_head)->data); /*data*/
		free(*msg_head); /*node*/
		*msg_head = NULL;
	}
#endif
}

#endif

/**
 * Function to get scheduler statistics
 *
 * @author m_richa 
 * @date 04/10/19
 *
 * @param[out] ss pointer to statistics structure
 */
void scheduler_get_stats(scheduler_stats_t* ss)
{
	ss->s_tasks_count   = sched_info.s_tasks_count;
	ss->s_tasks_cpu_usage = cpu_usage_p;
	ss->s_tasks_msg_count = mbox_info.mailbox_count;
}

#ifdef USE_SEMAPHORE
/**
 * Function to initialize semaphore
 *
 * @author m_richa 
 * @date 04/15/19
 *
 * @param[in,out] sema pointer to semaphore structure
 * @param[in] cs_tasks number of allowed concurrent tasks
 */
void semaphore_init(s_semaphore_t* sema, uint8_t cs_tasks)
{
	sema->sem			= cs_tasks;
	sema->s_tasks_count = cs_tasks;
}

/**
 * Function to release and make semaphore available
 *
 * @author m_richa 
 * @date 04/15/19
 *
 * @param[in,out] sema pointer to semaphore structure
 */
void semaphore_release(s_semaphore_t* sema)
{
	if (sema->sem < sema->s_tasks_count)
	{
		sema->sem++; /*increment*/
	}
}

/**
 * Function to hold semaphore
 *
 * @author m_richa 
 * @date 04/15/19
 *
 * @param[in,out] sema pointer to semaphore structure
 *
 * @return false: fail  true: success
 */
bool semaphore_hold(s_semaphore_t* sema)
{
	bool res = false;

	if (sema->sem)
	{
		sema->sem--; /*decrement*/
		res = true;
	}
	return (res);
}

/**
 * Function to check if a semaphore is available
 *
 * @author m_richa 
 * @date 04/15/19
 *
 * @param[in] sema pointer to semaphore structure
 *
 * @return false: not available  true: available
 */
bool semaphore_available(s_semaphore_t* sema)
{
	return (sema->sem) ? true : false;
}
#endif


/**
 * Function te generate blocking delay in milliseconds 
 * NOTE: Avoid using this function 
 * 
 * @author m_richa (03/11/21)
 * 
 * @param[in] ms delay in milliseconds
 */
void s_task_b_delay(uint32_t ms)
{
	uint32_t local_ticks = (*get_system_ticks)();

	while (((*get_system_ticks)() - local_ticks) < ms);
}


/**
 * Function to locate and return s_task_node_t's entry pointer
 *
 * @author m_richa
 * @date 03/22/19
 *
 * @param[in] s_task_hndl s_task's handle
 *
 * @return pointer to s_task_node_t entry
 */
static s_task_node_t* s_task_locate(s_task_handle_t s_task_hndl)
{
	s_task_node_t *task_node_ptr = sched_info.s_task_head;  /*point to start*/
	bool located = false;

	if (sched_info.s_tasks_count != 0)
	{
		while ((task_node_ptr != NULL) && (!located))
		{
			if (task_node_ptr->s_task_id == s_task_hndl)
			{
				located = true;
			}
			else
			{
				task_node_ptr = task_node_ptr->next;
			}
		}
	}
	return (task_node_ptr);
}

/**
 * Function to delete a given s_task_node_t
 *
 * @author m_richa
 * @date 03/22/19
 *
 * @param[in] s_t s_task_node_t's entry pointer
 */
static void s_task_delete_node(s_task_node_t* s_task_node)
{
	if(s_task_node == sched_info.s_task_head)
	{
		if(1 == sched_info.s_tasks_count) /*head = tail?*/
		{
			/*reset all:*/
			sched_info.s_task_head = NULL;
			sched_info.s_task_tail = NULL;
		}
		else
		{
			sched_info.s_task_head				= sched_info.s_task_head->next;
			sched_info.s_task_head->previous 	= NULL;
		}
	}
	else if(s_task_node == sched_info.s_task_tail) /*on tail?*/
	{
		sched_info.s_task_tail			= sched_info.s_task_tail->previous;
		sched_info.s_task_tail->next	= NULL;
	}
	else /*somewhere in the middle*/
	{
		s_task_node->previous->next = s_task_node->next;
		s_task_node->next->previous = s_task_node->previous;
	}

#ifdef USE_MESSAGING
	/*destroy related messages:*/
	s_task_extract_msg(s_task_node->s_task_id, &(s_task_node->s_task_msg));
	s_task_flush_msgs(&(s_task_node->s_task_msg));
#endif

	free(s_task_node);
	sched_info.s_tasks_count--;
}

/**
 * Function that suspends a s_task_node_t from the list of nodes.
 *
 * @param[in,out] 	s_task_node	pointer to the task node.
 */
static void s_task_suspend_node(s_task_node_t* s_task_node)
{
	s_task_node->active = 0;
}

/**
 * Function that resume a s_task_node_t from the list of nodes.
 *
 * @param[in,out] 	s_task_node	pointer to the task node. 
 * @param[in] run:	false: reset s_task's n_tick count.
 * 					true : run s_task on next tick.  
 */
static void s_task_resume_node(s_task_node_t* node, bool run)
{
	node->active  = 1;

	if(run) /*should run on next tick?*/
	{
		node->n_ticks = (*get_system_ticks)() - node->s_ticks; /*load ticks*/
	}
	else	/*Delayed?*/
	{
		node->n_ticks = (*get_system_ticks)(); /*load ticks*/
	}
}

#ifdef USE_MESSAGING
/**
 * Function to insert a message into the mailbox
 *
 * @author m_richa
 * @date 10/07/19
 *
 * @param[in] s_task_param pointer to s_task param node
 */
static void s_task_insert_message(s_task_msg_t* s_task_msg)
{
	if(mbox_info.mailbox_head != NULL) /*nth node?*/
	{
		s_task_msg->previous 			= mbox_info.mailbox_tail;
		mbox_info.mailbox_tail->next	= s_task_msg;
		mbox_info.mailbox_tail			= s_task_msg;
	}
	else /*1st node?*/
	{
		mbox_info.mailbox_head = s_task_msg;
		mbox_info.mailbox_tail = s_task_msg;
	}

	mbox_info.mailbox_count++; /*increment count*/
}
/**
 * Function to remove a message from the mailbox
 *
 * @author m_richa
 * @date 10/17/19
 *
 * @param[in] s_task_msg pointer to s_task msg node
 */
static void s_task_remove_message(s_task_msg_t* s_task_msg)
{
	if (s_task_msg == mbox_info.mailbox_head)	/*1st msg in mailbox?*/
	{
		if (1 == mbox_info.mailbox_count ) /*head = tail?*/
		{
			/*reset all:*/
			mbox_info.mailbox_head  = NULL;
			mbox_info.mailbox_tail = NULL;
		}
		else
		{
			mbox_info.mailbox_head = s_task_msg->next;	/*point to next node*/
			mbox_info.mailbox_head->previous = NULL; /*nthg behind*/
		}
	}
	else if (s_task_msg == mbox_info.mailbox_tail) /*on tail?*/
	{
		mbox_info.mailbox_tail = mbox_info.mailbox_tail->previous;
		mbox_info.mailbox_tail->next = NULL;
	}
	else    /*in the middle*/
	{
		s_task_msg->previous->next = s_task_msg->next;	/*skip this node*/
		s_task_msg->next->previous = s_task_msg->previous;
	}
}

/**
 * Function to locate messages tail
 *
 * @author m_richa
 * @date 10/17/19
 *
 * @param[in] s_task_msg pointer to s_task msg node
 *
 * @return messages' tail pointer
 */
static s_task_msg_t* s_task_locate_tail(s_task_msg_t* msg_head)
{
	s_task_msg_t* res = NULL;

	while (msg_head)
	{
		res = msg_head;
		msg_head = msg_head->next; /*skip*/
	}

	return (res);
}   

/**
 * Function to extract messages' list
 *
 * @author m_richa
 * @date 10/07/19
 *
 * @param[in] s_task_dst_hndl handle to s_task
 * @param[in,out] msg_head s_task message head
 *
 * @return s_task_msg_t message list head
 */
static void s_task_extract_msg(s_task_handle_t s_task_dst_hndl, s_task_msg_t** msg_head)
{
	s_task_msg_t* msg_tail	= s_task_locate_tail(*msg_head); /*local tail*/
	s_task_msg_t* scnr		= mbox_info.mailbox_head; /*point scanner to start*/
	s_task_msg_t* temp		= NULL;

	while(scnr != NULL)
	{
		if(scnr->s_task_dst_hndl == s_task_dst_hndl) /*match?*/
		{
			temp = scnr;
			
			s_task_remove_message(temp);	/*extract node from mailbox:*/

			mbox_info.mailbox_count--;	/*decrement msgs count*/
			scnr = scnr->next;			/*move next*/
			
			/*detach node:*/
			temp->next = NULL;
			temp->previous = NULL;

			/*add extracted node to msg list:*/
			if(NULL == *msg_head) /*1st message to add*/
			{
				*msg_head			= temp; /*set head*/
				msg_tail 			= temp; /*set local moving tail*/
			}
			else /*nth message to add?*/
			{
				msg_tail->next  = temp; /*attach node*/
				temp->previous  = msg_tail; /*point backward*/
				msg_tail 		= msg_tail->next; /*move next*/
			}

			msg_tail->next = NULL;

			if (mbox_info.mailbox_count == 0)	/*empty mailbox?*/
			{
				mbox_info.mailbox_tail = NULL;
			}
		}
		else
		{
			scnr = scnr->next; /*move next*/
		}
	}
}																						

#endif

/**
 * The idle s_task_node_t, used to calculate CPU usage over a second
 *
 * @author m_richa
 * @date 04/07/19
 *
 * @param[in] me handle to the current s_task
 * @param[in] msg pointer to data bytes (NULL if none)
 */
static void s_task_idle(s_task_handle_t me, s_task_msg_t** msg, void* arg)
{
	static cntr_size_t init_ticks = 0;
	cntr_size_t e_ticks_total = 0;
	s_task_node_t* cs_task = sched_info.s_task_head;	/*Pointer to head*/

	if(((*get_system_ticks)() - init_ticks) >= TICKSPERSECOND) /*1 s elapsed?*/
	{
		/*
		 * other alternative:
		 * add e_ticks of all s_tasks and divide by TICKSPERSECOND
		 * */

		if (cs_task)	/*s_tasks exist?*/
		{
			cs_task = cs_task->next;	/*skip idle_s_task*/
			while (cs_task)
			{
				e_ticks_total += cs_task->e_ticks;	/*accumulate elapsed ticks*/
				cs_task = cs_task->next;	/*move next*/
			}
		}

		/*trim accumulator if needed:*/
		e_ticks_total = (e_ticks_total > TICKSPERSECOND) ? TICKSPERSECOND : e_ticks_total;

		/*calculate CPU usage:*/
		cpu_usage_p = (uint8_t)(((float)e_ticks_total / (float)TICKSPERSECOND) * 100.0);

		init_ticks = (*get_system_ticks)(); /*reload*/
	}
}
/*EOF*/

