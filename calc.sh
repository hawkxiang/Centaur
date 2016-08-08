#!/bin/bash

function Calc {
    filelist=`ls $1`
    cd $1
    for file in $filelist
    do
        if [ -d $file ]
        then
            Calc $file
        else
            Line=$[$Line+`cat $file | wc -l`]
        fi
    done
    cd ..
}

Line=$[`cat calc.sh | wc -l`+`cat wscript | wc -l`]

Calc src

echo "Total line : "$Line
