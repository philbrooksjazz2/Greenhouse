#!/bin/bash
cp /home/rock/t_data* /home/rock/temp_data
C_DATE=$(date)
echo "GH temperatures" | mutt -a /home/rock/temp_data -s "$C_DATE" -- philbrooksjazz@gmail.com > /dev/null
sleep 5
rm /home/rock/temp_data
