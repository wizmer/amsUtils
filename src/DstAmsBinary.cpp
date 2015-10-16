#ifndef _DSTAMSBINARY__CPP_
#define _DSTAMSBINARY__CPP_

#include "DstAmsBinary.hpp"

using namespace rootUtils;

void DstAmsBinary::init(){
    maxRootFiles = 1;
    setOutputFile(false);
    Loop::init();

    
    registerVariables();

    chunkSize = maxRAM / ((fill.size()+1)*sizeof(float));

    for(auto it = fill.begin(); it != fill.end() ;it++){
        var[it->first] = std::vector<float>(chunkSize);
        varName.push_back(it->first);
    }

    std::cout << "chunkSize : " << chunkSize << std::endl;

    if(outputFileName == "")     outputFileName = generalUtils::getFileNameWithoutExtension(data[0]);

    if( mkdir(outputFileName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0){
        int i = 0;
        while( mkdir( (outputFileName+generalUtils::toString(i)).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0 ){
            i++;
        }
        outputFileName = outputFileName+generalUtils::toString(i);
    }
    
    nVar = var.size();
}

bool DstAmsBinary::process(){
    initPointers();
    for( int i = 0; i<nVar; i++){
        fill[varName[i]]();
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
    std::cout << "var.size() : " << var.size() << std::endl;

    for(auto it = var.begin(); it != var.end() ;it++){
        std::cout << "outputFileName : " << outputFileName << std::endl;
        std::stringstream fname;
        fname << outputFileName <<"/" << it->first << "_chunk" << chunkNumber << ".bin";
        std::cout << "fname.str() : " << fname.str() << std::endl;
        std::ofstream myfile( fname.str(), std::ios::out | std::ios::binary);

        // myfile.write((char*)&chunkStepNumber, sizeof(int));
        myfile.write((char*)&(it->second[0]), sizeof(float)*chunkStepNumber);
        myfile.close();
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

