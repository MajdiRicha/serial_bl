#ifndef  HARDWARE_H
#define  HARDWARE_H

/*LEDs*/
#define     LED1pin             PIN_A7
#define     LED1On()            output_high(LED1pin)
#define     LED1Off()           output_low(LED1pin)
#define     LED1Toggle()        output_toggle(LED1pin)
#define     LED1STATE()         input(LED1pin)

//EEPROM WP:
#define     EE_ASSERT_WP()      output_high(PIN_C5)
#define     EE_DEASSERT_WP()    output_low(PIN_C5)

//Relay:

//USB:
#define     USB_ATTACHED()  input(PIN_B2)

///////////////////////////////////////////////////////////////////////////////
void init_io_ports(void);
bool init_hw(void);
///////////////////////////////////////////////////////////////////////////////
#endif
