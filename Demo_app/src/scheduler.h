/**
 *
 * @file		scheduler.h
 * @author		m_richa
 * @date		03/22/19
 * @copyright
 * 2019 IPG Photonics Corporation ALL RIGHTS RESERVED
 * This program may not be reproduced, in whole or in part in any
 * form or any means whatsoever without the written permission of:\n
 * IPG Photonics Corporation
 * 50 Old Webster Road
 * Oxford, MA 01540, USA
 */

#ifndef	SCHEDULER_H
#define	SCHEDULER_H

#include "target_port.h"
#include "scheduler_options.h"

#ifdef USE_MESSAGING
/*Macro to check if message's available*/
#define	S_TASK_MSG_READY(m_h)		(m_h)
/*Macro to get source s_task's handle*/
#define	S_TASK_MSG_SRC(m_h)			(m_h->s_task_src_hndl)
/*Macro to get message data size*/
#define	S_TASK_MSG_DATA_SIZE(m_h)	(m_h->data_size)
/*Macro to get message data pointer*/
#define	S_TASK_MSG_DATA(m_h)		(m_h->data)
/*Macro to navigate to next message*/
#define	S_TASK_NEXT_MSG(m_h)		{m_h = m_h->next;}
#endif

typedef uint16_t s_task_handle_t;       /*!<s_task's handle*/

/**
 * Task Priority enumeration
 */
typedef enum s_task_priority
{
	S_TASK_HIGH_PRIORITY 	= 0,	/*!<Highest priority level*/
	S_TASK_NORMAL_PRIORITY 	= 1,	/*!Normal priority level*/
	S_TASK_LOW_PRIORITY 	= 2		/*!<Lowest priority level*/
}s_task_priority_t;


/**
 * s_task's message node
 */
typedef struct s_task_msg
{
	s_task_handle_t s_task_src_hndl;    /*!<Source task handle*/
	s_task_handle_t s_task_dst_hndl;    /*!<Destination task handle*/
	uint8_t* data;                      /*!<Data pointer*/
	uint16_t data_size;                 /*!<Length of data*/
	struct s_task_msg* next;            /*!< -> */
	struct s_task_msg* previous;        /*!< <- */
}s_task_msg_t;

/*!<s_task's signature:*/
typedef void (*s_task_t)(s_task_handle_t, s_task_msg_t **, void*);
typedef cntr_size_t(*ticks_cllbk_t)(void); /*!<counters width*/

typedef struct scheduler_stats
{
	uint16_t s_tasks_count;			/*!<Total number of tasks*/
	uint8_t  s_tasks_cpu_usage; 	/*!<CPU usage*/
	uint16_t s_tasks_msg_count;		/*!<Mailbox messages' count*/
}scheduler_stats_t;

typedef struct s_semaphore
{
	uint8_t sem;                    /*!<Semaphore variable*/
	uint8_t s_tasks_count;          /*!<Nb of concurrent tasks using this semaphore*/
	/*type	TBD*/
}s_semaphore_t;

bool scheduler_init(ticks_cllbk_t tks_cllbk);
void scheduler(void);

bool s_task_create(bool active, s_task_priority_t s_task_prio, cntr_size_t s_ticks, s_task_t s_task_ptr, s_task_handle_t*, void* s_arg);
s_task_handle_t s_task_get_handle(s_task_t s_task_ptr);
bool s_task_delete(s_task_handle_t s_task_hndl);
bool s_task_get_priority(s_task_handle_t s_task_hndl, s_task_priority_t* s_task_prio);
bool s_task_set_priority(s_task_handle_t s_task_hndl, s_task_priority_t s_task_prio);
bool s_task_suspend(s_task_handle_t s_task_hndl);
bool s_task_resume(s_task_handle_t s_task_hndl, bool run);
bool s_task_get_periodic_ticks(s_task_handle_t s_task_hndl, cntr_size_t* ticks_count);
bool s_task_set_periodic_ticks(s_task_handle_t s_task_hndl, cntr_size_t s_ticks, bool run);

void scheduler_get_stats(scheduler_stats_t* ss);

#ifdef USE_MESSAGING
bool s_task_send_msg(s_task_handle_t s_task_src_hndl, s_task_handle_t s_task_dst_hndl, uint8_t* msg_bytes, uint16_t bytes_count);
void s_task_flush_msgs(s_task_msg_t** msg_head);
#endif

#ifdef USE_SEMAPHORE
void semaphore_init(s_semaphore_t* sema, uint8_t cs_tasks);
void semaphore_release(s_semaphore_t* sema);
bool semaphore_hold(s_semaphore_t* sema);
bool semaphore_available(s_semaphore_t* sema);
#endif

/*Delay functions*/
void s_task_b_delay(uint32_t ms);

/*misc*/
#define	YIELD()		return()

#endif

/*EOF*/

