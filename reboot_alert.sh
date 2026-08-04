#!/bin/bash
sleep 1
if [ -f /home/rock/rbtime ]; then
    cat /home/rock/rbtime | mutt -s "$HOSTNAME reboot" -- philbrooksjazz@gmail.com > /dev/null
    rm -rf /home/rock/rbtime
fi
