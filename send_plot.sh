#!/bin/bash
cp /home/rock/t_data* /home/rock/plot/p1
/home/rock/plot/thplot_png.sh /home/rock/plot/p1
cp /home/rock/plot/GH_temp.png /home/rock/GHAh.png
C_DATE=$(date)
echo "GH temperatures GHAh" | mutt -a /home/rock/GHAh.png -s "$C_DATE" -- philbrooksjazz@gmail.com > /dev/null
sleep 5
rm /home/rock/GHAh.png
