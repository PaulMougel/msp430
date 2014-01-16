/*
 * ez430 lib
 * Simple interface : open - read/write -close
 */
#include <stdio.h>

#define EZ430_VID 0x0451
#define EZ430_PID 0xF432

#define DEBUG_LEVEL 2
#define UART_IFACE 0
#define MAX_PACKET_SIZE 64	// Bytes

#define TIME_OUT 10000 /* 10s, as in protothread demo */

struct ez430_dev {
	struct libusb_device_handle *ez430_handle;
	struct libusb_device *ez430_dev;
	struct libusb_config_descriptor *ez430_config;
	const struct libusb_interface_descriptor *ez430_interface;
	char ez430_uart_in_endpoint;
	char ez430_uart_out_endpoint;
};

/*
 * - Allocate the device
 * - Initialize libusb
 * - Open the device
 * - check the interfaces
 */
struct ez430_dev *ez430_open();

/*
 * - Close the device
 * - Close libusb
 * - free the device
 */
int ez430_close(struct ez430_dev *dev);

/*
 * Blocking function to read some data from the device
 */
ssize_t ez430_read(struct ez430_dev *dev, void *buf, ssize_t length);

/*
 * Blocking function to write some data to the device
 */
ssize_t ez430_write(struct ez430_dev *dev, const void *buf, int length);

void ez430_dump_info(struct ez430_dev *dev);
