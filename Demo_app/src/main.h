#ifndef MAIN_H
#define MAIN_H

#include <18F26K22.h>
#device ADC=10
#device ANSI
//High priority interrupts enable
//#device HIGH_INTS=TRUE

#FUSES NOWDT               	//No Watch Dog Timer
#FUSES PUT                 	//Power Up Timer
#FUSES NOBROWNOUT          	//No brownout reset
#FUSES BORV29              	//Brownout reset at 2.85V
#FUSES TIMER3B5            	//Timer3 Clock In is on pin B5
#FUSES LVP                  //Low Voltage Programming

#use delay(internal=64000000)

#use rs232(baud=115200, xmit=PIN_C6, rcv=PIN_C7, errors, uart1)
#use i2c(master, sda=PIN_C4, scl=PIN_C3, fast=400000, i2c1)

#use fast_io(ALL)

#include "target_port.h"

#define	__APP__
#include "boot.h"

#endif
