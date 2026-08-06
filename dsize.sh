#/bin/bash

HDIR=pmb

# backup old data
cd /home/${HDIR}
fpath="/home/${HDIR}/t_data*"
echo $fpath
fname="${fpath##*/}"

echo $fname

idx=$(<data_index)
echo $idx
echo $fname > nfilename
nf2=$(<nfilename)
echo $nf2
nfile3=${nf2}.${idx}
echo $nfile3
cp /home/${HDIR}/$fname /home/${HDIR}/archive/$nfile3
((idx++))
echo $idx>data_index

# truncate data file

tail -n 200000 $fname > ndata

# copy to current data file
cp ndata $fname

rm ndata

