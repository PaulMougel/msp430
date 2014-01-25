#!/bin/bash
PI_IP=${PI_IP:=172.16.5.2}
scp -r ../frontend pi@$PI_IP:msp430/
exit 0