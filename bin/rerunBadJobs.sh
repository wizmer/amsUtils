#!/bin/bash -x
if (( "$#" < 1 )); then
    echo usage : ./rerunBadJobs jobFolderName
    exit
fi

configPath=$1

IFS=$'\n'
export ROOTUPLES
source ${configPath}/env.sh
for run in `grep -nrv "return code: 0" ${configPath}/jobList.log`
do
    bsubCommand=`echo  $run | cut -c 3- | sed 's/, return code.*$//'`
    jobNumber=`echo $run | awk -F':' '{print $1-1}'`
    ROOTUPLES=`awk '{if(NR=='$jobNumber'+1) print $0}' ${configPath}/inputFileList.log`
    IFS=$' '
    echo $bsubCommand
    $bsubCommand
done