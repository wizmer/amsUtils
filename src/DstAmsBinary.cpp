#ifndef _DSTAMSBINARY__CPP_
#define _DSTAMSBINARY__CPP_

#include "DstAmsBinary.hpp"

using namespace rootUtils;

void DstAmsBinary::init(){
    maxRootFiles = 1;
    setOutputFile(false);
    Loop::init();

    
    registerVariables();
    nVar = variables.size();
    
    std::size_t memConsumption = 0;
    for(int iVar = 0; iVar < nVar; iVar++) memConsumption += variables[iVar] -> getSize();
    chunkSize = maxRAM / memConsumption;

    std::cout << "chunkSize : " << chunkSize << std::endl;

    if(outputFileName == "")     outputFileName = generalUtils::getFileNameWithoutExtension(data[0]);

    if( mkdir(outputFileName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0){
        int i = 0;
        while( mkdir( (outputFileName+generalUtils::toString(i)).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0 ){
            i++;
        }
        outputFileName = outputFileName+generalUtils::toString(i);
    }
    
    for(int iVar = 0; iVar < nVar; iVar++) variables[iVar] -> allocArray(chunkSize);

}

bool DstAmsBinary::process(){
    initPointers();
    for( int i = 0; i<nVar; i++){
        variables[i] -> assign(chunkStepNumber);
    }

    chunkStepNumber++;
    if( chunkStepNumber >= chunkSize) saveChunk();
}

void DstAmsBinary::end(){
    if( chunkStepNumber > 0 ) saveChunk();
}

void DstAmsBinary::saveChunk(){
    static int chunkNumber = 0;
        
    std::cout << "saving chunk" << std::endl;
    std::cout << "var.size() : " << nVar << std::endl;

    for(int iVar = 0; iVar < nVar; iVar++){
        variables[iVar] -> save(outputFileName, chunkNumber, chunkStepNumber);
    }
    

    chunkStepNumber = 0;
    chunkNumber++;
}

void DstAmsBinary::saveMetaData()
{
    std::ofstream myfile( outputFileName+"/metadata.txt", std::ios::out);
    myfile << "nVar "           << nVar          << std::endl;
    myfile << "chunkSize "      << chunkSize     << std::endl;
      
    myfile.close();
}

int DstAmsBinary::cutEvent(){
    return false;
}


#endif

