#!/bin/bash

#
# Note - change for time zone adj to time the "-7" is UTC -7
# This is really for the pacific TZ (UTC -8) in the current DST, so 1 hour added

# HOW TO USE
# ./ae_gnuplot_script_8chan.sh <name of csv file> <channel (optional)>

gnuplot -persist <<- EOF
    reset
    set title noenhanced
    set datafile separator " "
    set grid
    set key left

    # SET UP X AXIS
    set xlabel 'Time'
    set xdata time
    set timefmt "%s"
#    set format x "%T"
    set format x "%b %d %H:%M"
    set xtics out nomirror
    set autoscale xfix

    # SET UP Y AXIS DEPTH
    set ylabel 'Temperature - Deg F'
    set ytics out nomirror
    set y2range [0:100]
    set y2label 'Humidity %'
    set y2tics 

    set term png font Courier 18 size 1920,1080
#    set term x11 font Courier 16 size 1920,1080

    timezone = -7


#    print "\n\nFilePath: ".filePath
#    print filePathLength
#    print "FileName: ".fileName
#    print fileNameLength
#    print "FileID:   ".fileID."\n"            

    plotTitle = "Greenhouse outdoor ambient Temp Humidity"

    if (1) \
    {
        set title plotTitle
        set output "GH_temp.png"

        channel = 5
        plot '$1' using (\$10 + timezone * 3600):channel title 'Temperature' with lines, \
        '$1' using (\$10 + timezone * 3600):8 axes x1y2 title 'Humidity' with lines
    }
EOF

