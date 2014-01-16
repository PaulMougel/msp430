/*
 *	ez430 UART communication library.
 *	It is designed to be a userspace replacement of the
 *	buggy cdc_acm kernel driver.
 *
 *	Copyright 2013 INRIA
 *	Author : T. Pourcelot, CITI Lab
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>

#include <libusb.h>

#include "ez430.h"

#ifdef DEBUG 
#define DEBUG_PRINTF(...) fprintf(stderr,__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

struct ez430_dev *ez430_open()
{
	int r;
	struct ez430_dev *dev;

	/*
	 * Opening libusb
	 */
	r = libusb_init(NULL);
	if (r < 0) {
		error(1, 0, "Failed to initialize libusb");
	}
#ifdef DEBUG
	libusb_set_debug(NULL, DEBUG_LEVEL);
#endif

	/*
	 * Initialize device and configuration
	 */
	dev = malloc(sizeof(struct ez430_dev));
	if (dev == NULL) {
		perror("Failed to allocate device");
		return NULL;
	}

	dev->ez430_handle = libusb_open_device_with_vid_pid(NULL, EZ430_VID, EZ430_PID);	
	if (dev->ez430_handle == NULL) {
        DEBUG_PRINTF("Failed to find device matching %d:%d\n",EZ430_VID,EZ430_PID);
		return NULL;
	}
	dev->ez430_dev = libusb_get_device(dev->ez430_handle);

	/* Get config */
	r = libusb_get_active_config_descriptor(dev->ez430_dev,
						&(dev->ez430_config));

	if (r != 0) {
        DEBUG_PRINTF( "Error : no config found%d\n", r);
		return NULL;
	}

	/* We test if cdc_acm is present */
	if (libusb_kernel_driver_active(dev->ez430_handle, UART_IFACE) != 0) {
        DEBUG_PRINTF("Successfully unloading kernel driver\n");
	} else {
        DEBUG_PRINTF("Failed to remove kernel driver\n");
	}

	r = libusb_claim_interface(dev->ez430_handle, UART_IFACE);
	if (r == 0) {
	} else {
        DEBUG_PRINTF("Error claiming interface!\n");
		return NULL;
	}

	/* Get info on this interface endpoint */
	dev->ez430_interface = &(dev->ez430_config->interface[0].altsetting[0]);

	uint8_t my_endpoint = 0;	// TODO : refactor
	int i = 0;
	for (i = 0; i < (dev->ez430_interface)->bNumEndpoints; i++) {
		struct libusb_endpoint_descriptor my_descriptor =
		    ((dev->ez430_interface)->endpoint[i]);
		int endpoint_type = my_descriptor.bmAttributes & 0x3;
		if (endpoint_type == LIBUSB_TRANSFER_TYPE_BULK) {
			if (my_descriptor.bEndpointAddress & LIBUSB_ENDPOINT_IN)
				my_endpoint = my_descriptor.bEndpointAddress;
		}
	}

	if (my_endpoint == 0) {
        DEBUG_PRINTF( "Error : cannot find a correct endpoint\n");
		return NULL;
	}
	dev->ez430_uart_in_endpoint = (char)my_endpoint;

	/* second endpoint : OUT */
	for (i = 0; i < (dev->ez430_interface)->bNumEndpoints; i++) {
		struct libusb_endpoint_descriptor my_descriptor =
		    ((dev->ez430_interface)->endpoint[i]);
		int endpoint_type = my_descriptor.bmAttributes & 0x3;
		if (endpoint_type == LIBUSB_TRANSFER_TYPE_BULK) {
			if (!
			    (my_descriptor.bEndpointAddress &
			     LIBUSB_ENDPOINT_IN))
				my_endpoint = my_descriptor.bEndpointAddress;
		}
	}
	if (my_endpoint == 0) {
        DEBUG_PRINTF("Error : cannot find a correct endpoint\n");
		return NULL;
	}

	dev->ez430_uart_out_endpoint = (char)my_endpoint;
    DEBUG_PRINTF("OUT endpoint adress : %x \n",dev->ez430_uart_out_endpoint);
	DEBUG_PRINTF("IN endpoint adress : %x \n", dev->ez430_uart_in_endpoint);

	return dev;
}

