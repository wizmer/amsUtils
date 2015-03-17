#!/bin/bash
if [ "$#" -lt 1 ]; then
    echo usage: perpendicularRuns PATH
    exit
fi

path=$1

runs=(`awk '{if($2 == 101) print $1}' $HOME/doc/testBeamLogBook/ecalBeamDirection.txt`)
for run in "${runs[@]}"; do
    if [ "$runList" == "" ]; then
	runList=${run}
    else
        runList=${run}\|${runList}
    fi
done
find $path -name "*.root" | grep -E "$runList"