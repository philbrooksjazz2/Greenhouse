#/bin/bash

# backup old data
cd /home/pmb
cp /home/pmb/t_data* /home/pmb/archive
fpath="/home/pmb/t_data*"
fname="${fpath##*/}"

echo $fname
echo $fname

# truncate data file

tail -n 2000 $fname > ndata

# copy to current data file
cp ndata $fname

rm ndata

