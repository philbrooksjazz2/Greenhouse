#!/bin/bash
cp /home/ghdata/t_data* /home/ghdata/plot/p1
/home/ghdata/plot/thplot_png.sh /home/ghdata/plot/p1
C_DATE=$(date +%F-%H-%M-%S)
echo $C_DATE
cp /home/ghdata/GH_temp.png /home/ghdata/$HOSTNAME-$C_DATE.png
aws s3 cp $HOSTNAME-$C_DATE.png s3://edmonds-greenhouse 
sleep 2
rm /home/ghdata/$HOSTNAME-$C_DATE.png
