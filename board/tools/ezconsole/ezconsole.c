/*
 *	ez430 UART communication program.
 *	It is designed to be a userspace replacement of the
 *	buggy cdc_acm kernel driver.
 *
 *	Inspired by python script by Peter A. Bigot under BSD License
 *
 *	Copyright 2013 INRIA
 *	Author : T. Pourcelot, CITI Lab
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <strings.h>
#include <error.h>
#include <fcntl.h>
#include <termios.h>

#include <pthread.h>

#include "ez430.h"

#define BAUDRATE B9600

#ifdef DEBUG 
#define DEBUG_PRINTF(...) fprintf(stderr,__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

/*
 * Read from stdin, send the request to the device
 */

static void *writer_function(void *arg)
{
	struct termios oldtio, newtio;
	struct ez430_dev *dev = (struct ez430_dev *)arg;
	char entry_buffer[2];	// maximum for a POSIX line
	int res = 0;

	tcgetattr(STDIN_FILENO, &oldtio);	/* save current serial port settings */
	bzero(&newtio, sizeof(newtio));	/* clear struct for new port settings */

	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;	/* raw input  */
	newtio.c_oflag = 0;	/* raw output */
	newtio.c_lflag = ICANON;
	cfmakeraw(&newtio);
	//newtio.c_lflag |= ECHO; /* Even if we're raw, we echo input char */

	tcflush(STDIN_FILENO, TCIFLUSH);
	tcsetattr(STDIN_FILENO, TCSANOW, &newtio);

	for (;;) {
		res = read(STDIN_FILENO, entry_buffer, 1);
		if (entry_buffer[0] == 0x1b) {	/* exit on ESC */
			break;
		}
		if (res > 0 && res < MAX_PACKET_SIZE) {
			entry_buffer[res] = 0;

			/* We let the terminal echo or not the entered char */
			int toto = ez430_write(dev, entry_buffer, 1);

			if (toto < 0) {
                DEBUG_PRINTF("Error %i writing to dev\n",toto);
			}
		}
	}
	/* quitting */
	tcsetattr(STDIN_FILENO, TCSANOW, &oldtio);

	DEBUG_PRINTF("Exiting writer thread!\n");
	pthread_exit(NULL);
}

/*
 * read from the device, print it to stdout
 */
static void *reader_function(void *arg)
{
	struct ez430_dev *dev = (struct ez430_dev *)arg;
	char buf[MAX_PACKET_SIZE + 1];
	int r = 0;
	int cancel_state;
	for (;;) {
		bzero(&buf, MAX_PACKET_SIZE);
		/* critical section */
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_state);
		r = ez430_read(dev, &buf, MAX_PACKET_SIZE);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &cancel_state);
		/* end of critical section */

		pthread_testcancel();	/* avoid printing when exiting */
		if (r > 0) {
			if (r <= MAX_PACKET_SIZE)
				buf[r] = 0;
			fprintf(stdout, "%s", buf);
			fflush(stdout);
			r = 0;
		}
	}
	return 0;		//FU GCC
}

int main(int argc, char **argv)
{
	struct ez430_dev *my_dev = NULL;

	pthread_t reader_task, writer_task;

	my_dev = ez430_open(my_dev);

	if (my_dev == NULL) {
		error(EXIT_FAILURE, 0, "Failed to open device");
	}
#ifdef DEBUG
	ez430_dump_info(my_dev);
#endif

	if (pthread_create(&writer_task, NULL, writer_function, (void *)my_dev)) {
        DEBUG_PRINTF("Error creating writer thread\n");
        return 1;
	}

	if (pthread_create(&reader_task, NULL, reader_function, (void *)my_dev)) {
		DEBUG_PRINTF("Error creating reader thread\n");
		return 1;
	}

	pthread_join(writer_task, NULL);
	pthread_cancel(reader_task);
	pthread_join(reader_task, NULL);

    DEBUG_PRINTF("All threads have been joined, exiting\n");

	ez430_close(my_dev);
	return EXIT_SUCCESS;
}
