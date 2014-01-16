#if defined(__GNUC__) && defined(__MSP430__)
/* This is the MSPGCC compiler */
#include <msp430.h>
#include <legacymsp430.h>
#elif defined(__IAR_SYSTEMS_ICC__)
/* This is the IAR compiler */
#include <io430.h>
#endif

#include "watchdog.h"
#include "clock.h"
#include "flash.h"

void flash_init()
{
	/* Flash timing generator frequency: ideally around 366 Hz
	 *  257 Hz < Fftg < 476 Hz from (datasheet slas504 p55) */
	switch (get_mclk_freq_mhz()) {
	case 1:
		FCTL2 = FWKEY | FSSEL_2 | FN1 | FN0;	/* Fftg = MCLK/3 = 333 */
		break;
	case 8:
		FCTL2 = FWKEY | FSSEL_2 | FN4 | FN2 | FN1;	/* Fftg = MCLK/22 = 364 */
		break;
	case 12:
		FCTL2 = FWKEY | FSSEL_2 | FN5 | FN0;	/* Fftg = MCLK/33 = 364 */
		break;
	case 16:
		FCTL2 = FWKEY | FSSEL_2 | FN5 | FN3 | FN2;	/* Fftg = MCLK/44 = 364 */
		break;
	}
}

/* programs 1 byte (8 bit) into the flash memory,
 * returns non 0 if the write failed, that is when a segment erase
 * is needed because the data_ptr location was previously written */
int flash_write_byte(unsigned char *data_ptr, unsigned char byte)
{
	if (*data_ptr == byte) {
		return 0;
	} else if (*data_ptr != 0xFF) {
		/* don't allow 2 several consecutive writes on the same location */
		return -1;
	}

	flash_init();
	/* interrupts are automatically disabled during
	 * flash operation as EEI = 0 and EEIEX = 0 (FCTL1 bits) */
	watchdog_stop();
	FCTL3 = FWKEY;		/* Lock = 0 */
	FCTL1 = FWKEY | WRT;	/* WRT = 1 */
	*data_ptr = byte;	/* program Flash byte */
	FCTL1 = FWKEY;		/* WRT = 0 */
	FCTL3 = FWKEY | LOCK;	/* Lock = 1 */
	watchdog_restore();

	return 0;
}

/* programs 1 word (16 bit) into the flash memory,
 * returns non 0 if the write failed, that is when a segment erase
 * is needed because the data_ptr location was previously written */
int flash_write_word(unsigned int *data_ptr, unsigned int word)
{
	if (*data_ptr == word) {
		return 0;
	} else if (*data_ptr != 0xFFFF) {
		/* don't allow 2 several consecutive writes on the same location */
		return -1;
	}

	flash_init();
	watchdog_stop();
	FCTL3 = FWKEY;		/* Lock = 0 */
	FCTL1 = FWKEY | WRT;	/* WRT = 1 */
	*data_ptr = word;	/* program Flash word */
	FCTL1 = FWKEY;		/* WRT = 0 */
	FCTL3 = FWKEY | LOCK;	/* Lock = 1 */
	watchdog_restore();

	return 0;
}

/* erases 1 Segment of flash memory */
void flash_erase_segment(unsigned int *data_ptr)
{
	flash_init();
	watchdog_stop();
	FCTL3 = FWKEY;		/* Lock = 0 */
	FCTL1 = FWKEY | ERASE;	/* ERASE = 1 */
	*data_ptr = 0;		/* erase Flash segment */
	FCTL1 = FWKEY;		/* ERASE = 0 */
	FCTL3 = FWKEY | LOCK;	/* Lock = 1 */
	watchdog_restore();
}

void flash_erase_info_memory()
{
	flash_erase_segment((unsigned int *)INFOD_START);
	flash_erase_segment((unsigned int *)INFOC_START);
	flash_erase_segment((unsigned int *)INFOB_START);
	/* INFOA contains calibration data and is protected by LOCKA */
}
