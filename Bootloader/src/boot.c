#include "main.h"
#include "boot.h"

/*Macros*/
#define  BOOTINFOADRS   (getenv("DATA_EEPROM") - sizeof(boot_info_t))

/*globals*/
boot_info_t current_boot_info;

#if  getenv("DATA_EEPROM") > 256
    void int_ee_write_buff(uint8_t *buff, uint16_t start_address, uint16_t len)
    {
        uint16_t i;
#else
    void int_ee_write_buff(uint8_t *buff, uint8_t start_address, unsigned int8 len)
    {
        uint8_t i;
#endif

    for (i=0; i<len; i++)
    {
        write_eeprom(start_address + i, buff[i]);
    }
}
///////////////////////////////////////////////////////////////////////////////
#if  getenv("DATA_EEPROM") > 256
    void int_ee_read_buff(uint8_t *buff, uint16_t start_address, uint16_t len)
    {
        uint16_t i;
#else
    void int_ee_read_buff(uint8_t *buff, uint16_t start_address, uint16_t len)
    {
        uint8_t i;
#endif

    for (i=0; i<len; i++)
    {
        buff[i] = read_eeprom(start_address + i);
    }
}

boot_state_t get_boot_info()
{
    int_ee_read_buff((uint8_t*)&current_boot_info, BOOTINFOADRS, sizeof(boot_info_t));

    if (current_boot_info.data_valid != EEVALID)
        return BOOT_INVALID;

    switch (current_boot_info.boot_st)
    {
        case BOOT_LOADER:
        case BOOT_APP:
            return current_boot_info.boot_st;
            break;

        default:
            return BOOT_INVALID;
        break;
    }
}

void set_boot_info(boot_state_t bs)
{
    current_boot_info.boot_st = bs;
    current_boot_info.data_valid = EEVALID;
    int_ee_write_buff((uint8_t*)&current_boot_info, BOOTINFOADRS, sizeof(boot_info_t));
}

