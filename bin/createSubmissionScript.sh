#!/bin/bash
# This script will create a script that launches LSF jobs for a given executable

#set up the eos mounting point
function setupEosRoot {
    while true; do
	if [ ! -d "$HOME/eos" ]; then
	    echo Specify your eos mounting point:
	    read eosRoot
	    if [ ! -d "$eosRoot" ]; then
		echo This folder does not exist !
	    else
		isEosRootSetUp=1
		break;
	    fi
	else
	    eosRoot=$HOME/eos
	    isEosRootSetUp=1
	    break;
	fi
    done
    /afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select -b fuse mount $eosRoot
}

function promptTaskName {
    echo Enter the task name:
    read homeDir
    absoluteFolderPath=`pwd`/$homeDir
    while true; do
	if [ -d "$homeDir" ]; then
	    echo The name: $homeDir, is alread used !
	    echo Please provide an other one:
	    read homeDir
	else
	    break
	fi
    done
}

function createHomeDir {
    mkdir $homeDir
    if [ "$?" == "0" ]; then
	echo Directory: $homeDir created
    else
	echo Directory: $homeDir not created !
	echo Exit !
	return
    fi
    echo ""
}


function promptExecutable {
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
    echo "Executable path set to: ${executable}"
    echo ""
    defaultExecutable=${executable}
}

function promptLibrairies {
    echo "Enter the list of shared object dependancies"
    echo "(absolute path or relative from : $(pwd)"

    if [ ! -z "$defaultLibs" ]; then
	echo "Press Enter for default value: $defaultLibs"
    fi
    read libs

    if [[ ! -z "$defaultLibs" && "$libs" == "" ]]; then
	libs=$defaultLibs
    fi

    libs=(${libs[@]})

    for i in "${!libs[@]}"; do
        while true; do
	    if [[ ! -f "${libs[$i]}" ]]; then
	        echo "The librairy : ${libs[$i]} does not exist"
	        echo "Do you want to continue anyway [Y/n] ?"
	        read confirmation
	        if [[ "$confirmation" == "y" || "$confirmation" == "Y" || "$confirmation" == "" ]]; then
		    break;
	        else
		    echo Please provide the new path for this librairy:
		    read libs[$i]
	        fi
	    else
	        break
	    fi
        done
        echo "adding lib : ${libs[$i]}"
    done

    defaultLibs=${libs[@]}
}

function promptQueueName {
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
}

function promptInputFilesPath {
    echo Enter the directory path where input ROOT files are stored:
    if [ ! -z "$defaultInputFileDir" ]; then
	echo Press Enter for default path: $defaultInputFileDir
    fi
    read inputFileDir
    if [ "$inputFileDir" == "" ]; then
	inputFileDir="$defaultInputFileDir"
    fi

    if [ -n "`echo $inputFileDir | grep eos`"  ]; then
	setupEosRoot
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
}

function promptChunkSize {
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
}

function promptMaxJobsNumber {
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
}

function promptCreateOutputFolders {
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
	mkdir /afs/cern.ch/work/$initial/$USER/$homeDir
	if [ "$?" == "0" ]; then
	    echo Directory: /afs/cern.ch/work/$initial/$USER/$homeDir created
	else
	    echo Directory: /afs/cern.ch/work/$initial/$USER/$homeDir not created !
	    echo Exit !
	    return
	fi
	echo "Provide filename or pattern (ex: *.root) to be copied in work at the end of each job:"
	read toBeCopiedInWork
	if [[ "$toBeCopiedInWork" == "" ]]; then
	    echo "Nothing to be copied in work"
	    toBeCopiedInWorkCommand=""
	else
	    toBeCopiedInWorkCommand="cp -R $toBeCopiedInWork /afs/cern.ch/work/$initial/$USER/$homeDir/\${jobName}"
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
	if [ -z "$isEosRootSetUp" ]; then
	    setupEosRoot
	fi
	eosDirName=$eosRoot/ams/user/$initial/$USER/$homeDir
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
            toBeCopiedInEosCommand="cp -R $toBeCopiedInEos \${eosRoot}/ams/user/$initial/$USER/$homeDir/\${jobName}"
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
}

