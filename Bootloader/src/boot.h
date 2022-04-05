#ifndef	BOOT_H
#define	BOOT_H

#define LOADER_SIZE             (0x0800)
#define LOADER_START            (0)
#define LOADER_END              (LOADER_SIZE-1)
#define APPLICATION_START       (LOADER_SIZE)
#define APPLICATION_END         (getenv("PROGRAM_MEMORY")-1)
#define APPLICATION_ISR         (APPLICATION_START+8)

#define ISR_RELOC_H             (APPLICATION_START + 8)
#define ISR_RELOC_L             (APPLICATION_START + 18)

#define  EEVALID                0xA5

typedef enum boot_state
{
    BOOT_LOADER,
    BOOT_APP,
    BOOT_INVALID
}boot_state_t;

typedef struct boot_info
{
    boot_state_t boot_st;
    uint8_t data_valid;
}boot_info_t;

#ifdef  __APP__
    #build(reset=APPLICATION_START, interrupt=APPLICATION_ISR)
    #org 0, LOADER_END {}
#else
    #rom (getenv("EEPROM_ADDRESS") + getenv("DATA_EEPROM") - sizeof(boot_info_t)) ={BOOT_LOADER, EEVALID}
#endif



boot_state_t get_boot_info();
void set_boot_info(boot_state_t bs);

#endif
