#ifndef MSP430_FLASH_H
#define MSP430_FLASH_H

/*
   Information memory on msp430f2274:
   Segment A : 0x10FF to 0x10C0
   Segment B : 0x10BF to 0x1080
   Segment C : 0x107F to 0x1040
   Segment D : 0x103F to 0x1000

   Sements A contains calibration data and is protected by LOCKA.
 */

#define INFOB_END 0x10BF
#define INFOB_START 0x1080
#define INFOC_END 0x107F
#define INFOC_START 0x1040
#define INFOD_END 0x103F
#define INFOD_START 0x1000

/*
 * IMPORTANT: A flash word (low + high byte must not be written
 * more than twice between erasures. Otherwise, damage can occur)
 * (SLAU144 7-10)
 */

/* programs 1 byte (8 bit) into the flash memory */
int flash_write_byte(unsigned char *data_ptr, unsigned char byte);
/* programs 1 word (16 bits) into the flash memory */
int flash_write_word(unsigned int *data_ptr, unsigned int word);
/* erases 1 Segment of flash memory */
void flash_erase_segment(unsigned int *data_ptr);
/* errase all of INFO memory except segment A */
void flash_erase_info_memory();

#endif
