#ifndef _DSTAMSBINARY__HPP_
#define _DSTAMSBINARY__HPP_

#include <string>
#include <vector>
#include <iostream>
#include <functional>

#include "Loop.hpp"
#include "rootUtils.hpp"
#include "Stack.hpp"

class DstAmsBinary : public Loop{
public:
    DstAmsBinary( std::string _data, int _maxRAM ) : Loop(_data),
                                        chunkStepNumber(0),
                                        maxRAM(_maxRAM)
    {}

    virtual ~DstAmsBinary(){}
  
    virtual void registerVariables() = 0;
    virtual void initPointers() = 0;

    void saveChunk();
    void end();
    bool process();
    void init();
    void saveMetaData();
    int cutEvent();

protected:
    std::string filename;
    float p;
    float R;
    bool isMC;
    int nChi2Cut;
    unsigned int chunkStepNumber;
    unsigned int chunkSize;
    int nVar;
    int maxRAM;

    std::map< std::string, std::vector<float> > var;
    std::vector< std::string > varName;
    std::map< std::string, std::function<void ()>> fill;


};

#endif


