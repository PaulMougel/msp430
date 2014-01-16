NAME		= libez430
SRC		= adc10.c cc2500.c clock.c leds.c spi.c timer.c uart.c button.c flash.c watchdog.c
SRC_DIR		= src
INC_DIR		= inc
OUT_DIR		= bin
LIB_DIR		= lib
OBJ_DIR		= .obj
DOC_DIR		= doc
DEP_DIR 	= .deps
OBJ		= $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
DEPS		= $(patsubst %.c,$(DEP_DIR)/%.d,$(SRC))
LIBS		= $(patsubst %.c,$(LIB_DIR)/lib%.a,$(SRC))
# Platform EZ430
CPU		= msp430f2274
CFLAGS		= -g -Wall -mmcu=${CPU} -I ${INC_DIR}
LDFLAGS		= $(shell pkg-config --libs glib-2.0 gthread-2.0)
CC		= msp430-gcc
MAKEDEPEND	= $(CC) $(CFLAGS) -MM -MP -MT $@ -MF $(DEP_DIR)/$*.d

ifeq ($(DEBUG),1)
	CFLAGS += -g 
else
	#CFLAGS += -g
endif

# We have to make a choice in the order!
# If we put this before the DEBUG check,
# We will never have debug symbols for optimized binaries
ifdef OPT
	CFLAGS += -O${OPT}
else
	CFLAGS += -O0
endif


all: $(LIB_DIR)/${NAME}.a

$(LIB_DIR)/${NAME}.a: ${OBJ}
	@mkdir -p ${LIB_DIR}
	msp430-ar rcs $@ ${OBJ}

${OBJ_DIR}/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR) $(DEP_DIR)
	$(MAKEDEPEND) $<
	$(CC) $(CFLAGS) -c $< -o $@

${LIB_DIR}/lib%.a: ${OBJ_DIR}/%.o
	@mkdir -p ${LIB_DIR}
	msp430-ar rcs $@ $<

-include $(DEPS)

.PHONY: clean
clean:
	@rm -Rf $(BIN) $(OBJ) $(OBJ_DIR) $(DEP_DIR) $(DOC_DIR) ${LIB_DIR}

.PHONY: rebuild
rebuild: clean all

.PHONY: doc
doc:
	doxygen

