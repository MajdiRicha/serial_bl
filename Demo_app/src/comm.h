#ifndef COMM_H
#define COMM_H

#include "scheduler.h"

#define	COMM_MAX_BUFF_SIZE		(512)	/**/   	
#define	COMM_BUFF_INDEX_MASK	(COMM_MAX_BUFF_SIZE - 1)
#define	COMM_REQUEST	        0x80

/*Byte index in data packets:*/
typedef enum _comm_index
{
	COMM_INDEX_LEN_L,
	COMM_INDEX_LEN_H,
	COMM_INDEX_FUNC,
	COMM_INDEX_CMD,
	COMM_INDEX_ARG0,
	COMM_INDEX_ARG1,
	COMM_INDEX_ARG2,
	COMM_INDEX_ARG3,
	COMM_INDEX_ARG4,
	COMM_INDEX_ARG5,
	COMM_INDEX_ARG6,
	COMM_INDEX_ARG7,
	COMM_INDEX_ARG8,
	COMM_INDEX_ARG9,
	COMM_INDEX_ARG10,
	COMM_INDEX_ARG11,
	COMM_INDEX_ARG12,
	COMM_INDEX_ARG13,
	COMM_INDEX_ARG14,
	COMM_INDEX_ARG15
	/*...*/
	/*Last byte: 2's complement of all previous bytes' summation*/
}comm_index_t;

/*Functions*/
typedef enum _comm_function
{
	/*analog functions*/
	COMM_CURR = 0x04,
	COMM_VOLT,
	COMM_FREQ,
	/*logger functions*/
	COMM_LOG_COUNT,
	COMM_LOG_DATA,
	COMM_LOG_RATE,
	/*device functions*/
	COMM_FUNC_FW_REV = 0x80,
	COMM_FUNC_FW_DAT,
	/*Bootloader functions*/
	COMM_FUNC_FLASH = 0xF0
}comm_function_t;

/*Commands*/
typedef enum _comm_command
{
	COMM_CMD_CONFIG,
	COMM_CMD_INIT_RD,
	COMM_CMD_RD,
	COMM_CMD_INIT_WR,
	COMM_CMD_WR,
}comm_command_t;

typedef union uint16_u
{
	struct
	{
		uint8_t low:8;
		uint8_t high:8;
	}bytes;

	uint16_t data;
}uint16_ut;

typedef union uint32_u
{
	struct
	{
		uint8_t lower:8;
		uint8_t low:8;
		uint8_t high:8;
		uint8_t higher:8;
	}bytes;

	uint32_t data;
}uint32_ut;

#define	COMM_TIMEOUT_RECOVERY	5000

bool init_comm(void);

#endif