function mainFunction {
    scriptPath=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

    if [[ -f "$HOME/.defaultValueSubmissionScript.sh" ]]; then
	source $HOME/.defaultValueSubmissionScript.sh
    fi
    initial=`echo $HOME | awk -F'/' '{print $5}'`

    promptTaskName
    createHomeDir
    cd ${homeDir}
    promptExecutable
    promptLibrairies
    promptQueueName
    promptInputFilesPath
    promptChunkSize
    promptMaxJobsNumber
    promptCreateOutputFolders

    Name="$(tr '[:lower:]' '[:upper:]' <<< ${homeDir:0:1})${homeDir:1}"
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

if [ "\$#" -gt 1 ]; then
    echo "New file dir provided : \$2" 
    files=("\$2"/*.root)
fi

export libs=(${libs[@]})

if [[ ! -f "\${executable}" ]]; then
    echo "Executable: \${executable} not found !"
    echo Exit !
    exit
fi

if [[ -d "${absoluteFolderPath}/\$jobName" ]]; then
    echo "folder: ${absoluteFolderPath}/\$jobName exists !"
    echo Please, choose another job name
    echo Exit !
    exit
fi

mkdir ${absoluteFolderPath}/\$jobName
cp \${executable} \$jobName/`basename \${executable}`
executable=`pwd`/\$jobName/`basename \${executable}`

for lib in \${libs[@]}
do
    if [[ ! -f "\$lib" ]]; then
        echo "lib : \${lib} not found !"
        exit
    fi
    cp \${lib} `pwd`/\$jobName/
done

# Check if some depdencies have not been provided
(
    unset LD_LIBRARY_PATH
    source \${HOME}/.bashrc

    declare -a libsToCopy
    declare -a missingLibs

    for lib in \${libs[@]}; do
        libsToCopy=(\${libsToCopy[@]} \$(basename \${lib}))
    done

    libsToCheck=\$(ldd \${executable} | grep "not found" | awk '{print \$1}')

    for libToCheck in \${libsToCheck[@]}; do
        test=0
        for libToCopy in \${libsToCopy[@]}; do
            if [ "\${libToCopy}" = "\${libToCheck}" ]; then
                test=1
                break
            fi
        done
        if [ "\${test}" = "0" ]; then
            missingLibs=(\${missingLibs[@]} \${libToCheck})
        fi
    done

    if (( \${#missingLibs[@]} > 0 )); then
        echo "Number of missing shared dependancies : \${#missingLibs[@]}"
        for missing in \${missingLibs[@]}; do
            echo "missing : \${missing}"
        done
        echo "Please add this libraries to the \\\${libs} variable in this script"
        exit 1
    fi
    exit 0
)


if [ "\$?" != "0" ]; then
    exit
fi


if [[ \${isWorkFolder} == "1" ]]; then
    if [[ ! -d "/afs/cern.ch/work/$initial/$USER/${homeDir}" ]]; then
        echo folder: /afs/cern.ch/work/$initial/$USER/${homeDir} does not exist !
        echo Exit !
    fi

    if [[ -d "/afs/cern.ch/work/$initial/$USER/${homeDir}/\$jobName" ]]; then
        echo folder: /afs/cern.ch/work/$initial/$USER/${homeDir}/\$jobName exists !
        echo Please, choose another job name
        echo Exit !
        exit
    fi
    mkdir /afs/cern.ch/work/$initial/$USER/${homeDir}/\$jobName
fi

if [[ \${isEosFolder} == "1" ]]; then
    /afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select -b fuse mount \$eosRoot

    if [[ ! -d "\${eosRoot}/ams/user/$initial/$USER/$homeDir" ]]; then
        echo folder: \${eosRoot}/ams/user/$initial/$USER/$homeDir does not exist or is not accessible !
        echo Exit !
        exit
    fi

    if [[ -d "\${eosRoot}/ams/user/$initial/$USER/$homeDir/\$jobName" ]]; then
        echo folder: \${eosRoot}/ams/user/$initial/$USER/$homeDir/\$jobName exists or is not accessible !
        echo Please, choose another job name
        echo Exit !
        exit
    fi
    mkdir \${eosRoot}/ams/user/$initial/$USER/$homeDir/\$jobName
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
\$(declare -p libs)
END

cp job${Name}.sh \${jobName}

for ((i=0; i < \${#files[@]}; i+=chunkSize)); do
    export ROOTUPLES="\${files[@]:i:chunkSize}"
    j=\$((i/chunkSize))
    if (( j >= MAXJOB )); then
	break
    fi    

    bsubCommand="bsub -J \${jobName}_\${j} -q \${queue} k5reauth -R -- \$(pwd)/\${jobName}/job${Name}.sh \${jobName} \${j}"
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

klist

homeDir=\$(dirname \${executable})

# Make sure sourcing does not change your path
pushd .
source \$HOME/.bashrc
popd
source \${homeDir}/env.sh

jobName=\$1
jobNumber=\$2

mkdir eos
eosRoot=\`pwd\`/eos
echo making local eos mounting point ...
/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select -b fuse mount \${eosRoot}
cp \${executable} main

for lib in \${libs[@]}
do
    cp \${homeDir}/\$(basename \${lib}) .
done

LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}
ls -lrt

for inputFile in \${ROOTUPLES[@]}; do
    if [[ "\${inputFile}" == *"/eos/"* ]]; then
        #change the eos mounting point
        fileNameNewMountingPoint=\`echo \$inputFile | awk -F'eos/' '{printf("'\${eosRoot}/'%s",\$2)}'\`
        echo \$fileNameNewMountingPoint
        theLocalFile=\`basename "\$inputFile"\`
        cp \${fileNameNewMountingPoint} .
        ./main \${theLocalFile}
        rm -f \${theLocalFile}
    else
        ./main \${inputFile}
    fi

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

done

code=$?

if [[ "$toBeCopiedInMainFolderCommand" != "" ]]; then
    $toBeCopiedInMainFolderCommand
    code=\${code} || \$?
fi

sed -i 's/, return code.*\$/, return code: '\${code}'/' ${absoluteFolderPath}/\${jobName}/jobList.log

/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select -b fuse umount eos 


EOF

chmod 744 ${absoluteFolderPath}/job${Name}.sh

cp $scriptPath/rerunBadJobs.sh ${absoluteFolderPath}
}

function saveDefaultValue {

    cat << EOF > $HOME/.defaultValueSubmissionScript.sh
defaultExecutable=$defaultExecutable        
defaultLibs="$defaultLibs"
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

