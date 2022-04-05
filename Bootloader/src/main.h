#ifndef MAIN_H
#define MAIN_H

#include <18F26K22.h>

#FUSES NOWDT               	//No Watch Dog Timer
#FUSES PUT                 	//Power Up Timer
#FUSES NOBROWNOUT          	//No brownout reset
#FUSES BORV29              	//Brownout reset at 2.85V
#FUSES TIMER3B5            	//Timer3 Clock In is on pin B5
#FUSES LVP                  //Low Voltage Programming
#FUSES CPB                  //Boot Block Code Protected

#use delay(internal=64000000)

#use rs232(baud=115200, xmit=PIN_C6, rcv=PIN_C7, errors, uart1)

#use fast_io(ALL)

#include "stdbool.h"
#include <stdint.h>

#include "boot.h"

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

    struct
    {
        uint16_t low;
        uint16_t high;
    }words;
    
    uint32_t data;
}uint32_ut;

#define	COMM_TIMEOUT_RECOVERY	200000
#endif