int ez430_close(struct ez430_dev *dev)
{
    DEBUG_PRINTF("Closing device!\n");

	libusb_free_config_descriptor(dev->ez430_config);
	libusb_release_interface(dev->ez430_handle, UART_IFACE);
	libusb_close(dev->ez430_handle);
	libusb_exit(NULL);

	free(dev);
	return 0;
}

ssize_t ez430_read(struct ez430_dev * dev, void *buf, ssize_t length)
{
	// TODO : add a size check so it isn't > wMaxSize
	int count, ret;
	ret = libusb_bulk_transfer(dev->ez430_handle,
				   dev->ez430_uart_in_endpoint,
				   buf, length, &count, TIME_OUT);
	/*
	   if (ret != 0) {
	   fprintf(stdout,"Error during read : %i\n",
	   ret,
	   libusb_error_name(ret));
	   fflush(stdout);
	   return ret;
	   }
	 */
	if (ret != 0)
		return ret;
	return count;
}

ssize_t ez430_write(struct ez430_dev * dev, const void *buf, int length)
{
	int count, ret;
	ret = libusb_bulk_transfer(dev->ez430_handle,
				   dev->ez430_uart_out_endpoint,
				   (unsigned char *)buf, length, &count,
				   TIME_OUT);
	if (ret != 0) {

        DEBUG_PRINTF("Error during write : %i - %s\n",
			ret, libusb_error_name(ret));
	}
	return count;
}

/*
 * utilities related to libusb
 */
static void ez430_print_config(struct libusb_config_descriptor *config);
static void ez430_print_interface_desc(const struct libusb_interface_descriptor
				       *interface);
static void ez430_print_endpoint_desc(const struct libusb_endpoint_descriptor
				      *endpoint);

static void ez430_print_dev(struct ez430_dev *dev)
{
	printf("Device struct : 0x%p\n", dev);
}

static void ez430_print_config(struct libusb_config_descriptor *config)
{
	printf("Config descriptor size : %i\n", config->bLength);
	printf("Config descriptor type : %i\n", config->bDescriptorType);
	printf("Number of supported interfaces : %i\n", config->bNumInterfaces);
}

static void ez430_print_interface_desc(const struct libusb_interface_descriptor
				       *interface)
{
	printf("Number of endpoints : %i\n", interface->bNumEndpoints);
	printf("USB-IF Class code : %x \n", interface->bInterfaceClass);
	printf("USB-IF Subclass code : %x \n", interface->bInterfaceSubClass);
	int i = 0;
	for (i = 0; i < interface->bNumEndpoints; i++) {
		ez430_print_endpoint_desc(&(interface->endpoint[i]));
	}
}

static void ez430_print_endpoint_desc(const struct libusb_endpoint_descriptor
				      *endpoint)
{
	printf("Size of the descriptor : %i \n", endpoint->bLength);
	printf("Descriptor type: %i \n", endpoint->bDescriptorType);
	printf("Adress of the endpoint : %i \n", endpoint->bEndpointAddress);
	printf("Endpoint attributes  : %i \n", endpoint->bmAttributes);

	printf("Endpoint Type :");
	int endpoint_type;
	endpoint_type = endpoint->bmAttributes & 0x3;

	switch (endpoint_type) {
	case LIBUSB_TRANSFER_TYPE_CONTROL:
		printf("CONTROL ENDPOINT\n");
		break;
	case LIBUSB_TRANSFER_TYPE_BULK:
		printf("BULK ENDPOINT\n");
		break;
	case LIBUSB_TRANSFER_TYPE_INTERRUPT:
		printf("INT ENDPOINT\n");
		break;
	case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
		printf("ISOCHRONOUS ENDPOINT\n");
		break;
	default:
		printf("Unknown endpoint type!\n");
		break;
	}

	printf("Endpoint direction : ");
	(endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) ? printf("OUT\n") :
	    printf("IN\n");
}

void ez430_dump_info(struct ez430_dev *dev)
{
	ez430_print_dev(dev);
	ez430_print_interface_desc(dev->ez430_interface);
	ez430_print_config(dev->ez430_config);
}
