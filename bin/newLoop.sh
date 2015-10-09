#!/bin/bash
scriptDir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
originalPath=$scriptDir/../templates/templateEventLoop
if (( "$#" < 1 )); then
    echo usage: ./newLoop.sh folderName
    exit
fi

newFileName=$1

mkdir $newFileName
cp $originalPath/Makefile $newFileName/Makefile
cp $originalPath/analyse.cpp $newFileName/$newFileName.cpp
cp $originalPath/analyse.hpp $newFileName/$newFileName.hpp

cd $newFileName

className="$(tr '[:lower:]' '[:upper:]' <<< ${newFileName:0:1})${newFileName:1}"
sed -i "s/ANALYSE/${newFileName^^}/g" $newFileName.{c,h}pp
sed -i "s/Analyse/${className}/g" $newFileName.{c,h}pp
sed -i "s/analyse/${newFileName}/g" $newFileName.{c,h}pp
sed -i "s/analyse/${newFileName}/g" Makefile

mkdir bin
