#! /bin/sh

## =============Conf=====================
WSIM=wsim-ez430rf
WTRC=wtracer
WCNS=wconsole
WSNET1=wsnet1
WSNET2=wsnet
ELF=bin/store_id.elf

# set WSNET to "wsnet1", "wsnet2", or "" if you are using wsim alone
WSNET=""
WSNET2_CONF="./worldsens.xml"

UI="--ui"
LOG="--logfile=wsim.log --verbose=6"
TRC="--trace=wsim.trc"

# Mode = time | gdb
MODE="--mode=time --modearg=10s"
#MODE="--mode=gdb"

# Serial terminal emulation
#  stdout | UDP | TCP | wconsole
SERMODE="stdout"

## ======================================

DS0=0a:00:00:00:00:00:00:01
DS1=0a:00:00:00:00:00:01:01
DS2=53:00:00:00:00:00:02:01
DS3=64:00:00:00:00:00:03:01
DS4=e1:00:00:00:00:00:04:01
DS5=d6:00:00:00:00:00:05:01
DS6=8f:00:00:00:00:00:06:01
DS7=b8:00:00:00:00:00:07:01
DS8=9c:00:00:00:00:00:08:01
DS9=ab:00:00:00:00:00:09:01
DS10=f2:00:00:00:00:00:0a:01

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

## =============WSNET=====================
if [ "$WSNET_MODE" = "--wsnet1" ]
then
    xterm -T ${WSNET1} -e "${WSNET1}" &
    echo "${WSNET1}"
else
    if [ "$WSNET_MODE" = "--wsnet2" ]
    then
        xterm -T ${WSNET2} -e "${WSNET2} -c ${WSNET2_CONF}" &
        echo "${WSNET2} -c ${WSNET2_CONF}"
    fi
fi
## ======================================

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
killall -SIGQUIT ${WCNS}   > /dev/null 2>&1
## ======================================

## =============Traces===================
${WTRC} --in=wsim.trc --out=wsim.vcd --format=vcd &
## ======================================
