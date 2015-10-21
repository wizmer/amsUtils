#ifndef _DSTAMSBINARY__CPP_
#define _DSTAMSBINARY__CPP_

#include "DstAmsBinary.hpp"

using namespace rootUtils;

void DstAmsBinary::initPointers(){
}

void DstAmsBinary::registerVariables(){

}

void DstAmsBinary::init(){
    maxRootFiles = 1;
    setOutputFile(false);
    Loop::init();

    
    registerVariables();
    
    // std::size_t memConsumption = 0;
    // chunkSize = maxRAM / memConsumption;
    chunkSize = 100000;

    std::cout << "chunkSize : " << chunkSize << std::endl;

    if(outputFileName == "")     outputFileName = generalUtils::getFileNameWithoutExtension(data[0]);

    if( mkdir(outputFileName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0){
        int i = 0;
        while( mkdir( (outputFileName+generalUtils::toString(i)).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0 ){
            i++;
        }
        outputFileName = outputFileName+generalUtils::toString(i);
    }
}

enum{
    RUN,
    _SIZE_,
};


template <int N,typename T> struct VarBase {
    static void save(const std::string & outputFileName, int chunkNumber, int chunkStepNumber) {
        std::cout << "outputFileName : " << outputFileName << std::endl;
        std::stringstream fname;
        fname << outputFileName <<"/" << name << "_chunk" << chunkNumber << ".bin";
        std::cout << "fname.str() : " << fname.str() << std::endl;
        std::ofstream myfile( fname.str(), std::ios::out | std::ios::binary);

        // myfile.write((char*)&chunkStepNumber, sizeof(int));
        myfile.write((char*)var, sizeof(T)*chunkStepNumber);
        myfile.close();
    }

    static std::string name;
    static T var[];
};

template <int N, typename T> T VarBase<N,T>::var[100000] = {};


template <> struct DstAmsBinary::Var<RUN> : VarBase<RUN,unsigned int>{
    static void fill(DstAmsBinary *parent){
        var[parent -> chunkStepNumber] = parent -> ev ? parent -> ev -> Run() : 0;
        std::cout << "var[parent -> chunkStepNumber] : " << var[parent -> chunkStepNumber] << std::endl;
    }
};
template <> std::string VarBase<RUN,unsigned int>::name = "run";


template <> void DstAmsBinary::assign<-1>(){}
template <> void DstAmsBinary::saveChunk<-1>(){}

bool DstAmsBinary::process(){
    initPointers();

    assign< _SIZE_ - 1 >();

    chunkStepNumber++;
    if( chunkStepNumber >= chunkSize) saveAll();
}

void DstAmsBinary::saveAll(){
    static int chunkNumber = 0;
        
    std::cout << "saving chunk" << std::endl;
    std::cout << "var.size() : " << nVar << std::endl;

    saveChunk<_SIZE_ - 1>();    

    chunkStepNumber = 0;
    chunkNumber++;
}

void DstAmsBinary::end(){
    if(chunkStepNumber > 0) saveAll();
    saveMetaData();
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


template <typename T> std::string getType(){ std::cout << "Unknown specialization" << std::endl; exit(-1); }
template <> std::string getType<float>(){ return "float"; }
template <> std::string getType<unsigned long long>(){ return "unsigned long long"; }
template <> std::string getType<double>(){ return "double"; }
template <> std::string getType<unsigned int>(){ return "unsigned int"; }
template <> std::string getType<int>(){ return "int"; }

int main(int argc, char **argv){
    //Processing input options
    int c;
    int entries = 0;
    std::string outFname;
    std::string  inFname = "/afs/cern.ch/work/b/bcoste/protonB800.root";

    if (argc==1) std::cout
                     << "Example:  ./dst -o test.root -n 10000 $EOSPATH/ams/Data/AMS02/2014/ISS.B900/std/1439205227.00000001.root"
                     << std::endl;

    while((c = getopt(argc, argv, "o:n:")) != -1) {
        if(c == 'o') outFname = std::string(optarg);
        else if(c == 'n') entries = atoi(optarg);
    }

    if (optind < argc) inFname = std::string(argv[optind++]); 
    
    DstAmsBinary t( inFname );
    t.setMaxEntries(entries);
    if(!outFname.empty()) t.setOutputFileName(outFname);
    t.go();
    return 0;
}

#endif


