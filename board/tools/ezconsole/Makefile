

CFLAGS = -I/usr/include/libusb-1.0 -g -O0 -Wall

ifdef DEBUG
	CFLAGS += -DDEBUG=$(DEBUG)
endif

ezconsole: ezconsole.o ez430.o
	gcc -static -pthread -o $@ $^ -lusb-1.0 -lrt 

# note: on -pthread VS -lpthread, see
# http://stackoverflow.com/q/2348200/

%.o: %.c
	gcc -o $@ $(CFLAGS) -c $^

clean: 
	-rm -f *.o ezconsole

