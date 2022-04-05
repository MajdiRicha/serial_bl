/**
 *
 * @file		target_port.h
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

#ifndef TARGET_PORT_H
#define TARGET_PORT_H

//Specify target compiler/processor

#if (defined (__PCH__) || defined (__PCD__))	//CCS C compiler?
	//types:
	#include "stdbool.h"
	#include <stdint.h>
	typedef		signed int8		sint8_t;
	typedef	signed int16	sint16_t;
	typedef		signed int32	sint32_t;
	typedef	uint32_t cntr_size_t;
	
	#include <stdlibm.h>
	#include <string.h>
#elif defined (__NIOS2__)	//NIOS
	//types:
	#include "alt_types.h"
	#include "stdbool.h"
	typedef		alt_u8	uint8_t; 
	typedef		alt_8 		sint8_t; 
	typedef	alt_u16		uint16_t;
	typedef	alt_16	sint16_t;
	typedef		alt_u32		uint32_t;
	typedef		alt_32	sint32_t;
	typedef	uint32_t cntr_size_t;
	
	//#define	rom	?
	//misc
	//#define make8(Val, B)	(uint8)...	//extract byte B from Val
	//#define make16(H, L)	(uint16)...	//combine two bytes in 1 word
	#include <string.h>
	#include <malloc.h>
	#include <sys/alt_alarm.h>
#elif defined(STM32F40XX) || defined(STM32F4XX)	|| defined(STM32F2XX)//add more?
	#include <stdint.h>

	typedef	uint8_t  	bool;
	typedef	uint32_t cntr_size_t;
	
	#include <stdlib.h>
	#include <stddef.h>
	#include <alloca.h>
	#include <strings.h>
#elif defined(__XC16__) 	/*MCP XC16 compiler*/
    
    #include "stdbool.h"
	typedef		unsigned char  	uint8_t;
	typedef		signed char 	sint8_t;
	typedef	unsigned int	uint16_t;
	typedef	signed int  	sint16_t;
	typedef		unsigned long	uint32_t;
	typedef		signed long 	sint32_t;
	typedef	uint32_t cntr_size_t;
    
    #include <stdlib.h>
	#include <string.h>

#elif defined(__IAR__)		/*IAR compiler*/
	#include <stdbool.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdlib.h>
        
	typedef	uint32_t cntr_size_t;
#else
	
	#include "stdbool.h"
	#include "stdint.h"
	typedef		unsigned char  	uint8_t;
	typedef		signed char 	sint8_t;
	typedef	unsigned int	uint16_t;
	typedef	signed int  	sint16_t;
	typedef		unsigned long	uint32_t;
	typedef		signed long 	sint32_t;
	typedef	uint32_t cntr_size_t;
	#include <malloc.h>
	#include <string.h>
#endif

#endif

/*EOF*/


