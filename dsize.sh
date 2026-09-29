#/bin/bash

SHELL=/bin/bash
HDIR=rock

# backup old data
cd /home/${HDIR}
fpath="/home/${HDIR}/t_data*"
echo 1 > dlog
echo $fpath >> dlog
fname="${fpath##*/}"
echo 2 >> dlog
echo $fname >> dlog
echo 3 >> dlog
idx=$(cat /home/rock/data_index)
echo $idx >> dlog
echo $fname > nfilename
nf2=$(cat nfilename)
echo $nf2 >> dlog
nfile3=${nf2}.${idx}
echo $nfile3
cp /home/${HDIR}/$fname /home/${HDIR}/archive/$nfile3
idx=$((idx + 1))
echo $idx >> dlog
echo $idx>/home/rock/data_index

# truncate data file

tail -n 200000 $fname > ndata

# copy to current data file
cp ndata $fname

rm ndata

