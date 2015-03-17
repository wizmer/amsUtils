#!/bin/bash
# This script will create a script that launches LSF jobs for a given executable

function mainFunction {
    scriptPath=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

    if [[ -f "$HOME/.defaultValueSubmissionScript.sh" ]]; then
	source $HOME/.defaultValueSubmissionScript.sh
    fi
    initial=`echo $HOME | awk -F'/' '{print $5}'`
    echo Enter the task name:
    read name
    absoluteFolderPath=`pwd`/$name
    while true; do
	if [ -d "$name" ]; then
	    echo The name: $name, is alread used !
	    echo Please provide an other one:
	    read name
	else
	    break
	fi
    done

    mkdir $name
    if [ "$?" == "0" ]; then
	echo Directory: $name created
    else
	echo Directory: $name not created !
	echo Exit !
	return
    fi
    echo ""

    echo Enter the executable absolute path to process on workers:
    if [ ! -z "$defaultExecutable" ]; then
	echo Press Enter for default value: $defaultExecutable
    fi
    read executable
    if [[ ! -z "$defaultExecutable" && "$executable" == "" ]]; then
	executable=$defaultExecutable
    fi
    
    while true; do
	if [ ! -f "$executable" ]; then
	    echo This executable doesn\'t exist
	    echo "Do you want to continue anyway [Y/n] ?"
	    read confirmation
	    if [[ "$confirmation" == "y" || "$confirmation" == "Y" || "$confirmation" == "" ]]; then
		break;
	    else
		echo Please, provide the executable absolute path:
		read executable
	    fi
	else
	    break
	fi
    done
    echo Executable path set to: $executable
    echo ""
    defaultExecutable=$executable

    echo "Enter the queue name"
    if [ ! -z "$defaultQueue" ]; then
	echo Press Enter for default queue: $defaultQueue
    fi
    read queue
    if [ "$queue" == "" ]; then
	queue="$defaultQueue"
    fi
    echo queue set to: $queue
    echo ""
    defaultQueue=$queue

    echo Enter the directory path where input ROOT files are stored:
    if [ ! -z "$defaultInputFileDir" ]; then
	echo Press Enter for default path: $defaultInputFileDir
    fi
    read inputFileDir
    if [ "$inputFileDir" == "" ]; then
	inputFileDir="$defaultInputFileDir"
    fi

    while true; do
	if [ ! -d "$inputFileDir" ]; then
	    echo This directory doesn\'t exist
	    echo Do you want to continue anyway [Y,n] ?
	    read confirmation
	    if [[ "$confirmation" == "y" || "$confirmation" == "Y" || "$confirmation" == "" ]]; then
		break;
	    else
		echo Please, provide the input file directory:
		read inputFileDir
	    fi
	else
	    break
	fi
    done
    echo The following directory will be used: $inputFileDir
    echo ""
    defaultInputFileDir=$inputFileDir

    echo "Enter the chunk size (to process a number of 'chunk' files per run)"
    if [ ! -z "$defaultChunkSize" ]; then
	echo Press Enter for default chunk size: $defaultChunkSize
    fi
    read chunkSize
    if [ "$chunkSize" == "" ]; then
	chunkSize=$defaultChunkSize
    fi
    echo chunkSize set to: $chunkSize
    echo ""
    defaultChunkSize=$chunkSize

    echo Enter the maximum number of jobs to launch:
    if [ ! -z "$defaultMaxJob" ]; then
	echo Press Enter for default maximum number of jobs: $defaultMaxJob
    fi
    read maxJob
    if [ "$maxJob" == "" ]; then
	maxJob=$defaultMaxJob
    fi
    echo maxJob set to: $maxJob
    echo ""
    defaultMaxJob=$maxJob

    echo "Create one dedicated folder in your /afs/cern.ch/work/$initial/$USER directory [y/n] ?"
    if [ ! -z "$defaultCreateWorkFolder" ]; then
	echo Press Enter for default : $defaultCreateWorkFolder
    fi
    read createWorkFolderYesOrNo
    if [ "$createWorkFolderYesOrNo" == "" ]; then
	createWorkFolderYesOrNo=$defaultCreateWorkFolder
    fi
    if [[ "$createWorkFolderYesOrNo" == "n" || "$createWorkFolderYesOrNo" == "N" ]]; then
	createWorkFolder=0
	echo no folder created
    else
	createWorkFolder=1
	mkdir /afs/cern.ch/work/$initial/$USER/$name
	if [ "$?" == "0" ]; then
	    echo Directory: /afs/cern.ch/work/$initial/$USER/$name created
	else
	    echo Directory: /afs/cern.ch/work/$initial/$USER/$name not created !
	    echo Exit !
	    return
	fi
	echo "Provide filename or pattern (ex: *.root) to be copied in work at the end of each job:"
	read toBeCopiedInWork
	if [[ "$toBeCopiedInWork" == "" ]]; then
	    echo "Nothing to be copied in work"
	    toBeCopiedInWorkCommand=""
	else
	    toBeCopiedInWorkCommand="cp -R $toBeCopiedInWork /afs/cern.ch/work/$initial/$USER/$name/\${jobName}"
	    echo The following command will be executed at the end of each job
	    echo $toBeCopiedInWorkCommand
	fi
    fi
    echo ""
    defaultCreateWorkFolder=$createWorkFolderYesOrNo

    echo "Create one dedicated folder in your eos directory [Y,n] ?"
    if [ ! -z "$defaultCreateEosFolder" ]; then
	echo Press Enter for default : $defaultCreateEosFolder
    fi
    read createEosFolderYesOrNo
    if [ "$createEosFolderYesOrNo" == "" ]; then
	createEosFolderYesOrNo=$defaultCreateEosFolder
    fi
    if [[ "$createEosFolderYesOrNo" == "n" || "$createEosFolderYesOrNo" == "N" ]]; then
	createEosFolder=0
	echo no folder created
    else
	createEosFolder=1
	while true; do
	    if [ ! -d "$HOME/eos" ]; then
		echo Specify your eos mounting point:
		read eosRoot
		if [ ! -d "$eosRoot" ]; then
		    echo This folder does not exist !
		else
		    break;
		fi
	    else
		eosRoot=$HOME/eos
		break;
	    fi
	done
	/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select -b fuse mount $eosRoot
	eosDirName=$eosRoot/ams/user/$initial/$USER/$name
	mkdir $eosDirName
	if [ "$?" == "0" ]; then
	    echo Directory: $eosDirName created
	else
	    echo Directory: $eosDirName not created !
	fi
	echo "Provide filename or pattern (ex: *.root) to be copied in EOS at the end of each job:"
	read toBeCopiedInEos
	if [[ "$toBeCopiedInEos" == "" ]]; then
            echo "Nothing to be copied on EOS"
            toBeCopiedInEosCommand=""
	else
            toBeCopiedInEosCommand="cp -R $toBeCopiedInEos /afs/cern.ch/eos/$initial/$USER/$name/\${jobName}"
            echo The following command will be executed at the end of each job
            echo $toBeCopiedInEosCommand
	fi
	echo ""
    fi
    defaultCreateEosFolder=$createEosFolderYesOrNo

    echo "Provide filename or pattern (ex: *.root) to be copied in the main folder at the end of each job:"
    read toBeCopiedInMainFolder

    if [[ "$toBeCopiedInMainFolder" == "" ]]; then
	echo "Nothing to be copied in main folder"
	toBeCopiedInMainFolderCommand=""
    else
	toBeCopiedInMainFolderCommand="cp -R $toBeCopiedInMainFolder ${absoluteFolderPath}/\${jobName}"
	echo The following command will be executed at the end of each job
	echo $toBeCopiedInMainFolderCommand
    fi

# name=testForGeneral
# executable=/afs/cern.ch/user/b/bcoste/ecal/equalization/bin/rearleakagel
# queue=ams1nd
# inputFileDir=/afs/cern.ch/work/b/bcoste
# chunkSize=10
# maxJob=1
# createWorkFolder=1
# createEosFolder=1
# absoluteFolderPath=`pwd`/$name
# eosRoot=${HOME}/eos

    Name="$(tr '[:lower:]' '[:upper:]' <<< ${name:0:1})${name:1}"
    cat << EOF > ${absoluteFolderPath}/submit${Name}.sh
#!/bin/bash
if [ "\$#" -lt 1 ]; then
    echo usage: ./submit${Name}.sh jobName
    exit
fi

jobName=\$1
chunkSize=${chunkSize}
MAXJOB=${maxJob}
queue=${queue}
export isWorkFolder=$createWorkFolder
export isEosFolder=$createEosFolder
export eosRoot=${eosRoot}
export executable=${executable}
files=("${inputFileDir}"/*)

if [[ ! -f "\$executable" ]]; then
    echo Executable: \${executable} not found !
    echo Exit !
    exit
fi

if [[ -d "${absoluteFolderPath}/\$jobName" ]]; then
    echo folder: ${absoluteFolderPath}/\$jobName exists !
    echo Please, choose another job name
    echo Exit !
    exit
fi
mkdir ${absoluteFolderPath}/\$jobName

if [[ \${isWorkFolder} == "1" ]]; then
    if [[ ! -d "/afs/cern.ch/work/$initial/$USER/${name}" ]]; then
        echo folder: /afs/cern.ch/work/$initial/$USER/${name} does not exist !
        echo Exit !
    fi

    if [[ -d "/afs/cern.ch/work/$initial/$USER/${name}/\$jobName" ]]; then
        echo folder: /afs/cern.ch/work/$initial/$USER/${name}/\$jobName exists !
        echo Please, choose another job name
        echo Exit !
        exit
    fi
    mkdir /afs/cern.ch/work/$initial/$USER/${name}/\$jobName
fi

if [[ \${isEosFolder} == "1" ]]; then
    /afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select -b fuse mount \$eosRoot

    if [[ ! -d "\${eosRoot}/ams/user/$initial/$USER/$name" ]]; then
        echo folder: \${eosRoot}/ams/user/$initial/$USER/$name does not exist or is not accessible !
        echo Exit !
    fi

    if [[ -d "\${eosRoot}/ams/user/$initial/$USER/$name/\$jobName" ]]; then
        echo folder: \${eosRoot}/ams/user/$initial/$USER/$name/\$jobName exists or is not accessible !
        echo Please, choose another job name
        echo Exit !
        exit
    fi
    mkdir \${eosRoot}/ams/user/$initial/$USER/$name/\$jobName
fi

if [[ ! -f "${absoluteFolderPath}/job${Name}.sh" ]]; then
    echo Job file: ${absoluteFolderPath}/job${Name}.sh does not exist !
    echo Exit !
    exit
fi

cat << END > \$jobName/env.sh
export isWorkFolder=\$isWorkFolder
export isEosFolder=\$isEosFolder
export eosRoot=\$eosRoot
export executable=\$executable
END

for ((i=0; i < \${#files[@]}; i+=chunkSize)); do
    export ROOTUPLES="\${files[@]:i:chunkSize}"
    j=\$((i/chunkSize))
    if (( j >= MAXJOB )); then
	break
    fi    

    bsubCommand="bsub -J \${jobName}_\${j} -q \${queue} ${absoluteFolderPath}/job${Name}.sh \${jobName} \${j}"
    \$bsubCommand
    echo "\${bsubCommand}, return code: ">>${absoluteFolderPath}/\${jobName}/jobList.log
    echo \${ROOTUPLES}>>${absoluteFolderPath}/\${jobName}/inputFileList.log

    if [ "\$j" -gt 0 ]; then
    	wrapUpCondition="\$wrapUpCondition &&"
    fi
    wrapUpCondition="\$wrapUpCondition ended(\${jobName}_\$j)"
done

echo \$wrapUpCondition
bsub -o ${absoluteFolderPath}/\${jobName}/wrapup.out -q \$queue -w "\$wrapUpCondition" ${absoluteFolderPath}/rerunBadJobs.sh ${absoluteFolderPath}/\${jobName}

EOF

    chmod 744 ${absoluteFolderPath}/submit${Name}.sh

    cat << EOF > ${absoluteFolderPath}/job${Name}.sh
#!/bin/bash -x
if [ "\$#" -lt 2 ]; then
    echo usage: ./jobElectronSample.sh jobName jobNumber
    exit
fi

# Make sure sourcing does not change your path
curPATH=\`pwd\`
source \$HOME/.bashrc
cd \$curPATH

jobName=\$1
jobNumber=\$2

UUID=\$(cat /proc/sys/kernel/random/uuid) # Generate a unique folder name

mkdir \$UUID
cd \$UUID

#mount eos

echo reading eosdir
/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select -b fuse mount \${eosRoot}

\${executable} \${ROOTUPLES}
code=$?

if [[ "\$isWorkFolder" == "1" ]]; then
    if [[ "$toBeCopiedInWorkCommand" != "" ]]; then
        $toBeCopiedInWorkCommand
        code=\${code} || \$?
    fi
fi

if [[ "\$isEosFolder" == "1" ]]; then
    if [[ "$toBeCopiedInEosCommand" != "" ]]; then
        $toBeCopiedInEosCommand
        code=\${code} || \$?
    fi
    code=\${code} || \$?
fi

if [[ "$toBeCopiedInMainFolderCommand" != "" ]]; then
    $toBeCopiedInMainFolderCommand
    code=\${code} || \$?
fi

sed -i 's/, return code.*\$/, return code: '\${code}'/' ${absoluteFolderPath}/\${jobName}/jobList.log

EOF

chmod 744 ${absoluteFolderPath}/job${Name}.sh

cp $scriptPath/rerunBadJobs.sh ${absoluteFolderPath}
}

function saveDefaultValue {

    cat << EOF > $HOME/.defaultValueSubmissionScript.sh
defaultExecutable=$defaultExecutable        
defaultQueue=$defaultQueue
defaultInputFileDir=$defaultInputFileDir
defaultChunkSize=$defaultChunkSize
defaultMaxJob=$defaultMaxJob
defaultCreateWorkFolder=$defaultCreateWorkFolder
defaultCreateEosFolder=$defaultCreateEosFolder
EOF

}

mainFunction
saveDefaultValue
echo "Finished !"

