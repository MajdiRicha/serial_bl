/**
 *
 * @file		scheduler_options.h
 * @author		m_richa
 * @date		10/07/19
 * @copyright
 * 2019 IPG Photonics Corporation ALL RIGHTS RESERVED
 * This program may not be reproduced, in whole or in part in any
 * form or any means whatsoever without the written permission of:\n
 * IPG Photonics Corporation
 * 50 Old Webster Road
 * Oxford, MA 01540, USA
 */

#ifndef	SCHEDULER_OPTIONS_H
#define	SCHEDULER_OPTIONS_H

//#define	USE_MESSAGING	/*!<Enables s_tasks mailbox*/

//#define	USE_SEMAPHORE	/*!<Enables semaphore handling*/

/*This provides optimized dispatching*/ 
//#define	USE_RETIMER		/*!<Enables retimed synchronization*/		

/*comment if no busy LED is available */
#if (defined (__PCH__) || defined (__PCD__))	/*CCS C compiler?*/
//#define	USE_BUSY_LED	/*!<Enables busy LED indication*/
#endif

#ifdef	USE_BUSY_LED
#define	BLEDON()		output_high(PIN_C1)
#define	BLEDOFF()		output_low(PIN_C1)
#endif
#endif
/*EOF*/

