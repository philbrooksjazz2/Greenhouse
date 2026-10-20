#!/bin/bash
cp /home/ghdata/t_data* /home/ghdata/plot/p1
/home/ghdata/plot/thplot_png.sh /home/ghdata/plot/p1
cp /home/ghdata/plot/GH_temp.png /home/ghdata/$HOSTNAME.png
C_DATE=$(date)
echo "GH temperatures" | mutt -a /home/ghdata/$HOSTNAME.png -s "$C_DATE" -- philbrooksjazz@gmail.com > /dev/null
sleep 5
rm /home/ghdata/$HOSTNAME.png
