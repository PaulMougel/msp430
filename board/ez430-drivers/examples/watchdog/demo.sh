#! /bin/sh

## =============Conf=====================
WSIM=wsim-ez430rf
WTRC=wtracer
WCNS=wconsole
ELF=bin/watchdog.elf


UI="--ui"
LOG="--logfile=wsim.log --verbose=6"
TRC="--trace=wsim.trc"

# Mode = time | gdb
MODE="--mode=time --modearg=20s"
#MODE="--mode=gdb"

# Serial terminal emulation
#  stdout | UDP | TCP | wconsole
SERMODE="stdout"


## ======================================

if [ "x`which nc.traditional`" = "x" ]
then
    NETCAT=nc
else
    NETCAT=nc.traditional
fi

create_fifo()
{
    if [ ! -e $1 ] ; then 
	mkfifo $1
    fi
}

case $SERMODE in
    "stdout")
	SERIAL="--serial0_io=stdout"
	NCCMD=""
	;;
    "UDP")
	NCCMD="${NETCAT} -u -p 7000 localhost 6000"
	SERIAL="--serial0_io=bk:udp:localhost:6000:localhost:7000"
	;;
    "TCP")
	NCCMD="${NETCAT} localhost 6000"
	SERIAL="--serial0_io=bk:tcp:s:localhost:6000"
	;;
    "wconsole")
	create_fifo towsim
	create_fifo fromwsim
	NCCMD=""
	${WCNS} -m o -f fromwsim & 
	${WCNS} -m i -f towsim   &
	SERIAL="--serial0_io=bk:fromwsim,towsim"
	;;
esac


## ============ WSIM ====================
WS1="${WSIM} ${UI} ${MODE} ${LOG} ${TRC} ${SERIAL} ${ELF}"
xterm -T wsim-1 -e "${WS1}" &
echo "${WS1}"
sleep 0.5
## ======================================

## =============NETCAT / SERIAL ==========
if [ "${NCCMD}" != "" ] ; then
    xterm -T netcat-1 -e "${NCCMD}" &
    echo "${NCCMD}"
fi
## ======================================

## =============Wait=====================
read dummyval
## ======================================

## =============End======================
killall -SIGUSR1 ${WSIM}   > /dev/null 2>&1
killall -SIGQUIT ${WSNET1} > /dev/null 2>&1
killall -SIGQUIT ${WSNET2} > /dev/null 2>&1
killall -SIGUSR1 ${NETCAT} > /dev/null 2>&1
if [ "${SERMODE}" = "wconsole" ] ; then 
    killall -9       ${WCNS}   > /dev/null 2>&1 
    rm -f fromwsim towsim
fi
## ======================================

## =============Traces===================
${WTRC} --in=wsim.trc --out=wsim.vcd --format=vcd &
## ======================================
