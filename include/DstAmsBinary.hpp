#ifndef _DSTAMSBINARY__HPP_
#define _DSTAMSBINARY__HPP_

#include <string>
#include <vector>
#include <iostream>
#include <functional>

#include "Loop.hpp"
#include "rootUtils.hpp"
#include "Stack.hpp"


template <typename T> std::string getType();
template <> std::string getType<float>();
template <> std::string getType<unsigned long long>();
template <> std::string getType<double>();
template <> std::string getType<unsigned int>();
template <> std::string getType<int>();




class DstAmsBinary : public Loop{
public:
    DstAmsBinary( std::string _data ) : Loop(_data),
                                        chunkStepNumber(0)
    {}

    virtual ~DstAmsBinary(){}
    virtual void registerVariables();
    virtual void initPointers();

    void end();
    bool process();
    void init();
    void saveMetaData();
    int cutEvent();

    template <int N> void assign(){
        Var<N>::fill(this);
        assign<N-1>();
    }

    template <int N> void saveChunk(){
        Var<N>::save(outputFileName, 0, chunkStepNumber);
        saveChunk<N-1>();
    }

    template <int N> struct Var{
        static void fill(DstAmsBinary *parent){
            
        }
    };

    void saveAll();

protected:
    float p;
    float R;
    bool isMC;
    int nChi2Cut;
    unsigned int chunkStepNumber;
    unsigned int chunkSize;
    int nVar;
};

#endif


