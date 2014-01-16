#! /bin/sh

## =============Conf=====================
WSIM=wsim-ez430rf
WSNET1=wsnet1
WTRC=wtracer
WSNET_MODE="--wsnet1"
NB_NODE=2
NETCAT==""
SERMODE==""

MODE="--mode=time --modearg=20s"
UI="--ui"
UI=""



## =============WSNET=====================
xterm -T ${WSNET1} -e "${WSNET1}" &
echo "${WSNET1}"

## ======================================

sleep 0.5


## =============WSIM=====================
iter=1
while [ ${iter} -le ${NB_NODE} ]
do
    LOG="--verbose=6 --logfile=p${iter}.log"
    WS="${WSIM} ${MODE} ${WSNET_MODE} ${LOG} ${UI}"
    WS="${WS} --serial0_io=bk:stdout"
    WS="${WS} --node-id=${iter}"
    WS="${WS} --logpkt=rxtx --logpktfile=p${iter}.pkt"
    WS="${WS} --trace=p${iter}.trc"
    if [ "${iter}" = "1" ] ; then 
	WS="${WS} bin/radio-tx.elf"
    else
	WS="${WS} bin/radio-rx.elf"
    fi	

    xterm -T wsim-${iter} -e "${WS}" &
    echo "${WS}"
    iter=`expr ${iter} + 1`
    sleep 0.5
done
## ======================================


## =============Wait=====================
read dummyval
## ======================================


## =============End======================
killall -SIGUSR1 ${WSIM}   > /dev/null 2>&1
killall -SIGQUIT ${WSNET1} > /dev/null 2>&1
killall -SIGUSR1 ${NETCAT} > /dev/null 2>&1
if [ "${SERMODE}" = "wconsole" ] ; then 
    killall -9       ${WCNS}   > /dev/null 2>&1 
    rm -f fromwsim towsim
fi
## ======================================

## =============Traces===================
iter=1
while [ ${iter} -le ${NB_NODE} ]
do
    CMD="${WTRC} --in=p${iter}.trc --out=p${iter}.vcd --format=vcd"
    echo ${CMD}
    ${CMD}
    iter=`expr ${iter} + 1` 
done

${WTRC} --out=glob.vcd --format=vcd --merge --multi *.trc
## ======================================


