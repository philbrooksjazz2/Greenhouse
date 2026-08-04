#/bin/bash

# backup old data
cd /home/rock
cp /home/rock/t_data* /home/rock/archive
fpath="/home/rock/t_data*"
fname="${fpath##*/}"

echo $fname
echo $fname

# truncate data file

tail -n 200000 $fname > ndata

# copy to current data file
cp ndata $fname

rm ndata

