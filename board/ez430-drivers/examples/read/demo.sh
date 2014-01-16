#! /bin/sh

## =============Conf=====================
WSIM=wsim-ez430rf
WTRC=wtracer
WCNS=wconsole
ELF=bin/read.elf

SERMODE="wconsole"
MODE="--mode=time --modearg=20s"

UI="--ui"
## ======================================


create_fifo()
{
    if [ ! -e $1 ] ; then 
	mkfifo $1
    fi
}

create_fifo towsim
create_fifo fromwsim
${WCNS} -m o -f fromwsim & 
${WCNS} -m i -f towsim   &
SERIAL="--serial0_io=fromwsim,towsim"

## ============ WSIM ====================
WS1="${WSIM} ${UI} ${MODE} ${LOG} ${TRC} ${SERIAL} ${ELF}"
xterm -T wsim-1 -e "${WS1}" &
echo "${WS1}"
sleep 0.5
## ======================================


## =============Wait=====================
read dummyval
## ======================================


## =============End======================
killall -SIGUSR1 ${WSIM}   > /dev/null 2>&1
if [ "${SERMODE}" = "wconsole" ] ; then 
    killall -9       ${WCNS}   > /dev/null 2>&1 
    rm -f fromwsim towsim
fi
## ======================================

## =============Traces===================
${WTRC} --in=wsim.trc --out=wsim.vcd --format=vcd &
## ======================================

