#!/bin/bash
sleep 1
C_DATE=$(date)
echo "Alert $HOSTNAME rebooted at $C_DATE" > /home/rock/rbtime
